#include "Tom.hpp"
#include "Ex.hpp"

void Tom::debugWrite( uint32_t address, uint32_t data )
{
}

void Tom::debugWrite( uint32_t address, std::span<uint32_t const> data )
{
}

AdvanceResult Tom::busCycleIdle()
{
  return AdvanceResult::nop();
}

AdvanceResult Tom::busCycleWrite()
{
  return AdvanceResult::nop();
}

AdvanceResult Tom::busCycleRead( uint8_t value )
{
  return AdvanceResult::nop();
}

AdvanceResult Tom::busCycleRead( uint16_t value )
{
  return AdvanceResult::nop();
}

AdvanceResult Tom::busCycleRead( uint32_t value )
{
  return AdvanceResult::nop();
}

AdvanceResult Tom::busCycleRead( uint64_t value )
{
  return AdvanceResult::nop();
}
