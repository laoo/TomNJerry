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

    AdvanceResult result = AdvanceResult::nop();
    for ( int i = 0; i < options.cycles(); ++i )
    {
      switch ( result.getOperation() )
      {
      case AdvanceResult::kByteFlag:
        break;
      case AdvanceResult::kShortFlag:
        break;
      case AdvanceResult::kLongFlag:
        break;
      case AdvanceResult::kWriteFlag | AdvanceResult::kByteFlag:
      case AdvanceResult::kWriteFlag | AdvanceResult::kShortFlag:
      case AdvanceResult::kWriteFlag | AdvanceResult::kLongFlag:
        if ( i % 6 == 0 )
        {
          result = jerry.busCycleWrite();
        }
        else
        {
          result = jerry.busCycleIdle();
        }
        break;
      default:
        result = jerry.busCycleIdle();
        break;
      }
    }
  }
  catch ( const std::exception& e )
  {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
