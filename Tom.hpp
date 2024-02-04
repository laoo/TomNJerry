#pragma once
#include "IChip.hpp"

class Tom : public IChip
{
public:
  Tom() = default;
  ~Tom() override = default;

  void debugWrite( uint32_t address, uint32_t data ) override;
  void debugWrite( uint32_t address, std::span<uint32_t const> data ) override;


  AdvanceResult busCycleIdle() override;
  AdvanceResult busCycleWrite() override;
  AdvanceResult busCycleRead( uint8_t value ) override;
  AdvanceResult busCycleRead( uint16_t value ) override;
  AdvanceResult busCycleRead( uint32_t value ) override;
  AdvanceResult busCycleRead( uint64_t value ) override;

private:


  static constexpr uint32_t TOM_BASE = 0xf00000;
  static constexpr uint32_t GPU_BASE = 0xf02100;
  static constexpr uint32_t BLT_BASE = 0xf02200;

  static constexpr uint32_t RAM_BASE = 0xf03000;
  static constexpr uint32_t RAM_SIZE = 0x1000;

  std::array<uint32_t, RAM_SIZE / sizeof( uint32_t )> mLocalRAM;
};
