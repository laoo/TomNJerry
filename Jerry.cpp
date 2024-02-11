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
  if ( address >= RAM_BASE && address < RAM_BASE + RAM_SIZE )
    mLocalRAM[( address - RAM_BASE ) / sizeof( uint32_t )] = std::byteswap( data );
  else switch ( address )
  {
  case D_FLAGS:
    throw Ex{ "Jerry::debugWrite: Unhandled address " } << std::hex << address;
  case D_MTXC:
    mMTXC = data;
    break;
  case D_MTXA:
    mMTXA = data;
    break;
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
    mMod = data;
    break;
  case D_DIVCTRL:
    mDivCtrl = data;
    break;
  case D_MACHI:
    mMachi = data;
    break;
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
  halfCycle();
  halfCycle();

  return mBusGate;
}

void Jerry::halfCycle()
{
  io();
  stageWrite();
  compute();
  stageRead();
  decode();
  prefetch();
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

  ackWrite();
  return advance();
}

AdvanceResult Jerry::busCycleRead( uint8_t value )
{
  assert( mBusGate.getOperation() > 0 );
  assert( mBusGate.getSize() == 1 );

  if ( !mCtrl.go() )
    return AdvanceResult::nop();

  ackRead( value );
  return advance();
}

AdvanceResult Jerry::busCycleRead( uint16_t value )
{
  assert( mBusGate.getOperation() > 0 );
  assert( mBusGate.getSize() == 2 );

  if ( !mCtrl.go() )
    return AdvanceResult::nop();

  ackRead( value );
  return advance();
}

AdvanceResult Jerry::busCycleRead( uint32_t value )
{
  assert( mBusGate.getOperation() > 0 );
  assert( mBusGate.getSize() == 4 );

  if ( !mCtrl.go() )
    return AdvanceResult::nop();

  ackRead( value );
  return advance();
}

AdvanceResult Jerry::busCycleRead( uint64_t value )
{
  throw Ex{ "Jerry::busCycleRead: Unhandled size " };
}

void Jerry::ackWrite()
{
  assert( mBusGate );

  switch ( mBusGate.getSize() )
  {
  case 1:
    mLog->storeByte( mBusGate.getAddress(), mBusGate.getValue() );
    break;
  case 2:
    mLog->storeWord( mBusGate.getAddress(), mBusGate.getValue() );
    break;
  case 4:
    mLog->storeLong( mBusGate.getAddress(), mBusGate.getValue() );
    break;
  default:
    throw Ex{ "Jerry::ackWrite: Unhandled size " };
  }
  busGatePop();
}

void Jerry::ackRead( uint32_t value )
{
  assert( mBusGate );
  assert( mBusGate.getSize() == 4 );

  portWriteDst( mBusGate.getReg(), std::byteswap( value ) );
  mLog->loadLong( mBusGate.getAddress(), mStageWrite.data );
  busGatePop();
}

void Jerry::ackRead( uint16_t value )
{
  assert( mBusGate );
  assert( mBusGate.getSize() == 2 );

  portWriteDst( mBusGate.getReg(), std::byteswap( (uint32_t)value ) );
  mLog->loadWord( mBusGate.getAddress(), mStageWrite.data );
  busGatePop();
}

void Jerry::ackRead( uint8_t value )
{
  assert( mBusGate );
  assert( mBusGate.getSize() == 1 );

  portWriteDst( mBusGate.getReg(), std::byteswap( ( uint32_t )value ) );
  mLog->loadByte( mBusGate.getAddress(), mStageWrite.data );
  busGatePop();
}


void Jerry::storeByte( uint32_t address, uint8_t data )
{
  if ( address >= JERRY_BASE && address < JERRY_BASE + JERRY_SIZE )
  {
    throw EmulationViolation{ "Writing a byte to internal memory" };
  }
  else
  {
    busGatePush( AdvanceResult::writeByte( address, std::byteswap( ( uint32_t )data ) ) );
  }
}

void Jerry::storeWord( uint32_t address, uint16_t data )
{
  if ( address >= JERRY_BASE && address < JERRY_BASE + JERRY_SIZE )
  {
    throw EmulationViolation{ "Writing a word to internal memory" };
  }
  else
  {
    busGatePush( AdvanceResult::writeWord( address, std::byteswap( ( uint32_t )data ) ) );
  }
}

void Jerry::storeLong( uint32_t address, uint32_t data )
{
  if ( address >= JERRY_BASE && address < JERRY_BASE + JERRY_SIZE )
  {
    if ( address >= RAM_BASE && address < RAM_BASE + RAM_SIZE )
    {
      mLocalRAM[( address - RAM_BASE ) / sizeof( uint32_t )] = std::byteswap( data );
      mLastLocalRAMAccessCycle = mCycle;
      mLog->storeLong( address, data );
    }
    else switch ( address )
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
    busGatePush( AdvanceResult::writeLong( address, std::byteswap( data ) ) );
  }
}

void Jerry::loadByte( uint32_t address, uint32_t reg )
{
  if ( address >= JERRY_BASE && address < JERRY_BASE + JERRY_SIZE )
    throw EmulationViolation{ "Loading a byte from internal memory" };

  busGatePush( AdvanceResult::readByte( address, reg ) );
}

