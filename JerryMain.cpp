#include "Jerry.hpp"
#include "ProgramOptions.hpp" 
#include "BS94.hpp"
#include "DSPRaw.hpp"
#include "RAM.hpp"

int main( int argc, char const* argv[] )
{
  try
  {
    ProgramOptions options{ "Jerry", "DSP pipeline symulator", argc, argv};

    //BS94 input{ options.input() };
    DSPRaw input{ "d:\\home\\yarc_reloaded\\lsp_v15.bin" };

    RAM ram;
    uint32_t musicAddr = 65536;
    uint32_t bankAddr = musicAddr + ram.load( "d:\\home\\yarc_reloaded\\mod\\my.lsmusic", musicAddr );
    bankAddr = ( bankAddr + 3 ) & ~3;
    ram.load( "d:\\home\\yarc_reloaded\\mod\\my.lsbank", bankAddr );

    ram.l( 0x20 - 8, std::byteswap( musicAddr ) );
    ram.l( 0x20 - 4, std::byteswap( bankAddr ) );

    Jerry jerry;

    static constexpr uint64_t LATENCY = 3;
    

    jerry.debugWrite( input.address(), input.data() );
    jerry.debugWrite( Jerry::D_PC, input.address() );
    jerry.debugWrite( Jerry::D_CTRL, Jerry::CTRL::DSPGO );

    const uint64_t cycles = options.cycles();

    AdvanceResult result = AdvanceResult::nop();
    for ( uint64_t i = 0; i < cycles; ++i )
    {
      switch ( result.getOperation() )
      {
      case AdvanceResult::kByteFlag:
        if ( ( i & LATENCY ) == 0 )
        {
          result = jerry.busCycleRead( ram.b( result.getAddress() ) );
        }
        else
        {
          result = jerry.busCycleIdle();
        }
        break;
      case AdvanceResult::kShortFlag:
        if ( ( i & LATENCY ) == 0 )
        {
          uint32_t addr = result.getAddress();
          uint16_t value = ram.w( addr );
          result = jerry.busCycleRead( value );
        }
        else
        {
          result = jerry.busCycleIdle();
        }
        break;
      case AdvanceResult::kLongFlag:
        if ( ( i & LATENCY ) == 0 )
        {
          result = jerry.busCycleRead( ram.l( result.getAddress() ) );
        }
        else
        {
          result = jerry.busCycleIdle();
        }
        break;
      case AdvanceResult::kWriteFlag | AdvanceResult::kByteFlag:
        if ( ( i & LATENCY ) == 0 )
        {
          ram.b()[result.getAddress()] = (uint8_t)result.getValue();
          result = jerry.busCycleWrite();
        }
        else
        {
          result = jerry.busCycleIdle();
        }
        break;
      case AdvanceResult::kWriteFlag | AdvanceResult::kShortFlag:
        if ( ( i & LATENCY ) == 0 )
        {
          ram.w()[result.getAddress() >> 1] = ( uint16_t )result.getValue();
          result = jerry.busCycleWrite();
        }
        else
        {
          result = jerry.busCycleIdle();
        }
        break;
      case AdvanceResult::kWriteFlag | AdvanceResult::kLongFlag:
        if ( ( i & LATENCY ) == 0 )
        {
          ram.l()[result.getAddress() >> 2] = result.getValue();
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
