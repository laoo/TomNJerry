#pragma once
#include "AdvanceResult.hpp"
#include "Opcodes.hpp"
#include "RegFlags.hpp"
#include "wav.h"

class PipelineLog;

class Jerry
{
public:

  static constexpr uint32_t JPIT1     = 0xF10000;   //Timer 1 Pre - Scaler
  static constexpr uint32_t JPIT2     = 0xF10002;   //Timer 1 Divider
  static constexpr uint32_t JPIT3     = 0xF10004;   //Timer 2 Pre - Scaler
  static constexpr uint32_t JPIT4     = 0xF10006;   //Timer 2 Divider

  static constexpr uint32_t J_INT     = 0xF10020;   //Jerry Interrupt control( to TOM )

  static constexpr uint32_t ASIDATA   = 0xF10030;   //Asynchronous Data Register
  static constexpr uint32_t ASICTRL   = 0xF10032;   //Asynchronous Control Register
  static constexpr uint32_t ASISTAT   = 0xF10032;   //Asynchronous Status Register
  static constexpr uint32_t ASICLK    = 0xF10034;   //Asynchronous Clock Register

  static constexpr uint32_t JPIT1R    = 0xF10036;   //Timer 1 Pre - Scaler
  static constexpr uint32_t JPIT2R    = 0xF10038;   //Timer 1 Divider
  static constexpr uint32_t JPIT3R    = 0xF1003a;   //Timer 2 Pre - Scaler
  static constexpr uint32_t JPIT4R    = 0xF1003c;   //Timer 2 Divider


  static constexpr uint32_t JOYSTICK  = 0xF14000;   //Joystick register and mute
  static constexpr uint32_t JOYBUTS   = 0xF14002;   //Joystick register
  static constexpr uint32_t CONFIG    = 0xF14002;   //Also has NTSC / PAL

  static constexpr uint32_t SCLK      = 0xF1A150;   //SSI Clock Frequency
  static constexpr uint32_t SMODE     = 0xF1A154;   //SSI Control

  static constexpr uint32_t L_I2S     = 0xF1A148;   //I2S Serial Interface
  static constexpr uint32_t R_I2S     = 0xF1A14C;   //I2S Serial Interface


  static constexpr uint32_t D_FLAGS   = 0xF1A100;   //DSP Flags
  static constexpr uint32_t D_MTXC    = 0xF1A104;   //DSP Matrix Control
  static constexpr uint32_t D_MTXA    = 0xF1A108;   //DSP Matrix Address
  static constexpr uint32_t D_END     = 0xF1A10C;   //DSP Data Organization
  static constexpr uint32_t D_PC      = 0xF1A110;   //DSP Program Counter
  static constexpr uint32_t D_CTRL    = 0xF1A114;   //DSP Operation Control / Status
  static constexpr uint32_t D_MOD     = 0xF1A118;   //DSP Modulo Instruction Mask
  static constexpr uint32_t D_REMAIN  = 0xF1A11C;   //DSP Division Remainder
  static constexpr uint32_t D_DIVCTRL = 0xF1A11C;   //DSP Divider control
  static constexpr uint32_t D_MACHI   = 0xF1A120;   //DSP Hi byte of MAC operations

  static constexpr uint32_t DSP_BASE  = 0xf1a000;
  static constexpr uint32_t CTR_BASE  = 0xf1a100;
  static constexpr uint32_t RAM_BASE  = 0xf1b000;
  static constexpr uint32_t ROM_BASE  = 0xf1d000;
  static constexpr uint32_t RAM_SIZE  = 0x002000;
  static constexpr uint32_t ROM_SIZE  = 0x001000;

  struct JINTCTRL
  {
    static constexpr uint16_t J_EXTENA  = 0x0001; //Enable external interrupts
    static constexpr uint16_t J_DSPENA  = 0x0002; //Enable DSP interrupts
    static constexpr uint16_t J_TIM1ENA = 0x0004; //Enable Timer 1 interrupts
    static constexpr uint16_t J_TIM2ENA = 0x0008; //Enable Timer 2 interrupts
    static constexpr uint16_t J_ASYNENA = 0x0010; //Enable Asyncronous Serial interrupts
    static constexpr uint16_t J_SYNENA  = 0x0020; //Enable Syncronous Serial interrupts

