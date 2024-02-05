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
  dualPortCommit();
  decode();
  prefetch();

  io();
  stageWrite();
  compute();
  stageRead();
  dualPortCommit();
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
    if ( portWriteDst( mStageWrite.reg, mStageWrite.data ) )
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
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataSrc + mStageCompute.dataDst;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.c = mStageWrite.data < mStageCompute.dataSrc ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::ADDC:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataSrc + mStageCompute.dataDst + ( mFlags.value & ( FLAGS::CARRY_FLAG >> 1 ) );
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.c = mStageWrite.data < mStageCompute.dataSrc ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::ADDQT:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataSrc + mStageCompute.dataDst;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeReg( mStageWrite.reg );
    }
    break;
  case DSPI::SUB:
  case DSPI::SUBQ:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataDst - mStageCompute.dataSrc;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.c = mStageCompute.dataSrc < mStageCompute.dataDst ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::SUBC:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.regDst;
      uint64_t res = (uint64_t)mStageCompute.dataDst + (uint64_t)( mStageCompute.dataSrc ^ 0xffffffff ) + (uint64_t)( ( mFlags.value & ( FLAGS::CARRY_FLAG >> 1 ) ) ^ 1 );
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
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataDst - mStageCompute.dataSrc;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeReg( mStageWrite.reg );
    }
    break;
  case DSPI::NEG:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = -mStageCompute.dataDst;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.c = mStageCompute.dataDst != 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::AND:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataSrc & mStageCompute.dataDst;
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
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataSrc | mStageCompute.dataDst;
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
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataSrc ^ mStageCompute.dataDst;
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
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = ~mStageCompute.dataDst;
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
      mStageWrite.z = ( mStageCompute.dataDst & ( 1 << mStageCompute.dataSrc ) ) == 0 ? 1 : 0;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeFlags();
    }
    break;
  case DSPI::BSET:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataDst | ( 1 << mStageCompute.dataSrc );
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
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataDst & ~( 1 << mStageCompute.dataSrc );
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
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = (uint16_t)mStageCompute.dataSrc * (uint16_t)mStageCompute.dataDst;
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
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = (int16_t)mStageCompute.dataSrc * (int16_t)mStageCompute.dataDst;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::IMULTN:
    mMulatiplyAccumulator = (int16_t)mStageCompute.dataSrc * (int16_t)mStageCompute.dataDst;
    mStageWrite.z = mMulatiplyAccumulator == 0 ? 1 : 0;
    mStageWrite.n = mStageWrite.data >> 31;
    mStageWrite.updateFlags = true;
    mStageCompute.instruction = DSPI::EMPTY;
    mLog->computeFlags();
    break;
  case DSPI::RESMAC:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = mMulatiplyAccumulator;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeReg( mStageWrite.reg );
    }
    break;
  case DSPI::IMACN:
    mMulatiplyAccumulator += ( int16_t )mStageCompute.dataSrc * ( int16_t )mStageCompute.dataDst;
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
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataDst < 0 ? -mStageCompute.dataDst : mStageCompute.dataDst;
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
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataSrc > 0 ? mStageCompute.dataDst >> mStageCompute.dataSrc : mStageCompute.dataDst << mStageCompute.dataSrc;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.c = mStageCompute.dataSrc > 0 ? ( mStageCompute.dataDst & 1 ) : ( mStageCompute.dataDst >> 31 );
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::SHLQ:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataDst << ( 32 - mStageCompute.dataSrc );
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.c = mStageCompute.dataDst >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::SHRQ:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataDst >> mStageCompute.dataSrc;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.c = mStageCompute.dataDst & 1;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.reg );
    }
    break;
  case DSPI::SHA:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataSrc > 0 ? (uint32_t)( (int64_t)( mStageCompute.dataDst ) >> mStageCompute.dataSrc ) : mStageCompute.dataDst << mStageCompute.dataSrc;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data >> 31;
      mStageWrite.c = mStageCompute.dataSrc > 0 ? ( mStageCompute.dataDst & 1 ) : ( mStageCompute.dataDst >> 31 );
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
    if ( portReadBoth( mStageRead.regSrc, mStageRead.regDst ) )
    {
      mFlagsSemaphore += 1;
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataSrc = mStageRead.dataSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    break;
  case DSPI::ADDC:
  case DSPI::SUBC:
    if ( mFlagsSemaphore == 0 && portReadBoth( mStageRead.regSrc, mStageRead.regDst ) )
    {
      mFlagsSemaphore += 1;
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataSrc = mStageRead.dataSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    break;
  case DSPI::ADDQ:
  case DSPI::SUBQ:
  case DSPI::SUBQMOD:
  case DSPI::ADDQMOD:
    if ( portReadDst( mStageRead.regDst ) )
    {
      mLog->portImm( mStageRead.regSrc );
      mFlagsSemaphore += 1;
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataSrc = tabAddSubQ[ mStageRead.regSrc];
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    break;
  case DSPI::BSET:
  case DSPI::BCLR:
  case DSPI::SHRQ:
  case DSPI::SHARQ:
  case DSPI::RORQ:
    if ( portReadDst( mStageRead.regDst ) )
    {
      mLog->portImm( mStageRead.regSrc );
      mFlagsSemaphore += 1;
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataSrc = mStageRead.regSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    break;
  case DSPI::SHLQ:
    if ( portReadDst( mStageRead.regDst ) )
    {
      mLog->portImm( 32 - mStageRead.regSrc );
      mFlagsSemaphore += 1;
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataSrc = mStageRead.regSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    break;
  case DSPI::NEG:
  case DSPI::NOT:
  case DSPI::ABS:
  case DSPI::SAT16S:
  case DSPI::SAT32S:
  case DSPI::MIRROR:
  case DSPI::NORMI:
    if ( portReadDst( mStageRead.regDst ) )
    {
      mFlagsSemaphore += 1;
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    break;
  case DSPI::ADDQT:
  case DSPI::SUBQT:
    if ( portReadDst( mStageRead.regDst ) )
    {
      mLog->portImm( mStageRead.regSrc );
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataSrc = tabAddSubQ[mStageRead.regSrc];
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    break;
  case DSPI::BTST:
  case DSPI::CMPQ:
    if ( portReadDst( mStageRead.regDst ) )
    {
      mLog->portImm( mStageRead.regSrc );
      mFlagsSemaphore += 1;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.regSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    break;
  case DSPI::IMULTN:
  case DSPI::IMACN:
  case DSPI::CMP:
    if ( portReadBoth( mStageRead.regSrc, mStageRead.regDst ) )
    {
      mFlagsSemaphore += 1;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    break;
  case DSPI::RESMAC:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.regDst;
      mStageWrite.data = mMulatiplyAccumulator;
      mRegStatus[mStageRead.regDst] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
    }
    break;
  case DSPI::MOVEQ:
    if ( mStageWrite.reg < 0 )
    {
      mLog->portImm( mStageRead.regSrc );
      mStageWrite.reg = mStageRead.regDst;
      mStageWrite.data = mStageRead.regSrc;
      mRegStatus[mStageRead.regDst] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
    }
    break;
  case DSPI::MOVEFA:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.regDst;
      mStageWrite.data = mRegs[ mAnotherRegisterFile + mStageRead.regSrc];
      mRegStatus[mStageRead.regDst] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
    }
    break;
  case DSPI::DIV:
    if ( portReadBoth( mStageRead.regSrc, mStageRead.regDst ) )
    {
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataSrc = mStageRead.dataSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    break;
  case DSPI::MOVE:
    if ( mStageWrite.reg < 0 && portReadSrc( mStageRead.regSrc ) )
    {
      // mRegStatus[mStageRead.reg2] != FREE is an error;
      mStageWrite.reg = mStageRead.regDst;
      mStageWrite.data = mStageRead.dataSrc;
      mRegStatus[mStageRead.regDst] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
    }
    break;
  case DSPI::MOVETA:
    if ( portReadSrc( mStageRead.regSrc ) )
    {
      mRegs[mAnotherRegisterFile + mStageRead.regDst] = mStageRead.dataSrc;
      mStageRead.instruction = DSPI::EMPTY;
    }
    break;
  case DSPI::MOVEI:
    if ( mStageWrite.reg < 0 )
    {
      mRegStatus[mStageRead.regDst] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
      mStageRead.decodeStage = StageRead::MOVEI1;
    }
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
      mStageWrite.reg = mStageRead.regDst;
      mStageWrite.data = mPC;
      mRegStatus[mStageRead.regDst] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
    }
    break;
  case DSPI::JUMP:
  case DSPI::JR:
    if ( mFlagsSemaphore == 0 && portReadDst( mStageRead.regDst ) )
    {
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    break;
  case DSPI::MMULT:
    throw Ex{ "MMULT NYI" };
    break;
  }

}

void Jerry::decode()
{
  switch ( mStageRead.decodeStage )
  {
  case StageRead::NORMAL:
    if ( int32_t pull = mPrefetch.pull(); pull >= 0 )
    {
      mStageRead.instruction = ( DSPI )( pull >> 10 );
      mStageRead.regSrc = ( pull >> 5 ) & 0x1f;
      mStageRead.regDst = pull & 0x1f;

      mLog->decodeDSP( mStageRead.instruction, mStageRead.regSrc, mStageRead.regDst );
    }
    break;
  case StageRead::MOVEI1:
    if ( int32_t pull = mPrefetch.pull(); pull >= 0 )
    {
      mStageRead.dataSrc = ( uint16_t )pull;

      mLog->decodeMOVEI( 0, mStageRead.dataSrc );
      mStageRead.decodeStage = StageRead::MOVEI2;
    }
    break;
  case StageRead::MOVEI2:
    if ( int32_t pull = mPrefetch.pull(); pull >= 0 )
    {
      mStageRead.dataSrc |= ( uint32_t )pull << 16;

      mStageWrite.reg = mStageRead.regDst;
      mStageWrite.data = mStageRead.dataSrc;
      mLog->decodeMOVEI( 1, mStageRead.dataSrc );
      mStageRead.decodeStage = StageRead::NORMAL;
    }
    break;
  default:
    break;
  }
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

bool Jerry::portWriteDst( uint32_t reg, uint32_t data )
{
  if ( mPortWriteDstReg >= 0 )
    return false;

  mPortWriteDstReg = reg;
  mPortWriteDstData = data;
  return true;
}

bool Jerry::portReadSrc( uint32_t regSrc )
{
  assert( mPortReadSrcReg < 0 );
  assert( regSrc >= 0 );

  if ( mRegStatus[regSrc] != FREE && mPortWriteDstReg != regSrc )
    return false;

  mPortReadSrcReg = regSrc;

  return true;
}

bool Jerry::portReadDst( uint32_t regDst )
{
  assert( mPortReadDstReg < 0 );
  assert( regDst >= 0 );

  if ( mRegStatus[regDst] != FREE && mPortWriteDstReg != regDst )
    return false;

  mPortReadDstReg = regDst;

  return true;
}

bool Jerry::portReadBoth( uint32_t regSrc, uint32_t regDst )
{
  assert( mPortReadSrcReg < 0 );
  assert( mPortReadDstReg < 0 );
  assert( regSrc >= 0 );
  assert( regDst >= 0 );

  if ( mRegStatus[regSrc] != FREE && mPortWriteDstReg != regSrc )
    return false;

  if ( mRegStatus[regDst] != FREE && mPortWriteDstReg != regDst )
    return false;

  if ( mPortWriteDstReg >= 0 && mPortWriteDstReg != regSrc && mPortWriteDstReg != regDst )
    return false;

  mPortReadSrcReg = regSrc;
  mPortReadDstReg = regDst;

  return true;
}

void Jerry::dualPortCommit()
{
  if ( mPortWriteDstReg >= 0 )
  {
    mRegStatus[mPortWriteDstReg] = FREE;
    mRegs[mRegisterFile + mPortWriteDstReg] = mPortWriteDstData;
    mLog->portWriteDst( mPortWriteDstReg, mPortWriteDstData );
    mPortWriteDstReg = -1;
  }

  if ( mPortReadSrcReg >= 0 )
  {
    mStageRead.dataSrc = mRegs[mRegisterFile + mPortReadSrcReg];
    mLog->portReadSrc( mPortReadSrcReg, mStageRead.dataSrc );
    mPortReadSrcReg = -1;
  }

  if ( mPortReadDstReg >= 0 )
  {
    mStageRead.dataDst = mRegs[mRegisterFile + mPortReadDstReg];
    mLog->portReadDst( mPortReadDstReg, mStageRead.dataDst );
    mPortReadDstReg = -1;
  }
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
