#include "Jerry.hpp"
#include "ProgramOptions.hpp" 
#include "BS94.hpp"

int main( int argc, char const* argv[] )
{
  try
  {
    ProgramOptions options{ "Jerry", "DSP pipeline symulator", argc, argv};

    BS94 input{ options.input() };

    Jerry jerry;
    jerry.debugWrite( Jerry::D_PC, input.address() );
    jerry.debugWrite( input.address(), input.data() );
    jerry.debugWrite( Jerry::D_CTRL, Jerry::CTRL::DSPGO );
    for ( int i = 0; i < options.cycles(); ++i )
      jerry.busCycleIdle();
  }
  catch ( const std::exception& e )
  {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
