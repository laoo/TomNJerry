#include "Jerry.hpp"

int main()
{
  try
  {
    Jerry jerry;
    jerry.debugWrite( Jerry::D_PC, Jerry::RAM_BASE );
    jerry.debugWrite( Jerry::D_CTRL, Jerry::CTRL::DSPGO );
    for ( int i = 0; i < 10; ++i )
      jerry.busCycleIdle();
  }
  catch ( const std::exception& e )
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
