#pragma once
#include "IChip.hpp"
#include "Opcodes.hpp"

class PipelineLog;

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
  ~Jerry() override;

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
  void dualPortCommit( bool writeRead = false );


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



  struct Prefetch
  {
    uint64_t queue = 0;
    size_t queueSize = 0;

    int32_t pull();
    bool push( uint32_t value );
  } mPrefetch = {};

  struct StageRead
  {
    DSPI instruction = DSPI::EMPTY;
    uint32_t dataSrc = 0;
    uint32_t dataDst = 0;
    uint32_t regSrc = 0;
    uint32_t regDst = 0;
  } mStageRead = {};

  struct StageCompute
  {
    DSPI instruction = DSPI::EMPTY;
    uint32_t dataSrc = 0;
    uint32_t dataDst = 0;
    uint32_t regDst = 0;
  } mStageCompute = {};

  struct StageWrite 
  {
    bool updateFlags = false;
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
  int mFlagsSemaphore = 0;
  uint32_t mMulatiplyAccumulator = 0;

  std::unique_ptr<PipelineLog> mLog;
};
