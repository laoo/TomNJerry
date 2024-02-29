#pragma once
#include "wav.h"

class WaveOut
{
public:
  WaveOut( std::filesystem::path const& path, int sps );
  ~WaveOut();

  void put( int16_t samples[2] );

private:
  WavFile* mWav = nullptr;
  std::vector<uint32_t> mData;
};
