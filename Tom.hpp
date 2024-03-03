#pragma once
#include "Common.hpp"
#include "Opcodes.hpp"

class PipelineLog;

class Tom
{
public:
  static constexpr uint32_t HC        = 0xF00004;   // Horizontal Count
  static constexpr uint32_t VC        = 0xF00006;   // Vertical Count
  static constexpr uint32_t OB        = 0xF00010;   // Current Object Phrase
  static constexpr uint32_t OLP       = 0xF00020;   // Object List Pointer
  static constexpr uint32_t OBF       = 0xF00026;   // Object Processor Flag
  static constexpr uint32_t VMODE     = 0xF00028;   // Video Mode
  static constexpr uint32_t BORD1     = 0xF0002A;   // Border Color ( Red & Green )
  static constexpr uint32_t BORD2     = 0xF0002C;   // Border Color ( Blue )
  static constexpr uint32_t HP        = 0xF0002E;   // Horizontal Period
  static constexpr uint32_t HBB       = 0xF00030;   // Horizontal Blanking Begin
  static constexpr uint32_t HBE       = 0xF00032;   // Horizontal Blanking End
  static constexpr uint32_t HS        = 0xF00034;   // Horizontal Sync
  static constexpr uint32_t HVS       = 0xF00036;   // Horizontal Vertical Sync
  static constexpr uint32_t HDB1      = 0xF00038;   // Horizontal Display Begin One
  static constexpr uint32_t HDB2      = 0xF0003A;   // Horizontal Display Begin Two
  static constexpr uint32_t HDE       = 0xF0003C;   // Horizontal Display End
  static constexpr uint32_t VP        = 0xF0003e;   // Vertical Period
  static constexpr uint32_t VBB       = 0xF00040;   // Vertical Blanking Begin
  static constexpr uint32_t VBE       = 0xF00042;   // Vertical Blanking End
  static constexpr uint32_t VS        = 0xF00044;   // Vertical Sync
  static constexpr uint32_t VDB       = 0xF00046;   // Vertical Display Begin
  static constexpr uint32_t VDE       = 0xF00048;   // Vertical Display End
  static constexpr uint32_t VEB       = 0xF0004a;   // Vertical equalization Begin
  static constexpr uint32_t VEE       = 0xF0004c;   // Vertical equalization End
  static constexpr uint32_t VI        = 0xF0004E;   // Vertical Interrupt
  static constexpr uint32_t PIT0      = 0xF00050;   // Programmable Interrupt Timer (Lo)
  static constexpr uint32_t PIT1      = 0xF00052;   // Programmable Interrupt Timer (Hi)
  static constexpr uint32_t HEQ       = 0xF00054;   // Horizontal equalization End
  static constexpr uint32_t BWNOCRY   = 0xF00056;   // "If you set bit 2 in the undocumented register F00056, you get "black and white CRY" mode.
  static constexpr uint32_t BG        = 0xF00058;   // Background Color

  static constexpr uint32_t INT1      = 0xF000E0;   // CPU Interrupt Control Register
  static constexpr uint32_t INT2      = 0xF000E2;   // CPU Interrupt Resume Register

  static constexpr uint32_t CLUT      = 0xF00400;   // Color Lookup Table

  static constexpr uint32_t LBUFA     = 0xF00800;   // Line Buffer A
  static constexpr uint32_t LBUFB     = 0xF01000;   // Line Buffer B
  static constexpr uint32_t LBUFC     = 0xF01800;   // Line Buffer Current

  static constexpr uint32_t G_FLAGS   = 0xF02100;   //GPU Flags
  static constexpr uint32_t G_MTXC    = 0xF02104;   //GPU Matrix Control
  static constexpr uint32_t G_MTXA    = 0xF02108;   //GPU Matrix Address
  static constexpr uint32_t G_END     = 0xF0210C;   //GPU Data Organization
  static constexpr uint32_t G_PC      = 0xF02110;   //GPU Program Counter
  static constexpr uint32_t G_CTRL    = 0xF02114;   //GPU Operation Control / Status
  static constexpr uint32_t G_HIDATA  = 0xF02118;   //GPU Bus Interface high data
  static constexpr uint32_t G_REMAIN  = 0xF0211C;   //GPU Division Remainder
  static constexpr uint32_t G_DIVCTRL = 0xF0211C;   //GPU Divider control
  
  static constexpr uint32_t GPU_BASE  = 0xf02000;
  static constexpr uint32_t CTR_BASE  = 0xf02100;
  static constexpr uint32_t RAM_BASE  = 0xf03000;
  static constexpr uint32_t RAM_SIZE  = 0x001000;

