#pragma once

#include "Opcodes.hpp"

class PipelineLog
{
public:
  PipelineLog();


  void prefetch( uint32_t address, uint32_t code );
  void decodeDSP( DSPI instr, uint32_t reg1, uint32_t reg2 );
  void decodeMOVEI( int stage, uint32_t data );
  void portImm( uint32_t value );
  void portReadSrc( uint32_t reg, uint32_t value );
  void portReadDst( uint32_t reg, uint32_t value );
  void portWriteDst( uint32_t reg, uint32_t value );
  void computeReg( uint32_t reg );
  void computeRegFlags( uint32_t reg );
  void computeFlags();
  void flush();

private:

  void init();
  char const* prefetchDSPMapper( uint32_t code );

  char mBuffer[ 256 ];
  int mPrefetchDSPMapperState = 0;
  char mPrefetchDSPMapperBuf[8];

};
