#include "DSPRaw.hpp"
#include "Ex.hpp"

DSPRaw::DSPRaw( std::filesystem::path const& path )
{
  size_t size = std::filesystem::file_size( path );
  if ( size == 0 )
    throw Ex{} << "File " << path << " is empty";

  std::ifstream fin{ path, std::ios::binary };
  mData.resize( ( size + 3 ) / 4 );
  fin.read( reinterpret_cast< char* >( mData.data() ), size );
}

uint32_t DSPRaw::address() const
{
  return 0xf1b000;
}

std::span<uint32_t const> DSPRaw::data() const
{
  return mData;
}