void Jerry::loadWord( uint32_t address, uint32_t reg )
{
  if ( address >= JERRY_BASE && address < JERRY_BASE + JERRY_SIZE )
    throw EmulationViolation{ "Loading a word from internal memory" };

  busGatePush( AdvanceResult::readShort( address, reg ) );
}

void Jerry::loadLong( uint32_t address, uint32_t reg )
{
  if ( address >= JERRY_BASE && address < JERRY_BASE + JERRY_SIZE )
  {
    mStageWrite.regFlags.reg = reg;
    if ( address >= RAM_BASE && address < RAM_BASE + RAM_SIZE )
    {
      mStageWrite.data = std::byteswap( mLocalRAM[( address - RAM_BASE ) / sizeof( uint32_t )] );
    }
    else switch ( address )
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

    mStageIO.state = StageIO::IDLE;
    mLastLocalRAMAccessCycle = mCycle;
    mLog->loadLong( address, mStageWrite.data );
  }
  else
  {
    busGatePush( AdvanceResult::readLong( address, reg ) );
  }
}

bool Jerry::testCondition( uint32_t condition ) const
{
  switch ( condition )
  {
  case 0x00:
    return true;
  case 0x01:
    return ( mFlags.value & FLAGS::ZERO_FLAG ) == 0;
  case 0x02:
    return ( mFlags.value & FLAGS::ZERO_FLAG ) == FLAGS::ZERO_FLAG;
  case 0x04:
    return ( mFlags.value & FLAGS::CARRY_FLAG ) == 0;
  case 0x05:
    return ( mFlags.value & ( FLAGS::CARRY_FLAG | FLAGS::ZERO_FLAG ) ) == 0;
  case 0x06:
    return ( mFlags.value & ( FLAGS::CARRY_FLAG | FLAGS::ZERO_FLAG ) ) == FLAGS::ZERO_FLAG;
  case 0x08:
    return ( mFlags.value & FLAGS::CARRY_FLAG ) == FLAGS::CARRY_FLAG;
  case 0x09:
    return ( mFlags.value & ( FLAGS::CARRY_FLAG | FLAGS::ZERO_FLAG ) ) == FLAGS::CARRY_FLAG;
  case 0x0a:
    return ( mFlags.value & ( FLAGS::CARRY_FLAG | FLAGS::ZERO_FLAG ) ) == ( FLAGS::CARRY_FLAG | FLAGS::ZERO_FLAG );
  case 0x14:
    return ( mFlags.value & FLAGS::NEGA_FLAG ) == 0;
  case 0x15:
    return ( mFlags.value & ( FLAGS::NEGA_FLAG | FLAGS::ZERO_FLAG ) ) == 0;
  case 0x16:
    return ( mFlags.value & ( FLAGS::NEGA_FLAG | FLAGS::ZERO_FLAG ) ) == FLAGS::ZERO_FLAG;
  case 0x18:
    return ( mFlags.value & FLAGS::NEGA_FLAG ) == FLAGS::NEGA_FLAG;
  case 0x19:
    return ( mFlags.value & ( FLAGS::NEGA_FLAG | FLAGS::ZERO_FLAG ) ) == FLAGS::NEGA_FLAG;
  case 0x1a:
    return ( mFlags.value & ( FLAGS::NEGA_FLAG | FLAGS::ZERO_FLAG ) ) == ( FLAGS::NEGA_FLAG | FLAGS::ZERO_FLAG );
  case 0x1f:
    return false;
  default:
    throw EmulationViolation{ "Illegal condition code " } << std::hex << condition;
  }
}

void Jerry::io()
{
  switch ( mStageIO.state )
  {
    case StageIO::STORE_BYTE:
      storeByte( mStageIO.address, ( uint8_t )mStageIO.data );
      break;
    case StageIO::STORE_WORD:
      storeWord( mStageIO.address, ( uint16_t )mStageIO.data );
      break;
    case StageIO::STORE_LONG:
      storeLong( mStageIO.address, mStageIO.data );
      break;
    case StageIO::LOAD_BYTE:
      loadByte( mStageIO.address, mStageIO.reg );
      break;
    case StageIO::LOAD_WORD:
      loadWord( mStageIO.address, mStageIO.reg );
      break;
    case StageIO::LOAD_LONG:
      loadLong( mStageIO.address, mStageIO.reg );
      break;
    default:
      break;
  }
}

void Jerry::stageWrite()
{
  if ( mStageWrite.regFlags.reg >= 0 )
  {
    if ( portWriteDst( mStageWrite.regFlags.reg, mStageWrite.data ) )
    {
      mStageWrite.regFlags.reg = -1;
    }
  }

  if ( mStageWrite.updateFlags )
  {
    const uint32_t maskOut = ( mStageWrite.regFlags.z >= 0 ? ( FLAGS::ZERO_FLAG ) : 0 ) |
      ( mStageWrite.regFlags.c >= 0 ? ( FLAGS::CARRY_FLAG ) : 0 ) |
      ( mStageWrite.regFlags.n >= 0 ? ( FLAGS::NEGA_FLAG ) : 0 );

    mFlags.value &= ~maskOut;

    const uint32_t maskIn = ( mStageWrite.regFlags.z > 0 ? ( FLAGS::ZERO_FLAG ) : 0 ) |
      ( mStageWrite.regFlags.c > 0 ? ( FLAGS::CARRY_FLAG ) : 0 ) |
      ( mStageWrite.regFlags.n > 0 ? ( FLAGS::NEGA_FLAG ) : 0 );

    mFlags.value |= maskIn;

    mStageWrite.regFlags = RegFlags();
    mStageWrite.updateFlags = false;
    mFlagsSemaphore -= 1;
    assert( mFlagsSemaphore >= 0 );
  }
}

