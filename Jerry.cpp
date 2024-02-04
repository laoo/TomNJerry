#include "Jerry.hpp"
#include "PipelineLog.hpp"
#include "Ex.hpp"

Jerry::Jerry() : mLog{ std::make_unique<PipelineLog>() }
{
  std::ranges::fill( mRegStatus, FREE );
  std::ranges::fill( mRegs, 0 );
  std::ranges::fill( mLocalRAM, std::byteswap( (uint32_t)( (uint16_t)DSPI::NOP << 10 | ( uint16_t )DSPI::NOP << ( 10 + 16 ) ) ) );
}

Jerry::~Jerry()
{
}

void Jerry::debugWrite( uint32_t address, uint32_t data )
{
  switch ( address )
  {
  case D_FLAGS:
    throw Ex{ "Jerry::debugWrite: Unhandled address " } << std::hex << address;
  case D_MTXC:
    throw Ex{ "Jerry::debugWrite: Unhandled address " } << std::hex << address;
  case D_MTXA:
    throw Ex{ "Jerry::debugWrite: Unhandled address " } << std::hex << address;
  case D_END:
    throw Ex{ "Jerry::debugWrite: Unhandled address " } << std::hex << address;
  case D_PC:
    if ( data < RAM_BASE || data >= RAM_BASE + RAM_SIZE )
      throw Ex{ "Jerry unsupported PC " } << std::hex << data;
    mPC = data;
    break;
  case D_CTRL:
    mCtrl.value = data;
    //handle other bits
    break;
  case D_MOD:
    throw Ex{ "Jerry::debugWrite: Unhandled address " } << std::hex << address;
  case D_DIVCTRL:
    throw Ex{ "Jerry::debugWrite: Unhandled address " } << std::hex << address;
  case D_MACHI:
  default:
    throw Ex{ "Jerry::debugWrite: Unhandled address " } << std::hex << address;
    break;
  }
}

void Jerry::debugWrite( uint32_t address, std::span<uint32_t const> data )
{
  if ( address & 3 )
    throw Ex{ "Jerry::debugWrite: Unaligned address " } << std::hex << address;

  if ( address < RAM_BASE || address + data.size() * sizeof( uint32_t ) >= RAM_BASE + RAM_SIZE )
    throw Ex{ "Jerry::debugWrite: Unhandled address " } << std::hex << address;

  std::copy( data.begin(), data.end(), mLocalRAM.begin() + ( address - RAM_BASE ) / sizeof( uint32_t ) );
}

AdvanceResult Jerry::advance()
{
  io();
  stageWrite();
  compute();
  stageRead();
  decode();
  prefetch();

  io();
  stageWrite();
  compute();
  stageRead();
  decode();
  prefetch();

  return mResult;
}

AdvanceResult Jerry::busCycleIdle()
{
  if ( ( mCtrl.value & CTRL::DSPGO ) == 0 )
    return AdvanceResult::nop();

  return advance();
}

AdvanceResult Jerry::busCycleWrite()
{
  if ( !mCtrl.go() )
    return AdvanceResult::nop();

  return advance();
}

AdvanceResult Jerry::busCycleRead( uint8_t value )
{
  if ( !mCtrl.go() )
    return AdvanceResult::nop();

  return advance();
}

AdvanceResult Jerry::busCycleRead( uint16_t value )
{
  if ( !mCtrl.go() )
    return AdvanceResult::nop();

  return advance();
}

AdvanceResult Jerry::busCycleRead( uint32_t value )
{
  if ( !mCtrl.go() )
    return AdvanceResult::nop();

  return advance();
}

AdvanceResult Jerry::busCycleRead( uint64_t value )
{
  if ( !mCtrl.go() )
    return AdvanceResult::nop();

  return advance();
}

void Jerry::writeByte( uint32_t address, uint8_t data )
{
  if ( address > JERRY_BASE || address < JERRY_BASE + JERRY_SIZE )
    throw Ex{ "Jerry::writeByte: Unhandled address " } << std::hex << address;

  mResult = AdvanceResult::writeByte( address, data );
}

void Jerry::writeWord( uint32_t address, uint16_t data )
{
  if ( address > JERRY_BASE || address < JERRY_BASE + JERRY_SIZE )
    throw Ex{ "Jerry::writeWord: Unhandled address " } << std::hex << address;

  mResult = AdvanceResult::writeShort( address, data );
}