  struct CTRL
  {
    static constexpr uint32_t GPUGO       = 0b0000'0000'0000'0000'0000'0000'0000'0001;
    static constexpr uint32_t CPUINT      = 0b0000'0000'0000'0000'0000'0000'0000'0010;
    static constexpr uint32_t FORCEINT0   = 0b0000'0000'0000'0000'0000'0000'0000'0100;
    static constexpr uint32_t SINGLE_STEP = 0b0000'0000'0000'0000'0000'0000'0000'1000;
    static constexpr uint32_t SINGLE_GO   = 0b0000'0000'0000'0000'0000'0000'0001'0000;
    static constexpr uint32_t G_CPULAT    = 0b0000'0000'0000'0000'0000'0000'0100'0000;
    static constexpr uint32_t G_JERLAT    = 0b0000'0000'0000'0000'0000'0000'1000'0000;
    static constexpr uint32_t G_PITLAT    = 0b0000'0000'0000'0000'0000'0001'0000'0000;
    static constexpr uint32_t G_OPLAT     = 0b0000'0000'0000'0000'0000'0010'0000'0000;
    static constexpr uint32_t G_BLITLAT   = 0b0000'0000'0000'0000'0000'0100'0000'0000;
    static constexpr uint32_t BUS_HOG     = 0b0000'0000'0000'0000'0000'1000'0000'0000;
    static constexpr uint32_t VERSION     = 0b0000'0000'0000'0000'1111'0000'0000'0000;

    uint16_t get() const;

    uint32_t intLatches = 0;
    bool gpugo = false;
  };

  struct FLAGS
  {
    static constexpr uint32_t ZERO_FLAG    = 0b0000'0000'0000'0000'0000'0000'0000'0001;
    static constexpr uint32_t CARRY_FLAG   = 0b0000'0000'0000'0000'0000'0000'0000'0010;
    static constexpr uint32_t NEGA_FLAG    = 0b0000'0000'0000'0000'0000'0000'0000'0100;
    static constexpr uint32_t IMASK        = 0b0000'0000'0000'0000'0000'0000'0000'1000;
    static constexpr uint32_t G_CPUENA     = 0b0000'0000'0000'0000'0000'0000'0001'0000;
    static constexpr uint32_t G_JERENA     = 0b0000'0000'0000'0000'0000'0000'0010'0000;
    static constexpr uint32_t G_PITENA     = 0b0000'0000'0000'0000'0000'0000'0100'0000;
    static constexpr uint32_t G_OPENA      = 0b0000'0000'0000'0000'0000'0000'1000'0000;
    static constexpr uint32_t G_BLITENA    = 0b0000'0000'0000'0000'0000'0001'0000'0000;
    static constexpr uint32_t G_CPUCLR     = 0b0000'0000'0000'0000'0000'0010'0000'0000;
    static constexpr uint32_t G_JERCLR     = 0b0000'0000'0000'0000'0000'0100'0000'0000;
    static constexpr uint32_t G_PITCLR     = 0b0000'0000'0000'0000'0000'1000'0000'0000;
    static constexpr uint32_t G_OPCLR      = 0b0000'0000'0000'0000'0001'0000'0000'0000;
    static constexpr uint32_t G_BLITCLR    = 0b0000'0000'0000'0000'0010'0000'0000'0000;
    static constexpr uint32_t REGPAGE      = 0b0000'0000'0000'0000'0100'0000'0000'0000;
    static constexpr uint32_t DMAEN        = 0b0000'0000'0000'0000'1000'0000'0000'0000;

    uint16_t get() const;

    bool z = false;
    bool c = false;
    bool n = false;
    bool imask = false;
    bool cpuena = false;
    bool jerena = false;
    bool pitena = false;
    bool opena = false;
    bool blitena = false;
    bool regpage = false;
  };


  Tom( bool isNTSC );
  ~Tom();

  void debugWrite( uint32_t address, std::span<uint32_t const> data );

  AdvanceResult busCycle();
  uint16_t busCycleRequestReadWord( uint32_t address );
  uint32_t busCycleRequestReadLong( uint32_t address );
  void busCycleRequestWriteWord( uint32_t address, uint16_t data );
  void busCycleRequestWriteLong( uint32_t address, uint32_t data );

  void ackWrite();
  void ackReadByteRequest( uint8_t value );
  void ackReadWordRequest( uint16_t value );
  void ackReadLongRequest( uint32_t value );
  void ackReadPhraseRequest( uint64_t value );

  uint32_t getReg( int32_t index ) const;

  uint64_t clock() const;

private:

  void checkInterrupt();
  void divideUnit();
  void io();
  void stageWrite();
  void compute();
  void stageRead();
  void decode();
  void prefetch();
  void halfCycle();
  void ctrlSet( uint16_t value );
  void flagsSet( uint16_t value );
  uint16_t readWord( uint32_t address ) const;
  uint32_t readLong( uint32_t address ) const;
  uint64_t readPhrase( uint32_t address ) const;
  void writeWord( uint32_t address, uint16_t data );
  void writeLong( uint32_t address, uint32_t data );
  void writePhrase( uint32_t address, uint64_t data );

private:

  uint32_t mMTXC = 0;
  uint32_t mMTXA = 0;
  uint32_t mPC = 0;
  CTRL mCtrl = {};
  FLAGS mFlags = {};
  int32_t mRemain = 0;
  uint32_t mDivCtrl = 0;
  uint32_t mHiData = 0;

  std::array<uint32_t, RAM_SIZE / sizeof( uint32_t )> mLocalRAM;
  uint64_t mLastLocalRAMAccessCycle = ~0;

  RegStatus mRegStatus = {};
  Regs mRegs = {};

  int mFlagsSemaphore = 0;

  AdvanceResult mBusGate = AdvanceResult::nop();
  uint64_t mCycle = 0;
  uint32_t mClock = 0;
  uint32_t mInterruptVector = 0;
};
