#pragma once
#include "IChip.hpp"
#include "Opcodes.hpp"
#include "RegFlags.hpp"

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
  void storeByte( uint32_t address, uint8_t data );
  void storeWord( uint32_t address, uint16_t data );
  void storeLong( uint32_t address, uint32_t data );

  void loadByte( uint32_t address, uint32_t reg );
  void loadWord( uint32_t address, uint32_t reg );
  void loadLong( uint32_t address, uint32_t reg );

  bool testCondition( uint32_t condition ) const;

  void ackWrite();
  void ackRead( uint8_t data );
  void ackRead( uint16_t data );
  void ackRead( uint32_t data );


  AdvanceResult advance( bool skipIO );

  void io();
  void stageWrite();
  void compute();
  void stageRead();
  void decode();
  void prefetch();

  bool portWriteDst( uint32_t reg, uint32_t data );
  bool portReadSrc( uint32_t regSrc );
  bool portReadDst( uint32_t regDst );
  void portReadDstAndHiddenCommit( uint32_t regDst ); //to be used with indexed addressing modes
  bool portReadBoth( uint32_t regSrc, uint32_t regDst );
  void dualPortCommit();
  void busGatePush( AdvanceResult result );
  void busGatePop();


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

  uint64_t mCycle = 0;



  struct Prefetch
  {
    enum PullStatus : uint16_t
    {
      EMPTY,
      OPCODE,
      OPERAND1,
      OPERAND2
    } status = OPCODE;
    uint64_t queue = 0;
    size_t queueSize = 0;

    std::pair<PullStatus, uint16_t> pull();
    uint32_t push( uint32_t value, uint32_t oddWord );
  } mPrefetch = {};

  struct StageRead
  {
    enum DecodeStage
    {
      NORMAL,
      MOVEI1,
      MOVEI2
    } decodeStage = NORMAL;
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
    uint32_t regSrc = 0;
    uint32_t regDst = 0;
  } mStageCompute = {};

  struct StageWrite 
  {
    bool updateFlags = false;
    RegFlags regFlags = {};
    uint32_t data = 0;
  } mStageWrite = {};

  struct StageStore
  {
    enum State
    {
      IDLE,
      STORE_LONG,
      STORE_WORD,
      STORE_BYTE,
    } state = IDLE;
    uint32_t address = 0;
    uint32_t data = 0;
  } mStageStore;

  struct StageLoad
  {
    enum State
    {
      IDLE,
      LOAD_LONG,
      LOAD_WORD,
      LOAD_BYTE
    } state = IDLE;
    uint32_t address = 0;
    int32_t reg = -1;
  } mStageLoad;

  int32_t mPortReadSrcReg = -1;
  int32_t mPortReadDstReg = -1;
  int32_t mPortWriteDstReg = -1;
  uint32_t mPortWriteDstData = 0;

  struct BusGate
  {
    //two element queue
    AdvanceResult front = AdvanceResult::nop();
    AdvanceResult back = AdvanceResult::nop();

    explicit operator bool() const
    {
      return !( front && back );
    }
  } mBusGate;

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