void Jerry::writeLong( uint32_t address, uint32_t data )
{
  if ( address > JERRY_BASE || address < JERRY_BASE + JERRY_SIZE )
  {
    switch ( address )
    {
    case D_FLAGS:
      throw Ex{ "Jerry::writeLong: Unhandled address " } << std::hex << address;
      break;
    case D_MTXC:
      throw Ex{ "Jerry::writeLong: Unhandled address " } << std::hex << address;
      break;
    case D_MTXA:
      throw Ex{ "Jerry::writeLong: Unhandled address " } << std::hex << address;
      break;
    case D_END:
      throw Ex{ "Jerry::writeLong: Unhandled address " } << std::hex << address;
      break;
    case D_PC:
      throw Ex{ "Jerry::writeLong: Unhandled address " } << std::hex << address;
    case D_CTRL:
      throw Ex{ "Jerry::writeLong: Unhandled address " } << std::hex << address;
      mCtrl.value = data;
      break;
    case D_MOD:
      throw Ex{ "Jerry::writeLong: Unhandled address " } << std::hex << address;
      mMod = data;
      break;
    case D_DIVCTRL:
      throw Ex{ "Jerry::writeLong: Unhandled address " } << std::hex << address;
      mDivCtrl = data;
      break;
    case D_MACHI:
      throw Ex{ "Jerry::writeLong: Unhandled address " } << std::hex << address;
      mMachi = data;
      break;
    default:
      throw Ex{ "Jerry::writeLong: Unhandled address " } << std::hex << address;
    }
  }
  else
  {
    mResult = AdvanceResult::writeLong( address, data );
  }
}

void Jerry::readByte( uint32_t address )
{
  if ( address > JERRY_BASE || address < JERRY_BASE + JERRY_SIZE )
    throw Ex{ "Jerry::readByte: Unhandled address " } << std::hex << address;

  mResult = AdvanceResult::readByte( address );
}

void Jerry::readWord( uint32_t address )
{
  if ( address > JERRY_BASE || address < JERRY_BASE + JERRY_SIZE )
    throw Ex{ "Jerry::readWord: Unhandled address " } << std::hex << address;

  mResult = AdvanceResult::readShort( address );
}

void Jerry::readLong( uint32_t address )
{
  if ( address > JERRY_BASE || address < JERRY_BASE + JERRY_SIZE )
  {
    switch ( address )
    {
    case D_FLAGS:
      throw Ex{ "Jerry::readLong: Unhandled address " } << std::hex << address;
      break;
    case D_MTXC:
      throw Ex{ "Jerry::readLong: Unhandled address " } << std::hex << address;
      break;
    case D_MTXA:
      throw Ex{ "Jerry::readLong: Unhandled address " } << std::hex << address;
      break;
    case D_END:
      throw Ex{ "Jerry::readLong: Unhandled address " } << std::hex << address;
      break;
    case D_PC:
      throw Ex{ "Jerry::readLong: Unhandled address " } << std::hex << address;
    case D_CTRL:
      throw Ex{ "Jerry::readLong: Unhandled address " } << std::hex << address;
      break;
    case D_MOD:
      throw Ex{ "Jerry::readLong: Unhandled address " } << std::hex << address;
      break;
    case D_DIVCTRL:
      throw Ex{ "Jerry::readLong: Unhandled address " } << std::hex << address;
      break;
    case D_MACHI:
      throw Ex{ "Jerry::readLong: Unhandled address " } << std::hex << address;
      break;
    default:
      throw Ex{ "Jerry::readLong: Unhandled address " } << std::hex << address;
    }
  }
  else
  {
    mResult = AdvanceResult::readLong( address );
  }
}

void Jerry::io()
{
}

void Jerry::stageWrite()
{
  if ( mStageWrite.reg >= 0 )
  {
    if ( dualPortWrite( mStageWrite.reg, mStageWrite.data ) )
    {
      mStageWrite.reg = -1;
    }
  }

  if ( mStageWrite.updateFlags )
  {
    mFlags.value |= mStageWrite.z >= 0 ? mStageWrite.z : ( mFlags.value & FLAGS::ZERO_FLAG );
    mFlags.value |= mStageWrite.c >= 0 ? mStageWrite.c : ( mFlags.value & FLAGS::CARRY_FLAG );
    mFlags.value |= mStageWrite.n >= 0 ? mStageWrite.n : ( mFlags.value & FLAGS::NEGA_FLAG );
    mStageWrite.z = mStageWrite.c = mStageWrite.n = -1;
    mStageWrite.updateFlags = false;
    mFlagsSemaphore -= 1;
    assert( mFlagsSemaphore >= 0 );
  }
}

