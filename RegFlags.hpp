#pragma once

struct RegFlags
{
  int8_t reg;
  int8_t z;
  int8_t n;
  int8_t c;

  RegFlags()
  {
    *std::bit_cast< uint32_t* >( this ) = ~0;
  }
};

