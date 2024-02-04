#pragma once
#include "IChip.hpp"

class Jerry : public IChip
{
public:

  static constexpr uint32_t D_FLAGS = 0xf1a100; //DSP Flags
  static constexpr uint32_t D_MTXC = 0xf1a104; //DSP Matrix Control
  static constexpr uint32_t D_MTXA = 0xf1a108; //DSP Matrix Address
  static constexpr uint32_t D_END = 0xf1a10c; //DSP Data Organization
  static constexpr uint32_t D_PC = 0xf1a110; //DSP Program Counter
  static constexpr uint32_t D_CTRL = 0xf1a114; //DSP Operation Control / Status
  static constexpr uint32_t D_MOD = 0xf1a118; //DSP Modulo Instruction Mask
  static constexpr uint32_t D_REMAIN = 0xf1a11c; //DSP Division Remainder
  static constexpr uint32_t D_DIVCTRL = 0xf1a11c; //DSP Divider control
  static constexpr uint32_t D_MACHI = 0xf1a120; //DSP Hi byte of MAC operations

  static constexpr uint32_t JERRY_BASE = 0xf10000;
  static constexpr uint32_t DSP_BASE = 0xf1a100;
  static constexpr uint32_t ROM_TABLE = 0xf1d000;
  static constexpr uint32_t JERRY_SIZE = 0x10000;

  static constexpr uint32_t RAM_BASE = 0xf1b000;
  static constexpr uint32_t RAM_SIZE = 0x2000;

  struct CTRL
  {
    static constexpr uint32_t DSPGO = 0b0000'0000'0000'0000'0000'0000'0000'0001;
    static constexpr uint32_t CPUINT = 0b0000'0000'0000'0000'0000'0000'0000'0010;
    static constexpr uint32_t FORCEINT0 = 0b0000'0000'0000'0000'0000'0000'0000'0100;
    static constexpr uint32_t SINGLE_STEP = 0b0000'0000'0000'0000'0000'0000'0000'1000;
    static constexpr uint32_t SINGLE_GO = 0b0000'0000'0000'0000'0000'0000'0001'0000;
    static constexpr uint32_t D_CPULAT = 0b0000'0000'0000'0000'0000'0000'0100'0000;
    static constexpr uint32_t D_I2SLAT = 0b0000'0000'0000'0000'0000'0000'1000'0000;
    static constexpr uint32_t D_TIM1LAT = 0b0000'0000'0000'0000'0000'0001'0000'0000;
    static constexpr uint32_t D_TIM2LAT = 0b0000'0000'0000'0000'0000'0010'0000'0000;
    static constexpr uint32_t D_EXT0LAT = 0b0000'0000'0000'0000'0000'0100'0000'0000;
    static constexpr uint32_t BUS_HOG = 0b0000'0000'0000'0000'0000'1000'0000'0000;
    static constexpr uint32_t VERSION = 0b0000'0000'0000'0000'1111'0000'0000'0000;
    static constexpr uint32_t D_EXT1LAT = 0b0000'0000'0000'0001'0000'0000'0000'0000;

    uint32_t value;

    bool go() const
    {
      return ( value & DSPGO ) != 0;
    }

  };



  Jerry();
  ~Jerry() override = default;

  void debugWrite( uint32_t address, uint32_t data ) override;
  void debugWrite( uint32_t address, std::span<uint32_t const> data ) override;

  AdvanceResult busCycleIdle() override;
  AdvanceResult busCycleWrite() override;
  AdvanceResult busCycleRead( uint8_t value ) override;
  AdvanceResult busCycleRead( uint16_t value ) override;
  AdvanceResult busCycleRead( uint32_t value ) override;
  AdvanceResult busCycleRead( uint64_t value ) override;

private:
  void writeByte( uint32_t address, uint8_t data );
  void writeWord( uint32_t address, uint16_t data );
  void writeLong( uint32_t address, uint32_t data );

  void readByte( uint32_t address );
  void readWord( uint32_t address );
  void readLong( uint32_t address );

  AdvanceResult advance();

  void io();
  void stageWrite();
  void compute();
  void stageRead();
  void decode();
  void prefetch();