    static constexpr uint16_t J_EXTCLR  = 0x0100; //Clear pending external interrupts
    static constexpr uint16_t J_DSPCLR  = 0x0200; //Clear pending DSP interrupts
    static constexpr uint16_t J_TIM1CLR = 0x0400; //Clear pending Timer 1 interrupts
    static constexpr uint16_t J_TIM2CLR = 0x0800; //Clear pending Timer 2 interrupts
    static constexpr uint16_t J_ASYNCLR = 0x1000; //Clear pending Asynch.Serial interrupts
    static constexpr uint16_t J_SYNCLR  = 0x2000; //Clear pending Synch.Serial interrupts

    uint16_t get() const;
    void set( uint16_t value );

    bool extena = false;
    bool dspena = false;
    bool tim1ena = false;
    bool tim2ena = false;
    bool asynena = false;
    bool synena = false;

    bool extpend = false;
    bool dsppend = false;
    bool tim1pend = false;
    bool tim2pend = false;
    bool asynpend = false;
    bool synpend = false;
  };

  struct Joystick
  {
    bool audioEnable = false;
    bool ntsc = false;

    uint32_t get() const;
    uint16_t getJoy() const;
    uint16_t getBut() const;
    void set( uint32_t );
    void setJoy( uint16_t );
  };

  struct SSMODE
  {
    static constexpr uint16_t INTERNAL = 0b000001;
    static constexpr uint16_t RESERVED = 0b000010;
    static constexpr uint16_t WSEN = 0b000100;
    static constexpr uint16_t RISING = 0b001000;
    static constexpr uint16_t FALLING = 0b010000;
    static constexpr uint16_t EVERYWORD = 0b100000;

    bool internal = false;
    bool wsen = false;
    bool rising = false;
    bool falling = false;
    bool everyword = false;
  };


  struct CTRL
  {
    static constexpr uint32_t DSPGO       = 0b0000'0000'0000'0000'0000'0000'0000'0001;
    static constexpr uint32_t CPUINT      = 0b0000'0000'0000'0000'0000'0000'0000'0010;
    static constexpr uint32_t FORCEINT0   = 0b0000'0000'0000'0000'0000'0000'0000'0100;
    static constexpr uint32_t SINGLE_STEP = 0b0000'0000'0000'0000'0000'0000'0000'1000;
    static constexpr uint32_t SINGLE_GO   = 0b0000'0000'0000'0000'0000'0000'0001'0000;
    static constexpr uint32_t D_CPULAT    = 0b0000'0000'0000'0000'0000'0000'0100'0000;
    static constexpr uint32_t D_I2SLAT    = 0b0000'0000'0000'0000'0000'0000'1000'0000;
    static constexpr uint32_t D_TIM1LAT   = 0b0000'0000'0000'0000'0000'0001'0000'0000;
    static constexpr uint32_t D_TIM2LAT   = 0b0000'0000'0000'0000'0000'0010'0000'0000;
    static constexpr uint32_t D_EXT0LAT   = 0b0000'0000'0000'0000'0000'0100'0000'0000;
    static constexpr uint32_t BUS_HOG     = 0b0000'0000'0000'0000'0000'1000'0000'0000;
    static constexpr uint32_t VERSION     = 0b0000'0000'0000'0000'1111'0000'0000'0000;
    static constexpr uint32_t D_EXT1LAT   = 0b0000'0000'0000'0001'0000'0000'0000'0000;

    uint16_t get() const;

    uint32_t intLatches = 0;
    bool dspgo = false;
  };

