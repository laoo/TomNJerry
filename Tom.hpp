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

  enum struct I
  {
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
    SAT8    = 32,
    SAT16   = 33,
    MOVE    = 34,
    MOVEQ   = 35,
    MOVETA  = 36,
    MOVEFA  = 37,
    MOVEI   = 38,
    LOADB   = 39,
    LOADW   = 40,
    LOAD    = 41,
    LOADP   = 42,
    LOAD14N = 43,
    LOAD15N = 44,
    STOREB  = 45,
    STOREW  = 46,
    STORE   = 47,
    STOREP  = 48,
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
    SAT24   = 62,
    UN_PACK = 63
  };

  static constexpr uint32_t TOM_BASE = 0xf00000;
  static constexpr uint32_t GPU_BASE = 0xf02100;
  static constexpr uint32_t BLT_BASE = 0xf02200;

  static constexpr uint32_t RAM_BASE = 0xf03000;
  static constexpr uint32_t RAM_SIZE = 0x1000;

  std::array<uint32_t, RAM_SIZE / sizeof( uint32_t )> mLocalRAM;
};
