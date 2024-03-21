#include "DSP.hpp"
#include "Ex.hpp"


DSP::DSP() : mPrefetcher{ Prefetcher::create<DSP>() }
{
  std::ranges::generate( mExecutionUnitPool, ExecutionUnit::create );

  std::ranges::fill( mLocalRAM, std::byteswap( ( ( uint32_t )0xe400 | ( uint32_t )0xe400 << 16 ) ) );
  std::ranges::fill( mRegs, 0 );
  std::ranges::fill( mRegLocks, false );
}

DSP::~DSP()
{
}

void DSP::advance()
{
  processCycle();
  prefetch();
}

void DSP::debugWrite( uint32_t address, std::span<uint32_t const> data )
{
  if ( address & 3 )
    throw Ex{ "Jerry::debugWrite: Unaligned address " } << std::hex << address;

  if ( address < RAM_BASE || address + data.size() * sizeof( uint32_t ) >= RAM_BASE + RAM_SIZE )
    throw Ex{ "Jerry::debugWrite: Unhandled address " } << std::hex << address;

  std::copy( data.begin(), data.end(), mLocalRAM.begin() + ( address - RAM_BASE ) / sizeof( uint32_t ) );
}

void DSP::debugWrite( uint32_t address, uint32_t data )
{
  storeLong( address, data );
}

RISCOpcode DSP::mapOpcode( uint16_t code )
{
  static constexpr std::array<RISCOpcode::Opcode, 64> map =
  {
    RISCOpcode::ADD,    RISCOpcode::ADDC,   RISCOpcode::ADDQ,   RISCOpcode::ADDQT,
    RISCOpcode::SUB,    RISCOpcode::SUBC,   RISCOpcode::SUBQ,   RISCOpcode::SUBQT,
    RISCOpcode::NEG,    RISCOpcode::AND,    RISCOpcode::OR,     RISCOpcode::XOR,
    RISCOpcode::NOT,    RISCOpcode::BTST,   RISCOpcode::BSET,   RISCOpcode::BCLR,
    RISCOpcode::MULT,   RISCOpcode::IMULT,  RISCOpcode::IMULTN, RISCOpcode::RESMAC,
    RISCOpcode::IMACN,  RISCOpcode::DIV,    RISCOpcode::ABS,    RISCOpcode::SH,
    RISCOpcode::SHLQ,   RISCOpcode::SHRQ,   RISCOpcode::SHA,    RISCOpcode::SHARQ,
    RISCOpcode::ROR,    RISCOpcode::RORQ,   RISCOpcode::CMP,    RISCOpcode::CMPQ,
    RISCOpcode::SUBQMOD,RISCOpcode::SAT16S, RISCOpcode::MOVE,   RISCOpcode::MOVEQ,
    RISCOpcode::MOVETA, RISCOpcode::MOVEFA, RISCOpcode::MOVEI,  RISCOpcode::LOADB,
    RISCOpcode::LOADW,  RISCOpcode::LOAD,   RISCOpcode::SAT32S, RISCOpcode::LOAD14N,
    RISCOpcode::LOAD15N,RISCOpcode::STOREB, RISCOpcode::STOREW, RISCOpcode::STORE,
    RISCOpcode::MIRROR, RISCOpcode::STORE14N,RISCOpcode::STORE15N,RISCOpcode::MOVEPC,
    RISCOpcode::JUMP,   RISCOpcode::JR,     RISCOpcode::MMULT,  RISCOpcode::MTOI,
    RISCOpcode::NORMI,  RISCOpcode::NOP,    RISCOpcode::LOAD14R,RISCOpcode::LOAD15R,
    RISCOpcode::STORE14R,RISCOpcode::STORE15R,RISCOpcode::_ILL, RISCOpcode::ADDQMOD
  };

  return { map[( code >> 10 )], (uint8_t)( ( code >> 5 ) & 31), (uint8_t)(code & 31) };
}

