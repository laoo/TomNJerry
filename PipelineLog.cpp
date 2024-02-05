#include "PipelineLog.hpp"

namespace
{
static constexpr std::array<char const*, 32> kJumpConditions =
{
  "",
  "!z,",
  "z,",
  "?,",
  "!c,",
  "!c !z,",
  "!c z,",
  "?,",

  "c,",
  "c !z,",
  "c z,",
  "?,",
  "?,",
  "?,",
  "?,",
  "?,",

  "?,",
  "?,",
  "?,",
  "?,",
  "!n,",
  "!n !z,",
  "!n z,",
  "?,",
  "n,",
  "n !z,",
  "n z,",
  "?,",
  "?,",
  "?,",
  "?,",
  "!,"
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
  //012345:01234567 | store   r31,(r31+r31) |    #31          #31       | C31 F | W31:01234567
  mBuffer[0x00 + sprintf( mBuffer + 0x00, "%06x:%08x", address, code )] = ' ';
}

void PipelineLog::decodeDSP( DSPI instr, uint32_t reg1, uint32_t reg2 )
{
  switch ( instr )
  {
  case DSPI::ADD:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "add     r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::ADDC:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "addc    r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::ADDQ:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "addq    #%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::ADDQT:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "addqt   #%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SUB:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "sub     r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SUBC:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "subc    r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SUBQ:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "subq    #%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SUBQT:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "subqt   #%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::NEG:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "neg           r%02d", reg2 )] = ' ';
    break;
  case DSPI::AND:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "and     r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::OR:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "or      r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::XOR:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "xor     r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::NOT:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "not           r%02d", reg2 )] = ' ';
    break;
  case DSPI::BTST:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "btst    #%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::BSET:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "bset    #%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::BCLR:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "bclr    #%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::MULT:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "mult    r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::IMULT:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "imult   r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::IMULTN:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "imultn  r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::RESMAC:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "resmac        r%02d", reg2 )] = ' ';
    break;
  case DSPI::IMACN:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "imacn   r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::DIV:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "div     r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::ABS:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "abs           r%02d", reg2 )] = ' ';
    break;
  case DSPI::SH:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "sh      r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SHLQ:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "shlq    #%02d,r%02d", 32-reg1, reg2 )] = ' ';
    break;
  case DSPI::SHRQ:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "shrq    #%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SHA:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "sha     r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SHARQ:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "sharq   #%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::ROR:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "ror     r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::RORQ:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "rorq    #%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::CMP:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "cmp     r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::CMPQ:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "cmpq    #%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SUBQMOD:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "sumbmod #%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SAT16S:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "sat16s        r%02d", reg2 )] = ' ';
    break;
  case DSPI::MOVE:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "move    r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::MOVEQ:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "moveq   #%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::MOVETA:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "moveta  r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::MOVEFA:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "movefa  r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::MOVEI:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "movei              ")] = ' ';
    break;
  case DSPI::LOADB:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "loadb   (r%02d),r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::LOADW:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "loadw   (r%02d),r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::LOAD:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "load    (r%02d),r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::SAT32S:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "sat32s        r%02d", reg2 )] = ' ';
    break;
  case DSPI::LOAD14N:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "load    (r14+#%02d),r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::LOAD15N:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "load    (r15+#%02d),r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::STOREB:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "storeb  r%02d,(r%02d)", reg1, reg2 )] = ' ';
    break;
  case DSPI::STOREW:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "storew  r%02d,(r%02d)", reg1, reg2 )] = ' ';
    break;
  case DSPI::STORE:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "store   r%02d,(r%02d)", reg1, reg2 )] = ' ';
    break;
  case DSPI::MIRROR:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "mirror        r%02d", reg2 )] = ' ';
    break;
  case DSPI::STORE14N:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "store   r%02d,(r14+#%02d)", reg1, reg2 )] = ' ';
    break;
  case DSPI::STORE15N:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "store   r%02d,(r14+#%02d)", reg1, reg2 )] = ' ';
    break;
  case DSPI::MOVEPC:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "move    pc,r%02d",       reg2 )] = ' ';
    break;
  case DSPI::JUMP:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "jump    %s(r%02d)", kJumpConditions[reg2 & 31], reg1 )] = ' ';
    break;
  case DSPI::JR:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "jump    %s%c%d", kJumpConditions[reg2 & 31], (reg1&5) == 0?'+':'-', ( ( int8_t )( reg1 << 3 ) >> 2 ) )] = ' ';
    break;
  case DSPI::MMULT:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "mmult   r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::MTOI:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "mtoi    r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::NORMI:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "normi   r%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::NOP:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "nop" )] = ' ';
    break;
  case DSPI::LOAD14R:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "load    (r14+r%02d),r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::LOAD15R:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "load    (r15+#%02d),r%02d", reg1, reg2 )] = ' ';
    break;
  case DSPI::STORE14R:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "store   r%02d,(r14+r%02d)", reg1, reg2 )] = ' ';
    break;
  case DSPI::STORE15R:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "store   r%02d,(r15+r%02d)", reg1, reg2 )] = ' ';
    break;
  case DSPI::ADDQMOD:
    mBuffer[0x12 + sprintf( mBuffer + 0x12, "addqmod #%02d,r%02d", reg1, reg2 )] = ' ';
    break;
  }
}

void PipelineLog::portImm( uint32_t value )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:01234567 | store   r31,(r31+r31) | R31:01234567 R31:01234567 | CR31 F | W31:01234567
  mBuffer[0x33 + sprintf( mBuffer + 0x33, "#%02x", value )] = ' ';
}
                                                  
void PipelineLog::portReadSrc( uint32_t reg, uint32_t value )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:01234567 | store   r31,(r31+r31) | R31:01234567 R31:01234567 | CR31 F | W31:01234567
  mBuffer[0x2a + sprintf( mBuffer + 0x2a, "R%02d:%08x", reg, value )] = ' ';
}

void PipelineLog::portReadDst( uint32_t reg, uint32_t value )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:01234567 | store   r31,(r31+r31) | R31:01234567 R31:01234567 | CR31 F | W31:01234567
  mBuffer[0x37 + sprintf( mBuffer + 0x37, "R%02d:%08x", reg, value )] = ' ';
}

void PipelineLog::portWriteDst( uint32_t reg, uint32_t value )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:01234567 | store   r31,(r31+r31) | R31:01234567 R31:01234567 | CR31 F | W31:01234567
  mBuffer[0x4f + sprintf( mBuffer + 0x4f, "W%02d:%08x", reg, value )] = ' ';
}

void PipelineLog::computeReg( uint32_t reg )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:01234567 | store   r31,(r31+r31) | R31:01234567 R31:01234567 | Cr31 F | W31:01234567
  mBuffer[0x46 + sprintf( mBuffer + 0x46, "Cr%02d", reg )] = ' ';
}

void PipelineLog::computeRegFlags( uint32_t reg )
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:01234567 | store   r31,(r31+r31) | R31:01234567 R31:01234567 | Cr31 F | W31:01234567
  mBuffer[0x46 + sprintf( mBuffer + 0x46, "Cr%02d F", reg )] = ' ';
}

void PipelineLog::computeFlags()
{
  //00000000000000001111111111111111222222222222222233333333333333334444444444444444555555555555
  //0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab
  //012345:01234567 | store   r31,(r31+r31) | R31:01234567 R31:01234567 | Cr31 F | W31:01234567
  mBuffer[0x4b] = 'F';
}

void PipelineLog::flush()
{
  std::cout << mBuffer;
  init();
}

void PipelineLog::init()
{
  sprintf( mBuffer, "                |                       |                           |        |                \n" );
}

