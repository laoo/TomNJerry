#include "Bus.hpp"
#include "Jerry.hpp"
#include "RAM.hpp"

Bus::Bus( std::shared_ptr<RAM> ram, std::shared_ptr<Jerry> jerry ) : mRAM{ std::move( ram ) }, mJerry{ std::move( jerry ) }
{
}

std::shared_ptr<Bus> Bus::create( std::shared_ptr<RAM> ram, std::shared_ptr<Jerry> jerry )
{
  return std::shared_ptr<Bus>{ new Bus( std::move( ram ), std::move( jerry ) ) };
}

int64_t Bus::advance( int64_t cycles )
{
  for ( uint64_t i = 0; i < cycles; i += 4 )
  {
    mJerry->busCycle();
    mJerry->busCycle();
    mJerry->busCycle();
    auto req = mJerry->busCycle();
    switch ( req.getOperation() )
    {
    case AdvanceResult::kByteFlag:
      mJerry->ackReadByteRequest( mRAM->readByte( req.getAddress() ) );
      break;
    case AdvanceResult::kWordFlag:
      mJerry->ackReadWordRequest( mRAM->readWord( req.getAddress() ) );
      break;
    case AdvanceResult::kLongFlag:
      mJerry->ackReadLongRequest( mRAM->readLong( req.getAddress() ) );
      break;
    case AdvanceResult::kWriteFlag | AdvanceResult::kByteFlag:
      mRAM->writeByte( req.getAddress(), ( uint8_t )req.getValue() );
      mJerry->ackWrite();
      break;
    case AdvanceResult::kWriteFlag | AdvanceResult::kWordFlag:
      mRAM->writeWord( req.getAddress(), ( uint16_t )req.getValue() );
      mJerry->ackWrite();
      break;
    case AdvanceResult::kWriteFlag | AdvanceResult::kLongFlag:
      mRAM->writeLong( req.getAddress(), req.getValue() );
      mJerry->ackWrite();
      break;
    case AdvanceResult::kFinishFlag:
      return i;
    }
  }

  return cycles;
}
