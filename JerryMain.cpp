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

int main( int argc, char const* argv[] )
{
  try
  {
    //ProgramOptions options{ "Jerry", "DSP pipeline symulator", argc, argv};
    std::filesystem::path wavOut{ "out.wav" }; //options.wavOut()
    bool isNTSC = false; //options.isNTSC()
    int64_t cycles = -1; //options.cycles()
    bool dumpRegisters = false; //options.dumpRegisters()


    DSPRaw input{ "sample/excerpt_from_yarc_reloaded/lsp_v15.bin" };

    auto ram = std::make_shared<RAM>();
    uint32_t musicAddr = 0x6ad8;
    uint32_t bankAddr = 0x10C34;

    ram->load( "sample/excerpt_from_yarc_reloaded/mod/my.lsmusic", musicAddr );
    ram->load( "sample/excerpt_from_yarc_reloaded/mod/my.lsbank", bankAddr );

    ram->writeLong( 0x20 - 8, std::byteswap( musicAddr ) );
    ram->writeLong( 0x20 - 4, std::byteswap( bankAddr ) );

    auto jerry = std::make_shared<Jerry>( isNTSC, wavOut );

    jerry->debugWrite( input.address(), input.data() );
    jerry->busCycleRequestWriteLong( Jerry::JOYSTICK, 0x1000000 );
    jerry->busCycleRequestWriteLong( Jerry::D_FLAGS, Jerry::FLAGS::REGPAGE );
    jerry->busCycleRequestWriteLong( Jerry::D_PC, input.address() );
    jerry->busCycleRequestWriteLong( Jerry::D_CTRL, Jerry::CTRL::DSPGO );

    auto bus = Bus::create( ram, jerry );

#ifdef LOG_PIPELINE
    bus->advance( cycles );
#else
    loop( *bus, static_cast<uint64_t>( cycles ) );
#endif
    if ( dumpRegisters ){
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