void Jerry::compute()
{
  switch ( mStageCompute.instruction )
  {
  case DSPI::ADD:
  case DSPI::ADDQ:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataSrc + mStageCompute.dataDst;
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = mStageWrite.data < mStageCompute.dataSrc ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::ADDC:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataSrc + mStageCompute.dataDst + ( mFlags.value & ( FLAGS::CARRY_FLAG >> 1 ) );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = mStageWrite.data < mStageCompute.dataSrc ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::ADDQT:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataSrc + mStageCompute.dataDst;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeReg( mStageWrite.regFlags );
    }
    break;
  case DSPI::SUB:
  case DSPI::SUBQ:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataDst - mStageCompute.dataSrc;
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = mStageCompute.dataSrc < mStageCompute.dataDst ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::SUBC:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      uint64_t res = (uint64_t)mStageCompute.dataDst + (uint64_t)( mStageCompute.dataSrc ^ 0xffffffff ) + (uint64_t)( ( mFlags.value & ( FLAGS::CARRY_FLAG >> 1 ) ) ^ 1 );
      mStageWrite.data = (uint32_t)res;
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = ( ( res >> 32 ) & 1 ) ^ 1;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::SUBQT:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataDst - mStageCompute.dataSrc;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeReg( mStageWrite.regFlags );
    }
    break;
  case DSPI::NEG:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = -mStageCompute.dataDst;
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = mStageCompute.dataDst != 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::AND:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataSrc & mStageCompute.dataDst;
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::OR:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataSrc | mStageCompute.dataDst;
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::XOR:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataSrc ^ mStageCompute.dataDst;
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::NOT:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = ~mStageCompute.dataDst;
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::BTST:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.z = ( mStageCompute.dataDst & ( 1 << mStageCompute.dataSrc ) ) == 0 ? 1 : 0;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::BSET:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataDst | ( 1 << mStageCompute.dataSrc );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::BCLR:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataDst & ~( 1 << mStageCompute.dataSrc );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::MULT:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = (uint16_t)mStageCompute.dataSrc * (uint16_t)mStageCompute.dataDst;
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::IMULT:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = (int16_t)mStageCompute.dataSrc * (int16_t)mStageCompute.dataDst;
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::IMULTN:
    mMacStage.acc = (int16_t)mStageCompute.dataSrc * (int16_t)mStageCompute.dataDst;
    mStageWrite.regFlags.z = mMacStage.acc == 0 ? 1 : 0;
    mStageWrite.regFlags.n = mStageWrite.data >> 31;
    mStageWrite.updateFlags = true;
    mStageCompute.instruction = DSPI::EMPTY;
    mLog->computeMul();
    mLog->computeFlags( mStageWrite.regFlags );
    break;
  case DSPI::RESMAC:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = mMacStage.acc;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeReg( mStageWrite.regFlags );
    }
    break;
  case DSPI::IMACN:
    mMacStage.acc += ( int16_t )mStageCompute.dataSrc * ( int16_t )mStageCompute.dataDst;
    mStageCompute.instruction = DSPI::EMPTY;
    mLog->computeMac();
    break;
  case DSPI::DIV:
    throw Ex{ "NYI" };
  case DSPI::ABS:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataDst < 0 ? -mStageCompute.dataDst : mStageCompute.dataDst;
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = 0;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::SH:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataSrc > 0 ? mStageCompute.dataDst >> mStageCompute.dataSrc : mStageCompute.dataDst << mStageCompute.dataSrc;
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.regFlags.c = mStageCompute.dataSrc > 0 ? ( mStageCompute.dataDst & 1 ) : ( mStageCompute.dataDst >> 31 );
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::SHLQ:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataDst << ( 32 - mStageCompute.dataSrc );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.regFlags.c = mStageCompute.dataDst >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::SHRQ:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataDst >> mStageCompute.dataSrc;
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.regFlags.c = mStageCompute.dataDst & 1;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::SHA:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;
      mStageWrite.data = mStageCompute.dataSrc > 0 ? (uint32_t)( (int64_t)( mStageCompute.dataDst ) >> mStageCompute.dataSrc ) : mStageCompute.dataDst << mStageCompute.dataSrc;
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.regFlags.c = mStageCompute.dataSrc > 0 ? ( mStageCompute.dataDst & 1 ) : ( mStageCompute.dataDst >> 31 );
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::SHARQ:
    throw Ex{ "NYI" };
  case DSPI::ROR:
    throw Ex{ "NYI" };
  case DSPI::RORQ:
    throw Ex{ "NYI" };
  case DSPI::CMP:
    {
      uint32_t data = mStageCompute.dataDst - mStageCompute.dataSrc;
      mStageWrite.regFlags.z = data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = (int32_t)mStageCompute.dataSrc < ( int32_t )mStageCompute.dataDst ? 1 : 0;
      mStageWrite.regFlags.n = data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::CMPQ:
    {
      auto i0 = ( int8_t )( mStageCompute.dataSrc << 3 );
      auto i1 = ( int64_t )i0;
      auto i2 = i1 >> 3;
      auto i3 = ( uint32_t )i2;
      uint32_t data = mStageCompute.dataDst - i3;
      mStageWrite.regFlags.z = data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = (int32_t)mStageCompute.dataSrc < ( int32_t )mStageCompute.dataDst ? 1 : 0;
      mStageWrite.regFlags.n = data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::SUBQMOD:
    throw Ex{ "NYI" };
  case DSPI::SAT16S:
    throw Ex{ "NYI" };
  case DSPI::SAT32S:
    throw Ex{ "NYI" };
  case DSPI::LOAD14N:
  case DSPI::LOAD15N:
  case DSPI::LOAD14R:
  case DSPI::LOAD15R:
    if ( mStageIO.state == StageIO::IDLE )
    {
      mStageCompute.instruction = DSPI::EMPTY;
      mStageIO.state = StageIO::LOAD_LONG;
      mStageIO.address = mStageCompute.dataSrc + mStageCompute.regDst;
      mStageIO.reg = mStageCompute.regSrc;
      mLog->computeIndex();
    }
    break;
  case DSPI::MIRROR:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      mStageWrite.regFlags.reg = mStageCompute.regDst;

      uint8_t b0 = mStageCompute.dataDst & 0xff;
      uint8_t b1 = ( mStageCompute.dataDst >> 8 ) & 0xff;
      uint8_t b2 = ( mStageCompute.dataDst >> 16 ) & 0xff;
      uint8_t b3 = ( mStageCompute.dataDst >> 24 ) & 0xff;

      //https://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith64Bits
      uint8_t m0 = ( ( b0 * 0x80200802ULL ) & 0x0884422110ULL ) * 0x0101010101ULL >> 32;
      uint8_t m1 = ( ( b1 * 0x80200802ULL ) & 0x0884422110ULL ) * 0x0101010101ULL >> 32;
      uint8_t m2 = ( ( b2 * 0x80200802ULL ) & 0x0884422110ULL ) * 0x0101010101ULL >> 32;
      uint8_t m3 = ( ( b3 * 0x80200802ULL ) & 0x0884422110ULL ) * 0x0101010101ULL >> 32;

      mStageWrite.data = ( m0 << 24 ) | ( m1 << 16 ) | ( m2 << 8 ) | m3;
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateFlags = true;
      mStageCompute.instruction = DSPI::EMPTY;
      mLog->computeRegFlags( mStageWrite.regFlags );
    }
    break;
  case DSPI::STORE14N:
  case DSPI::STORE15N:
  case DSPI::STORE14R:
  case DSPI::STORE15R:
    if ( mStageIO.state == StageIO::IDLE )
    {
      //Indexed store is not guarded by the scoreboard mechanism
      portReadDstAndHiddenCommit( mStageCompute.regSrc );
      mStageCompute.instruction = DSPI::EMPTY;
      mStageIO.state = StageIO::STORE_LONG;
      mStageIO.address = mStageCompute.dataSrc + mStageCompute.regDst;
      mStageIO.data = mStageCompute.dataDst;
      mLog->computeIndex();
    }
    break;
  case DSPI::JUMP:
    if ( testCondition( mStageCompute.regDst ) )
    {
      mPC = mStageCompute.dataSrc;
      mPrefetch.queueSize = 0;
    }
    mStageCompute.instruction = DSPI::EMPTY;
    break;
  case DSPI::JR:
    if ( testCondition( mStageCompute.regDst ) )
    {
      int32_t off = ( (int8_t)( mStageCompute.dataSrc << 3 ) / 8 );
      mPC = mPC + ( off - mPrefetch.queueSize - 1 ) * 2;
      mPrefetch.queueSize = 0;
    }
    mStageCompute.instruction = DSPI::EMPTY;
    break;
  case DSPI::MMULT:
    if ( mStageIO.state != StageIO::IDLE )
      throw EmulationViolation{ "MMULT instruction executed while IO was in progress" };
    mStageCompute.instruction = DSPI::EMPTY;
    break;
  case DSPI::MTOI:
    throw Ex{ "NYI" };
  case DSPI::NORMI:
    throw Ex{ "NYI" };
  case DSPI::ADDQMOD:
    throw Ex{ "NYI" };
    break;
  case DSPI::MM_IMULTN:
    mMacStage.acc = ( int16_t )mStageCompute.dataSrc * ( int16_t )mStageWrite.data;
    mStageCompute.instruction = DSPI::EMPTY;
    mLog->computeMul();
    break;
  case DSPI::MM_IMACN:
  {
    int32_t add = ( int16_t )mStageCompute.dataSrc * ( int16_t )mStageWrite.data;
    int32_t old = mMacStage.acc;
    mMacStage.acc += add;
    mStageCompute.instruction = DSPI::EMPTY;
    mStageWrite.regFlags.z = mMacStage.acc == 0 ? 1 : 0;
    mStageWrite.regFlags.c = ( int32_t )mMacStage.acc < ( int32_t )old ? 1 : 0;
    mStageWrite.regFlags.n = mMacStage.acc >> 31;
    mStageWrite.updateFlags = true;
    mLog->computeMac( mStageWrite.regFlags );
  }
    break;
  default:
    break;
  }
}

