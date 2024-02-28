#pragma once

class RAM
{
public:
  RAM();
  uint32_t load( std::filesystem::path const& path, uint32_t address );

  uint8_t readByte( uint32_t address ) const;
  uint16_t readWord( uint32_t address ) const;
  uint32_t readLong( uint32_t address ) const;

  void writeByte( uint32_t address, uint8_t value );
  void writeWord( uint32_t address, uint16_t value );
  void writeLong( uint32_t address, uint32_t value );

  std::span<uint8_t> bytes();
  std::span<uint16_t> words();
  std::span<uint32_t> longs();

private:
  std::vector<uint8_t> mRAM;
};