void DSP::storeLong( uint32_t address, uint32_t data )
{
  assert( ( address & 0xff0000 ) == 0xf10000 );
  if ( ( address & 3 ) != 0 )
  {
    address &= ~3;
  }

  switch ( address )
  {
  case D_FLAGS:
    flagsSet( data );
    break;
  case D_MTXC:
    mMTXC = data;
    break;
  case D_MTXA:
    mMTXA = data;
    break;
  case D_END:
    break;
  case D_PC:
    if ( data < RAM_BASE || data >= RAM_BASE + RAM_SIZE )
      throw Ex{ "Jerry unsupported PC " } << std::hex << data;
    mPC = data;
    break;
  case D_CTRL:
    ctrlSet( data );
    break;
  case D_MOD:
    mMod = data;
    break;
  case D_DIVCTRL:
    mDivCtrl = data;
    break;
  case D_MACHI:
    break;
  default:
    if ( address >= RAM_BASE && address < RAM_BASE + RAM_SIZE )
    {
      mLocalRAM[( address - RAM_BASE ) / sizeof( uint32_t )] = std::byteswap( data );
    }
    else if ( address >= ROM_BASE && address < ROM_BASE + ROM_SIZE )
    {
    }
    else
    {
    }
    break;
  }
}

uint32_t DSP::loadLong( uint32_t address )
{
  return 0;
}


