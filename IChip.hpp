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
  static constexpr uint64_t kShortFlag    = 0x0200000000000000ull;
  static constexpr uint64_t kLongFlag     = 0x0400000000000000ull;
  static constexpr uint64_t kWriteFlag    = 0x8000000000000000ull;

  static AdvanceResult readByte( uint32_t addres, uint32_t reg )
  {
    return AdvanceResult{ kByteFlag | ( (uint64_t)addres << 32 ) & kAddressMask | ( uint64_t )reg };
  }
  static AdvanceResult readShort( uint32_t addres, uint32_t reg )
  {
    return AdvanceResult{ kShortFlag | ( (uint64_t)addres << 32 ) & kAddressMask | ( uint64_t )reg };
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
    return AdvanceResult{ kShortFlag | kWriteFlag | ( (uint64_t)addres << 32 ) & kAddressMask | (uint64_t)value };
  }
  static AdvanceResult writeLong( uint32_t addres, uint32_t value )
  {
    return AdvanceResult{ kLongFlag | kWriteFlag | ( (uint64_t)addres << 32 ) & kAddressMask | (uint64_t)value };
  }
  static AdvanceResult nop()
  {
    return AdvanceResult{ 0 };
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

class IChip
{
public:
  virtual ~IChip() = default;

  virtual void debugWrite( uint32_t address, uint32_t data ) = 0;
  virtual void debugWrite( uint32_t address, std::span<uint32_t const> data ) = 0;

  // No I/O operation in this bus cycle
  virtual AdvanceResult busCycleIdle() = 0;
  // Write request succeeded
  virtual AdvanceResult busCycleWrite() = 0;
  // Read request succeeded with a byte
  virtual AdvanceResult busCycleRead( uint8_t value ) = 0;
  // Read request succeeded with a short
  virtual AdvanceResult busCycleRead( uint16_t value ) = 0;
  // Read request succeeded with a long
  virtual AdvanceResult busCycleRead( uint32_t value ) = 0;
  // Read request succeeded with a phrase
  virtual AdvanceResult busCycleRead( uint64_t value ) = 0;




};