void Jerry::compute()
{
  switch ( mStageCompute.instruction )
  {
  case DSPI::EMPTY:
  case DSPI::MOVE:
  case DSPI::MOVEQ:
  case DSPI::MOVEFA:
  case DSPI::MOVETA:
  case DSPI::NOP:
    break;
  case DSPI::ADD:
  case DSPI::ADDQ:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = mStageCompute.data1 + mStageCompute.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.c = mStageWrite.data < mStageCompute.data1 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::ADDC:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = mStageCompute.data1 + mStageCompute.data2 + ( mFlags.value & ( FLAGS::CARRY_FLAG >> 1 ) );
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.c = mStageWrite.data < mStageCompute.data1 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::ADDQT:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = mStageCompute.data1 + mStageCompute.data2;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeReg( mStageWrite.reg );
    }
    break;
  case DSPI::SUB:
  case DSPI::SUBQ:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = mStageCompute.data2 - mStageCompute.data1;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.c = mStageCompute.data1 < mStageCompute.data2 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::SUBC:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      uint64_t res = (uint64_t)mStageCompute.data2 + (uint64_t)( mStageCompute.data1 ^ 0xffffffff ) + (uint64_t)( ( mFlags.value & ( FLAGS::CARRY_FLAG >> 1 ) ) ^ 1 );
      mStageWrite.data = (uint32_t)res;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.c = ( ( res >> 32 ) & 1 ) ? 0 : 1;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::SUBQT:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = mStageCompute.data2 - mStageCompute.data1;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeReg( mStageWrite.reg );
    }
    break;
  case DSPI::NEG:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = -mStageCompute.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.c = mStageCompute.data2 != 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::AND:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = mStageCompute.data1 & mStageCompute.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::OR:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = mStageCompute.data1 | mStageCompute.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::XOR:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = mStageCompute.data1 ^ mStageCompute.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::NOT:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = ~mStageCompute.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::BTST:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.z = ( mStageCompute.data2 & ( 1 << mStageCompute.data1 ) ) == 0 ? 1 : 0;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeFlags();
    }
    break;
  case DSPI::BSET:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = mStageCompute.data2 | ( 1 << mStageCompute.data1 );
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::BCLR:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = mStageCompute.data2 & ~( 1 << mStageCompute.data1 );
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::MULT:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = (uint16_t)mStageCompute.data1 * (uint16_t)mStageCompute.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::IMULT:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = (int16_t)mStageCompute.data1 * (int16_t)mStageCompute.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::IMULTN:
    mMulatiplyAccumulator = (int16_t)mStageCompute.data1 * (int16_t)mStageCompute.data2;
    mStageWrite.z = mMulatiplyAccumulator == 0 ? 1 : 0;
    mStageWrite.n = mStageWrite.data >> 31;
    mStageWrite.updateFlags = true;
    mStageCompute.instruction = DSPI::EMPTY;
    mLog->computeFlags();
    break;
  case DSPI::RESMAC:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = mMulatiplyAccumulator;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeReg( mStageWrite.reg );
    }
    break;
  case DSPI::IMACN:
    mMulatiplyAccumulator += ( int16_t )mStageCompute.data1 * ( int16_t )mStageCompute.data2;
    mStageWrite.z = mMulatiplyAccumulator == 0 ? 1 : 0;
    mStageWrite.n = mStageWrite.data >> 31;
    mStageWrite.updateFlags = true;
    mStageCompute.instruction = DSPI::EMPTY;
    mLog->computeFlags();
    break;
  case DSPI::DIV:
    throw Ex{ "NYI" };
  case DSPI::ABS:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = mStageCompute.data2 < 0 ? -mStageRead.data2 : mStageCompute.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = 0;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::SH:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = mStageCompute.data1 > 0 ? mStageCompute.data2 >> mStageCompute.data1 : mStageCompute.data2 << mStageCompute.data1;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.c = mStageCompute.data1 > 0 ? ( mStageCompute.data2 & 1 ) : ( mStageCompute.data2 >> 31 );
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::SHLQ:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = mStageCompute.data2 << ( 32 - mStageCompute.data1 );
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.c = mStageCompute.data2 >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::SHRQ:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = mStageCompute.data2 >> mStageCompute.data1;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.c = mStageCompute.data2 & 1;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::SHA:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.reg2;
      mStageWrite.data = mStageCompute.data1 > 0 ? (uint32_t)( (int64_t)( mStageCompute.data2 ) >> mStageCompute.data1 ) : mStageCompute.data2 << mStageCompute.data1;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.c = mStageCompute.data1 > 0 ? ( mStageCompute.data2 & 1 ) : ( mStageCompute.data2 >> 31 );
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::SHARQ:
    throw Ex{ "NYI" };
  case DSPI::ROR:
    throw Ex{ "NYI" };
  case DSPI::RORQ:
    throw Ex{ "NYI" };
  case DSPI::CMP:
    throw Ex{ "NYI" };
  case DSPI::CMPQ:
    throw Ex{ "NYI" };
  case DSPI::SUBQMOD:
    throw Ex{ "NYI" };
  case DSPI::SAT16S:
    throw Ex{ "NYI" };
  case DSPI::MOVEI:
    throw Ex{ "NYI" };
  case DSPI::LOADB:
    throw Ex{ "NYI" };
  case DSPI::LOADW:
    throw Ex{ "NYI" };
  case DSPI::LOAD:
    throw Ex{ "NYI" };
  case DSPI::SAT32S:
    throw Ex{ "NYI" };
  case DSPI::LOAD14N:
    throw Ex{ "NYI" };
  case DSPI::LOAD15N:
    throw Ex{ "NYI" };
  case DSPI::STOREB:
    throw Ex{ "NYI" };
  case DSPI::STOREW:
    throw Ex{ "NYI" };
  case DSPI::STORE:
    throw Ex{ "NYI" };
  case DSPI::MIRROR:
    throw Ex{ "NYI" };
  case DSPI::STORE14N:
    throw Ex{ "NYI" };
  case DSPI::STORE15N:
    throw Ex{ "NYI" };
  case DSPI::MOVEPC:
    throw Ex{ "NYI" };
  case DSPI::JUMP:
    throw Ex{ "NYI" };
  case DSPI::JR:
    throw Ex{ "NYI" };
  case DSPI::MMULT:
    throw Ex{ "NYI" };
  case DSPI::MTOI:
    throw Ex{ "NYI" };
  case DSPI::NORMI:
    throw Ex{ "NYI" };
  case DSPI::LOAD14R:
    throw Ex{ "NYI" };
  case DSPI::LOAD15R:
    throw Ex{ "NYI" };
  case DSPI::STORE14R:
    throw Ex{ "NYI" };
  case DSPI::STORE15R:
    throw Ex{ "NYI" };
  case DSPI::ADDQMOD:
    throw Ex{ "NYI" };
    break;
  }
}

