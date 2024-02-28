#pragma once

class RAM;
class Jerry;

class Bus
{
public:

  static std::shared_ptr<Bus> create( std::shared_ptr<RAM> ram, std::shared_ptr<Jerry> jerry );

  int64_t advance( int64_t cycles );

  RAM & ram() { return *mRAM; }
  Jerry & jerry() { return *mJerry; }

private:
  Bus( std::shared_ptr<RAM> ram, std::shared_ptr<Jerry> jerry );

private:
  std::shared_ptr<RAM> mRAM;
  std::shared_ptr<Jerry> mJerry;

};
