#include "BS94.hpp"
#include "Ex.hpp"

BS94::BS94( std::filesystem::path const& path )
{
  size_t size = std::filesystem::file_size( path );
  if ( size == 0 )
    throw Ex{} << "File " << path << " is empty";

  std::ifstream fin{ path, std::ios::binary };

  if ( size > 4 )
  {
    char magic[4];
    fin.read( magic, 4 );
    if ( magic[0] == 'B' && magic[1] == 'S' && magic[2] == '9' && magic[3] == '4' )
    {
      size -= 4;
      if ( size > 8 )
      {
        int address;
        int binarySize;

        fin.read( reinterpret_cast<char*>( &address ), 4 );
        fin.read( reinterpret_cast<char*>( &binarySize ), 4 );

        size -= 8;

        mAddress = std::byteswap( address );
        mData.resize( ( std::byteswap( binarySize ) + 3 ) / 4 );

        fin.read( reinterpret_cast<char*>( mData.data() ), size );
        return;
      }
    }
  }

  throw Ex{} << "File " << path << " is not a valid BS94 file";
}

uint32_t BS94::address() const
{
  return mAddress;
}

std::span<uint32_t const> BS94::data() const
{
  return mData;
}
