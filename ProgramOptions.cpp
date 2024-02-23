#include "ProgramOptions.hpp"
#include "Ex.hpp"
#include "cxxopts.hpp"

ProgramOptions::ProgramOptions( char const* name, char const* desc, int argc, char const* argv[] ) : mOpt{ std::make_shared<cxxopts::Options>( name, desc ) }
{
  mOpt->add_options()
    ( "h,help", "produce help message" )
    ( "i,input", "input file, first implicit agrument", cxxopts::value<std::string>(), "input" )
    ( "w,wav", "wave output file", cxxopts::value<std::string>(), "wav" )
    ( "c,cycles", "number of bus cycles to analyze", cxxopts::value<int>() )
    ( "system", "PAL or NTSC", cxxopts::value<std::string>()->default_value("PAL") )
    ( "r,regdump","dump register when finished")
    ;

  mOpt->
    positional_help("input").
    show_positional_help().
    parse_positional( { "input" } );

  try
  {
    mRes = std::make_shared<cxxopts::ParseResult>( mOpt->parse( argc, argv ) );
  }
  catch ( std::exception const& e )
  {
    throw Ex{} << "Error parsing command line: " << e.what();
  }

  if ( argc == 1 || mRes->count( "help" ) )
  {
    throw Ex{} << mOpt->help() << "\n";
  }

  if ( mRes->count( "cycles" ) )
  {
    mCycles = ( *mRes )["cycles"].as<int>();
  }

  if ( !mRes->count( "input" ) )
  {
    throw Ex{} << "Input file not specified.\n";
  }

  std::filesystem::path const input = ( *mRes )["input"].as<std::string>();
  if ( !std::filesystem::exists( input ) )
  {
    throw Ex{} << "Input file does not exist.\n";
  }

  if ( mRes->count( "wav" ) )
  {
    mWavOut = std::filesystem::absolute( ( *mRes )["wav"].as<std::string>() );
  }

  if ( mRes->count( "regdump" ) ) {
    mDumpRegisters = 1;
  }
  mInput = std::filesystem::absolute( input );

  {
    std::string const system = ( *mRes )["system"].as<std::string>();
    if ( system != "PAL" && system != "NTSC" )
    {
      throw Ex{} << "Invalid system: " << system << "\n";
    }
  }
}

std::filesystem::path ProgramOptions::input() const
{
  return mInput;
}

std::filesystem::path ProgramOptions::wavOut() const
{
  return mWavOut;
}

int ProgramOptions::cycles() const
{
  return mCycles;
}

bool ProgramOptions::isNTSC() const
{
  return ( *mRes )["system"].as<std::string>() == "NTSC";
}

int ProgramOptions::dumpRegisters() const
{
  return mDumpRegisters;
}
