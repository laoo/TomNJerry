#include "PipelineLog.hpp"
#include "RegFlags.hpp"

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

static constexpr std::array<char const*, 64> kPrefetchDSPOpcodes =
{
  "add    ",
  "addc   ",
  "addq   ",
  "addqt  ",
  "sub    ",
  "subc   ",
  "subq   ",
  "subqt  ",
  "neg    ",
  "and    ",
  "or     ",
  "xor    ",
  "not    ",
  "btst   ",
  "bset   ",
  "bclr   ",
  "mult   ",
  "imult  ",
  "imultn ",
  "resmac ",
  "imacn  ",
  "div    ",
  "abs    ",
  "sh     ",
  "shlq   ",
  "shrq   ",
  "sha    ",
  "sharq  ",
  "ror    ",
  "rorq   ",
  "cmp    ",
  "cmpq   ",
  "sumbmod",
  "sat16s ",
  "move   ",
  "moveq  ",
  "moveta ",
  "movefa ",
  "movei  ",
  "loadb  ",
  "loadw  ",
  "load   ",
  "sat32s ",
  "load   ",
  "load   ",
  "storeb ",
  "storew ",
  "store  ",
  "mirror ",
  "store  ",
  "store  ",
  "move   ",
  "jump   ",
  "jump   ",
  "mmult  ",
  "mtoi   ",
  "normi  ",
  "nop    ",
  "load   ",
  "load   ",
  "store  ",
  "store  ",
  "addqmod",
  "???????"
};

}

PipelineLog::PipelineLog() : mBuffer{}
{
  init();
}

void PipelineLog::prefetch( uint32_t address, uint32_t code )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:addqmod addqmod | store   r31,(r31+r31) |    #31          #31       | C31 NCZ | W31:01234567
  mBuffer[0x00 + sprintf( mBuffer + 0x00, "%06x", address )] = ':';
  if ( ( address & 2 ) == 0 )
  {
    mBuffer[0x07 + sprintf( mBuffer + 0x07, "%s", prefetchDSPMapper( code >> 16 ) )] = ' ';
    mBuffer[0x0f + sprintf( mBuffer + 0x0f, "%s", prefetchDSPMapper( code & 0xffff ) )] = ' ';
  }
  else
  {
    mBuffer[0x07 + sprintf( mBuffer + 0x07, "%s", prefetchDSPMapper( code & 0xffff ) )] = ' ';
  }
}