void Jerry::stageRead()
{
  static constexpr std::array<uint32_t, 32> tabAddSubQ = { 32, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };

  if ( mStageCompute.instruction != DSPI::EMPTY )
    return;

  switch ( mStageRead.instruction )
  {
  case DSPI::EMPTY:
    break;
  case DSPI::NOP:
    dualPortCommit();
    mStageRead.instruction = DSPI::EMPTY;
    break;
  case DSPI::ADD:
  case DSPI::SUB:
  case DSPI::AND:
  case DSPI::OR:
  case DSPI::XOR:
  case DSPI::MULT:
  case DSPI::IMULT:
  case DSPI::SH:
  case DSPI::SHA:
  case DSPI::ROR:
  case DSPI::MTOI:
    if ( dualPortRead( mStageRead.reg1, mStageRead.reg2 ) )
    {
      mFlagsSemaphore += 1;
      assert( mRegStatus[mStageRead.reg1] == FREE );
      assert( mRegStatus[mStageRead.reg2] == FREE );
      mRegStatus[mStageRead.reg2] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.reg2 = mStageRead.reg2;
      mStageCompute.data1 = mStageRead.data1;
      mStageCompute.data2 = mStageRead.data2;
    }
    break;
  case DSPI::ADDC:
  case DSPI::SUBC:
    if ( mFlagsSemaphore == 0 && dualPortRead( mStageRead.reg1, mStageRead.reg2 ) )
    {
      mFlagsSemaphore += 1;
      assert( mRegStatus[mStageRead.reg1] == FREE );
      assert( mRegStatus[mStageRead.reg2] == FREE );
      mRegStatus[mStageRead.reg2] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.reg2 = mStageRead.reg2;
      mStageCompute.data1 = mStageRead.data1;
      mStageCompute.data2 = mStageRead.data2;
    }
    break;
  case DSPI::ADDQ:
  case DSPI::SUBQ:
  case DSPI::SUBQMOD:
  case DSPI::ADDQMOD:
    if ( dualPortRead( mStageRead.reg2 ) )
    {
      mLog->portImm( mStageRead.reg1 );
      mFlagsSemaphore += 1;
      assert( mRegStatus[mStageRead.reg1] == FREE );
      assert( mRegStatus[mStageRead.reg2] == FREE );
      mRegStatus[mStageRead.reg2] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.reg2 = mStageRead.reg2;
      mStageCompute.data1 = tabAddSubQ[ mStageRead.reg1 ];
      mStageCompute.data2 = mStageRead.data2;
    }
    break;
  case DSPI::BSET:
  case DSPI::BCLR:
  case DSPI::SHRQ:
  case DSPI::SHARQ:
  case DSPI::RORQ:
    if ( dualPortRead( mStageRead.reg2 ) )
    {
      mLog->portImm( mStageRead.reg1 );
      mFlagsSemaphore += 1;
      assert( mRegStatus[mStageRead.reg1] == FREE );
      assert( mRegStatus[mStageRead.reg2] == FREE );
      mRegStatus[mStageRead.reg2] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.reg2 = mStageRead.reg2;
      mStageCompute.data1 = mStageRead.reg1;
      mStageCompute.data2 = mStageRead.data2;
    }
    break;
  case DSPI::SHLQ:
    if ( dualPortRead( mStageRead.reg2 ) )
    {
      mLog->portImm( 32 - mStageRead.reg1 );
      mFlagsSemaphore += 1;
      assert( mRegStatus[mStageRead.reg1] == FREE );
      assert( mRegStatus[mStageRead.reg2] == FREE );
      mRegStatus[mStageRead.reg2] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.reg2 = mStageRead.reg2;
      mStageCompute.data1 = mStageRead.reg1;
      mStageCompute.data2 = mStageRead.data2;
    }
    break;
  case DSPI::NEG:
  case DSPI::NOT:
  case DSPI::ABS:
  case DSPI::SAT16S:
  case DSPI::SAT32S:
  case DSPI::MIRROR:
  case DSPI::NORMI:
    if ( dualPortRead( mStageRead.reg2 ) )
    {
      mFlagsSemaphore += 1;
      assert( mRegStatus[mStageRead.reg1] == FREE );
      assert( mRegStatus[mStageRead.reg2] == FREE );
      mRegStatus[mStageRead.reg2] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.reg2 = mStageRead.reg2;
      mStageCompute.data2 = mStageRead.data2;
    }
    break;
  case DSPI::ADDQT:
  case DSPI::SUBQT:
    if ( dualPortRead( mStageRead.reg2 ) )
    {
      mLog->portImm( mStageRead.reg1 );
      assert( mRegStatus[mStageRead.reg1] == FREE );
      assert( mRegStatus[mStageRead.reg2] == FREE );
      mRegStatus[mStageRead.reg2] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.reg2 = mStageRead.reg2;
      mStageCompute.data1 = tabAddSubQ[mStageRead.reg1];
      mStageCompute.data2 = mStageRead.data2;
    }
    break;
  case DSPI::BTST:
  case DSPI::CMPQ:
    if ( dualPortRead( mStageRead.reg2 ) )
    {
      mLog->portImm( mStageRead.reg1 );
      mFlagsSemaphore += 1;
      assert( mRegStatus[mStageRead.reg1] == FREE );
      assert( mRegStatus[mStageRead.reg2] == FREE );
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.data1 = mStageRead.reg1;
      mStageCompute.data2 = mStageRead.data2;
    }
    break;
  case DSPI::IMULTN:
  case DSPI::IMACN:
  case DSPI::CMP:
    if ( dualPortRead( mStageRead.reg1, mStageRead.reg2 ) )
    {
      mFlagsSemaphore += 1;
      assert( mRegStatus[mStageRead.reg1] == FREE );
      assert( mRegStatus[mStageRead.reg2] == FREE );
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.data1 = mStageRead.data1;
      mStageCompute.data2 = mStageRead.data2;
    }
    break;
  case DSPI::RESMAC:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mMulatiplyAccumulator;
      mRegStatus[mStageRead.reg2] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
    }
    break;
  case DSPI::MOVEQ:
    if ( mStageWrite.reg < 0 )
    {
      dualPortCommit();
      mLog->portImm( mStageRead.reg1 );
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.reg1;
      mRegStatus[mStageRead.reg2] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
    }
    break;
  case DSPI::MOVEFA:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mRegs[ mAnotherRegisterFile + mStageRead.reg1 ];
      mRegStatus[mStageRead.reg2] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
    }
    break;
  case DSPI::DIV:
    if ( dualPortRead( mStageRead.reg1, mStageRead.reg2 ) )
    {
      assert( mRegStatus[mStageRead.reg1] == FREE );
      assert( mRegStatus[mStageRead.reg2] == FREE );
      mRegStatus[mStageRead.reg2] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.reg2 = mStageRead.reg2;
      mStageCompute.data1 = mStageRead.data1;
      mStageCompute.data2 = mStageRead.data2;
    }
    break;
  case DSPI::MOVE:
    if ( mStageWrite.reg < 0 && dualPortRead( mStageRead.reg1 ) )
    {
      assert( mRegStatus[mStageRead.reg1] == FREE );
      // mRegStatus[mStageRead.reg2] != FREE is an error;
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.data1;
      mRegStatus[mStageRead.reg2] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
    }
    break;
  case DSPI::MOVETA:
    if ( dualPortRead( mStageRead.reg1 ) )
    {
      assert( mRegStatus[mStageRead.reg1] == FREE );
      mRegs[mAnotherRegisterFile + mStageRead.reg2] = mStageRead.data1;
      mStageRead.instruction = DSPI::EMPTY;
    }
    break;
  case DSPI::MOVEI:
    throw Ex{ "MOVEI NYI" };
    break;
  case DSPI::LOADB:
  case DSPI::LOADW:
  case DSPI::LOAD:
    throw Ex{ "LOAD NYI" };
    break;
  case DSPI::LOAD14R:
  case DSPI::LOAD15R:
    throw Ex{ "LOAD NYI" };
    break;
  case DSPI::LOAD14N:
  case DSPI::LOAD15N:
    throw Ex{ "LOAD NYI" };
    break;
  case DSPI::STOREB:
  case DSPI::STOREW:
  case DSPI::STORE:
    throw Ex{ "STORE NYI" };
    break;
  case DSPI::STORE14N:
  case DSPI::STORE15N:
    throw Ex{ "STORE NYI" };
    break;
  case DSPI::STORE14R:
  case DSPI::STORE15R:
    throw Ex{ "STORE NYI" };
    break;
  case DSPI::MOVEPC:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mPC;
      mRegStatus[mStageRead.reg2] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
    }
    break;
  case DSPI::JUMP:
  case DSPI::JR:
    if ( mFlagsSemaphore == 0 && dualPortRead( mStageRead.reg2 ) )
    {
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.data1 = mStageRead.data1;
      mStageCompute.data2 = mStageRead.data2;
    }
    break;
  case DSPI::MMULT:
    throw Ex{ "MMULT NYI" };
    break;
  }
}

