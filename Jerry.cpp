#include "Jerry.hpp"
#include "Ex.hpp"

Jerry::Jerry()
{
  std::ranges::fill( mRegStatus, FREE );
  std::ranges::fill( mLocalRAM, (uint16_t)I::NOP << 10 | ( uint16_t )I::NOP << ( 10 + 16 ) );
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

  if ( mFlagsStatus != FREE )
  {
    mFlags.value |= mStageWrite.z >= 0 ? mStageWrite.z : ( mFlags.value & FLAGS::ZERO_FLAG );
    mFlags.value |= mStageWrite.c >= 0 ? mStageWrite.c : ( mFlags.value & FLAGS::CARRY_FLAG );
    mFlags.value |= mStageWrite.n >= 0 ? mStageWrite.n : ( mFlags.value & FLAGS::NEGA_FLAG );
    mStageWrite.z = mStageWrite.c = mStageWrite.n = -1;
    mFlagsStatus = FREE;
  }
}

void Jerry::compute()
{
  switch ( mStageCompute.instruction )
  {
  case I::EMPTY:
  case I::MOVE:
  case I::MOVEQ:
  case I::MOVEFA:
  case I::MOVETA:
  case I::NOP:
    break;
  case I::ADD:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.data1 + mStageRead.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.c = mStageWrite.data < mStageRead.data1 ? 1 : 0;
      mStageWrite.n = mStageWrite.data < 0 ? 1 : 0;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::ADDC:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.data1 + mStageRead.data2 + ( mFlags.value & ( FLAGS::CARRY_FLAG >> 1 ) );
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.c = mStageWrite.data < mStageRead.data1 ? 1 : 0;
      mStageWrite.n = mStageWrite.data < 0 ? 1 : 0;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::ADDQ:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.data1 + mStageRead.reg1;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.c = mStageWrite.data < mStageRead.data1 ? 1 : 0;
      mStageWrite.n = mStageWrite.data < 0 ? 1 : 0;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::ADDQT:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.data1 + mStageRead.reg1;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::SUB:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.data1 - mStageRead.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.c = mStageRead.data1 < mStageRead.data2 ? 1 : 0;
      mStageWrite.n = mStageWrite.data < 0 ? 1 : 0;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::SUBC:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.data1 - mStageRead.data2 + 1 - ( mFlags.value & ( FLAGS::CARRY_FLAG >> 1 ) );
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.c = mStageRead.data1 < mStageRead.data2 ? 1 : 0;
      mStageWrite.n = mStageWrite.data < 0 ? 1 : 0;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::SUBQ:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.data1 - mStageRead.reg1;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.c = mStageRead.data1 < mStageRead.reg1 ? 1 : 0;
      mStageWrite.n = mStageWrite.data < 0 ? 1 : 0;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::SUBQT:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.data1 - mStageRead.reg1;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::NEG:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = -mStageRead.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.c = mStageRead.data2 != 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data < 0 ? 1 : 0;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::AND:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.data1 & mStageRead.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data < 0 ? 1 : 0;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::OR:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.data1 | mStageRead.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data < 0 ? 1 : 0;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::XOR:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.data1 ^ mStageRead.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data < 0 ? 1 : 0;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::NOT:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = ~mStageRead.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data < 0 ? 1 : 0;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::BTST:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.z = ( mStageRead.data2 & ( 1 << mStageRead.reg1 ) ) == 0 ? 1 : 0;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::BSET:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.data2 | ( 1 << mStageRead.reg1 );
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data < 0 ? 1 : 0;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::BCLR:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.data2 & ~( 1 << mStageRead.reg1 );
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data < 0 ? 1 : 0;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::MULT:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = (uint16_t)mStageRead.data1 * (uint16_t)mStageRead.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data < 0 ? 1 : 0;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::IMULT:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = (int16_t)mStageRead.data1 * (int16_t)mStageRead.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data < 0 ? 1 : 0;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::IMULTN:
    mMulatiplyAccumulator = (int16_t)mStageRead.data1 * (int16_t)mStageRead.data2;
    mStageWrite.z = mMulatiplyAccumulator == 0 ? 1 : 0;
    mStageWrite.n = mMulatiplyAccumulator < 0 ? 1 : 0;
    mStageRead.instruction = I::EMPTY;
    break;
  case I::RESMAC:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mMulatiplyAccumulator;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::IMACN:
    mMulatiplyAccumulator += ( int16_t )mStageRead.data1 * ( int16_t )mStageRead.data2;
    mStageWrite.z = mMulatiplyAccumulator == 0 ? 1 : 0;
    mStageWrite.n = mMulatiplyAccumulator < 0 ? 1 : 0;
    mStageRead.instruction = I::EMPTY;
    break;
  case I::DIV:
    throw Ex{ "NYI" };
  case I::ABS:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.data2 < 0 ? -mStageRead.data2 : mStageRead.data2;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = 0;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::SH:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.data1 > 0 ? mStageRead.data2 >> mStageRead.data1 : mStageRead.data2 << mStageRead.data1;
      mStageWrite.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.n = mStageWrite.data < 0 ? 1 : 0;
      mStageWrite.c = mStageRead.data1 > 0 ? ( mStageRead.data2 & 1 ) : ( mStageRead.data2 >> 31 );
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::SHLQ:
    throw Ex{ "NYI" };
  case I::SHRQ:
    throw Ex{ "NYI" };
  case I::SHA:
    throw Ex{ "NYI" };
  case I::SHARQ:
    throw Ex{ "NYI" };
  case I::ROR:
    throw Ex{ "NYI" };
  case I::RORQ:
    throw Ex{ "NYI" };
  case I::CMP:
    throw Ex{ "NYI" };
  case I::CMPQ:
    throw Ex{ "NYI" };
  case I::SUBQMOD:
    throw Ex{ "NYI" };
  case I::SAT16S:
    throw Ex{ "NYI" };
  case I::MOVEI:
    throw Ex{ "NYI" };
  case I::LOADB:
    throw Ex{ "NYI" };
  case I::LOADW:
    throw Ex{ "NYI" };
  case I::LOAD:
    throw Ex{ "NYI" };
  case I::SAT32S:
    throw Ex{ "NYI" };
  case I::LOAD14N:
    throw Ex{ "NYI" };
  case I::LOAD15N:
    throw Ex{ "NYI" };
  case I::STOREB:
    throw Ex{ "NYI" };
  case I::STOREW:
    throw Ex{ "NYI" };
  case I::STORE:
    throw Ex{ "NYI" };
  case I::MIRROR:
    throw Ex{ "NYI" };
  case I::STORE14N:
    throw Ex{ "NYI" };
  case I::STORE15N:
    throw Ex{ "NYI" };
  case I::MOVEPC:
    throw Ex{ "NYI" };
  case I::JUMP:
    throw Ex{ "NYI" };
  case I::JR:
    throw Ex{ "NYI" };
  case I::MMULT:
    throw Ex{ "NYI" };
  case I::MTOI:
    throw Ex{ "NYI" };
  case I::NORMI:
    throw Ex{ "NYI" };
  case I::LOAD14R:
    throw Ex{ "NYI" };
  case I::LOAD15R:
    throw Ex{ "NYI" };
  case I::STORE14R:
    throw Ex{ "NYI" };
  case I::STORE15R:
    throw Ex{ "NYI" };
  case I::ADDQMOD:
    throw Ex{ "NYI" };
    break;
  }
}