  struct FLAGS
  {
    static constexpr uint32_t ZERO_FLAG    = 0b0000'0000'0000'0000'0000'0000'0000'0001;
    static constexpr uint32_t CARRY_FLAG   = 0b0000'0000'0000'0000'0000'0000'0000'0010;
    static constexpr uint32_t NEGA_FLAG    = 0b0000'0000'0000'0000'0000'0000'0000'0100;
    static constexpr uint32_t IMASK        = 0b0000'0000'0000'0000'0000'0000'0000'1000;
    static constexpr uint32_t D_CPUENA     = 0b0000'0000'0000'0000'0000'0000'0001'0000;
    static constexpr uint32_t D_I2SENA     = 0b0000'0000'0000'0000'0000'0000'0010'0000;
    static constexpr uint32_t D_TIM1ENA    = 0b0000'0000'0000'0000'0000'0000'0100'0000;
    static constexpr uint32_t D_TIM2ENA    = 0b0000'0000'0000'0000'0000'0000'1000'0000;
    static constexpr uint32_t D_EXT0ENA    = 0b0000'0000'0000'0000'0000'0001'0000'0000;
    static constexpr uint32_t D_CPUCLR     = 0b0000'0000'0000'0000'0000'0010'0000'0000;
    static constexpr uint32_t D_I2SCLR     = 0b0000'0000'0000'0000'0000'0100'0000'0000;
    static constexpr uint32_t D_TIM1CLR    = 0b0000'0000'0000'0000'0000'1000'0000'0000;
    static constexpr uint32_t D_TIM2CLR    = 0b0000'0000'0000'0000'0001'0000'0000'0000;
    static constexpr uint32_t D_EXT0CLR    = 0b0000'0000'0000'0000'0010'0000'0000'0000;
    static constexpr uint32_t REGPAGE      = 0b0000'0000'0000'0000'0100'0000'0000'0000;
    static constexpr uint32_t DMAEN        = 0b0000'0000'0000'0000'1000'0000'0000'0000;
    static constexpr uint32_t D_EXT1ENA    = 0b0000'0000'0000'0001'0000'0000'0000'0000;
    static constexpr uint32_t D_EXT1CLR    = 0b0000'0000'0000'0010'0000'0000'0000'0000;

    uint16_t get() const;

    bool z = false;
    bool c = false;
    bool n = false;
    bool imask = false;
    bool cpuena = false;
    bool i2sena = false;
    bool tim1ena = false;
    bool tim2ena = false;
    bool ext0ena = false;
    bool ext1ena = false;
    bool regpage = false;
  };


  Jerry( bool isNTSC, std::filesystem::path wavOut );
  ~Jerry();

  void debugWrite( uint32_t address, std::span<uint32_t const> data );

  void busCycleIdle();

  AdvanceResult busCycleGetRequest();
  uint16_t busCycleRequestReadWord( uint32_t address );
  uint32_t busCycleRequestReadLong( uint32_t address );
  void busCycleRequestWriteWord( uint32_t address, uint16_t data );
  void busCycleRequestWriteLong( uint32_t address, uint32_t data );

  void busCycleAckWrite();
  void busCycleAckReadByteRequest( uint8_t value );
  void busCycleAckReadWordRequest( uint16_t value );
  void busCycleAckReadLongRequest( uint32_t value );

  uint32_t getReg( int32_t index ) const;

private:

  uint16_t readWord( uint32_t address ) const;
  uint32_t readLong( uint32_t address ) const;
  //must write little endian word
  void writeWord( uint32_t address, uint16_t data );
  //must write little endian long
  void writeLong( uint32_t address, uint32_t data );

  void ctrlSet( uint16_t value );
  void flagsSet( uint16_t value );
  void smodeSet( uint16_t value );

  void doInt( uint32_t mask );
  void prioritizeInt();
  void launchInt( int priority );

  void cpuint();
  void forceint0();
  void checkInterrupt();
  void setI2S( uint32_t period );
  void setTimer1( uint32_t period );
  void setTimer2( uint32_t period );
  void reconfigureDAC();
  void reconfigureTimer1();
  void reconfigureTimer2();
  void sample();

  struct Prefetch
  {
    enum PullStatus : uint16_t
    {
      EMPTY,
      OPCODE,
      MOVEI1,
      MOVEI2,
      MACSEQ,
      NO_PREFETCH,
      IMULTN = NO_PREFETCH,
      IMACN,
      RESMAC,
      INT0,
      INT1,
      INT2,
      INT3,
      INT4,
      INT5,
      INT6,
      INT7
    } status = OPCODE;
    uint64_t queue = 0;
    uint32_t queueSize = 0;
    uint32_t decodedAddress = 0;
    bool doPrefetch = false;

