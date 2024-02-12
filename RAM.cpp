#include "RAM.hpp"
#include "Ex.hpp"


RAM::RAM() : mRAM{}
{
  mRAM.resize( 2 * 1024 * 1024, 0 );
}

uint32_t RAM::load( std::filesystem::path const& path, uint32_t address )
{
  size_t size = std::filesystem::file_size( path );
  if ( size == 0 )
    throw Ex{} << "File " << path << " is empty";

  std::ifstream fin{ path, std::ios::binary };

  if ( address + size > mRAM.size() )
    throw Ex{} << "File " << path << " is too big for RAM (address " << address << ", size " << size << ")";

  fin.read( reinterpret_cast< char* >( mRAM.data() + address ), size );

  return size;
}

uint8_t RAM::b( uint32_t address ) const
{
  if ( address >= mRAM.size() )
    throw Ex{} << "Address " << address << " is out of range";

  return mRAM[address];
}

uint16_t RAM::w( uint32_t address ) const
{
  if ( address >= mRAM.size() )
    throw Ex{} << "Address " << address << " is out of range";

  if ( address & 1 )
    throw Ex{} << "Odd address " << address;

  return *( (uint16_t const*)mRAM.data() + ( address >> 1 ) );
}

uint32_t RAM::l( uint32_t address ) const
{
  if ( address >= mRAM.size() )
    throw Ex{} << "Address " << address << " is out of range";

  if ( address & 3 )
    throw Ex{} << "Odd address " << address;

  return *( ( uint32_t const* )mRAM.data() + ( address >> 2 ) );
}

void RAM::b( uint32_t address, uint8_t value )
{
  if ( address >= mRAM.size() )
    throw Ex{} << "Address " << address << " is out of range";

  mRAM[address] = value;
}

void RAM::w( uint32_t address, uint16_t value )
{
  if ( address >= mRAM.size() )
    throw Ex{} << "Address " << address << " is out of range";

  if ( address & 1 )
    throw Ex{} << "Odd address " << address;

  *( (uint16_t*)mRAM.data() + ( address >> 1 ) ) = value;
}

void RAM::l( uint32_t address, uint32_t value )
{
  if ( address >= mRAM.size() )
    throw Ex{} << "Address " << address << " is out of range";

  if ( address & 3 )
    throw Ex{} << "Odd address " << address;

  *( ( uint32_t* )mRAM.data() + ( address >> 2 ) ) = value;
}

std::span<uint8_t> RAM::b()
{
  return mRAM;
}

std::span<uint16_t> RAM::w()
{
  return std::span<uint16_t>( (uint16_t*)mRAM.data(), mRAM.size() >> 1 );
}

std::span<uint32_t> RAM::l()
{
  return std::span<uint32_t>( ( uint32_t* )mRAM.data(), mRAM.size() >> 2 );
}
