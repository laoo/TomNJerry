#include "Tom.hpp"
#include "PipelineLog.hpp"
#include "Ex.hpp"

void Tom::checkInterrupt()
{

}

void Tom::divideUnit()
{

}

void Tom::io()
{

}

void Tom::stageWrite()
{

}

void Tom::compute()
{

}

void Tom::stageRead()
{

}

void Tom::decode()
{

}

void Tom::prefetch()
{

}

void Tom::halfCycle()
{
  checkInterrupt();
  divideUnit();
  io();
  stageWrite();
  compute();
  stageRead();
  decode();
  prefetch();
  mCycle += 1;
}

void Tom::ctrlSet( uint16_t value )
{
}

void Tom::flagsSet( uint16_t value )
{
}

AdvanceResult Tom::busCycle()
{
  if ( mCtrl.gpugo )
  {
    halfCycle();
    halfCycle();
  }
  return mBusGate;
}

void Tom::ackWrite()
{
}

void Tom::ackReadByteRequest( uint8_t value )
{
}

void Tom::ackReadWordRequest( uint16_t value )
{
}

void Tom::ackReadLongRequest( uint32_t value )
{
}

void Tom::ackReadPhraseRequest( uint64_t value )
{
}

uint32_t Tom::getReg( int32_t index ) const
{
  return mRegs[GlobalReg{ index }];
}

uint64_t Tom::clock() const
{
  return mClock;
}

uint16_t Tom::readWord( uint32_t address ) const
{
  return 0;
}

uint32_t Tom::readLong( uint32_t address ) const
{
  return 0;
}

uint64_t Tom::readPhrase( uint32_t address ) const
{
  return 0;
}

void Tom::writeWord( uint32_t address, uint16_t data )
{
}

void Tom::writeLong( uint32_t address, uint32_t data )
{
  assert( ( address & 0xff0000 ) == 0xf00000 );
  if ( ( address & 3 ) != 0 )
  {
    LOG_WARN( WARN_BAD_ADDRESS );
    address &= ~3;
  }

  switch ( address )
  {
  case G_FLAGS:
    flagsSet( data );
    break;
  case G_MTXC:
    mMTXC = data;
    break;
  case G_MTXA:
    mMTXA = data;
    break;
  case G_END:
    if ( ( data & 1 ) == 0 || ( data & 4 ) == 0 )
      LOG_WARN( WARN_UNIMPLEMENTED );
    break;
  case G_PC:
    if ( data < RAM_BASE || data >= RAM_BASE + RAM_SIZE )
      throw Ex{ "Jerry unsupported PC " } << std::hex << data;
    mPC = data;
    break;
  case G_CTRL:
    ctrlSet( data );
    break;
  case G_HIDATA:
    mHiData = data;
    break;
  case G_DIVCTRL:
    mDivCtrl = data;
    break;
  default:
    if ( address >= RAM_BASE && address < RAM_BASE + RAM_SIZE )
    {
      mLocalRAM[( address - RAM_BASE ) / sizeof( uint32_t )] = std::byteswap( data );
    }
    else if( address >= CLUT && address < CLUT + 0x400 )
    {
    }
    else if ( address >= LBUFA && address < LBUFC + 0x5a0 )
    {
    }
    else
    {
      writeWord( address, data >> 16 );
      writeWord( address + 2, data & 0xffff );
    }
    break;
  }
}

uint16_t Tom::busCycleRequestReadWord( uint32_t address )
{
  busCycle();

  mLastLocalRAMAccessCycle = mCycle;
  return readWord( address );
}

uint32_t Tom::busCycleRequestReadLong( uint32_t address )
{
  busCycle();

  mLastLocalRAMAccessCycle = mCycle;
  return readLong( address );
}

void Tom::busCycleRequestWriteWord( uint32_t address, uint16_t data )
{
  busCycle();

  mLastLocalRAMAccessCycle = mCycle;
  writeWord( address, data );
}

void Tom::busCycleRequestWriteLong( uint32_t address, uint32_t data )
{
  busCycle();

  mLastLocalRAMAccessCycle = mCycle;
  writeLong( address, data );
}

void Tom::debugWrite( uint32_t address, std::span<uint32_t const> data )
{
  if ( address & 3 )
    throw Ex{ "Tom::debugWrite: Unaligned address " } << std::hex << address;

  if ( address < RAM_BASE || address + data.size() * sizeof( uint32_t ) >= RAM_BASE + RAM_SIZE )
    throw Ex{ "Tom::debugWrite: Unhandled address " } << std::hex << address;

  std::copy( data.begin(), data.end(), mLocalRAM.begin() + ( address - RAM_BASE ) / sizeof( uint32_t ) );
}

Tom::~Tom()
{
}

Tom::Tom( bool isNTSC ) : mClock{ isNTSC ? NTSC_CLOCK : PAL_CLOCK }
{
  std::ranges::fill( mLocalRAM, std::byteswap( ( uint32_t )( ( uint16_t )GPUI::NOP << 10 | ( uint16_t )GPUI::NOP << ( 10 + 16 ) ) ) );
}