void PipelineLog::decodeDSP( DSPI instr, uint32_t reg1, uint32_t reg2 )
{
  switch ( instr )
  {
  case DSPI::ADD:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "add     r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::ADDC:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "addc    r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::ADDQ:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "addq    #%02x,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::ADDQT:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "addqt   #%02x,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SUB:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "sub     r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SUBC:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "subc    r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SUBQ:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "subq    #%02x,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SUBQT:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "subqt   #%02x,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::NEG:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "neg     r%02d", reg2 )] = ' ';
    break;
  case DSPI::AND:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "and     r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::OR:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "or      r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::XOR:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "xor     r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::NOT:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "not     r%02d", reg2 )] = ' ';
    break;
  case DSPI::BTST:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "btst    #%02x,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::BSET:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "bset    #%02x,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::BCLR:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "bclr    #%02x,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::MULT:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "mult    r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::IMULT:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "imult   r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::IMULTN:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "imultn  r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::RESMAC:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "resmac  r%02d", reg2 )] = ' ';
    break;
  case DSPI::IMACN:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "imacn   r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::DIV:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "div     r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::ABS:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "abs     r%02d", reg2 )] = ' ';
    break;
  case DSPI::SH:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "sh      r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SHLQ:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "shlq    #%02x,r%02d", 32-reg1, reg2 )] = ' ';
    break;
  case DSPI::SHRQ:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "shrq    #%02x,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SHA:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "sha     r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SHARQ:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "sharq   #%02x,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::ROR:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "ror     r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::RORQ:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "rorq    #%02x,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::CMP:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "cmp     r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::CMPQ:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "cmpq    #%02x,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SUBQMOD:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "sumbmod #%02x,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SAT16S:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "sat16s  r%02d", reg2 )] = ' ';
    break;
  case DSPI::MOVE:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "move    r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::MOVEQ:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "moveq   #%02x,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::MOVETA:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "moveta  r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::MOVEFA:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "movefa  r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::MOVEI:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "movei   #--------,r%02d", reg2)] = ' ';
    break;
  case DSPI::LOADB:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "loadb   (r%02d),r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::LOADW:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "loadw   (r%02d),r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::LOAD:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "load    (r%02d),r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SAT32S:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "sat32s  r%02d", reg2 )] = ' ';
    break;
  case DSPI::LOAD14N:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "load    (r14+#%02x),r%02d", tabAddSubQ[reg1] * 4, reg2)] = ' ';
    break;
  case DSPI::LOAD15N:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "load    (r15+#%02x),r%02d", tabAddSubQ[reg1], reg2 )] = ' ';
    break;
  case DSPI::STOREB:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "storeb  r%02d,(r%02d)", reg2, reg1 )] = ' ';
    break;
  case DSPI::STOREW:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "storew  r%02d,(r%02d)", reg2, reg1 )] = ' ';
    break;
  case DSPI::STORE:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "store   r%02d,(r%02d)", reg2, reg1 )] = ' ';
    break;
  case DSPI::MIRROR:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "mirror  r%02d", reg2 )] = ' ';
    break;
  case DSPI::STORE14N:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "store   r%02d,(r14+#%02x)", reg2, tabAddSubQ[reg1] * 4 )] = ' ';
    break;
  case DSPI::STORE15N:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "store   r%02d,(r15+#%02x)", reg2, tabAddSubQ[reg1] * 4 )] = ' ';
    break;
  case DSPI::MOVEPC:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "move    pc,r%02d",       reg2 )] = ' ';
    break;
  case DSPI::JUMP:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "jump    %s%s(r%02d)", kJumpConditions[reg2 & 31].name, kJumpConditions[reg2 & 31].size == 0 ? "" : ",", reg1)] = ' ';
    break;
  case DSPI::JR:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "jr      %s%s%s%d", kJumpConditions[reg2 & 31].name, kJumpConditions[reg2 & 31].size == 0 ? "" : ",", ( reg1 & 5 ) == 0?"+":"", ( ( int8_t )( reg1 << 3 ) / 8 ))] = ' ';
    break;
  case DSPI::MMULT:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "mmult   r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::MTOI:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "mtoi    r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::NORMI:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "normi   r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::NOP:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "nop" )] = ' ';
    break;
  case DSPI::LOAD14R:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "load    (r14+r%02d),r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::LOAD15R:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "load    (r15+#%02d),r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::STORE14R:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "store   r%02d,(r14+r%02d)", reg2, reg1 )] = ' ';
    break;
  case DSPI::STORE15R:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "store   r%02d,(r15+r%02d)", reg2, reg1 )] = ' ';
    break;
  case DSPI::ADDQMOD:
    mBuffer[0x19 + sprintf( mBuffer + 0x19, "addqmod #%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  }
}

void PipelineLog::decodeMOVEI( int stage, uint32_t data )
{
  if ( stage == 0 )
  {
    mBuffer[0x22 + sprintf( mBuffer + 0x22, "----%04x", data )] = ' ';
    mBuffer[0x38 + sprintf( mBuffer + 0x38, "#%04x", data )] = ' ';
  }
  else
  {
    mBuffer[0x22 + sprintf( mBuffer + 0x22, "%04x----", data >> 16 )] = ' ';
    mBuffer[0x38 + sprintf( mBuffer + 0x38, "#%04x", data >> 16 )] = ' ';
  }

  mBuffer[0x17] = ':';
}

void PipelineLog::decodeIMULTN( uint32_t reg, int width, int count )
{
  mBuffer[0x19 + sprintf( mBuffer + 0x19, "imultn  r%02d%c,m+%d", reg + 32 + count / 2, count & 1 ? 'h' : 'l', width * count )] = ' ';
  mBuffer[0x17] = ':';
}

void PipelineLog::decodeRESMAC( uint32_t reg2 )
{
  mBuffer[0x19 + sprintf( mBuffer + 0x19, "resmac  r%02d", reg2 )] = ' ';
  mBuffer[0x17] = ':';
}

void PipelineLog::decodeIMACN( uint32_t reg, int width, int count )
{
  mBuffer[0x19 + sprintf( mBuffer + 0x19, "imacn   r%02d%c,m+%d", reg + 32 + count / 2, count & 1 ? 'h' : 'l', width * count )] = ' ';
  mBuffer[0x17] = ':';
}

void PipelineLog::portImm( uint32_t value )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:addqmod addqmod | store   r31,(r31+r31) | R31:01234567 R31:01234567 | C31 NCZ | W31:01234567
  mBuffer[0x3a + sprintf( mBuffer + 0x3a, "#%02x", value )] = ' ';
}