    struct Pull
    {
      uint32_t address() const
      {
        return value >> 32;
      }
      DSPI opcode() const
      {
        return static_cast<DSPI>( ( value >> 26 ) & 63 );
      }
      LocalReg regSrc() const
      {
        return LocalReg( ( value >> ( 16 + 5 ) ) & 31 );
      }
      LocalReg regDst() const
      {
        return LocalReg( ( value >> 16 ) & 31 );
      }
      uint32_t data() const
      {
        return ( value >> 16 ) & 0xffff;
      }
      PullStatus status() const
      {
        return static_cast<PullStatus>( value & 0xffff );
      }

      Pull( PullStatus status, uint32_t address, uint16_t data )
      {
        value = ( static_cast<uint64_t>( status ) & 0xffff ) | ( static_cast<uint64_t>( data ) << 16 ) | ( static_cast<uint64_t>( address ) << 32 );
      }

      uint64_t value;
    };
  };


  void storeByte( uint32_t address, uint8_t data );
  void storeWord( uint32_t address, uint16_t data );
  void storeLong( uint32_t address, uint32_t data );

  void loadByte( uint32_t address, GlobalReg reg );
  void loadWord( uint32_t address, GlobalReg reg );
  void loadLong( uint32_t address, GlobalReg reg );

  bool testCondition( uint32_t condition ) const;

  void halfCycle();
  void divideUnit();

  void io();
  void localBus();
  void ioDispatch();
  void stageWrite();
  void compute();
  void stageRead();
  void decode();
  void prefetch();

  bool stageWriteReg();
  bool stageWriteRegL();
  void stageWriteFlags();
  void stageWriteFlagsL();
  bool portWriteDst( GlobalReg reg, uint32_t data );
  bool portReadSrc( GlobalReg regSrc );
  bool portReadDst( GlobalReg regDst );
  void portReadDstAndHiddenCommit( GlobalReg regDst ); //to be used with indexed addressing modes
  bool portReadBoth( GlobalReg regSrc, GlobalReg regDst );
  void dualPortCommit();
  void lockReg( GlobalReg reg );
  void dualPortCommitMMULT( bool high );
  void busGatePush( AdvanceResult result );
  void busGatePop();
  Prefetch::Pull prefetchPull();
  int prefetchFill();

private:
  uint16_t mJPIT1; // original values
  uint16_t mJPIT2;
  uint16_t mJPIT3;
  uint16_t mJPIT4;
  uint16_t mJPIT1R; // modified values
  uint16_t mJPIT2R;
  uint16_t mJPIT3R;
  uint16_t mJPIT4R;

  JINTCTRL mJIntCtrl = {};
  Joystick mJoystick = {};
  uint16_t mSCLK = 0xffff;
  SSMODE mSMODE = {};

  struct
  {
    uint16_t left = 0;
    uint16_t right = 0;
  } mI2S = {};

  uint32_t mMTXC = 0;
  uint32_t mMTXA = 0;
  uint32_t mPC = 0;
  CTRL mCtrl = {};
  FLAGS mFlags = {};
  uint32_t mMod = ~0;
  int32_t mRemain = 0;
  uint32_t mDivCtrl = 0;
  uint32_t mMachi = 0;

  uint32_t mRegisterFile = 0;
  uint64_t mCycle = 0;

  Prefetch mPrefetch = {};

  struct StageRead
  {
    DSPI instruction = DSPI::EMPTY;
    uint32_t dataSrc = 0;
    uint32_t dataDst = 0;
    LocalReg regSrc = LocalReg{};
    LocalReg regDst = LocalReg{};
  } mStageRead = {};

  struct StageCompute
  {
    DSPI instruction = DSPI::EMPTY;
    uint32_t dataSrc = 0;
    uint32_t dataDst = 0;
    GlobalReg regSrc = GlobalReg{};
    GlobalReg regDst = GlobalReg{};
  } mStageCompute = {};

