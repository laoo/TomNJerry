#pragma once

class NonCopyable
{
public:
  NonCopyable( NonCopyable const& ) = delete;
  NonCopyable& operator=( NonCopyable const& ) = delete;

protected:
  NonCopyable() = default;
  ~NonCopyable() = default;
};

//Global register index translated by the register file
struct GlobalReg
{
  explicit GlobalReg( int32_t idx = -1 ) : idx( idx ) {}
  int32_t idx;

  friend bool operator==( GlobalReg lhs, GlobalReg rhs ) { return lhs.idx == rhs.idx; }
};

struct LocalReg
{
  explicit LocalReg( int32_t idx = -1 ) : idx( idx ) {}
  int32_t idx;

  GlobalReg translate( int32_t file ) const
  {
    return GlobalReg{ idx >= 0 ? idx + file : idx };
  }
};


class AdvanceResult
{
public:

  static constexpr uint64_t kRegMask      = 0x000000000000003full;
  static constexpr uint64_t kValueMask    = 0x00000000ffffffffull;
  static constexpr uint64_t kAddressMask  = 0x00ffffff00000000ull;
  static constexpr uint64_t kFlagsMask    = 0xff00000000000000ull;
  static constexpr uint64_t kByteFlag     = 0x0100000000000000ull;
  static constexpr uint64_t kWordFlag     = 0x0200000000000000ull;
  static constexpr uint64_t kLongFlag     = 0x0300000000000000ull;
  static constexpr uint64_t kFinishFlag   = 0x0400000000000000ull;
  static constexpr uint64_t kWriteFlag    = 0x8000000000000000ull;

  static AdvanceResult readByte( uint32_t addres, GlobalReg reg )
  {
    return AdvanceResult{ kByteFlag | ( (uint64_t)addres << 32 ) & kAddressMask | ( uint64_t )reg.idx };
  }
  static AdvanceResult readWord( uint32_t addres, GlobalReg reg )
  {
    return AdvanceResult{ kWordFlag | ( (uint64_t)addres << 32 ) & kAddressMask | ( uint64_t )reg.idx };
  }
  static AdvanceResult readLong( uint32_t addres, GlobalReg reg )
  {
    return AdvanceResult{ kLongFlag | ( (uint64_t)addres << 32 ) & kAddressMask | ( uint64_t )reg.idx };
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
  GlobalReg getReg() const { return GlobalReg{ ( int32_t )( mData & kRegMask ) }; }

  explicit operator bool() const { return ( mData & kFlagsMask ) != 0; }

private:
  AdvanceResult( uint64_t data ) : mData( data ) {}
  uint64_t mData;
};

static constexpr int32_t REG_STATUS_FREE = -1;

struct RegStatus : protected std::array<int32_t, 64>
{
  RegStatus()
  {
    std::fill( begin(), end(), REG_STATUS_FREE );
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
};

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
};

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


static constexpr uint64_t NTSC_CLOCK = 26590906ull;
static constexpr uint64_t PAL_CLOCK = 26593900ull;