void Jerry::decode()
{
  if ( mStageRead.instruction != DSPI::EMPTY )
    return;

  int32_t pull = mPrefetch.pull();

  if ( pull < 0 )
    return;

  mStageRead.instruction = ( DSPI )( pull >> 10 );
  mStageRead.reg1 = ( pull >> 5 ) & 0x1f;
  mStageRead.reg2 = pull & 0x1f;

  mLog->decodeDSP( mStageRead.instruction, mStageRead.reg1, mStageRead.reg2 );
}

void Jerry::prefetch()
{
  //Only local RAM for now
  assert( mPC >= RAM_BASE && mPC < RAM_BASE + RAM_SIZE );

  if ( mLastLocalRAMAccessCycle != mCycle )
  {
    auto code = std::byteswap( mLocalRAM[(mPC - RAM_BASE)>>2] );

    if ( mPrefetch.push( code ) )
    {
      mLog->prefetch( mPC, code );
      mLastLocalRAMAccessCycle = mCycle;
      mPC += 4;
    }
  }

  mCycle += 1;
  mLog->flush();
}

bool Jerry::dualPortWrite( uint32_t reg, uint32_t data )
{
  if ( mDualPort.port1.status == DualPort::EMPTY )
  {
    mDualPort.port1.status = DualPort::WRITE;
    mDualPort.port1.reg = reg;
    mDualPort.port1.data = data;
    mLog->port1Write( reg, data );
    return true;
  }
  else
  {
    return false;
  }
}