  bool dualPortWrite( uint32_t reg, uint32_t data );
  bool dualPortRead( uint32_t reg1, uint32_t reg2 );
  bool dualPortRead( uint32_t reg );
  void dualPortCommit();


private:
  struct FLAGS
  {
    static constexpr uint32_t ZERO_FLAG    = 0b0000'0000'0000'0000'0000'0000'0000'0001;
    static constexpr uint32_t CARRY_FLAG   = 0b0000'0000'0000'0000'0000'0000'0000'0010;
    static constexpr uint32_t NEGA_FLAG    = 0b0000'0000'0000'0000'0000'0000'0000'0100;
    static constexpr uint32_t IMASK        = 0b0000'0000'0000'0000'0000'0000'0000'1000;
    static constexpr uint32_t D_CPUENA     = 0b0000'0000'0000'0000'0000'0000'0001'0000;
    static constexpr uint32_t D_I2SENA     = 0b0000'0000'0000'0000'0000'0000'0010'0000;
    static constexpr uint32_t D_TIM1ENA    = 0b0000'0000'0000'0000'0000'0000'0100'0000;
    static constexpr uint32_t D_IIM2ENA    = 0b0000'0000'0000'0000'0000'0000'1000'0000;
    static constexpr uint32_t D_EXT0ENA    = 0b0000'0000'0000'0000'0000'0001'0000'0000;
    static constexpr uint32_t D_CPUCLR     = 0b0000'0000'0000'0000'0000'0010'0000'0000;
    static constexpr uint32_t D_I2SCLR     = 0b0000'0000'0000'0000'0000'0100'0000'0000;
    static constexpr uint32_t D_TIM1CLR    = 0b0000'0000'0000'0000'0000'1000'0000'0000;
    static constexpr uint32_t D_IIM2CLR    = 0b0000'0000'0000'0000'0001'0000'0000'0000;
    static constexpr uint32_t D_EXT0CLR    = 0b0000'0000'0000'0000'0010'0000'0000'0000;
    static constexpr uint32_t REGPAGE      = 0b0000'0000'0000'0000'0100'0000'0000'0000;
    static constexpr uint32_t DMAEN        = 0b0000'0000'0000'0000'1000'0000'0000'0000;
    static constexpr uint32_t D_EXT1ENA    = 0b0000'0000'0000'0001'0000'0000'0000'0000;
    static constexpr uint32_t D_EXT1CLR    = 0b0000'0000'0000'0010'0000'0000'0000'0000;

    uint32_t value;
  } mFlags = {};

  CTRL mCtrl = {};

  uint32_t mPC = 0;
  uint32_t mMod = 0;
  uint32_t mRemain = 0;
  uint32_t mDivCtrl = 0;
  uint32_t mMachi = 0;
  uint32_t mRegisterFile = 0;
  uint32_t mAnotherRegisterFile = 32;

  AdvanceResult mResult = AdvanceResult::nop();

  uint64_t mCycle = 0;


  enum struct I
  {
    EMPTY   = -1,
    ADD     = 0,
    ADDC    = 1,
    ADDQ    = 2,
    ADDQT   = 3,
    SUB     = 4,
    SUBC    = 5,
    SUBQ    = 6,
    SUBQT   = 7,
    NEG     = 8,
    AND     = 9,
    OR      = 10,
    XOR     = 11,
    NOT     = 12,
    BTST    = 13,
    BSET    = 14,
    BCLR    = 15,
    MULT    = 16,
    IMULT   = 17,
    IMULTN  = 18,
    RESMAC  = 19,
    IMACN   = 20,
    DIV     = 21,
    ABS     = 22,
    SH      = 23,
    SHLQ    = 24,
    SHRQ    = 25,
    SHA     = 26,
    SHARQ   = 27,
    ROR     = 28,
    RORQ    = 29,
    CMP     = 30,
    CMPQ    = 31,
    SUBQMOD = 32,
    SAT16S  = 33,
    MOVE    = 34,
    MOVEQ   = 35,
    MOVETA  = 36,
    MOVEFA  = 37,
    MOVEI   = 38,
    LOADB   = 39,
    LOADW   = 40,
    LOAD    = 41,
    SAT32S  = 42,
    LOAD14N = 43,
    LOAD15N = 44,
    STOREB  = 45,
    STOREW  = 46,
    STORE   = 47,
    MIRROR  = 48,
    STORE14N= 49,
    STORE15N= 50,
    MOVEPC  = 51,
    JUMP    = 52,
    JR      = 53,
    MMULT   = 54,
    MTOI    = 55,
    NORMI   = 56,
    NOP     = 57,
    LOAD14R = 58,
    LOAD15R = 59,
    STORE14R= 60,
    STORE15R= 61,
    ADDQMOD = 63
  };

  struct Prefetch
  {
    uint64_t queue = 0;
    size_t queueSize = 0;

    int32_t pull();
    bool push( uint32_t value );
  } mPrefetch = {};

  struct StageRead
  {
    I instruction = I::EMPTY;
    uint32_t data1 = 0;
    uint32_t data2 = 0;
    uint16_t reg1 = 0;
    uint16_t reg2 = 0;
  } mStageRead = {};

  struct StageCompute
  {
    I instruction = I::EMPTY;
  } mStageCompute = {};

  struct StageWrite 
  {
    int32_t z = -1;
    int32_t n = -1;
    int32_t c = -1;
    int32_t reg = -1;
    uint32_t data = 0;
  } mStageWrite = {};

  struct DualPort
  {
    enum PortStatus
    {
      EMPTY,
      READ,
      WRITE
    };

    struct Port
    {
      PortStatus status = EMPTY;
      uint32_t reg = 0;
      uint32_t data = 0;
    };

    Port port1;
    Port port2;
  } mDualPort;

  std::array<uint32_t, RAM_SIZE / sizeof( uint32_t )> mLocalRAM;
  uint64_t mLastLocalRAMAccessCycle = ~0;

  enum RegStatus
  {
    FREE,
    LOCKED
  };

  std::array<RegStatus, 32> mRegStatus;
  std::array<uint32_t, 64> mRegs;
  RegStatus mFlagsStatus = FREE;
  uint32_t mMulatiplyAccumulator = 0;
};
