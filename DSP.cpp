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

bool DSP::isAddressLocal( uint32_t address ) const
{
  return true;
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

uint32_t DSP::loadByteLocal( uint32_t address )
{
  return 0;
}

uint32_t DSP::loadWordLocal( uint32_t address )
{
  return 0;
}

uint32_t DSP::loadLongLocal( uint32_t address )
{
  return {};
}

std::optional<uint32_t> DSP::loadByteExternal( uint32_t address )
{
  return std::optional<uint32_t>();
}

std::optional<uint32_t> DSP::loadWordExternal( uint32_t address )
{
  return std::optional<uint32_t>();
}

std::optional<uint32_t> DSP::loadLongExternal( uint32_t address )
{
  return std::optional<uint32_t>();
}

void DSP::storeByteLocal( uint32_t address, uint8_t value )
{
}

void DSP::storeWordLocal( uint32_t address, uint16_t value )
{
}

void DSP::storeLongLocal( uint32_t address, uint32_t value )
{
}

bool DSP::storeByteExternal( uint32_t address, uint8_t value )
{
  return false;
}

bool DSP::storeWordExternal( uint32_t address, uint16_t value )
{
  return false;
}

bool DSP::storeLongExternal( uint32_t address, uint32_t value )
{
  return false;
}


void DSP::writeFlags( uint32_t nz, uint16_t c )
{
}

std::tuple<bool, bool, bool> DSP::readFlags() const
{
  return std::tuple<bool, bool, bool>();
}

void DSP::processCycle()
{
  mCycle += 1;

  bool localMemoryAccessed = false;
  int8_t writeSlot = -1;
  int32_t divideCycle = -1;

  if ( mWriteUnit )
  {
    switch ( mWriteUnit.getType() )
    {
    case ExecutionUnit::AwaiterType::WRITE_PC:
      {
        auto [pc] = mWriteUnit.writePC();
        mPC = pc;
        mWriteUnit.advance();
        assert( mWriteUnit.getType() == ExecutionUnit::AwaiterType::DECODE );
        mExecutionUnitPool[++mPoolTop] = std::move( mWriteUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::WRITE_DST:
      {
        auto [value,reg] = mWriteUnit.writeDst();
        mRegs[reg] = value;
        writeSlot = reg;
        mWriteUnit.advance();
        assert( mWriteUnit.getType() == ExecutionUnit::AwaiterType::DECODE );
        mExecutionUnitPool[++mPoolTop] = std::move( mWriteUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::WRITE_DST_WRITE_FLAGS:
      {
        auto [valuenz,c,reg] = mWriteUnit.writeDstUnlockWriteFlags();
        writeFlags( valuenz, c );
        mRegs[reg] = valuenz;
        writeSlot = reg;
        mWriteUnit.advance();
        assert( mWriteUnit.getType() == ExecutionUnit::AwaiterType::DECODE );
        mExecutionUnitPool[++mPoolTop] = std::move( mWriteUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::UNLOCK_WRITE_FLAGS:
      {
        auto [valuenz, c] = mWriteUnit.unlockWriteFlags();
        writeFlags( valuenz, c );
        assert( mFlagsSemaphore > 0 );
        mFlagsSemaphore -= 1;
        assert( mFlagsSemaphore >= 0 );
        mWriteUnit.advance();
        assert( mWriteUnit.getType() == ExecutionUnit::AwaiterType::DECODE );
        mExecutionUnitPool[++mPoolTop] = std::move( mWriteUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::UNLOCK_WRITE_DST:
      {
        auto [value,reg] = mWriteUnit.unlockWriteDst();
        mRegs[reg] = value;
        assert( mRegLocks[reg] );
        mRegLocks[reg] = false;
        writeSlot = reg;
        mWriteUnit.advance();
        assert( mWriteUnit.getType() == ExecutionUnit::AwaiterType::DECODE );
        mExecutionUnitPool[++mPoolTop] = std::move( mWriteUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::UNLOCK_WRITE_DST_UNLOCK_WRITE_FLAGS:
      {
        auto [valuenz,c,reg] = mWriteUnit.unlockWriteDstUnlockWriteFlags();
        mRegs[reg] = valuenz;
        writeSlot = reg;
        assert( mRegLocks[reg] );
        mRegLocks[reg] = false;
        writeFlags( valuenz, c );
        assert( mFlagsSemaphore > 0 );
        mFlagsSemaphore -= 1;
        assert( mFlagsSemaphore >= 0 );
        mWriteUnit.advance();
        assert( mWriteUnit.getType() == ExecutionUnit::AwaiterType::DECODE );
        mExecutionUnitPool[++mPoolTop] = std::move( mWriteUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::UNLOCK_WRITE_DST_REMAIN:
      {
        auto [value,remain,reg] = mWriteUnit.unlockWriteDstRemain();
        mRegs[reg] = value;
        writeSlot = reg;
        assert( mRegLocks[reg] );
        mRegLocks[reg] = false;
        mRemain = remain;
        mWriteUnit.advance();
        assert( mWriteUnit.getType() == ExecutionUnit::AwaiterType::DECODE );
        mExecutionUnitPool[++mPoolTop] = std::move( mWriteUnit );
      }
      break;
    default:
      throw Ex{} << "Unexpected write unit type " << ( int )mComputeUnit.getType();
    }
  }

  if ( mDivideUnit )
  {
    //TODO  make proper transition to write unit taking into account write slot
    assert( mDivideUnit.getType() == ExecutionUnit::AwaiterType::DIVIDE_CYCLE );
    auto [cycle] = mDivideUnit.divideCycle();
    divideCycle = cycle;
    mDivideUnit.advance();
    if ( cycle == 0 )
    {
      assert( mDivideUnit.getType() == ExecutionUnit::AwaiterType::DECODE );
      mExecutionUnitPool[++mPoolTop] = std::move( mComputeUnit );
    }
  }

  if ( mMemoryUnit )
  {
    switch ( mMemoryUnit.getType() )
    {
    case ExecutionUnit::AwaiterType::MEMORY_LOAD_BYTE_EXTERNAL:
      if ( !mWriteUnit )
      {
        if ( auto opt = loadByteExternal( mMemoryUnit.memoryLoadByteExternal().src ) )
        {
          mMemoryUnit.memoryLoadByteExternal( *opt );
          mWriteUnit = std::move( mMemoryUnit );
        }
      }
      break;
    case ExecutionUnit::AwaiterType::MEMORY_LOAD_WORD_EXTERNAL:
      if ( !mWriteUnit )
      {
        if ( auto opt = loadWordExternal( mMemoryUnit.memoryLoadWordExternal().src ) )
        {
          mMemoryUnit.memoryLoadWordExternal( *opt );
          mWriteUnit = std::move( mMemoryUnit );
        }
      }
      break;
    case ExecutionUnit::AwaiterType::MEMORY_LOAD_LONG_EXTERNAL:
      if ( !mWriteUnit )
      {
        if ( auto opt = loadLongExternal( mMemoryUnit.memoryLoadLongExternal().src ) )
        {
          mMemoryUnit.memoryLoadLongExternal( *opt );
          mWriteUnit = std::move( mMemoryUnit );
        }
      }
      break;
    case ExecutionUnit::AwaiterType::MEMORY_STORE_BYTE_EXTERNAL:
      if ( auto [addr, value] = mMemoryUnit.memoryStoreByteExternal(); storeByteExternal( addr, value ) )
      {
        mComputeUnit.advance();
        assert( mComputeUnit.getType() == ExecutionUnit::AwaiterType::DECODE );
        mExecutionUnitPool[++mPoolTop] = std::move( mComputeUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::MEMORY_STORE_WORD_EXTERNAL:
      if ( auto [addr, value] = mMemoryUnit.memoryStoreWordExternal(); storeWordExternal( addr, value ) )
      {
        mComputeUnit.advance();
        assert( mComputeUnit.getType() == ExecutionUnit::AwaiterType::DECODE );
        mExecutionUnitPool[++mPoolTop] = std::move( mComputeUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::MEMORY_STORE_LONG_EXTERNAL:
      if ( auto [addr, value] = mMemoryUnit.memoryStoreLongExternal(); storeLongExternal( addr, value ) )
      {
        mComputeUnit.advance();
        assert( mComputeUnit.getType() == ExecutionUnit::AwaiterType::DECODE );
        mExecutionUnitPool[++mPoolTop] = std::move( mComputeUnit );
      }
      break;
    default:
      throw Ex{} << "Unexpected memory unit type " << ( int )mMemoryUnit.getType();
    }
  }

  if ( mComputeUnit )
  {
    switch ( mComputeUnit.getType() )
    {
    case ExecutionUnit::AwaiterType::COMPUTE_PENDING:
      mComputeUnit.advance();
      break;
    case ExecutionUnit::AwaiterType::COMPUTE:
      if ( !mWriteUnit )
      {
        mComputeUnit.advance();
        mWriteUnit = std::move( mComputeUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::GET_MOD:
      if ( !mWriteUnit )
      {
        mComputeUnit.getMod( mMod );
        mWriteUnit = std::move( mComputeUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::PREPARE_JUMP:
      if ( !mWriteUnit )
      {
        mPrefetcher.prepareJump();
        mComputeUnit.prepareJump( mPC );
        mWriteUnit = std::move( mComputeUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::DIVIDE_INIT:
      if ( !mDivideUnit )
      {
        mComputeUnit.divideInit( ( mDivCtrl & 1 ) != 0 );
        mDivideUnit = std::move( mComputeUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::READ_REG_NO_SCOREBOARD:
      {
        auto [reg] = mComputeUnit.readRegNoScoreboard();
        mComputeUnit.readRegNoScoreboard( mRegs[reg] );
      }
      break;
    case ExecutionUnit::AwaiterType::MEMORY_LOAD_BYTE:
      if ( auto [addr] = mComputeUnit.memoryLoadByte(); isAddressLocal( addr ) )
      {
        if ( !mWriteUnit && !localMemoryAccessed )
        {
          uint32_t value = loadByteLocal( addr );
          mComputeUnit.memoryLoadByte( value );
          mWriteUnit = std::move( mComputeUnit );
          localMemoryAccessed = true;
        }
      }
      else if ( !mMemoryUnit )
      {
        mComputeUnit.advance();
        mMemoryUnit = std::move( mComputeUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::MEMORY_LOAD_WORD:
      if ( auto [addr] = mComputeUnit.memoryLoadWord(); isAddressLocal( addr ) )
      {
        if ( !mWriteUnit && !localMemoryAccessed )
        {
          uint32_t value = loadWordLocal( addr );
          mComputeUnit.memoryLoadWord( value );
          mWriteUnit = std::move( mComputeUnit );
          localMemoryAccessed = true;
        }
      }
      else if ( !mMemoryUnit )
      {
        mComputeUnit.advance();
        mMemoryUnit = std::move( mComputeUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::MEMORY_LOAD_LONG:
      if ( auto [addr] = mComputeUnit.memoryLoadLong(); isAddressLocal( addr ) )
      {
        if ( !mWriteUnit && !localMemoryAccessed )
        {
          uint32_t value = loadLongLocal( addr );
          mComputeUnit.memoryLoadLong( value );
          mWriteUnit = std::move( mComputeUnit );
          localMemoryAccessed = true;
        }
      }
      else if ( !mMemoryUnit )
      {
        mComputeUnit.advance();
        mMemoryUnit = std::move( mComputeUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::MEMORY_STORE_BYTE:
      if ( auto [addr,value] = mComputeUnit.memoryStoreByte(); isAddressLocal( addr ) && !localMemoryAccessed )
      {
        storeByteLocal( addr, value );
        mComputeUnit.advance();
        assert( mComputeUnit.getType() == ExecutionUnit::AwaiterType::DECODE );
        mExecutionUnitPool[++mPoolTop] = std::move( mComputeUnit );
        localMemoryAccessed = true;
      }
      else if ( !mMemoryUnit )
      {
        mComputeUnit.advance();
        mMemoryUnit = std::move( mComputeUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::MEMORY_STORE_WORD:
      if ( auto [addr, value] = mComputeUnit.memoryStoreWord(); isAddressLocal( addr ) && !localMemoryAccessed )
      {
        storeWordLocal( addr, value );
        mComputeUnit.advance();
        assert( mComputeUnit.getType() == ExecutionUnit::AwaiterType::DECODE );
        mExecutionUnitPool[++mPoolTop] = std::move( mComputeUnit );
        localMemoryAccessed = true;
      }
      else if ( !mMemoryUnit )
      {
        mComputeUnit.advance();
        mMemoryUnit = std::move( mComputeUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::MEMORY_STORE_LONG:
      if ( auto [addr, value] = mComputeUnit.memoryStoreLong(); isAddressLocal( addr ) && !localMemoryAccessed )
      {
        storeLongLocal( addr, value );
        mComputeUnit.advance();
        assert( mComputeUnit.getType() == ExecutionUnit::AwaiterType::DECODE );
        mExecutionUnitPool[++mPoolTop] = std::move( mComputeUnit );
        localMemoryAccessed = true;
      }
      else if ( !mMemoryUnit )
      {
        mComputeUnit.advance();
        mMemoryUnit = std::move( mComputeUnit );
      }
      break;
    default:
      throw Ex{} << "Unexpected compute unit type " << ( int )mComputeUnit.getType();
    }
  }

  if ( mReadUnit )
  {
    switch( mReadUnit.getType() )
    {
    case ExecutionUnit::AwaiterType::READ_REG:
      if ( !mComputeUnit )
      {
        auto [reg] = mReadUnit.readReg();
        if ( !mRegLocks[reg] )
        {
          mReadUnit.readReg( mRegs[reg] );
          mComputeUnit = std::move( mReadUnit );
        }
      }
      break;
    case ExecutionUnit::AwaiterType::READ_REG_MOVE:
      if ( !mWriteUnit )
      {
        auto [reg] = mReadUnit.readReg();
        if ( !mRegLocks[reg] )
        {
          mReadUnit.readReg( mRegs[reg] );
          mWriteUnit = std::move( mReadUnit );
        }
      }
      break;
    case ExecutionUnit::AwaiterType::READ_REG_READ_FLAGS:
      if ( !mComputeUnit )
      {
        auto [reg] = mReadUnit.readRegReadFlags();
        if ( !mRegLocks[reg] && mFlagsSemaphore == 0 )
        {
          auto [n,z,c] = readFlags();
          mReadUnit.readRegReadFlags( { mRegs[reg], n, z, c } );
          mComputeUnit = std::move( mReadUnit );
        }
      }
      break;
    case ExecutionUnit::AwaiterType::READ_REG_LOCK_FLAGS:
      if ( !mComputeUnit )
      {
        auto [reg] = mReadUnit.readRegLockFlags();
        if ( !mRegLocks[reg] )
        {
          mFlagsSemaphore += 1;
          mReadUnit.readRegLockFlags( mRegs[reg] );
          mComputeUnit = std::move( mReadUnit );
        }
      }
      break;
    case ExecutionUnit::AwaiterType::READ_LOCK_REG:
      if ( !mComputeUnit )
      {
        auto [reg] = mReadUnit.readLockReg();
        if ( !mRegLocks[reg] )
        {
          mRegLocks[reg] = true;
          mReadUnit.readLockReg( mRegs[reg] );
          mComputeUnit = std::move( mReadUnit );
        }
      }
      break;
    case ExecutionUnit::AwaiterType::READ_LOCK_REG_LOCK_FLAGS:
      if ( !mComputeUnit )
      {
        auto [reg] = mReadUnit.readLockRegLockFlags();
        if ( !mRegLocks[reg] )
        {
          mRegLocks[reg] = true;
          mFlagsSemaphore += 1;
          mReadUnit.readLockRegLockFlags( mRegs[reg] );
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
          if ( writeSlot < 0 || writeSlot == src || writeSlot == dst )
          {
            mReadUnit.readSrcReadDst( { mRegs[src], mRegs[dst] } );
            mComputeUnit = std::move( mReadUnit );
          }
        }
      }
      break;
    case ExecutionUnit::AwaiterType::READ_SRC_READ_LOCK_DST:
      if ( !mComputeUnit )
      {
        auto [src, dst] = mReadUnit.readSrcReadDst();
        if ( !mRegLocks[src] && !mRegLocks[dst] )
        {
          if ( writeSlot < 0 || writeSlot == src || writeSlot == dst )
          {
            mRegLocks[dst] = true;
            mReadUnit.readSrcReadDst( { mRegs[src], mRegs[dst] } );
            mComputeUnit = std::move( mReadUnit );
          }
        }
      }
      break;
    case ExecutionUnit::AwaiterType::READ_SRC_READ_DST_LOCK_FLAGS:
      if ( !mComputeUnit )
      {
        auto [src, dst] = mReadUnit.readSrcReadDst();
        if ( !mRegLocks[src] && !mRegLocks[dst] )
        {
          if ( writeSlot < 0 || writeSlot == src || writeSlot == dst )
          {
            mFlagsSemaphore += 1;
            mReadUnit.readSrcReadDst( { mRegs[src], mRegs[dst] } );
            mComputeUnit = std::move( mReadUnit );
          }
        }
      }
      break;
    case ExecutionUnit::AwaiterType::READ_SRC_READ_LOCK_DST_LOCK_FLAGS:
      if ( !mComputeUnit )
      {
        auto [src, dst] = mReadUnit.readSrcReadDst();
        if ( !mRegLocks[src] && !mRegLocks[dst] )
        {
          if ( writeSlot < 0 || writeSlot == src || writeSlot == dst )
          {
            mRegLocks[dst] = true;
            mFlagsSemaphore += 1;
            mReadUnit.readSrcReadDst( { mRegs[src], mRegs[dst] } );
            mComputeUnit = std::move( mReadUnit );
          }
        }
      }
      break;
    case ExecutionUnit::AwaiterType::READ_SRC_READ_LOCK_DST_READ_LOCK_FLAGS:
      if ( !mComputeUnit )
      {
        auto [src, dst] = mReadUnit.readSrcReadDst();
        if ( !mRegLocks[src] && !mRegLocks[dst] && mFlagsSemaphore == 0 )
        {
          if ( writeSlot < 0 || writeSlot == src || writeSlot == dst )
          {
            mRegLocks[dst] = true;
            mFlagsSemaphore = 1;
            mReadUnit.readSrcReadDst( { mRegs[src], mRegs[dst] } );
            mComputeUnit = std::move( mReadUnit );
          }
        }
      }
      break;
    case ExecutionUnit::AwaiterType::GET_CODE_LO:
      if ( !localMemoryAccessed )
      {
        int code = fetchOperand();
        if ( code >= 0 )
        {
          mReadUnit.getCodeLo( code );
        }
      }
      break;
    case ExecutionUnit::AwaiterType::GET_CODE_HI:
      if ( !mWriteUnit && !localMemoryAccessed )
      {
        int code = fetchOperand();
        if ( code >= 0 )
        {
          mReadUnit.getCodeHi( code );
          mWriteUnit = std::move( mReadUnit );
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
    case ExecutionUnit::AwaiterType::GET_ACC:
      if ( !mWriteUnit )
      {
        mReadUnit.getAcc( (int8_t)( mAccumulator >> 32 ) );
        mWriteUnit = std::move( mReadUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::MOVE_Q:
      if ( !mWriteUnit )
      {
        mReadUnit.advance();
        mWriteUnit = std::move( mReadUnit );
      }
      break;
    case ExecutionUnit::AwaiterType::NOP:
      mReadUnit.advance();
      assert( mReadUnit.getType() == ExecutionUnit::AwaiterType::DECODE );
      mExecutionUnitPool[++mPoolTop] = std::move( mReadUnit );
      break;
    case ExecutionUnit::AwaiterType::JR_INIT:
      if ( !mComputeUnit )
      {
        mReadUnit.advance();
        mComputeUnit = std::move( mReadUnit );
      }
      break;
    default:
      throw Ex{} << "Unexpected read unit type " << ( int )mReadUnit.getType();
    }
  }

  if ( !mReadUnit && !localMemoryAccessed )
  {
    RISCOpcode opcode;
    if ( fetchOpcode( opcode ) )
    {
      opcode.srcReg = mRegisterFile + opcode.srcValue;
      opcode.dstReg = mRegisterFile + opcode.dstValue;
      mReadUnit = std::move( mExecutionUnitPool[mPoolTop--] );
      mReadUnit.decode( opcode );
    }
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

bool DSP::fetch()
{
  if ( !mPrefetcher )
  {
    if ( auto pull = prefetchPull() )
    {
      mPrefetcher.put( pull.data() );
    }
    else
    {
      return false;
    }
  }
  assert( mPrefetcher );
  return true;
}

bool DSP::fetchOpcode( RISCOpcode& out )
{
  if ( fetch() )
  {
    out = mPrefetcher.get();
    return true;
  }
  else
  {
    return false;
  }
}

int DSP::fetchOperand()
{
  if ( fetch() )
  {
    return mPrefetcher.getCode();
  }
  else
  {
    return -1;
  }
}
