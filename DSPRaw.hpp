#pragma once

class DSPRaw
{
public:
  DSPRaw( std::filesystem::path const& path );

  uint32_t address() const;
  std::span<uint32_t const> data() const;

private:
  std::vector<uint32_t> mData;
};
