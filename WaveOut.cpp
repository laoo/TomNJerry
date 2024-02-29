#include "WaveOut.hpp"
#include "Ex.hpp"

WaveOut::WaveOut( std::filesystem::path const& path, int sps )
{
  mWav = wav_open( path.string().c_str(), WAV_OPEN_WRITE );
  if ( !mWav )
  {
    throw Ex{ "Failed to open " } << path;
  }

  wav_set_format( mWav, WAV_FORMAT_PCM );
  wav_set_num_channels( mWav, 2 );
  wav_set_valid_bits_per_sample( mWav, 16 );
  wav_set_sample_rate( mWav, sps );

  mData.reserve( 65536 );
}

WaveOut::~WaveOut()
{
  if ( mWav )
  {
    wav_close( mWav );
    mWav = nullptr;
  }
}

void WaveOut::put( int16_t samples[2] )
{
  mData.push_back( *std::bit_cast<uint32_t const*>( samples ) );
  if ( mData.size() >= 65536 )
  {
    wav_write( mWav, mData.data(), mData.size() );
    mData.clear();
  }
}
