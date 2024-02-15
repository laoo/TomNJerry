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
    jerry.busCycleRequestWriteLong( Jerry::D_PC, input.address() );
    jerry.busCycleRequestWriteLong( Jerry::D_CTRL, Jerry::CTRL::DSPGO );

    const uint64_t cycles = options.cycles();

    AdvanceResult req = AdvanceResult::nop();
    for ( uint64_t i = 0; i < cycles; ++i )
    {
      switch ( i & 0x3 )
      {
      case 1:
        req = jerry.busCycleGetRequest();
        break;
      case 3:
        switch ( req.getOperation() )
        {
        case AdvanceResult::kByteFlag:
          jerry.busCycleAckReadByteRequest( ram.b( req.getAddress() ) );
          break;
        case AdvanceResult::kShortFlag:
          {
            uint32_t addr = req.getAddress();
            uint16_t value = ram.w( addr );
            jerry.busCycleAckReadWordRequest( value );
          }
          break;
        case AdvanceResult::kLongFlag:
          jerry.busCycleAckReadLongRequest( ram.l( req.getAddress() ) );
          break;
        case AdvanceResult::kWriteFlag | AdvanceResult::kByteFlag:
          ram.b()[req.getAddress()] = ( uint8_t )req.getValue();
          jerry.busCycleIdle();
          break;
        case AdvanceResult::kWriteFlag | AdvanceResult::kShortFlag:
          ram.w()[req.getAddress() >> 1] = ( uint16_t )req.getValue();
          jerry.busCycleIdle();
          break;
        case AdvanceResult::kWriteFlag | AdvanceResult::kLongFlag:
          ram.l()[req.getAddress() >> 2] = req.getValue();
          jerry.busCycleIdle();
          break;
        }
        break;
      default:
        jerry.busCycleIdle();
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