void Jerry::stageRead()
{
  if ( mStageCompute.instruction != I::EMPTY )
    return;

  switch ( mStageRead.instruction )
  {
  case I::EMPTY:
    break;
  case I::NOP:
    mStageRead.instruction = I::EMPTY;
    break;
  case I::ADD:
  case I::SUB:
  case I::AND:
  case I::OR:
  case I::XOR:
  case I::MULT:
  case I::IMULT:
  case I::SH:
  case I::SHA:
  case I::ROR:
  case I::MTOI:
    if ( dualPortRead( mStageRead.reg1, mStageRead.reg2 ) )
    {
      dualPortCommit();
      mFlagsStatus = LOCKED;
      assert( mRegStatus[mStageRead.reg1] = FREE );
      assert( mRegStatus[mStageRead.reg2] = FREE );
      mRegStatus[mStageRead.reg2] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
    }
    break;
  case I::ADDC:
  case I::SUBC:
    if ( mFlagsStatus == FREE && dualPortRead( mStageRead.reg1, mStageRead.reg2 ) )
    {
      dualPortCommit();
      mFlagsStatus = LOCKED;
      assert( mRegStatus[mStageRead.reg1] = FREE );
      assert( mRegStatus[mStageRead.reg2] = FREE );
      mRegStatus[mStageRead.reg2] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
    }
    break;
  case I::ADDQ:
  case I::SUBQ:
  case I::NEG:
  case I::NOT:
  case I::BSET:
  case I::BCLR:
  case I::ABS:
  case I::SHLQ:
  case I::SHRQ:
  case I::SHARQ:
  case I::RORQ:
  case I::SUBQMOD:
  case I::SAT16S:
  case I::SAT32S:
  case I::MIRROR:
  case I::NORMI:
  case I::ADDQMOD:
    if ( dualPortRead( mStageRead.reg2 ) )
    {
      dualPortCommit();
      mFlagsStatus = LOCKED;
      assert( mRegStatus[mStageRead.reg1] = FREE );
      assert( mRegStatus[mStageRead.reg2] = FREE );
      mRegStatus[mStageRead.reg2] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
    }
    break;
  case I::ADDQT:
  case I::SUBQT:
    if ( dualPortRead( mStageRead.reg2 ) )
    {
      dualPortCommit();
      assert( mRegStatus[mStageRead.reg1] = FREE );
      assert( mRegStatus[mStageRead.reg2] = FREE );
      mRegStatus[mStageRead.reg2] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
    }
    break;
  case I::BTST:
  case I::CMPQ:
    if ( dualPortRead( mStageRead.reg2 ) )
    {
      dualPortCommit();
      mFlagsStatus = LOCKED;
      assert( mRegStatus[mStageRead.reg1] = FREE );
      assert( mRegStatus[mStageRead.reg2] = FREE );
      std::swap( mStageRead.instruction, mStageCompute.instruction );
    }
    break;
  case I::IMULTN:
  case I::IMACN:
  case I::CMP:
    if ( dualPortRead( mStageRead.reg1, mStageRead.reg2 ) )
    {
      dualPortCommit();
      mFlagsStatus = LOCKED;
      assert( mRegStatus[mStageRead.reg1] = FREE );
      assert( mRegStatus[mStageRead.reg2] = FREE );
      std::swap( mStageRead.instruction, mStageCompute.instruction );
    }
    break;
  case I::RESMAC:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mMulatiplyAccumulator;
      mRegStatus[mStageRead.reg2] = LOCKED;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::MOVEQ:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.reg1;
      mRegStatus[mStageRead.reg2] = LOCKED;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::MOVEFA:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mRegs[ mAnotherRegisterFile + mStageRead.reg1 ];
      mRegStatus[mStageRead.reg2] = LOCKED;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::DIV:
    if ( dualPortRead( mStageRead.reg1, mStageRead.reg2 ) )
    {
      dualPortCommit();
      assert( mRegStatus[mStageRead.reg1] = FREE );
      assert( mRegStatus[mStageRead.reg2] = FREE );
      mRegStatus[mStageRead.reg2] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
    }
    break;
  case I::MOVE:
    if ( mStageWrite.reg < 0 && dualPortRead( mStageRead.reg1 ) )
    {
      dualPortCommit();
      assert( mRegStatus[mStageRead.reg1] = FREE );
      // mRegStatus[mStageRead.reg2] != FREE is an error;
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mStageRead.data1;
      mRegStatus[mStageRead.reg2] = LOCKED;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::MOVETA:
    if ( dualPortRead( mStageRead.reg1 ) )
    {
      dualPortCommit();
      assert( mRegStatus[mStageRead.reg1] = FREE );
      mRegs[mAnotherRegisterFile + mStageRead.reg2] = mStageRead.data1;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::MOVEI:
    throw Ex{ "MOVEI NYI" };
    break;
  case I::LOADB:
  case I::LOADW:
  case I::LOAD:
    throw Ex{ "LOAD NYI" };
    break;
  case I::LOAD14R:
  case I::LOAD15R:
    throw Ex{ "LOAD NYI" };
    break;
  case I::LOAD14N:
  case I::LOAD15N:
    throw Ex{ "LOAD NYI" };
    break;
  case I::STOREB:
  case I::STOREW:
  case I::STORE:
    throw Ex{ "STORE NYI" };
    break;
  case I::STORE14N:
  case I::STORE15N:
    throw Ex{ "STORE NYI" };
    break;
  case I::STORE14R:
  case I::STORE15R:
    throw Ex{ "STORE NYI" };
    break;
  case I::MOVEPC:
    if ( mStageWrite.reg < 0 )
    {
      mStageWrite.reg = mStageRead.reg2;
      mStageWrite.data = mPC;
      mRegStatus[mStageRead.reg2] = LOCKED;
      mStageRead.instruction = I::EMPTY;
    }
    break;
  case I::JUMP:
  case I::JR:
    if ( mFlagsStatus == FREE && dualPortRead( mStageRead.reg2 ) )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
    }
    break;
  case I::MMULT:
    throw Ex{ "MMULT NYI" };
    break;
  }
}

void Jerry::decode()
{
  if ( mStageRead.instruction != I::EMPTY )
    return;

  int32_t pull = mPrefetch.pull();

  if ( pull < 0 )
    return;

  mStageRead.instruction = (I)( pull >> 10 );
  mStageRead.reg1 = ( pull >> 5 ) & 0x1f;
  mStageRead.reg2 = pull & 0x1f;
}

void Jerry::prefetch()
{
  //Only local RAM for now
  assert( mPC >= RAM_BASE && mPC < RAM_BASE + RAM_SIZE );

  if ( mLastLocalRAMAccessCycle != mCycle )
  {
    auto code = mLocalRAM[mPC - RAM_BASE];
    if ( mPrefetch.push( code ) )
    {
      mLastLocalRAMAccessCycle = mCycle;
      mPC += 2;
    }
  }

  mCycle += 1;
}

bool Jerry::dualPortWrite( uint32_t reg, uint32_t data )
{
  if ( mDualPort.port1.status == DualPort::EMPTY )
  {
    mDualPort.port1.status = DualPort::WRITE;
    mDualPort.port1.reg = reg;
    mDualPort.port1.data = data;
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
      return true;
    }
    else if ( mDualPort.port1.status == DualPort::WRITE )
    {
      if ( mDualPort.port1.reg == reg1 )
      {
        mDualPort.port2.status = DualPort::READ;
        mDualPort.port2.reg = reg2;
        return true;
      }
      else if ( mDualPort.port1.reg == reg2 )
      {
        mDualPort.port2.status = DualPort::READ;
        mDualPort.port2.reg = reg1;
        return true;
      }
    }
  }

  return false;
}

