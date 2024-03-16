#include "Jerry.hpp"
#include "ProgramOptions.hpp"
#include "BS94.hpp"
#include "DSPRaw.hpp"
#include "RAM.hpp"
#include "Bus.hpp"

void loop( Bus & bus, uint64_t cycles )
{
  auto clock = bus.jerry().clock();

  for ( uint64_t cycle = 0;; )
  {
    uint64_t size = std::min( cycles - cycle, clock );

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    int64_t slice = bus.advance( size );
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

    if ( slice != clock || end == start )
      break;

    const std::chrono::duration<double, std::milli> fp_ms = end - start;
    std::cout << "Time: " << fp_ms.count() << " ms\tSpeed: " << 1000.0 / fp_ms.count() << " %" << std::endl;

    cycle += slice;
  }
}

#define ROOT_DIR "d:\\home\\hively\\"

int main( int argc, char const* argv[] )
{
  try
  {
    auto ram = std::make_shared<RAM>();
    ProgramOptions options{ "Jerry", "DSP pipeline symulator", argc, argv};

    //BS94 input{ options.input() };
    //DSPRaw input{ ROOT_DIR "hively_player.bin" };
    DSPRaw input{ "d:\\home\\yarc_reloaded\\lsp_v15.bin" };
    uint32_t musicAddr = 0x6ad8;
    uint32_t bankAddr = 0x10C34;
    ram->load( "d:\\home\\yarc_reloaded\\mod\\my.lsmusic", musicAddr );
    ram->load( "d:\\home\\yarc_reloaded\\mod\\my.lsbank", bankAddr );

    //ram->load( ROOT_DIR "AHX_panning.bin", 0x5000 );
    //ram->load( ROOT_DIR "AHX_FilterPrecalc.bin", 0x5400 );
    //ram->load( ROOT_DIR "gone.ahx.streambits", 0x6000 );

    auto jerry = std::make_shared<Jerry>( options.isNTSC(), options.wavOut() );

    //jerry->busCycleRequestWriteLong( 0xf1d000 - 4, 0x6000 );
    //jerry->busCycleRequestWriteLong( 0xf1d000 - 8, 0x100 );
    //jerry->busCycleRequestWriteLong( 0xf1d000 - 12, 0x5400 );
    //jerry->busCycleRequestWriteLong( 0xf1d000 - 16, 0x5000 );
    //jerry->busCycleRequestWriteLong( 0xf1d000 - 20, 0 );

    jerry->debugWrite( input.address(), input.data() );
    jerry->busCycleRequestWriteLong( Jerry::JOYSTICK, 0x1000000 );
    jerry->busCycleRequestWriteLong( Jerry::D_FLAGS, Jerry::FLAGS::REGPAGE );
    jerry->busCycleRequestWriteLong( Jerry::D_PC, input.address() );
    jerry->busCycleRequestWriteLong( Jerry::D_CTRL, Jerry::CTRL::DSPGO );

    
    auto bus = Bus::create( ram, jerry );

#ifdef LOG_PIPELINE
    bus->advance( options.cycles() );
#else
    loop( *bus, static_cast<uint64_t>( options.cycles() ) );
#endif
    if ( options.dumpRegisters() ){
      int i;
      printf("Bank 0");
      for(i = 0; i < 32; ++i){
        if ( (i & 7) == 0 ){
          printf("\n%02x:",i);
        }
        printf(" %08x",jerry->getReg(i));
      }
      printf("\nBank 1");
      for(i = 0; i < 32; ++i){
        if ( (i & 7) == 0 ){
          printf("\n%02x:",i);
        }
        printf(" %08x",jerry->getReg(32+i));
      }
      printf("\n");
    }
  }
  catch ( const std::exception& e )
  {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