void PipelineLog::portCond( uint32_t value )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:addqmod addqmod | store   r31,(r31+r31) | R31:01234567 R31:01234567 | C31 NCZ | W31:01234567
  int off = sprintf( mBuffer + 0x3d - kJumpConditions[value & 31].size, "%s", kJumpConditions[value & 31].name );
  mBuffer[0x3d + off -kJumpConditions[value & 31].size] = ' ';
}

void PipelineLog::portReadSrc( uint32_t reg, uint32_t value )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:addqmod addqmod | store   r31,(r31+r31) | R31:01234567 R31:01234567 | C31 NCZ | W31:01234567
  mBuffer[0x31 + sprintf( mBuffer + 0x31, "R%02d:%08x", reg, value )] = ' ';
}

void PipelineLog::portReadSrcMMULT( uint32_t reg, bool high, uint32_t value )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:addqmod addqmod | store   r31,(r31+r31) | R31:01234567 R31:01234567 | C31 NCZ | W31:01234567
  mBuffer[0x31 + sprintf( mBuffer + 0x31, "R%02d%c:    %04x", reg, high ? 'h':'l', value )] = ' ';
}

void PipelineLog::portReadDst( uint32_t reg, uint32_t value )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:addqmod addqmod | store   r31,(r31+r31) | R31:01234567 R31:01234567 | C31 NCZ | W31:01234567
  mBuffer[0x3e + sprintf( mBuffer + 0x3e, "R%02d:%08x", reg, value )] = ' ';
}

void PipelineLog::portWriteDst( uint32_t reg, uint32_t value )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:addqmod addqmod | store   r31,(r31+r31) | R31:01234567 R31:01234567 | C31 NCZ | W31:01234567
  mBuffer[0x57 + sprintf( mBuffer + 0x57, "W%02d:%08x", reg, value )] = ' ';
}

void PipelineLog::computeReg( RegFlags regFlags )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:addqmod addqmod | store   r31,(r31+r31) | R31:01234567 R31:01234567 | C31 NCZ | W31:01234567
  mBuffer[0x4d + sprintf( mBuffer + 0x4d, "C%02d", (int32_t)regFlags.reg )] = ' ';
}

void PipelineLog::computeRegFlags( RegFlags regFlags )
{

  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:addqmod addqmod | store   r31,(r31+r31) | R31:01234567 R31:01234567 | C31 NCZ | W31:01234567
  mBuffer[0x4d + sprintf( mBuffer + 0x4d, "C%02d %c%c%c", ( int32_t )regFlags.reg, NFlag[regFlags.n+1], CFlag[regFlags.c + 1], ZFlag[regFlags.z + 1] )] = ' ';
}

void PipelineLog::computeFlags( RegFlags flags )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:addqmod addqmod | store   r31,(r31+r31) | R31:01234567 R31:01234567 | C31 NCZ | W31:01234567
  mBuffer[0x51 + sprintf( mBuffer + 0x51, "%c%c%c", NFlag[flags.n + 1], CFlag[flags.c + 1], ZFlag[flags.z + 1] )] = ' ';
}

void PipelineLog::computeMul()
{
  mBuffer[0x4d] = 'm';
  mBuffer[0x4e] = 'u';
  mBuffer[0x4f] = 'l';
}

void PipelineLog::computeMac()
{
  mBuffer[0x4d] = 'm';
  mBuffer[0x4e] = 'a';
  mBuffer[0x4f] = 'c';
}

void PipelineLog::computeMac( RegFlags flags )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:addqmod addqmod | store   r31,(r31+r31) | R31:01234567 R31:01234567 | C31 NCZ | W31:01234567
  mBuffer[0x4d + sprintf( mBuffer + 0x4d, "mac %c%c%c", NFlag[flags.n + 1], CFlag[flags.c + 1], ZFlag[flags.z + 1] )] = ' ';
}