  struct StageWrite
  {
    static const uint32_t UPDATE_NONE   = 0x00;
    static const uint32_t UPDATE_REG    = 0x01;
    static const uint32_t UPDATE_FLAGS  = 0x02;
    static const uint32_t UPDATE_REG_L  = 0x04;

    uint32_t updateMask = UPDATE_NONE;
    RegFlags regFlags = {};
    uint32_t data = 0;
    GlobalReg regL = GlobalReg{};
    uint32_t dataL = {};

    void updateReg( GlobalReg reg, uint32_t data );
    void updateRegL( GlobalReg reg, uint32_t data );
    bool canUpdateReg() const;

  } mStageWrite = {};

  struct StageIO
  {
    enum State
    {
      IDLE,
      STORE_LONG,
      STORE_WORD,
      STORE_BYTE,
      LOAD_LONG,
      LOAD_WORD,
      LOAD_BYTE
    } state = IDLE;
    uint32_t address = 0;
    union
    {
      uint32_t data = 0;
      GlobalReg reg;
    };
  } mStageIO;

  struct LocalBus
  {
    enum State
    {
      IDLE,
      WRITE_LONG,
      WRITE_WORD,
      WRITE_BYTE,
      READ_LONG,
      READ_WORD,
      READ_BYTE
    } state = IDLE;
    uint32_t address = 0;
    union
    {
      uint32_t data = 0;
      GlobalReg reg;
    };
  } mLocalBus;

  struct Interruptor
  {
    uint64_t cycleI2S = ~0;
    uint64_t cycleTimer1 = ~0;
    uint64_t cycleTimer2 = ~0;
    uint64_t cycleMin = ~0;
    uint32_t periodI2S = 0;
    uint32_t periodTimer1 = 0;
    uint32_t periodTimer2 = 0;
  } mInterruptor;

  struct MACStage
  {
    uint64_t acc = 0;
    uint32_t size = 0;
    uint32_t cnt = 0;
    uint64_t addr = 0;
  } mMacStage;

  struct DivideUnit
  {
    int32_t cycle = -1;
    GlobalReg reg = GlobalReg{};
    uint32_t divisor = 0;
    uint32_t q = 0;
    uint32_t r = 0;
    bool busy = false;
  } mDivideUnit;

  GlobalReg mPortReadSrcReg = GlobalReg{};
  GlobalReg mPortReadDstReg = GlobalReg{};
  GlobalReg mPortWriteDstReg = GlobalReg{};
  uint32_t mPortWriteDstData = 0;

  AdvanceResult mBusGate = AdvanceResult::nop();

  std::array<uint32_t, RAM_SIZE / sizeof( uint32_t )> mLocalRAM;
  uint64_t mLastLocalRAMAccessCycle = ~0;

  static constexpr int32_t FREE = -1;

  struct RegStatus : protected std::array<int32_t, 64>
  {
    RegStatus()
    {
      std::fill( begin(), end(), FREE );
    }
    int32_t& operator[]( GlobalReg index ) noexcept
    {
      assert( index.idx < 64 );
      return std::array<int32_t, 64>::operator[]( index.idx );
    }

    int32_t operator[]( GlobalReg index ) const noexcept
    {
      assert( index.idx < 64 );
      return std::array<int32_t, 64>::operator[]( index.idx );
    }
  } mRegStatus = {};

  struct Regs : protected std::array<uint32_t, 64>
  {
    Regs()
    {
      std::fill( begin(), end(), 0 );
    }

    uint32_t& operator[]( GlobalReg index ) noexcept
    {
      assert( index.idx < 64 );
      return std::array<uint32_t, 64>::operator[]( index.idx );
    }

    uint32_t operator[]( GlobalReg index ) const noexcept
    {
      assert( index.idx < 64 );
      return std::array<uint32_t, 64>::operator[]( index.idx );
    }
  } mRegs = {};

  int mFlagsSemaphore = 0;

  std::unique_ptr<PipelineLog> mLog;
  WavFile * mWav = nullptr;
  std::filesystem::path mWavOut;
  uint32_t mClock = 0;
  uint32_t mClocksPerSample = 0;
  uint32_t mInterruptVector = 0;
};