bool Jerry::dualPortRead( uint32_t reg1, uint32_t reg2 )
{
  if ( mDualPort.port2.status == DualPort::EMPTY )
  {
    if ( mDualPort.port1.status == DualPort::EMPTY )
    {
      mDualPort.port1.status = DualPort::READ;
      mDualPort.port1.reg = reg1;
      mDualPort.port2.status = DualPort::READ;
      mDualPort.port2.reg = reg2;
      dualPortCommit();
      return true;
    }
    else if ( mDualPort.port1.status == DualPort::WRITE )
    {
      if ( mDualPort.port1.reg == reg1 )
      {
        mDualPort.port2.status = DualPort::READ;
        mDualPort.port2.reg = reg2;
        dualPortCommit( true );
        return true;
      }
      else if ( mDualPort.port1.reg == reg2 )
      {
        mDualPort.port2.status = DualPort::READ;
        mDualPort.port2.reg = reg1;
        dualPortCommit( true );
        return true;
      }
    }
  }

  dualPortCommit();
  return false;
}

bool Jerry::dualPortRead( uint32_t reg )
{
  if ( mRegStatus[reg] == FREE )
  {
    assert( mDualPort.port2.status == DualPort::EMPTY );
    mDualPort.port2.status = DualPort::READ;
    mDualPort.port2.reg = reg;
    dualPortCommit();
    return true;
  }
  else
  {
    dualPortCommit();
    if ( mRegStatus[reg] == FREE )
    {
      assert( mDualPort.port2.status == DualPort::EMPTY );
      mDualPort.port2.status = DualPort::READ;
      mDualPort.port2.reg = reg;
      dualPortCommit();
      return true;
    }
  }

  return false;
}

