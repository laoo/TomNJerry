#pragma once

#include <filesystem>

namespace cxxopts
{
  class Options;
  class ParseResult;
}

class ProgramOptions
{
public:
  ProgramOptions( char const* name, char const* desc, int argc, char const* argv[] );
  ProgramOptions( ProgramOptions const& ) = delete;
  ProgramOptions& operator=( ProgramOptions const& ) = delete;

  std::filesystem::path input() const;
  std::filesystem::path wavOut() const;
  uint64_t cycles() const;
  bool isNTSC() const;
  int dumpRegisters() const;
  int finish() const;

private:
  std::shared_ptr<cxxopts::Options> mOpt;
  std::shared_ptr<cxxopts::ParseResult> mRes;

  std::filesystem::path mInput = {};
  std::filesystem::path mWavOut = {};
  uint64_t mCycles = std::numeric_limits<uint64_t>::max();
  int mDumpRegisters = 0;
  int mFinish = 0;
};
