#pragma once
#include <span>

class AdvanceResult
{
public:

  static constexpr uint64_t kRegMask      = 0x000000000000001full;
  static constexpr uint64_t kValueMask    = 0x00000000ffffffffull;
  static constexpr uint64_t kAddressMask  = 0x00ffffff00000000ull;
  static constexpr uint64_t kFlagsMask    = 0xff00000000000000ull;
  static constexpr uint64_t kByteFlag     = 0x0100000000000000ull;
  static constexpr uint64_t kWordFlag     = 0x0200000000000000ull;
  static constexpr uint64_t kLongFlag     = 0x0300000000000000ull;
  static constexpr uint64_t kFinishFlag   = 0x0400000000000000ull;
  static constexpr uint64_t kWriteFlag    = 0x8000000000000000ull;

  static AdvanceResult readByte( uint32_t addres, uint32_t reg )
  {
    return AdvanceResult{ kByteFlag | ( (uint64_t)addres << 32 ) & kAddressMask | ( uint64_t )reg };
  }
  static AdvanceResult readWord( uint32_t addres, uint32_t reg )
  {
    return AdvanceResult{ kWordFlag | ( (uint64_t)addres << 32 ) & kAddressMask | ( uint64_t )reg };
  }
  static AdvanceResult readLong( uint32_t addres, uint32_t reg )
  {
    return AdvanceResult{ kLongFlag | ( (uint64_t)addres << 32 ) & kAddressMask | ( uint64_t )reg };
  }
  static AdvanceResult writeByte( uint32_t addres, uint8_t value )
  {
    return AdvanceResult{ kByteFlag | kWriteFlag | ( (uint64_t)addres << 32 ) & kAddressMask | (uint64_t)value };
  }
  static AdvanceResult writeWord( uint32_t addres, uint16_t value )
  {
    return AdvanceResult{ kWordFlag | kWriteFlag | ( (uint64_t)addres << 32 ) & kAddressMask | (uint64_t)value };
  }
  static AdvanceResult writeLong( uint32_t addres, uint32_t value )
  {
    return AdvanceResult{ kLongFlag | kWriteFlag | ( (uint64_t)addres << 32 ) & kAddressMask | (uint64_t)value };
  }
  static AdvanceResult nop()
  {
    return AdvanceResult{ 0 };
  }
  static AdvanceResult finish()
  {
    return AdvanceResult{ kFinishFlag };
  }


  uint32_t getValue() const { return (uint32_t)mData; }
  uint32_t getAddress() const { return ( mData & kAddressMask ) >> 32; }
  // Returns 0 for nop, positive for read, negative for write
  int64_t getOperation() const { return std::bit_cast< int64_t >( mData & kFlagsMask ); }
  size_t getSize() const { return ( mData >> 56 ) & 7; }
  uint32_t getReg() const { return (uint32_t)( mData & kRegMask ); }

  explicit operator bool() const { return ( mData & kFlagsMask ) != 0; }

private:
  AdvanceResult( uint64_t data ) : mData( data ) {}
  uint64_t mData;
};