bool Jerry::dualPortRead( uint32_t reg )
{
  if ( mDualPort.port1.status == DualPort::EMPTY )
  {
    mDualPort.port1.status = DualPort::READ;
    mDualPort.port1.reg = reg;
    return true;
  }
  else if ( mDualPort.port1.status == DualPort::WRITE )
  {
    if ( mDualPort.port1.reg == reg )
    {
      return true;
    }
    else
    {
      assert( mDualPort.port2.status == DualPort::EMPTY );
      mDualPort.port2.status = DualPort::READ;
      mDualPort.port2.reg = reg;
      return true;
    }
  }

  return false;
}

void Jerry::dualPortCommit()
{
  if ( mDualPort.port1.status == DualPort::WRITE )
  {
    mRegStatus[mDualPort.port1.reg] = FREE;
    mRegs[mRegisterFile + mDualPort.port1.reg] = mDualPort.port1.data;
    mDualPort.port1.status = DualPort::EMPTY;
  }
  else if ( mDualPort.port1.status == DualPort::READ )
  {
    assert( mRegStatus[mDualPort.port1.reg] == FREE );
    assert( mStageRead.reg1 == mDualPort.port1.reg );
    mStageRead.data1 = mRegs[mRegisterFile + mDualPort.port1.reg];
    mDualPort.port1.status = DualPort::EMPTY;
  }
  else if ( mDualPort.port2.status == DualPort::READ )
  {
    assert( mRegStatus[mDualPort.port2.reg] == FREE );
    assert( mStageRead.reg2 == mDualPort.port2.reg );
    mStageRead.data2 = mRegs[mRegisterFile + mDualPort.port2.reg];
    mDualPort.port2.status = DualPort::EMPTY;
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
    queue |= (uint64_t)value << ( queueSize * 16 );
    queueSize += 2;
    return true;
  }
  else
  {
    return false;
  }
}
