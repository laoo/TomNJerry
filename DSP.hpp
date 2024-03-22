#pragma once
#include "ExecutionUnit.hpp"
#include "Prefetcher.hpp"

class DSP
{
public:

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

  DSP();
  ~DSP();

  void advance();
  void debugWrite( uint32_t address, std::span<uint32_t const> data );
  void debugWrite( uint32_t address, uint32_t data );

  static RISCOpcode mapOpcode( uint16_t );

private:
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

    uint32_t nz;
    uint16_t c;

    bool imask = false;
    bool cpuena = false;
    bool i2sena = false;
    bool tim1ena = false;
    bool tim2ena = false;
    bool ext0ena = false;
    bool ext1ena = false;
    bool regpage = false;
  };

  struct Prefetch
  {
    uint32_t queue = 0;
    uint32_t queueSize = 0;
    uint32_t decodedAddress = 0;
    bool doPrefetch = false;

    bool needsPrefetching() const
    {
      return queueSize == 0;
    }

    struct Pull
    {
      uint32_t address() const
      {
        return value >> 32;
      }
      uint16_t opcode() const
      {
        return ( value >> 26 ) & 63;
      }
      uint16_t regSrc() const
      {
        return ( value >> ( 16 + 5 ) ) & 31;
      }
      uint16_t regDst() const
      {
        return ( value >> 16 ) & 31;
      }
      uint32_t data() const
      {
        return ( value >> 16 ) & 0xffff;
      }

      explicit operator bool() const
      {
        return value != 0;
      }

      Pull( uint16_t status = 0, uint32_t address = 0, uint16_t data = 0 )
      {
        value = ( static_cast< uint64_t >( status ) & 0xffff ) | ( static_cast< uint64_t >( data ) << 16 ) | ( static_cast< uint64_t >( address ) << 32 );
      }

      uint64_t value;
    };
  };


private:
  bool isAddressLocal( uint32_t address ) const;

  void storeLong( uint32_t address, uint32_t data );

  uint32_t loadByteLocal( uint32_t address );
  uint32_t loadWordLocal( uint32_t address );
  uint32_t loadLongLocal( uint32_t address );

  std::optional<uint32_t> loadByteExternal( uint32_t address );
  std::optional<uint32_t> loadWordExternal( uint32_t address );
  std::optional<uint32_t> loadLongExternal( uint32_t address );

  void storeByteLocal( uint32_t address, uint8_t value );
  void storeWordLocal( uint32_t address, uint16_t value );
  void storeLongLocal( uint32_t address, uint32_t value );

  bool storeByteExternal( uint32_t address, uint8_t value );
  bool storeWordExternal( uint32_t address, uint16_t value );
  bool storeLongExternal( uint32_t address, uint32_t value );

  void writeFlags( uint32_t nz, uint16_t c );
  std::tuple<bool,bool,bool> readFlags() const;
  void processCycle();
  void prefetch();
  int prefetchFill();
  Prefetch::Pull prefetchPull();
  void ctrlSet( uint16_t value );
  void flagsSet( uint16_t value );

  bool fetch();
  bool fetchOpcode( RISCOpcode & out );
  int fetchOperand();

private:
  Prefetcher mPrefetcher;

  std::array<ExecutionUnit, 6> mExecutionUnitPool = {};
  uint32_t mPoolTop = 5;
  
  ExecutionUnit mReadUnit{};
  ExecutionUnit mComputeUnit{};
  ExecutionUnit mWriteUnit{};
  ExecutionUnit mMemoryUnit{};
  ExecutionUnit mDivideUnit{};


  std::array<uint32_t, RAM_SIZE / sizeof( uint32_t )> mLocalRAM;
  std::array<uint32_t, 64> mRegs = {};
  std::array<bool, 64> mRegLocks = {};
  int mFlagsSemaphore = 0;

  FLAGS mFlags = {};
  uint32_t mMTXC = 0;
  uint32_t mMTXA = 0;
  uint32_t mPC = 0;
  CTRL mCtrl = {};
  uint32_t mMod = ~0;
  int32_t mRemain = 0;
  uint32_t mDivCtrl = 0;
  uint32_t mMachi = 0;
  uint64_t mAccumulator = 0;

  Prefetch mPrefetch = {};
  uint64_t mCycle = ~0;
  uint64_t mLastLocalRAMAccessCycle = ~0;
  uint32_t mRegisterFile = 0;
  bool mExternalAccessAck = false;
  uint32_t mExternalAccessAddress = 0;
  uint32_t mExternalLoadValue = 0;

};
