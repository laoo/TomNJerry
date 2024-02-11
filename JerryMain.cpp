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
    jerry.debugWrite( Jerry::D_MTXC, 4 );
    jerry.debugWrite( Jerry::D_MTXA, 0xf1b800 );
    jerry.debugWrite( 0xf1b800, 3 );
    jerry.debugWrite( 0xf1b804, 5 );
    jerry.debugWrite( 0xf1b808, 7 );
    jerry.debugWrite( 0xf1b80c, 9 );



    AdvanceResult result = AdvanceResult::nop();
    for ( int i = 0; i < options.cycles(); ++i )
    {
      switch ( result.getOperation() )
      {
      case AdvanceResult::kByteFlag:
        if ( i % 6 == 0 )
        {
          result = jerry.busCycleRead( (uint8_t)0x42 );
        }
        else
        {
          result = jerry.busCycleIdle();
        }
        break;
      case AdvanceResult::kShortFlag:
        if ( i % 6 == 0 )
        {
          result = jerry.busCycleRead( ( uint16_t )0x4242 );
        }
        else
        {
          result = jerry.busCycleIdle();
        }
        break;
      case AdvanceResult::kLongFlag:
        if ( i % 6 == 0 )
        {
          result = jerry.busCycleRead( ( uint32_t )0x42424242 );
        }
        else
        {
          result = jerry.busCycleIdle();
        }
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
