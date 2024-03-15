#pragma once

#include "Opcodes.hpp"
#include "Common.hpp"

enum WarnReason
{
  WARN_UNIMPLEMENTED,
  WARN_BAD_SIZE,
  WARN_REG_IN_USE,
  WARN_BAD_ADDRESS,
  WARN_ILLEGAL_CONDITION,
  WARN_IO_PENDING,
  WARN_DIV_TOO_CLOSE,
  WARN_BAD_MTXA,
  WARN_BAD_MATRIX_SIZE,
  WARN_REGPAGE_HAZARD,
  WARN__COUNT
};

class PipelineLog
{
public:
  PipelineLog();


  void prefetch( uint32_t address );
  void instrAddr( uint32_t address );
  void decodeDSP( DSPI instr, uint32_t reg1, uint32_t reg2 );
  void decodeMOVEI( int stage, uint32_t data );
  void decodeIMACN( uint32_t reg, int width, int count );
  void decodeIMULTN( uint32_t reg, int width, int count );
  void decodeRESMAC( uint32_t reg2 );
  void portImm( uint32_t value );
  void portCond( uint32_t value );
  void jumpT();
  void jumpF();
  void jr( uint32_t condition, uint32_t address );
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
  void warn( WarnReason reason );
  void tagUninterruptibleSequence();
  void flush();

private:

  void init();
  char const* prefetchDSPMapper( uint32_t code );

  char mBuffer[ 256 ];
  int mPrefetchDSPMapperState = 0;
  char mPrefetchDSPMapperBuf[8];

};

#ifdef LOG_PIPELINE

#define LOG_PREFETCH( ADDRESS ) mLog->prefetch( ADDRESS );
#define LOG_INSTRADDR( ADDRESS ) mLog->instrAddr( ADDRESS );
#define LOG_DECODEDSP( INSTR, REG1, REG2 ) mLog->decodeDSP( INSTR, REG1, REG2 );
#define LOG_DECODEMOVEI( STAGE, DATA ) mLog->decodeMOVEI( STAGE, DATA );
#define LOG_DECODEIMACN( REG, WIDTH, COUNT ) mLog->decodeIMACN( REG, WIDTH, COUNT );
#define LOG_DECODEIMULTN( REG, WIDTH, COUNT ) mLog->decodeIMULTN( REG, WIDTH, COUNT );
#define LOG_DECODERESMAC( REG2 ) mLog->decodeRESMAC( REG2 );
#define LOG_PORTIMM( VALUE ) mLog->portImm( VALUE );
#define LOG_PORTCOND( VALUE ) mLog->portCond( VALUE );
#define LOG_JUMPT() mLog->jumpT();
#define LOG_JUMPF() mLog->jumpF();
#define LOG_JR( COND, ADDR ) mLog->jr( COND, ADDR );
#define LOG_PORTREADSRC( REG, VALUE ) mLog->portReadSrc( REG, VALUE );
#define LOG_PORTREADSRCMMULT( REG, HIGH, VALUE ) mLog->portReadSrcMMULT( REG, HIGH, VALUE );
#define LOG_PORTREADDST( REG, VALUE ) mLog->portReadDst( REG, VALUE );
#define LOG_PORTWRITEDST( REG, VALUE ) mLog->portWriteDst( REG, VALUE );
#define LOG_COMPUTEREG( REGFLAGS ) mLog->computeReg( REGFLAGS );
#define LOG_COMPUTEREGFLAGS( REGFLAGS ) mLog->computeRegFlags( REGFLAGS );
#define LOG_COMPUTEFLAGS( REGFLAGS ) mLog->computeFlags( REGFLAGS );
#define LOG_COMPUTEMUL() mLog->computeMul();
#define LOG_COMPUTEMAC() mLog->computeMac();
#define LOG_COMPUTEMACARG( REGFLAGS ) mLog->computeMac( REGFLAGS );
#define LOG_COMPUTEINDEX() mLog->computeIndex();
#define LOG_DIV( CYCLE ) mLog->div( CYCLE );
#define LOG_STORELONG( ADDRESS, VALUE ) mLog->storeLong( ADDRESS, VALUE );
#define LOG_STOREWORD( ADDRESS, VALUE ) mLog->storeWord( ADDRESS, VALUE );
#define LOG_STOREBYTE( ADDRESS, VALUE ) mLog->storeByte( ADDRESS, VALUE );
#define LOG_LOADLONG( ADDRESS, VALUE ) mLog->loadLong( ADDRESS, VALUE );
#define LOG_LOADWORD( ADDRESS, VALUE ) mLog->loadWord( ADDRESS, VALUE );
#define LOG_LOADBYTE( ADDRESS, VALUE ) mLog->loadByte( ADDRESS, VALUE );
#define LOG_WARN( REASON ) mLog->warn( REASON );
#define LOG_TAGUNINTERRUPTIBLESEQUENCE() mLog->tagUninterruptibleSequence();
#define LOG_FLUSH() mLog->flush();

#else

#define LOG_PREFETCH( ADDRESS )
#define LOG_INSTRADDR( ADDRESS )
#define LOG_DECODEDSP( INSTR, REG1, REG2 )
#define LOG_DECODEMOVEI( STAGE, DATA )
#define LOG_DECODEIMACN( REG, WIDTH, COUNT )
#define LOG_DECODEIMULTN( REG, WIDTH, COUNT )
#define LOG_DECODERESMAC( REG2 )
#define LOG_PORTIMM( VALUE )
#define LOG_PORTCOND( VALUE )
#define LOG_JUMPT()
#define LOG_JUMPF()
#define LOG_JR( COND, ADDR )
#define LOG_PORTREADSRC( REG, VALUE )
#define LOG_PORTREADSRCMMULT( REG, HIGH, VALUE )
#define LOG_PORTREADDST( REG, VALUE )
#define LOG_PORTWRITEDST( REG, VALUE )
#define LOG_COMPUTEREG( REGFLAGS )
#define LOG_COMPUTEREGFLAGS( REGFLAGS )
#define LOG_COMPUTEFLAGS( REGFLAGS )
#define LOG_COMPUTEMUL()
#define LOG_COMPUTEMAC()
#define LOG_COMPUTEMACARG( REGFLAGS )
#define LOG_COMPUTEINDEX()
#define LOG_DIV( CYCLE )
#define LOG_STORELONG( ADDRESS, VALUE )
#define LOG_STOREWORD( ADDRESS, VALUE )
#define LOG_STOREBYTE( ADDRESS, VALUE )
#define LOG_LOADLONG( ADDRESS, VALUE )
#define LOG_LOADWORD( ADDRESS, VALUE )
#define LOG_LOADBYTE( ADDRESS, VALUE )
#define LOG_WARN( REASON )
#define LOG_TAGUNINTERRUPTIBLESEQUENCE()
#define LOG_FLUSH()

#endif

