#include "PipelineLog.hpp"
#include "RegFlags.hpp"
#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_NOFLOAT
#define STB_SPRINTF_STATIC
//modified stb_sprintf.h to remove final null-char terminator at line 1349
#include "stb_sprintf.h"

namespace
{

struct JumpCondition
{
  char const* name;
  int size;
};

static constexpr JumpCondition kJumpConditions[32] =
{
  { "", 0 },
  { "!z", 2 },
  { "z", 1 },
  { "?", 1 },
  { "!c", 2 },
  { "!c !z", 5 },
  { "!c z", 4 },
  { "?", 1 },

  { "c", 1 },
  { "c !z", 4 },
  { "c z", 3 },
  { "?", 1 },
  { "?", 1 },
  { "?", 1 },
  { "?", 1 },
  { "?", 1 },

  { "?", 1 },
  { "?", 1 },
  { "?", 1 },
  { "?", 1 },
  { "!n", 2 },
  { "!n !z", 5 },
  { "!n z", 4 },
  { "?", 1 },
  { "n", 1 },
  { "n !z", 4 },
  { "n z", 3 },
  { "?", 1 },
  { "?", 1 },
  { "?", 1 },
  { "?", 1 },
  { "!", 1 }
};

static constexpr char ZFlag[] = "*-Z";
static constexpr char CFlag[] = "*-C";
static constexpr char NFlag[] = "*-N";

}

PipelineLog::PipelineLog() : mBuffer{}
{
  init();
}

void PipelineLog::prefetch( uint32_t address )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345 | 012345: store   r31,(r31+r31) |    #31            #31       | C31' NCZ | W31:01234567
  stbsp_sprintf( mBuffer + 0x00, "%06x", address );
}

void PipelineLog::instrAddr( uint32_t address )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345 | 012345: store   r31,(r31+r31) |    #31            #31       | C31' NCZ | W31:01234567
  stbsp_sprintf( mBuffer + 0x09, "%06x:", address );
}