void Jerry::stageRead()
{
  if ( mStageCompute.instruction != DSPI::EMPTY )
    return;

  switch ( mStageRead.instruction )
  {
  case DSPI::EMPTY:
    dualPortCommit();
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
    if ( portReadBoth( mStageRead.regSrc, mStageRead.regDst ) )
    {
      dualPortCommit();
      mFlagsSemaphore += 1;
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataSrc = mStageRead.dataSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::ADDC:
  case DSPI::SUBC:
    if ( mFlagsSemaphore == 0 && portReadBoth( mStageRead.regSrc, mStageRead.regDst ) )
    {
      dualPortCommit();
      mFlagsSemaphore += 1;
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataSrc = mStageRead.dataSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::ADDQ:
  case DSPI::SUBQ:
  case DSPI::SUBQMOD:
  case DSPI::ADDQMOD:
    if ( portReadDst( mStageRead.regDst ) )
    {
      dualPortCommit();
      mLog->portImm( mStageRead.regSrc );
      mFlagsSemaphore += 1;
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataSrc = tabAddSubQ[ mStageRead.regSrc];
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::BSET:
  case DSPI::BCLR:
  case DSPI::SHRQ:
  case DSPI::SHARQ:
  case DSPI::RORQ:
    if ( portReadDst( mStageRead.regDst ) )
    {
      dualPortCommit();
      mLog->portImm( mStageRead.regSrc );
      mFlagsSemaphore += 1;
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataSrc = mStageRead.regSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::SHLQ:
    if ( portReadDst( mStageRead.regDst ) )
    {
      dualPortCommit();
      mLog->portImm( 32 - mStageRead.regSrc );
      mFlagsSemaphore += 1;
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataSrc = mStageRead.regSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    else
    {
      dualPortCommit();
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
      dualPortCommit();
      mFlagsSemaphore += 1;
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::ADDQT:
  case DSPI::SUBQT:
    if ( portReadDst( mStageRead.regDst ) )
    {
      dualPortCommit();
      mLog->portImm( mStageRead.regSrc );
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataSrc = tabAddSubQ[mStageRead.regSrc];
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::BTST:
  case DSPI::CMPQ:
    if ( portReadDst( mStageRead.regDst ) )
    {
      dualPortCommit();
      mLog->portImm( mStageRead.regSrc );
      mFlagsSemaphore += 1;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.regSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::IMULTN:
  case DSPI::IMACN:
  case DSPI::CMP:
    if ( portReadBoth( mStageRead.regSrc, mStageRead.regDst ) )
    {
      dualPortCommit();
      mFlagsSemaphore += 1;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::RESMAC:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      if ( mRegStatus[mStageRead.regDst] != FREE )
        throw EmulationViolation{ "RESMAC writes to a register in use" };
      dualPortCommit();
      mStageWrite.regFlags.reg = mStageRead.regDst;
      mStageWrite.data = (uint32_t)mMacStage.acc;
      mRegStatus[mStageRead.regDst] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::MOVEQ:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      if ( mRegStatus[mStageRead.regDst] != FREE )
        throw EmulationViolation{ "MOVEQ writes to a register in use" };
      dualPortCommit();
      mLog->portImm( mStageRead.regSrc );
      mStageWrite.regFlags.reg = mStageRead.regDst;
      mStageWrite.data = mStageRead.regSrc;
      mRegStatus[mStageRead.regDst] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::MOVEFA:
    if ( mStageWrite.regFlags.reg < 0 && portReadSrc( mStageRead.regSrc + 32 ) )
    {
      if ( mRegStatus[mStageRead.regDst] != FREE )
        throw EmulationViolation{ "MOVEFA writes to a register in use" };

      dualPortCommit();
      mStageWrite.regFlags.reg = mStageRead.regDst;
      mStageWrite.data = mStageRead.dataSrc;
      mRegStatus[mStageRead.regDst] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::DIV:
    if ( portReadBoth( mStageRead.regSrc, mStageRead.regDst ) )
    {
      dualPortCommit();
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataSrc = mStageRead.dataSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::MOVE:
    if ( mStageWrite.regFlags.reg < 0 && portReadSrc( mStageRead.regSrc ) )
    {
      if ( mRegStatus[mStageRead.regDst] != FREE )
        throw EmulationViolation{ "MOVE writes to a register in use" };

      dualPortCommit();
      mStageWrite.regFlags.reg = mStageRead.regDst;
      mStageWrite.data = mStageRead.dataSrc;
      mRegStatus[mStageRead.regDst] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::MOVETA:
    if ( mStageWrite.regFlags.reg < 0 && portReadSrc( mStageRead.regSrc ) )
    {
      dualPortCommit();
      mStageWrite.regFlags.reg = mStageRead.regDst + 32;
      mStageWrite.data = mStageRead.dataSrc;
      mStageRead.instruction = DSPI::EMPTY;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::MOVEI:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      if ( mRegStatus[mStageRead.regDst] != FREE )
        throw EmulationViolation{ "MOVEI writes to a register in use" };
      dualPortCommit();
      mRegStatus[mStageRead.regDst] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::LOADB:
    if ( mStageIO.state == StageIO::IDLE && portReadSrc( mStageRead.regSrc ) )
    {
      dualPortCommit();
      mRegStatus[mStageRead.regDst] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
      mStageIO.state = StageIO::LOAD_BYTE;
      mStageIO.address = mStageRead.dataSrc;
      mStageIO.reg = mStageRead.regDst;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::LOADW:
    if ( mStageIO.state == StageIO::IDLE && portReadSrc( mStageRead.regSrc ) )
    {
      dualPortCommit();
      mRegStatus[mStageRead.regDst] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
      mStageIO.state = StageIO::LOAD_WORD;
      mStageIO.address = mStageRead.dataSrc;
      mStageIO.reg = mStageRead.regDst;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::LOAD:
    if ( mStageIO.state == StageIO::IDLE && portReadSrc( mStageRead.regSrc ) )
    {
      dualPortCommit();
      mRegStatus[mStageRead.regDst] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
      mStageIO.state = StageIO::LOAD_LONG;
      mStageIO.address = mStageRead.dataSrc;
      mStageIO.reg = mStageRead.regDst;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::LOAD14R:
    if ( portReadBoth( 14, mStageRead.regSrc ) )
    {
      dualPortCommit();
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;   //base address to store
      mStageCompute.regSrc = mStageRead.regDst;     //register to store to
      mStageCompute.regDst = mStageRead.dataDst;    //offset
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::LOAD15R:
    if ( portReadBoth( 15, mStageRead.regSrc ) )
    {
      dualPortCommit();
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;   //base address to store
      mStageCompute.regSrc = mStageRead.regDst;     //register to store to
      mStageCompute.regDst = mStageRead.dataDst;    //offset
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::LOAD14N:
    if ( portReadSrc( 14 ) )
    {
      dualPortCommit();
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;   //base address to store
      mStageCompute.regSrc = mStageRead.regDst;     //register to store
      mStageCompute.regDst = tabAddSubQ[mStageRead.regSrc] * 4;     //offset
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::LOAD15N:
    if ( portReadSrc( 15 ) )
    {
      dualPortCommit();
      mRegStatus[mStageRead.regDst] = LOCKED;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;   //base address to store
      mStageCompute.regSrc = mStageRead.regDst;     //register to store
      mStageCompute.regDst = tabAddSubQ[mStageRead.regSrc] * 4;     //offset
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::STOREB:
    if ( mStageIO.state == StageIO::IDLE && portReadBoth( mStageRead.regDst, mStageRead.regSrc ) )
    {
      dualPortCommit();
      mStageRead.instruction = DSPI::EMPTY;
      mStageIO.state = StageIO::STORE_BYTE;
      mStageIO.address = mStageRead.dataDst;
      mStageIO.data = mStageRead.dataSrc;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::STOREW:
    if ( mStageIO.state == StageIO::IDLE && portReadBoth( mStageRead.regDst, mStageRead.regSrc ) )
    {
      dualPortCommit();
      mStageRead.instruction = DSPI::EMPTY;
      mStageIO.state = StageIO::STORE_WORD;
      mStageIO.address = mStageRead.dataDst;
      mStageIO.data = mStageRead.dataSrc;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::STORE:
    if ( mStageIO.state == StageIO::IDLE && portReadBoth( mStageRead.regDst, mStageRead.regSrc ) )
    {
      dualPortCommit();
      mStageRead.instruction = DSPI::EMPTY;
      mStageIO.state = StageIO::STORE_LONG;
      mStageIO.address = mStageRead.dataDst;
      mStageIO.data = mStageRead.dataSrc;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::STORE14R:
    if ( portReadBoth( 14, mStageRead.regSrc ) )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;   //base address to store
      mStageCompute.regSrc = mStageRead.regDst;     //register to store
      mStageCompute.regDst = mStageRead.dataDst;    //offset
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::STORE15R:
    if ( portReadBoth( 15, mStageRead.regSrc ) )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;   //base address to store
      mStageCompute.regSrc = mStageRead.regDst;     //register to store
      mStageCompute.regDst = mStageRead.dataDst;    //offset
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::STORE14N:
    if ( portReadSrc( 14 ) )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;   //base address to store
      mStageCompute.regSrc = mStageRead.regDst;     //register to store
      mStageCompute.regDst = tabAddSubQ[mStageRead.regSrc] * 4;     //offset
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::STORE15N:
    if ( portReadSrc( 15 ) )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;   //base address to store
      mStageCompute.regSrc = mStageRead.regDst;     //register to store
      mStageCompute.regDst = tabAddSubQ[mStageRead.regSrc] * 4;     //offset
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::MOVEPC:
    if ( mStageWrite.regFlags.reg < 0 )
    {
      if ( mRegStatus[mStageRead.regDst] != FREE )
        throw EmulationViolation{ "MOVE PC writes to a register in use" };
      dualPortCommit();
      mStageWrite.regFlags.reg = mStageRead.regDst;
      mStageWrite.data = mPC - ( mPrefetch.queueSize + 1 ) * 2;
      mRegStatus[mStageRead.regDst] = LOCKED;
      mStageRead.instruction = DSPI::EMPTY;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::JUMP:
    if ( mFlagsSemaphore == 0 && portReadDst( mStageRead.regSrc ) )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mLog->portCond( mStageRead.regDst );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataSrc = mStageRead.dataSrc;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::JR:
    if ( mFlagsSemaphore == 0 )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mLog->portCond( mStageRead.regDst );
      mStageCompute.regDst = mStageRead.regDst;
      mStageCompute.dataSrc = mStageRead.regSrc;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::MMULT:
    dualPortCommit();
    std::swap( mStageRead.instruction, mStageCompute.instruction );
    mStageCompute.regSrc = mStageRead.regSrc;
    mStageCompute.regDst = mStageRead.regDst;
    break;
  case DSPI::MM_IMACN:
    mFlagsSemaphore += 1;
    [[fallthrough]];
  case DSPI::MM_IMULTN:
    if ( portReadSrc( mStageCompute.regSrc + ( mMacStage.cnt >> 1 ) ) )
    {
      dualPortCommitMMULT( ( mMacStage.cnt & 1 ) == 1 );
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;
      mStageIO.state = StageIO::LOAD_LONG;
      mStageIO.address = mMTXA + mMacStage.cnt * 4 * ( ( mMTXC & 16 ) ? mMacStage.size : 1 );
      //destination register used as a temporary register
      mStageIO.reg = mStageCompute.regDst;
      mMacStage.cnt += 1;
    }
    break;
  case DSPI::MM_RESMAC:
      //resetting write flag of internal memory load
      mPortWriteDstReg = -1;
      mStageWrite.regFlags.reg = mStageRead.regDst;
      mStageWrite.data = ( uint32_t )mMacStage.acc;
      mStageRead.instruction = DSPI::EMPTY;
      break;
  }
}

void Jerry::decode()
{
  if ( mStageRead.instruction != DSPI::EMPTY )
    return;

  auto [pullStatus, opcode] = prefetchPull();

  switch ( pullStatus )
  {
  case Prefetch::OPCODE:
    mStageRead.instruction = ( DSPI )( opcode >> 10 );
    mStageRead.regSrc = ( opcode >> 5 ) & 0x1f;
    mStageRead.regDst = opcode & 0x1f;
    mLog->decodeDSP( mStageRead.instruction, mStageRead.regSrc, mStageRead.regDst );
    break;
  case Prefetch::MOVEI1:
    mStageRead.dataSrc = ( uint16_t )opcode;
    mLog->decodeMOVEI( 0, mStageRead.dataSrc );
    break;
  case Prefetch::MOVEI2:
    mStageRead.dataSrc |= ( uint32_t )opcode << 16;
    mStageWrite.regFlags.reg = mStageRead.regDst;
    mStageWrite.data = mStageRead.dataSrc;
    mLog->decodeMOVEI( 1, mStageRead.dataSrc );
    break;
  case Prefetch::IMULTN:
    mStageRead.instruction = DSPI::MM_IMULTN;
    mLog->decodeIMULTN( mStageCompute.regSrc, 4 * ( ( mMTXC & 16 ) ? mMacStage.size : 1 ), mMacStage.cnt );
    break;
  case Prefetch::IMACN:
    mStageRead.instruction = DSPI::MM_IMACN;
    mLog->decodeIMACN( mStageCompute.regSrc, 4 * ( ( mMTXC & 16 ) ? mMacStage.size : 1 ), mMacStage.cnt );
    break;
  case Prefetch::RESMAC:
    mStageRead.instruction = DSPI::MM_RESMAC;
    mLog->decodeRESMAC( mStageCompute.regDst );
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
    assert( ( mPC & 1 ) == 0 );
    uint32_t code = std::byteswap( mLocalRAM[( mPC - RAM_BASE )>>2] );
    uint32_t off = prefetchPush( code, mPC & 2 );

    if ( off )
    {
      mLog->prefetch( mPC, code );
      mLastLocalRAMAccessCycle = mCycle;
      mPC += off;
    }
  }

  mCycle += 1;
  mLog->flush();
}

std::pair<Jerry::Prefetch::PullStatus, uint16_t> Jerry::prefetchPull()
{
  if ( mPrefetch.queueSize > 0 )
  {
    uint16_t result = mPrefetch.queue & 0xffff;

    switch ( mPrefetch.status )
    {
    case Prefetch::OPCODE:
      switch ( ( DSPI )( result >> 10 ) )
      {
      case DSPI::MOVEI:
        mPrefetch.status = Prefetch::MOVEI1;
        break;
      case DSPI::MMULT:
        mPrefetch.status = Prefetch::IMULTN;
        mMacStage.size = mMTXC & 15;
        mMacStage.cnt = 0;
        mMacStage.addr = mMTXA;
        if ( mMacStage.addr < 0xf1b000 || mMacStage.addr > 0xf1bffc )
        {
          throw EmulationViolation{ "Invalid D_MTXA address of " } << mMacStage.addr;
        }
        if ( mMacStage.size < 3 )
        {
          throw EmulationViolation{ "MMULT with invalid size of " } << mMacStage.size;
        }
        break;
      default:
        break;
      }
      mPrefetch.queue >>= 16;
      mPrefetch.queueSize -= 1;
      return { Prefetch::OPCODE, result };
    case Prefetch::MOVEI1:
      mPrefetch.status = Prefetch::MOVEI2;
      mPrefetch.queue >>= 16;
      mPrefetch.queueSize -= 1;
      return { Prefetch::MOVEI1, result };
    case Prefetch::MOVEI2:
      mPrefetch.status = Prefetch::OPCODE;
      mPrefetch.queue >>= 16;
      mPrefetch.queueSize -= 1;
      return { Prefetch::MOVEI2, result };
    case Prefetch::IMULTN:
      mPrefetch.status = Prefetch::IMACN;
      return { Prefetch::IMULTN, 0 };
    case Prefetch::IMACN:
      if ( mMacStage.cnt < mMacStage.size )
      {
        return { Prefetch::IMACN, 0 };
      }
      else
      {
        mPrefetch.status = Prefetch::OPCODE;
        return { Prefetch::RESMAC, 0 };
      }
    default:
      assert( false );
      return { Prefetch::EMPTY, 0 };
    }
  }
  else
  {
    return { Prefetch::EMPTY, 0 };
  }
}

uint32_t Jerry::prefetchPush( uint32_t value, uint32_t oddWord )
{
  static constexpr std::array<uint64_t, 4> mask = {
    0x0000000000000000,
    0x000000000000ffff,
    0x00000000ffffffff,
    0x0000ffffffffffff
  };

  if ( oddWord && mPrefetch.queueSize <= 3 )
  {
    mPrefetch.queue &= mask[mPrefetch.queueSize];
    mPrefetch.queue |= ( uint64_t )( value & 0xffff ) << ( mPrefetch.queueSize * 16 );
    mPrefetch.queueSize += 1;
    return 2;
  }
  else if ( mPrefetch.queueSize <= 2 )
  {
    mPrefetch.queue &= mask[mPrefetch.queueSize];
    mPrefetch.queue |= ( uint64_t )( ( value >> 16 ) | ( value << 16 ) ) << ( mPrefetch.queueSize * 16 );
    mPrefetch.queueSize += 2;
    return 4;
  }
  else
  {
    return 0;
  }
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
  assert( regSrc >= 0 );

  if ( mRegStatus[regSrc] != FREE && mPortWriteDstReg != regSrc )
    return false;

  if ( mPortReadSrcReg >= 0 )
    return false;

  mPortReadSrcReg = regSrc;

  return true;
}

bool Jerry::portReadDst( uint32_t regDst )
{
  assert( regDst >= 0 );

  if ( mRegStatus[regDst] != FREE && mPortWriteDstReg != regDst )
    return false;

  if ( mPortReadDstReg >= 0 )
    return false;

  mPortReadDstReg = regDst;

  return true;
}

void Jerry::portReadDstAndHiddenCommit( uint32_t regDst )
{
  assert( mPortReadDstReg < 0 );
  assert( regDst >= 0 );

  if ( mRegStatus[regDst] != FREE )
  {
    if ( mPortWriteDstReg != regDst )
    {
      throw EmulationViolation{ "Indexed store of data from a long latency instruction" };
    }
    else
    {
      //a hack for indexed addressing done in compute stage
      mStageCompute.dataDst = mPortWriteDstData;
    }
  }
  else
  {
    //a hack for indexed addressing done in compute stage
    mStageCompute.dataDst = mRegs[mRegisterFile + regDst];
  }

  mPortReadDstReg = regDst;
}

bool Jerry::portReadBoth( uint32_t regSrc, uint32_t regDst )
{
  assert( regSrc >= 0 );
  assert( regDst >= 0 );

  if ( mRegStatus[regSrc] != FREE && mPortWriteDstReg != regSrc )
    return false;

  if ( mRegStatus[regDst] != FREE && mPortWriteDstReg != regDst )
    return false;

  if ( mPortWriteDstReg >= 0 && mPortWriteDstReg != regSrc && mPortWriteDstReg != regDst )
    return false;

  if ( mPortReadSrcReg >= 0 || mPortReadDstReg >= 0 )
    return false;

  mPortReadSrcReg = regSrc;
  mPortReadDstReg = regDst;

  return true;
}

void Jerry::dualPortCommit()
{
  if ( mPortWriteDstReg >= 0 )
  {
    if ( mPortWriteDstReg < 0x20 )
      mRegStatus[mPortWriteDstReg] = FREE;
    mRegs[(mRegisterFile + mPortWriteDstReg)&63] = mPortWriteDstData;
    mLog->portWriteDst( mRegisterFile + mPortWriteDstReg, mPortWriteDstData );
    mPortWriteDstReg = -1;
  }

  if ( mPortReadSrcReg >= 0 )
  {
    mStageRead.dataSrc = mRegs[(mRegisterFile + mPortReadSrcReg)&63];
    mLog->portReadSrc( mRegisterFile + mPortReadSrcReg, mStageRead.dataSrc );
    mPortReadSrcReg = -1;
  }

  if ( mPortReadDstReg >= 0 )
  {
    mStageRead.dataDst = mRegs[(mRegisterFile + mPortReadDstReg)&63];
    mLog->portReadDst( mRegisterFile + mPortReadDstReg, mStageRead.dataDst );
    mPortReadDstReg = -1;
  }
}

void Jerry::dualPortCommitMMULT( bool high )
{
  if ( mPortReadSrcReg >= 0 )
  {
    mStageRead.dataSrc = high ? mRegs[32 + mPortReadSrcReg] >> 16 : mRegs[32 + mPortReadSrcReg] & 0xffff;
    mLog->portReadSrcMMULT( 32 + mPortReadSrcReg, high, mStageRead.dataSrc );
    mPortReadSrcReg = -1;
  }
}

void Jerry::busGatePush( AdvanceResult result )
{
  if ( !mBusGate )
  {
    mBusGate = result;
  }
}

void Jerry::busGatePop()
{
  mBusGate = AdvanceResult::nop();
  mStageIO.state = StageIO::IDLE;
}