void DSP::processCycle()
{
  mCycle += 1;

  int8_t writeSlot = -1;
  int32_t divideCycle = -1;

  if ( mDivideUnit )
  {
    switch ( mDivideUnit.getType() )
    {
    case ExecutionUnit::AwaiterType::DIVIDE_INIT:
      mDivideUnit.divideInit( ( mDivCtrl & 1 ) != 0 );
      break;
    case ExecutionUnit::AwaiterType::DIVIDE_CYCLE:
      {
        auto [cycle] = mDivideUnit.divideCycle();
        divideCycle = cycle;
        mDivideUnit.advance();
      }
      break;
    case ExecutionUnit::AwaiterType::UNLOCK_WRITE_DST_REMAIN:
      {
        auto [quotient, remain, reg] = mDivideUnit.unlockWriteDstRemain();
        mRegLocks[reg] = false;
        mRegs[reg] = quotient;
        mRemain = remain;
        mDivideUnit.advance();
        assert( mDivideUnit.getType() == ExecutionUnit::AwaiterType::DECODE );
        mExecutionUnitPool[++mPoolTop] = std::move( mDivideUnit );
      }
      break;
    default:
      throw Ex{} << "Unexpected divide unit type " << (int)mDivideUnit.getType();
    }
  }

  if ( mReadUnit )
  {
    switch( mReadUnit.getType() )
    {
    case ExecutionUnit::AwaiterType::GET_CODE_LO:
      {
        if ( !mPrefetcher )
        {
          if ( auto pull = prefetchPull() )
          {
            mPrefetcher.put( pull.data() );
          }
          else
          {
            break;
          }
        }
        assert( mPrefetcher );
        mReadUnit.getCodeLo( mPrefetcher.getCode() );
      }
      break;
    case ExecutionUnit::AwaiterType::GET_CODE_HI:
      if ( !mWriteUnit )
      {
        if ( !mPrefetcher )
        {
          if ( auto pull = prefetchPull() )
          {
            mPrefetcher.put( pull.data() );
          }
          else
          {
            break;
          }
        }
        assert( mPrefetcher );
        mReadUnit.getCodeHi( mPrefetcher.getCode() );
        mWriteUnit = std::move( mReadUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::READ_DST_LOCK_FLAGS:
      if ( !mComputeUnit )
      {
        auto [dst] = mReadUnit.readDstLockFlags();
        if ( !mRegLocks[dst] )
        {
          mReadUnit.readDstLockFlags( { mRegs[dst] } );
          mFlagsSemaphore += 1;
          mComputeUnit = std::move( mReadUnit );
        }
      }
      break;
    case ExecutionUnit::AwaiterType::READ_LOCK_DST:
      if ( !mComputeUnit )
      {
        auto [dst] = mReadUnit.readLockDst();
        if ( !mRegLocks[dst] )
        {
          mRegLocks[dst] = true;
          mReadUnit.readLockDst( { mRegs[dst] } );
          mComputeUnit = std::move( mReadUnit );
        }
      }
      break;
    case ExecutionUnit::AwaiterType::READ_LOCK_DST_LOCK_FLAGS:
      if ( !mComputeUnit )
      {
        auto [dst] = mReadUnit.readLockDstLockFlags();
        if ( !mRegLocks[dst] )
        {
          mRegLocks[dst] = true;
          mFlagsSemaphore += 1;
          mReadUnit.readLockDstLockFlags( { mRegs[dst] } );
          mComputeUnit = std::move( mReadUnit );
        }
      }
      break;
    case ExecutionUnit::AwaiterType::READ_SRC:
      if ( !mComputeUnit )
      {
        auto [src] = mReadUnit.readSrc();
        if ( !mRegLocks[src] )
        {
          mReadUnit.readSrc( { mRegs[src] } );
          mComputeUnit = std::move( mReadUnit );
        }
      }
      break;
    case ExecutionUnit::AwaiterType::READ_SRC_READ_DST:
      if ( !mComputeUnit )
      {
        auto [src, dst] = mReadUnit.readSrcReadDst();
        if ( !mRegLocks[src] && !mRegLocks[dst] )
        {
          mReadUnit.readSrcReadDst( { mRegs[src], mRegs[dst] } );
          mComputeUnit = std::move( mReadUnit );
        }
      }
      break;
    case ExecutionUnit::AwaiterType::READ_SRC_READ_LOCK_DST:
      if ( !mComputeUnit )
      {
        auto [src, dst] = mReadUnit.readSrcReadLockDst();
        if ( !mRegLocks[src] && !mRegLocks[dst] )
        {
          mRegLocks[dst] = true;
          mReadUnit.readSrcReadLockDst( { mRegs[src], mRegs[dst] } );
          assert( mReadUnit.getType() == ExecutionUnit::AwaiterType::DIVIDE_INIT );
          mDivideUnit = std::move( mReadUnit );
        }
      }
      break;
    case ExecutionUnit::AwaiterType::READ_SRC_READ_LOCK_DST_LOCK_FLAGS:
      if ( !mComputeUnit )
      {
        auto [src, dst] = mReadUnit.readSrcReadLockDstLockFlags();
        if ( !mRegLocks[src] && !mRegLocks[dst] )
        {
          mRegLocks[dst] = true;
          mFlagsSemaphore += 1;
          mReadUnit.readSrcReadLockDstLockFlags( { mRegs[src], mRegs[dst] } );
          mComputeUnit = std::move( mReadUnit );
        }
      }
      break;
    case ExecutionUnit::AwaiterType::READ_SRC_READ_LOCK_DST_READ_LOCK_FLAGS:
      if ( !mComputeUnit )
      {
        auto [src, dst] = mReadUnit.readSrcReadLockDstReadLockFlags();
        if ( !mRegLocks[src] && !mRegLocks[dst] && mFlagsSemaphore == 0 )
        {
          mRegLocks[dst] = true;
          mFlagsSemaphore += 1;
          mReadUnit.readSrcReadLockDstReadLockFlags( { mRegs[src], mRegs[dst] } );
          mComputeUnit = std::move( mReadUnit );
        }
      }
      break;
    case ExecutionUnit::AwaiterType::READ_PC:
      if ( !mWriteUnit )
      {
        mReadUnit.readPC( mPC );
        mWriteUnit = std::move( mReadUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::MOVE_Q:
      if ( !mWriteUnit )
      {
        mReadUnit.advance();
        assert( mReadUnit.getType() == ExecutionUnit::AwaiterType::WRITE_DST );
        mWriteUnit = std::move( mReadUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::NOP:
      mReadUnit.advance();
      assert( mReadUnit.getType() == ExecutionUnit::AwaiterType::DECODE );
      mExecutionUnitPool[++mPoolTop] = std::move( mReadUnit );
      break;
    }
  }


  if ( !mReadUnit )
  {
    if ( !mPrefetcher )
    {
      if ( auto pull = prefetchPull() )
      {
        mPrefetcher.put( pull.data() );
      }
      else
      {
        return;
      }
    }
    assert( mPrefetcher );
    auto opcode = mPrefetcher.get();
    opcode.srcReg = mRegisterFile + opcode.srcValue;
    opcode.dstReg = mRegisterFile + opcode.dstValue;
    mReadUnit = std::move( mExecutionUnitPool[mPoolTop--] );
    mReadUnit.decode( opcode );
  }
}

void DSP::prefetch()
{
  //Only local RAM for now
  assert( ( mPC >= RAM_BASE ) && ( mPC < RAM_BASE + RAM_SIZE ) );

  if ( mPrefetch.doPrefetch && mLastLocalRAMAccessCycle != mCycle )
  {
    if ( int off = prefetchFill() )
    {
      mLastLocalRAMAccessCycle = mCycle;
      mPC += off;
    }
  }
}

int DSP::prefetchFill()
{
  assert( mPrefetch.queueSize < 4 );
  assert( ( mPC & 1 ) == 0 );

  mPrefetch.decodedAddress = mPC;

  if ( mPC & 2 )
  {
    auto pCode = ( uint16_t const* )mLocalRAM.data() + ( ( mPC - RAM_BASE ) >> 1 );

    //fetching word from odd address
    mPrefetch.queue = std::byteswap( *pCode );
    mPrefetch.queueSize = 1;
    return 2;
  }
  else
  {
    auto pCode = ( uint16_t const* )( ( uint8_t const* )mLocalRAM.data() + ( mPC - RAM_BASE ) );

    mPrefetch.queue = std::byteswap( pCode[0] );
    mPrefetch.queue |= ( uint32_t )std::byteswap( pCode[1] ) << 16;

    mPrefetch.queueSize = 2;
    return 4;
  }

  return 0;
}

DSP::Prefetch::Pull DSP::prefetchPull()
{
  if ( mPrefetch.queueSize == 0 )
  {
    mPrefetch.doPrefetch = true;
    return {};
  }
  else
  {
    uint16_t fetch = mPrefetch.queue & 0xffff;
    mPrefetch.queue >>= 16;
    mPrefetch.queueSize -= 1;
    mPrefetch.doPrefetch = mPrefetch.needsPrefetching();
    mPrefetch.decodedAddress += 2;
    return { 1, mPrefetch.decodedAddress - 2, fetch };
  }
}

void DSP::ctrlSet( uint16_t value )
{
  mCtrl.dspgo = ( value & CTRL::DSPGO ) != 0;
}

void DSP::flagsSet( uint16_t value )
{
  mFlags.nz = ( ( value & FLAGS::NEGA_FLAG ) << 29 ) | ( ( value & FLAGS::ZERO_FLAG ) ^ 1 );
  mFlags.imask &= ( value & FLAGS::IMASK ) != 0;
  mFlags.cpuena = ( value & FLAGS::D_CPUENA ) != 0;
  mFlags.i2sena = ( value & FLAGS::D_I2SENA ) != 0;
  mFlags.tim1ena = ( value & FLAGS::D_TIM1ENA ) != 0;
  mFlags.tim2ena = ( value & FLAGS::D_TIM2ENA ) != 0;
  mFlags.ext0ena = ( value & FLAGS::D_EXT0ENA ) != 0;
  mFlags.ext1ena = ( value & FLAGS::D_EXT1ENA ) != 0;
  mCtrl.intLatches &= ( value & FLAGS::D_CPUCLR ) != 0 ? ~CTRL::D_CPULAT : 0xffff;
  mCtrl.intLatches &= ( value & FLAGS::D_I2SCLR ) != 0 ? ~CTRL::D_I2SLAT : 0xffff;
  mCtrl.intLatches &= ( value & FLAGS::D_TIM1CLR ) != 0 ? ~CTRL::D_TIM1LAT : 0xffff;
  mCtrl.intLatches &= ( value & FLAGS::D_TIM2CLR ) != 0 ? ~CTRL::D_TIM2LAT : 0xffff;
  mCtrl.intLatches &= ( value & FLAGS::D_EXT0CLR ) != 0 ? ~CTRL::D_EXT0LAT : 0xffff;
  mCtrl.intLatches &= ( value & FLAGS::D_EXT1CLR ) != 0 ? ~CTRL::D_EXT1LAT : 0xffff;
  mFlags.regpage = ( value & FLAGS::REGPAGE ) != 0;

  mRegisterFile = mFlags.regpage ? 32 : 0;
}