void PipelineLog::computeIndex()
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:addqmod addqmod | store   r31,(r31+r31) | R31:01234567 R31:01234567 |  index  | W31:01234567
  mBuffer[0x4e] = 'i';
  mBuffer[0x4f] = 'n';
  mBuffer[0x50] = 'd';
  mBuffer[0x51] = 'e';
  mBuffer[0x52] = 'x';
}

void PipelineLog::div( int cycle )
{
  mBuffer[0x4b + sprintf( mBuffer + 0x4b, "%x", cycle )] = ' ';
}

void PipelineLog::storeLong( uint32_t address, uint32_t value )
{
  //000000000000000011111111111111112222222222222222333333333333333344444444444444445555555555555555666666666666666777777777
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01234567
  //012345:addqmod addqmod | store   r31,(r31+r31) | R31:01234567 R31:01234567 | C31 NCZ | W31:01234567 | S(123456)=12345678
  mBuffer[0x66 + sprintf( mBuffer + 0x66, "S(%06x)=%08x", address, value )] = ' ';
}

void PipelineLog::storeWord( uint32_t address, uint16_t value )
{
  //000000000000000011111111111111112222222222222222333333333333333344444444444444445555555555555555666666666666666777777777
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01234567
  //012345:addqmod addqmod | store   r31,(r31+r31) | R31:01234567 R31:01234567 | C31 NCZ | W31:01234567 | S(123456)=12345678
  mBuffer[0x66 + sprintf( mBuffer + 0x66, "S(%06x)=%04x", address, value )] = ' ';
}

void PipelineLog::storeByte( uint32_t address, uint8_t value )
{
  //000000000000000011111111111111112222222222222222333333333333333344444444444444445555555555555555666666666666666777777777
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01234567
  //012345:addqmod addqmod | store   r31,(r31+r31) | R31:01234567 R31:01234567 | C31 NCZ | W31:01234567 | S(123456)=12345678
  mBuffer[0x66 + sprintf( mBuffer + 0x66, "S(%06x)=%02x", address, value )] = ' ';
}

void PipelineLog::loadLong( uint32_t address, uint32_t value )
{
  //000000000000000011111111111111112222222222222222333333333333333344444444444444445555555555555555666666666666666777777777
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01234567
  //012345:addqmod addqmod | store   r31,(r31+r31) | R31:01234567 R31:01234567 | C31 NCZ | W31:01234567 | L(123456):12345678
  mBuffer[0x66 + sprintf( mBuffer + 0x66, "L(%06x):%08x", address, value )] = ' ';
}

void PipelineLog::loadWord( uint32_t address, uint16_t value )
{
  //000000000000000011111111111111112222222222222222333333333333333344444444444444445555555555555555666666666666666777777777
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01234567
  //012345:addqmod addqmod | store   r31,(r31+r31) | R31:01234567 R31:01234567 | C31 NCZ | W31:01234567 | L(123456):12345678
  mBuffer[0x66 + sprintf( mBuffer + 0x66, "L(%06x):%04x", address, value )] = ' ';
}

void PipelineLog::loadByte( uint32_t address, uint8_t value )
{
  //000000000000000011111111111111112222222222222222333333333333333344444444444444445555555555555555666666666666666777777777
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01234567
  //012345:addqmod addqmod | store   r31,(r31+r31) | R31:01234567 R31:01234567 | C31 NCZ | W31:01234567 | L(123456):12345678
  mBuffer[0x66 + sprintf( mBuffer + 0x66, "L(%06x):%02x", address, value )] = ' ';
}

void PipelineLog::warnMemoryAccess()
{
  mBuffer[0x2f] = '!';
}

void PipelineLog::flush()
{
  std::cout << mBuffer;
  init();
}

void PipelineLog::init()
{
  sprintf( mBuffer, "                       |                       |                           |         |              |                    \n" );
}

char const* PipelineLog::prefetchDSPMapper( uint32_t code )
{
  switch ( mPrefetchDSPMapperState )
  {
  case 1:
    sprintf( mPrefetchDSPMapperBuf, "%04x", code );
    mPrefetchDSPMapperState = 2;
    return mPrefetchDSPMapperBuf;
  case 2:
    sprintf( mPrefetchDSPMapperBuf, "%04x", code );
    mPrefetchDSPMapperState = 0;
    return mPrefetchDSPMapperBuf;
  default:
    code >>= 10;
    if ( (DSPI)code == DSPI::MOVEI )
      mPrefetchDSPMapperState = 1;
    return kPrefetchDSPOpcodes[code];
  }
}

