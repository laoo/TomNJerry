#pragma once
#include "ExecutionUnit.hpp"

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

  void advanve();

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

private:
  void storeLong( uint32_t address, uint32_t data );
  uint32_t loadLong( uint32_t address );

private:
  std::array<ExecutionUnit, 8> mPipeline = {};
  uint32_t mPipelineTop = 7;

  std::array<uint32_t, RAM_SIZE / sizeof( uint32_t )> mLocalRAM;
  std::array<uint32_t, 64> mRegs = {};
  std::array<bool, 64> mRegLocks = {};
};
