#include "DSP.hpp"

DSP::DSP()
{
  for ( size_t i = 0; i < mPipeline.size(); ++i )
  {
    mPipeline[i] = ExecutionUnit::create();
  }
}

void DSP::advanve()
{
}

void DSP::storeLong( uint32_t address, uint32_t data )
{
}

uint32_t DSP::loadLong( uint32_t address )
{
  return 0;
}