void Jerry::dualPortCommit( bool writeRead )
{
  if ( mDualPort.port1.status == DualPort::WRITE )
  {
    mRegStatus[mDualPort.port1.reg] = FREE;
    mRegs[mRegisterFile + mDualPort.port1.reg] = mDualPort.port1.data;
    mDualPort.port1.status = DualPort::EMPTY;
    if ( writeRead )
    {
      mLog->port1Read( mDualPort.port1.reg, mDualPort.port1.data );
    }
  }
  else if ( mDualPort.port1.status == DualPort::READ )
  {
    assert( mRegStatus[mDualPort.port1.reg] == FREE );
    assert( mStageRead.reg1 == mDualPort.port1.reg );
    mStageRead.data1 = mRegs[mRegisterFile + mDualPort.port1.reg];
    mDualPort.port1.status = DualPort::EMPTY;
    mLog->port1Read( mDualPort.port1.reg, mStageRead.data1 );
  }
  
  if ( mDualPort.port2.status == DualPort::READ )
  {
    assert( mRegStatus[mDualPort.port2.reg] == FREE );
    mStageRead.data2 = mRegs[mRegisterFile + mDualPort.port2.reg];
    mDualPort.port2.status = DualPort::EMPTY;
    mLog->port2Read( mDualPort.port2.reg, mStageRead.data2 );
  }
  assert( mDualPort.port2.status == DualPort::EMPTY );
}

int32_t Jerry::Prefetch::pull()
{
  if ( queueSize > 0 )
  {
    int32_t result = queue & 0xffff;
    queue >>= 16;
    queueSize -= 1;
    return result;
  }
  else
  {
    return -1;
  }
}

bool Jerry::Prefetch::push( uint32_t value )
{
  if ( queueSize <= 2 )
  {
    queue &= ~0ull >> ( 64 - queueSize * 16 );
    queue |= (uint64_t)( ( value >> 16 ) | ( value << 16 ) ) << ( queueSize * 16 );
    queueSize += 2;
    return true;
  }
  else
  {
    return false;
  }
}