void PipelineLog::decodeDSP( DSPI instr, uint32_t reg1, uint32_t reg2 )
{
  switch ( instr )
  {
  case DSPI::ADD:
    stbsp_sprintf( mBuffer + 0x11, "add     r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::ADDC:
    stbsp_sprintf( mBuffer + 0x11, "addc    r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::ADDQ:
    stbsp_sprintf( mBuffer + 0x11, "addq    #%02x,r%02d", reg1, reg2 );
    break;
  case DSPI::ADDQT:
    stbsp_sprintf( mBuffer + 0x11, "addqt   #%02x,r%02d", reg1, reg2 );
    break;
  case DSPI::SUB:
    stbsp_sprintf( mBuffer + 0x11, "sub     r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::SUBC:
    stbsp_sprintf( mBuffer + 0x11, "subc    r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::SUBQ:
    stbsp_sprintf( mBuffer + 0x11, "subq    #%02x,r%02d", reg1, reg2 );
    break;
  case DSPI::SUBQT:
    stbsp_sprintf( mBuffer + 0x11, "subqt   #%02x,r%02d", reg1, reg2 );
    break;
  case DSPI::NEG:
    stbsp_sprintf( mBuffer + 0x11, "neg     r%02d", reg2 );
    break;
  case DSPI::AND:
    stbsp_sprintf( mBuffer + 0x11, "and     r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::OR:
    stbsp_sprintf( mBuffer + 0x11, "or      r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::XOR:
    stbsp_sprintf( mBuffer + 0x11, "xor     r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::NOT:
    stbsp_sprintf( mBuffer + 0x11, "not     r%02d", reg2 );
    break;
  case DSPI::BTST:
    stbsp_sprintf( mBuffer + 0x11, "btst    #%02x,r%02d", reg1, reg2 );
    break;
  case DSPI::BSET:
    stbsp_sprintf( mBuffer + 0x11, "bset    #%02x,r%02d", reg1, reg2 );
    break;
  case DSPI::BCLR:
    stbsp_sprintf( mBuffer + 0x11, "bclr    #%02x,r%02d", reg1, reg2 );
    break;
  case DSPI::MULT:
    stbsp_sprintf( mBuffer + 0x11, "mult    r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::IMULT:
    stbsp_sprintf( mBuffer + 0x11, "imult   r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::IMULTN:
    stbsp_sprintf( mBuffer + 0x11, "imultn  r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::RESMAC:
    stbsp_sprintf( mBuffer + 0x11, "resmac  r%02d", reg2 );
    break;
  case DSPI::IMACN:
    stbsp_sprintf( mBuffer + 0x11, "imacn   r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::DIV:
    stbsp_sprintf( mBuffer + 0x11, "div     r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::ABS:
    stbsp_sprintf( mBuffer + 0x11, "abs     r%02d", reg2 );
    break;
  case DSPI::SH:
    stbsp_sprintf( mBuffer + 0x11, "sh      r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::SHLQ:
    stbsp_sprintf( mBuffer + 0x11, "shlq    #%02x,r%02d", 32-reg1, reg2 );
    break;
  case DSPI::SHRQ:
    stbsp_sprintf( mBuffer + 0x11, "shrq    #%02x,r%02d", reg1, reg2 );
    break;
  case DSPI::SHA:
    stbsp_sprintf( mBuffer + 0x11, "sha     r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::SHARQ:
    stbsp_sprintf( mBuffer + 0x11, "sharq   #%02x,r%02d", reg1, reg2 );
    break;
  case DSPI::ROR:
    stbsp_sprintf( mBuffer + 0x11, "ror     r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::RORQ:
    stbsp_sprintf( mBuffer + 0x11, "rorq    #%02x,r%02d", reg1, reg2 );
    break;
  case DSPI::CMP:
    stbsp_sprintf( mBuffer + 0x11, "cmp     r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::CMPQ:
    stbsp_sprintf( mBuffer + 0x11, "cmpq    #%02x,r%02d", reg1, reg2 );
    break;
  case DSPI::SUBQMOD:
    stbsp_sprintf( mBuffer + 0x11, "sumbmod #%02x,r%02d", reg1, reg2 );
    break;
  case DSPI::SAT16S:
    stbsp_sprintf( mBuffer + 0x11, "sat16s  r%02d", reg2 );
    break;
  case DSPI::MOVE:
    stbsp_sprintf( mBuffer + 0x11, "move    r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::MOVEQ:
    stbsp_sprintf( mBuffer + 0x11, "moveq   #%02x,r%02d", reg1, reg2 );
    break;
  case DSPI::MOVETA:
    stbsp_sprintf( mBuffer + 0x11, "moveta  r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::MOVEFA:
    stbsp_sprintf( mBuffer + 0x11, "movefa  r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::MOVEI:
    stbsp_sprintf( mBuffer + 0x11, "movei   #--------,r%02d", reg2);
    break;
  case DSPI::LOADB:
    stbsp_sprintf( mBuffer + 0x11, "loadb   (r%02d),r%02d", reg1, reg2 );
    break;
  case DSPI::LOADW:
    stbsp_sprintf( mBuffer + 0x11, "loadw   (r%02d),r%02d", reg1, reg2 );
    break;
  case DSPI::LOAD:
    stbsp_sprintf( mBuffer + 0x11, "load    (r%02d),r%02d", reg1, reg2 );
    break;
  case DSPI::SAT32S:
    stbsp_sprintf( mBuffer + 0x11, "sat32s  r%02d", reg2 );
    break;
  case DSPI::LOAD14N:
    stbsp_sprintf( mBuffer + 0x11, "load    (r14+#%02x),r%02d", tabAddSubQ[reg1] * 4, reg2);
    break;
  case DSPI::LOAD15N:
    stbsp_sprintf( mBuffer + 0x11, "load    (r15+#%02x),r%02d", tabAddSubQ[reg1], reg2 );
    break;
  case DSPI::STOREB:
    stbsp_sprintf( mBuffer + 0x11, "storeb  r%02d,(r%02d)", reg2, reg1 );
    break;
  case DSPI::STOREW:
    stbsp_sprintf( mBuffer + 0x11, "storew  r%02d,(r%02d)", reg2, reg1 );
    break;
  case DSPI::STORE:
    stbsp_sprintf( mBuffer + 0x11, "store   r%02d,(r%02d)", reg2, reg1 );
    break;
  case DSPI::MIRROR:
    stbsp_sprintf( mBuffer + 0x11, "mirror  r%02d", reg2 );
    break;
  case DSPI::STORE14N:
    stbsp_sprintf( mBuffer + 0x11, "store   r%02d,(r14+#%02x)", reg2, tabAddSubQ[reg1] * 4 );
    break;
  case DSPI::STORE15N:
    stbsp_sprintf( mBuffer + 0x11, "store   r%02d,(r15+#%02x)", reg2, tabAddSubQ[reg1] * 4 );
    break;
  case DSPI::MOVEPC:
    stbsp_sprintf( mBuffer + 0x11, "move    pc,r%02d",       reg2 );
    break;
  case DSPI::JUMP:
    stbsp_sprintf( mBuffer + 0x11, "jump    %s%s(r%02d)", kJumpConditions[reg2 & 31].name, kJumpConditions[reg2 & 31].size == 0 ? "" : ",", reg1);
    break;
  case DSPI::JR:
    stbsp_sprintf( mBuffer + 0x11, "jr      %s%s%s%d", kJumpConditions[reg2 & 31].name, kJumpConditions[reg2 & 31].size == 0 ? "" : ",", ( reg1 & 5 ) == 0?"+":"", ( ( int8_t )( reg1 << 3 ) / 8 ));
    break;
  case DSPI::MMULT:
    stbsp_sprintf( mBuffer + 0x11, "mmult   r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::MTOI:
    stbsp_sprintf( mBuffer + 0x11, "mtoi    r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::NORMI:
    stbsp_sprintf( mBuffer + 0x11, "normi   r%02d,r%02d", reg1, reg2 );
    break;
  case DSPI::NOP:
    stbsp_sprintf( mBuffer + 0x11, "nop" );
    break;
  case DSPI::LOAD14R:
    stbsp_sprintf( mBuffer + 0x11, "load    (r14+r%02d),r%02d", reg1, reg2 );
    break;
  case DSPI::LOAD15R:
    stbsp_sprintf( mBuffer + 0x11, "load    (r15+#%02d),r%02d", reg1, reg2 );
    break;
  case DSPI::STORE14R:
    stbsp_sprintf( mBuffer + 0x11, "store   r%02d,(r14+r%02d)", reg2, reg1 );
    break;
  case DSPI::STORE15R:
    stbsp_sprintf( mBuffer + 0x11, "store   r%02d,(r15+r%02d)", reg2, reg1 );
    break;
  case DSPI::ADDQMOD:
    stbsp_sprintf( mBuffer + 0x11, "addqmod #%02d,r%02d", reg1, reg2 );
    break;
  }
}

void PipelineLog::decodeMOVEI( int stage, uint32_t data )
{
  if ( stage == 0 )
  {
    stbsp_sprintf( mBuffer + 0x1a, "----%04x", data );
    stbsp_sprintf( mBuffer + 0x31, "#%04x", data );
  }
  else
  {
    stbsp_sprintf( mBuffer + 0x1a, "%04x----", data >> 16 );
    stbsp_sprintf( mBuffer + 0x31, "#%04x", data >> 16 );
  }

  tagUninterruptibleSequence();
}

void PipelineLog::decodeIMULTN( uint32_t reg, int width, int count )
{
  stbsp_sprintf( mBuffer + 0x11, "imultn  r%02d%c,m+%d", reg + 32 + count / 2, count & 1 ? 'h' : 'l', width * count );
  tagUninterruptibleSequence();
}

void PipelineLog::decodeRESMAC( uint32_t reg2 )
{
  stbsp_sprintf( mBuffer + 0x11, "resmac  r%02d", reg2 );
  tagUninterruptibleSequence();
}

void PipelineLog::decodeIMACN( uint32_t reg, int width, int count )
{
  stbsp_sprintf( mBuffer + 0x11, "imacn   r%02d%c,m+%d", reg + 32 + count / 2, count & 1 ? 'h' : 'l', width * count );
  tagUninterruptibleSequence();
}

void PipelineLog::portImm( uint32_t value )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcde
  //012345 | 012345: store   r31,(r31+r31) | R31':01234567 R31':01234567 | C31' NCZ | W31':01234567
  stbsp_sprintf( mBuffer + 0x33, "#%02x", value );
}

void PipelineLog::portCond( uint32_t value )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345 | 012345: store   r31,(r31+r31) | R31':01234567 R31':01234567 | C31' NCZ | W31':01234567
  int off = stbsp_sprintf( mBuffer + 0x36 - kJumpConditions[value & 31].size, "%s", kJumpConditions[value & 31].name );
}

void PipelineLog::portReadSrc( uint32_t reg, uint32_t value )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345 | 012345: store   r31,(r31+r31) | R31':01234567 R31':01234567 | C31' NCZ | W31':01234567
  stbsp_sprintf( mBuffer + 0x29, "R%02d%c:%08x", reg & 31, reg >= 32 ? '"' : '\'', value);
}

void PipelineLog::portReadSrcMMULT( uint32_t reg, bool high, uint32_t value )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345 | 012345: store   r31,(r31+r31) | R31':01234567 R31':01234567 | C31' NCZ | W31':01234567
  stbsp_sprintf( mBuffer + 0x29, "R%02d\"%c:    %04x", reg & 31, high ? 'h':'l', value );
}

void PipelineLog::portReadDst( uint32_t reg, uint32_t value )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345 | 012345: store   r31,(r31+r31) | R31':01234567 R31':01234567 | C31' NCZ | W31':01234567
  stbsp_sprintf( mBuffer + 0x37, "R%02d%c:%08x", reg & 31, reg >= 32 ? '"' : '\'', value );
}

void PipelineLog::portWriteDst( uint32_t reg, uint32_t value )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345 | 012345: store   r31,(r31+r31) | R31':01234567 R31':01234567 | C31' NCZ | W31':01234567
  stbsp_sprintf( mBuffer + 0x52, "W%02d%c:%08x", reg & 31, reg >= 32 ? '"' : '\'', value );
}

void PipelineLog::computeReg( RegFlags regFlags )
{
  int32_t reg = regFlags.reg;
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345 | 012345: store   r31,(r31+r31) | R31':01234567 R31':01234567 | C31' NCZ | W31':01234567
  stbsp_sprintf( mBuffer + 0x47, "%02d%c", reg & 31, reg >= 32 ? '"' : '\'' );
}

void PipelineLog::computeRegFlags( RegFlags regFlags )
{
  int32_t reg = regFlags.reg;
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345 | 012345: store   r31,(r31+r31) | R31':01234567 R31':01234567 | C31' NCZ | W31':01234567
  stbsp_sprintf( mBuffer + 0x47, "%02d%c %c%c%c", reg & 31, reg >= 32 ? '"' : '\'', NFlag[regFlags.n+1], CFlag[regFlags.c + 1], ZFlag[regFlags.z + 1] );
}

void PipelineLog::computeFlags( RegFlags flags )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345 | 012345: store   r31,(r31+r31) | R31':01234567 R31':01234567 | C31' NCZ | W31':01234567
  stbsp_sprintf( mBuffer + 0x4c, "%c%c%c", NFlag[flags.n + 1], CFlag[flags.c + 1], ZFlag[flags.z + 1] );
}

void PipelineLog::jumpT()
{
  mBuffer[0x4a] = 'T';
}

void PipelineLog::jumpF()
{
  mBuffer[0x4a] = 'F';
}

void PipelineLog::computeMul()
{
  mBuffer[0x47] = 'm';
  mBuffer[0x48] = 'u';
  mBuffer[0x49] = 'l';
}

void PipelineLog::computeMac()
{
  mBuffer[0x47] = 'm';
  mBuffer[0x48] = 'a';
  mBuffer[0x49] = 'c';
}

void PipelineLog::computeMac( RegFlags flags )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345 | 012345: store   r31,(r31+r31) | R31':01234567 R31':01234567 | C31' NCZ | W31':01234567
  stbsp_sprintf( mBuffer + 0x47, "mac %c%c%c", NFlag[flags.n + 1], CFlag[flags.c + 1], ZFlag[flags.z + 1] );
}

void PipelineLog::computeIndex()
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345 | 012345: store   r31,(r31+r31) | R31':01234567 R31':01234567 |  index   | W31':01234567
  mBuffer[0x48] = 'i';
  mBuffer[0x49] = 'n';
  mBuffer[0x4a] = 'd';
  mBuffer[0x4b] = 'e';
  mBuffer[0x4c] = 'x';
}

void PipelineLog::div( int cycle )
{
  stbsp_sprintf( mBuffer + 0x45, "%x", cycle );
}

void PipelineLog::storeLong( uint32_t address, uint32_t value )
{
  //000000000000000011111111111111112222222222222222333333333333333344444444444444445555555555555555666666666666666777777777
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01234567
  //012345 | 012345: store   r31,(r31+r31) | R31':01234567 R31':01234567 | C31' NCZ | W31':01234567 | S(123456)=12345678
  stbsp_sprintf( mBuffer + 0x62, "S(%06x)=%08x", address, value );
}

void PipelineLog::storeWord( uint32_t address, uint16_t value )
{
  //000000000000000011111111111111112222222222222222333333333333333344444444444444445555555555555555666666666666666777777777
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01234567
  //012345 | 012345: store   r31,(r31+r31) | R31':01234567 R31':01234567 | C31' NCZ | W31':01234567 | S(123456)=12345678
  stbsp_sprintf( mBuffer + 0x62, "S(%06x)=%04x", address, value );
}

void PipelineLog::storeByte( uint32_t address, uint8_t value )
{
  //000000000000000011111111111111112222222222222222333333333333333344444444444444445555555555555555666666666666666777777777
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01234567
  //012345 | 012345: store   r31,(r31+r31) | R31':01234567 R31':01234567 | C31' NCZ | W31':01234567 | S(123456)=12345678
  stbsp_sprintf( mBuffer + 0x62, "S(%06x)=%02x", address, value );
}

void PipelineLog::loadLong( uint32_t address, uint32_t value )
{
  //000000000000000011111111111111112222222222222222333333333333333344444444444444445555555555555555666666666666666777777777
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01234567
  //012345 | 012345: store   r31,(r31+r31) | R31':01234567 R31':01234567 | C31' NCZ | W31':01234567 | L(123456):12345678
  stbsp_sprintf( mBuffer + 0x62, "L(%06x):%08x", address, value );
}

void PipelineLog::loadWord( uint32_t address, uint16_t value )
{
  //000000000000000011111111111111112222222222222222333333333333333344444444444444445555555555555555666666666666666777777777
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01234567
  //012345 | 012345: store   r31,(r31+r31) | R31':01234567 R31':01234567 | C31' NCZ | W31':01234567 | L(123456):12345678
  stbsp_sprintf( mBuffer + 0x62, "L(%06x):%04x", address, value );
}

void PipelineLog::loadByte( uint32_t address, uint8_t value )
{
  //000000000000000011111111111111112222222222222222333333333333333344444444444444445555555555555555666666666666666777777777
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01234567
  //012345 | 012345: store   r31,(r31+r31) | R31':01234567 R31':01234567 | C31' NCZ | W31':01234567 | L(123456):12345678
  stbsp_sprintf( mBuffer + 0x62, "L(%06x):%02x", address, value );
}

void PipelineLog::warn( WarnReason reason )
{
  static constexpr std::array<char const*, 2> kWarnReasons =
  {
    "? memSize\n",
    "? regInUse\n"
  };

  assert( (size_t)reason < kWarnReasons.size() );

  stbsp_sprintf( mBuffer + 0x75, kWarnReasons[(size_t)reason] );
}

void PipelineLog::tagUninterruptibleSequence()
{
  mBuffer[0x07] = ':';
}

void PipelineLog::flush()
{
  std::cout << mBuffer;
  init();
}

void PipelineLog::init()
{
                                      //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555555566666666666666677777777777777777
                                      //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
  static const char stringTemplate[] = "       |                               |                             |          |               |                    |\n\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\n";
  memcpy( mBuffer, stringTemplate, sizeof( stringTemplate ) );
}
