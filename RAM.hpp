#pragma once

class RAM
{
public:
  RAM();
  uint32_t load( std::filesystem::path const& path, uint32_t address );

  uint8_t b( uint32_t address ) const;
  uint16_t w( uint32_t address ) const;
  uint32_t l( uint32_t address ) const;

  void b( uint32_t address, uint8_t value );
  void w( uint32_t address, uint16_t value );
  void l( uint32_t address, uint32_t value );

  std::span<uint8_t> b();
  std::span<uint16_t> w();
  std::span<uint32_t> l();

private:
  std::vector<uint8_t> mRAM;
};
