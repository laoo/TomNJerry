#pragma once

#include "Opcodes.hpp"
#include "RegFlags.hpp"

class PipelineLog
{
public:
  PipelineLog();


  void prefetch( uint32_t address, uint32_t code );
  void decodeDSP( DSPI instr, uint32_t reg1, uint32_t reg2 );
  void decodeMOVEI( int stage, uint32_t data );
  void decodeIMACN( uint32_t reg, int width, int count );
  void decodeIMULTN( uint32_t reg, int width, int count );
  void decodeRESMAC( uint32_t reg2 );
  void portImm( uint32_t value );
  void portCond( uint32_t value );
  void portReadSrc( uint32_t reg, uint32_t value );
  void portReadSrcMMULT( uint32_t reg, bool high, uint32_t value );
  void portReadDst( uint32_t reg, uint32_t value );
  void portWriteDst( uint32_t reg, uint32_t value );
  void computeReg( RegFlags regFlags );
  void computeRegFlags( RegFlags regFlags );
  void computeFlags( RegFlags regFlags );
  void computeMul();
  void computeMac();
  void computeMac( RegFlags regFlags );
  void computeIndex();
  void div( int cycle );
  void storeLong( uint32_t address, uint32_t value );
  void storeWord( uint32_t address, uint16_t value );
  void storeByte( uint32_t address, uint8_t value );
  void loadLong( uint32_t address, uint32_t value );
  void loadWord( uint32_t address, uint16_t value );
  void loadByte( uint32_t address, uint8_t value );
  void flush();

private:

  void init();
  char const* prefetchDSPMapper( uint32_t code );

  char mBuffer[ 256 ];
  int mPrefetchDSPMapperState = 0;
  char mPrefetchDSPMapperBuf[8];

};

static constexpr std::array<uint32_t, 32> tabAddSubQ = { 32, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };

