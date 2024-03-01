#include "Tom.hpp"
#include "Ex.hpp"

AdvanceResult Tom::busCycle()
{
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
}

void Tom::writePhrase( uint32_t address, uint64_t data )
{
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

uint64_t Tom::busCycleRequestReadPhrase( uint32_t address )
{
  busCycle();

  mLastLocalRAMAccessCycle = mCycle;
  return readPhrase( address );
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

void Tom::busCycleRequestWritePhrase( uint32_t address, uint64_t data )
{
  busCycle();

  mLastLocalRAMAccessCycle = mCycle;
  writePhrase( address, data );
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
}
