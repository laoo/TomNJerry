#include "Jerry.hpp"
#include "PipelineLog.hpp"
#include "Ex.hpp"

static constexpr std::array<int16_t,1024> gWaveTable =
{
  //ROM_TRI
  0xC201, 0xC401, 0xC601, 0xC801, 0xCA01, 0xCC01, 0xCE01, 0xD001, 0xD201, 0xD401, 0xD601, 0xD801, 0xDA01, 0xDC01, 0xDE01, 0xE001,
  0xE201, 0xE401, 0xE601, 0xE801, 0xEA01, 0xEC01, 0xEE01, 0xF001, 0xF201, 0xF401, 0xF601, 0xF801, 0xFA01, 0xFC01, 0xFE01, 0x0000,
  0x01FF, 0x03FF, 0x05FF, 0x07FF, 0x09FF, 0x0BFF, 0x0DFF, 0x0FFF, 0x11FF, 0x13FF, 0x15FF, 0x17FF, 0x19FF, 0x1BFF, 0x1DFF, 0x1FFF,
  0x21FF, 0x23FF, 0x25FF, 0x27FF, 0x29FF, 0x2BFF, 0x2DFF, 0x2FFF, 0x31FF, 0x33FF, 0x35FF, 0x37FF, 0x39FF, 0x3BFF, 0x3DFF, 0x3FFF,
  0x3DFF, 0x3BFF, 0x39FF, 0x37FF, 0x35FF, 0x33FF, 0x31FF, 0x2FFF, 0x2DFF, 0x2BFF, 0x29FF, 0x27FF, 0x25FF, 0x23FF, 0x21FF, 0x1FFF,
  0x1DFF, 0x1BFF, 0x19FF, 0x17FF, 0x15FF, 0x13FF, 0x11FF, 0x0FFF, 0x0DFF, 0x0BFF, 0x09FF, 0x07FF, 0x05FF, 0x03FF, 0x01FF, 0x0000,
  0xFE01, 0xFC01, 0xFA01, 0xF801, 0xF601, 0xF401, 0xF201, 0xF001, 0xEE01, 0xEC01, 0xEA01, 0xE801, 0xE601, 0xE401, 0xE201, 0xE001,
  0xDE01, 0xDC01, 0xDA01, 0xD801, 0xD601, 0xD401, 0xD201, 0xD001, 0xCE01, 0xCC01, 0xCA01, 0xC801, 0xC601, 0xC401, 0xC201, 0xC001,
  //ROM_SINE
  0x0647, 0x0C8B, 0x12C7, 0x18F8, 0x1F19, 0x2527, 0x2B1E, 0x30FB, 0x36B9, 0x3C56, 0x41CD, 0x471C, 0x4C3F, 0x5133, 0x55F4, 0x5A81,
  0x5ED6, 0x62F1, 0x66CE, 0x6A6C, 0x6DC9, 0x70E1, 0x73B5, 0x7640, 0x7883, 0x7A7C, 0x7C29, 0x7D89, 0x7E9C, 0x7F61, 0x7FD7, 0x7FFF,
  0x7FD7, 0x7F61, 0x7E9C, 0x7D89, 0x7C29, 0x7A7C, 0x7883, 0x7640, 0x73B5, 0x70E1, 0x6DC9, 0x6A6C, 0x66CE, 0x62F1, 0x5ED6, 0x5A81,
  0x55F4, 0x5133, 0x4C3F, 0x471C, 0x41CD, 0x3C56, 0x36B9, 0x30FB, 0x2B1E, 0x2527, 0x1F19, 0x18F8, 0x12C7, 0x0C8B, 0x0647, 0x0000,
  0xF9B9, 0xF375, 0xED39, 0xE708, 0xE0E7, 0xDAD9, 0xD4E2, 0xCF05, 0xC947, 0xC3AA, 0xBE33, 0xB8E4, 0xB3C1, 0xAECD, 0xAA0C, 0xA57F,
  0xA12A, 0x9D0F, 0x9932, 0x9594, 0x9237, 0x8F1F, 0x8C4B, 0x89C0, 0x877D, 0x8584, 0x83D7, 0x8277, 0x8164, 0x809F, 0x8029, 0x8001,
  0x8029, 0x809F, 0x8164, 0x8277, 0x83D7, 0x8584, 0x877D, 0x89C0, 0x8C4B, 0x8F1F, 0x9237, 0x9594, 0x9932, 0x9D0F, 0xA12A, 0xA57F,
  0xAA0C, 0xAECD, 0xB3C1, 0xB8E4, 0xBE33, 0xC3AA, 0xC947, 0xCF05, 0xD4E2, 0xDAD9, 0xE0E7, 0xE708, 0xED39, 0xF375, 0xF9B9, 0x0000,
  //ROM_AMSINE
  0x0010, 0x0040, 0x0090, 0x00FF, 0x018E, 0x023A, 0x0304, 0x03EB, 0x04EC, 0x0608, 0x073D, 0x0888, 0x09E9, 0x0B5E, 0x0CE4, 0x0E7B,
  0x101F, 0x11CF, 0x1388, 0x1548, 0x170E, 0x18D5, 0x1A9C, 0x1C61, 0x1E20, 0x1FD8, 0x2185, 0x2326, 0x24B7, 0x2636, 0x27A1, 0x28F5,
  0x2A30, 0x2B4F, 0x2C50, 0x2D31, 0x2DF0, 0x2E8B, 0x2F00, 0x2F4D, 0x2F70, 0x2F69, 0x2F35, 0x2ED3, 0x2E43, 0x2D83, 0x2C93, 0x2B71,
  0x2A1E, 0x2899, 0x26E2, 0x24FA, 0x22E0, 0x2094, 0x1E19, 0x1B6E, 0x1894, 0x158C, 0x1259, 0x0EFB, 0x0B74, 0x07C7, 0x03F4, 0x0000,
  0xFBEB, 0xF7B9, 0xF36B, 0xEF06, 0xEA8B, 0xE5FE, 0xE163, 0xDCBC, 0xD80D, 0xD35A, 0xCEA6, 0xC9F5, 0xC54B, 0xC0AB, 0xBC19, 0xB799,
  0xB32F, 0xAEDF, 0xAAAC, 0xA69B, 0xA2AF, 0x9EEC, 0x9B56, 0x97F0, 0x94BF, 0x91C4, 0x8F04, 0x8C82, 0x8A41, 0x8844, 0x868D, 0x8520,
  0x83FF, 0x832B, 0x82A8, 0x8277, 0x829A, 0x8311, 0x83DF, 0x8505, 0x8682, 0x8859, 0x8A88, 0x8D10, 0x8FF1, 0x932A, 0x96BB, 0x9AA2,
  0x9EDF, 0xA36F, 0xA851, 0xAD83, 0xB303, 0xB8CE, 0xBEE1, 0xC539, 0xCBD3, 0xD2AC, 0xD9C0, 0xE10A, 0xE887, 0xF032, 0xF807, 0x0000,
  //ROM_12W
  0x0969, 0x12C2, 0x1BF7, 0x24F9, 0x2DB7, 0x3622, 0x3E29, 0x45BE, 0x4CD5, 0x5361, 0x5957, 0x5EAE, 0x635D, 0x675E, 0x6AAB, 0x6D40,
  0x6F1B, 0x703D, 0x70A5, 0x7056, 0x6F55, 0x6DA7, 0x6B53, 0x6861, 0x64DB, 0x60CC, 0x5C3F, 0x5742, 0x51E2, 0x4C2C, 0x4631, 0x3FFF,
  0x39A5, 0x3334, 0x2CBA, 0x2647, 0x1FE9, 0x19AF, 0x13A8, 0x0DDF, 0x0861, 0x033A, 0xFE74, 0xFA17, 0xF62A, 0xF2B4, 0xEFBB, 0xED42,
  0xEB4A, 0xE9D5, 0xE8E2, 0xE86E, 0xE876, 0xE8F5, 0xE9E5, 0xEB3D, 0xECF6, 0xEF06, 0xF162, 0xF3FF, 0xF6D1, 0xF9CA, 0xFCDF, 0x0000,
  0x0321, 0x0636, 0x092F, 0x0C01, 0x0E9E, 0x10FA, 0x130A, 0x14C3, 0x161B, 0x170B, 0x178A, 0x1792, 0x171E, 0x162B, 0x14B6, 0x12BE,
  0x1045, 0x0D4C, 0x09D6, 0x05E9, 0x018C, 0xFCC6, 0xF79F, 0xF221, 0xEC58, 0xE651, 0xE017, 0xD9B9, 0xD346, 0xCCCC, 0xC65B, 0xC001,
  0xB9CF, 0xB3D4, 0xAE1E, 0xA8BE, 0xA3C1, 0x9F34, 0x9B25, 0x979F, 0x94AD, 0x9259, 0x90AB, 0x8FAA, 0x8F5B, 0x8FC3, 0x90E5, 0x92C0,
  0x9555, 0x98A2, 0x9CA3, 0xA152, 0xA6A9, 0xAC9F, 0xB32B, 0xBA42, 0xC1D7, 0xC9DE, 0xD249, 0xDB07, 0xE409, 0xED3E, 0xF697, 0x0000,
  //ROM_CHIRP16
  0x0704, 0x0F79, 0x194E, 0x2467, 0x3092, 0x3D8B, 0x4AF0, 0x5842, 0x64E0, 0x7009, 0x78E3, 0x7E7E, 0x7FE5, 0x7C35, 0x72B3, 0x62F1,
  0x4CEA, 0x3129, 0x10E2, 0xEE00, 0xCB1A, 0xAB5D, 0x923E, 0x831E, 0x80C8, 0x8CE4, 0xA76D, 0xCE4C, 0xFD34, 0x2DE1, 0x58CA, 0x7640,
  0x7FEF, 0x726A, 0x4E79, 0x19BD, 0xDE4A, 0xA908, 0x86FC, 0x81E4, 0x9D07, 0xD339, 0x16FE, 0x555F, 0x7B06, 0x7A8A, 0x51D7, 0x0C8B,
  0xC1A4, 0x8C61, 0x823B, 0xA977, 0xF495, 0x45A2, 0x7985, 0x776B, 0x3DC2, 0xE5AF, 0x9975, 0x808C, 0xAA4D, 0x02F1, 0x5B06, 0x7FFF,
  0x59FC, 0xFD0F, 0xA0EF, 0x808C, 0xB367, 0x1A51, 0x701C, 0x776B, 0x2832, 0xBA5E, 0x8084, 0xA977, 0x17C4, 0x739F, 0x6FC6, 0x0C8B,
  0x9D97, 0x8576, 0xDCAF, 0x555F, 0x7DE9, 0x2CC7, 0xAED7, 0x81E4, 0xD651, 0x56F8, 0x7B7A, 0x19BD, 0x9AE3, 0x8D96, 0x03E0, 0x7640,
  0x5C31, 0xD21F, 0x8009, 0xCE4C, 0x5C65, 0x731C, 0xF1F1, 0x831E, 0xBE28, 0x54A3, 0x748D, 0xEE00, 0x8120, 0xCED7, 0x664E, 0x62F1,
  0xC733, 0x83CB, 0x050D, 0x7E7E, 0x2A0E, 0x8FF7, 0xB137, 0x5842, 0x67C3, 0xC275, 0x8995, 0x2467, 0x7D78, 0xF087, 0x8033, 0x0000,
  //ROM_NTRI
  0xC695, 0xCB25, 0xC9BC, 0xCD56, 0xCD2E, 0xCE03, 0xCEC0, 0xD1AC, 0xD681, 0xD7D8, 0xD623, 0xD9A3, 0xD96F, 0xDE63, 0xE18B, 0xE02F,
  0xDE31, 0xE6BA, 0xED65, 0xEA39, 0xEBAB, 0xED8B, 0xF0BA, 0xF4C5, 0xF599, 0xF43B, 0xF689, 0xF8DF, 0xF8AE, 0xFE67, 0xFE69, 0x02F5,
  0x0188, 0x06EF, 0x0217, 0x0B0F, 0x069D, 0x0967, 0x0D64, 0x0C6F, 0x11FC, 0x1409, 0x1768, 0x1607, 0x197E, 0x1B11, 0x1DD6, 0x1F99,
  0x1E69, 0x258E, 0x235F, 0x2444, 0x2601, 0x2C11, 0x2B65, 0x32BC, 0x2F1F, 0x3121, 0x3414, 0x32F5, 0x3692, 0x35FE, 0x3CE4, 0x3D5F,
  0x3891, 0x3921, 0x3658, 0x37D6, 0x35E4, 0x32A6, 0x306E, 0x2F1E, 0x2A3D, 0x2A7D, 0x2724, 0x27A4, 0x23C2, 0x25C2, 0x20A0, 0x20D0,
  0x1C6E, 0x17BF, 0x1964, 0x166B, 0x1242, 0x136F, 0x119D, 0x0E80, 0x0FEA, 0x09E2, 0x07B8, 0x0798, 0x0673, 0x02D5, 0x0163, 0xFDF2,
  0x01ED, 0xFFE5, 0xF769, 0xFC16, 0xF4CD, 0xF406, 0xF3C2, 0xF030, 0xF172, 0xEF53, 0xE812, 0xE72D, 0xE5DC, 0xE634, 0xE2AE, 0xE2F2,
  0xE023, 0xDBDF, 0xDC2B, 0xD895, 0xD6E8, 0xD8DA, 0xD12C, 0xCE93, 0xD170, 0xCBA6, 0xCC70, 0xCB45, 0xC865, 0xC5EC, 0xC4BF, 0xC580,
  //ROM_DELTA
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1FFF, 0x3FFF, 0x5FFF, 0x7FFF,
  0x5FFF, 0x3FFF, 0x1FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  //ROM_NOISE
  0xEFA6, 0x0F80, 0xF004, 0x0EA6, 0xED75, 0x1E84, 0x04C7, 0x28D4, 0x0839, 0x1630, 0x2111, 0x0B1E, 0xF61E, 0xFD1F, 0xE751, 0xCB57,
  0x03D6, 0x0DE0, 0x0683, 0xF823, 0xF553, 0x1D19, 0x0A04, 0x2917, 0x1524, 0x25A4, 0xDCB4, 0xF957, 0x1977, 0x2EF3, 0xD413, 0x0239,
  0x3210, 0x0422, 0xEFE8, 0xDB2B, 0xFE4C, 0x098E, 0x0590, 0xFB68, 0x00A8, 0x106F, 0x17A1, 0xD831, 0x10FC, 0xF066, 0x0E78, 0xF05E,
  0x1F83, 0x1974, 0xEB6F, 0xF0DE, 0x2B4D, 0x1867, 0xCE98, 0x0D20, 0x0A13, 0xE8D3, 0xDF60, 0x08DF, 0xE168, 0x1114, 0xFE44, 0xD392,
  0x14A4, 0xE895, 0xF2D8, 0x308A, 0xF985, 0xFB89, 0x190E, 0x2102, 0xF26B, 0xEE43, 0xE9FE, 0x0D8A, 0xFA2B, 0x09A5, 0x1F45, 0x1C1D,
  0xEA2A, 0xE8DB, 0x11E2, 0x0C5A, 0x14C7, 0x0864, 0xD761, 0xE86E, 0xF047, 0xF76F, 0x020E, 0xFEC9, 0xF044, 0x0627, 0x0333, 0xFA47,
  0xF5FE, 0xDC9F, 0x1B8D, 0x207F, 0x0C79, 0xE785, 0x0B90, 0xDFE8, 0x0690, 0x0AC8, 0x1AAF, 0x1EF3, 0x13FD, 0xE1CC, 0x19A2, 0xE5C4,
  0xE4F3, 0x49EC, 0xF926, 0xF37F, 0x0ABC, 0xECFD, 0x0EAA, 0xFC53, 0xFBFD, 0xF058, 0x0291, 0xE383, 0x1880, 0xF135, 0xF30C, 0xE6D6
};

Jerry::Jerry( bool isNTSC, std::filesystem::path wavOut ) : mLog{ std::make_unique<PipelineLog>() }, mClock{ isNTSC ? NTSC_CLOCK : PAL_CLOCK }, mWavePath{ std::move( wavOut ) }
{
  std::ranges::fill( mLocalRAM, std::byteswap( (uint32_t)( (uint16_t)DSPI::NOP << 10 | ( uint16_t )DSPI::NOP << ( 10 + 16 ) ) ) );

  mJoystick.ntsc = isNTSC;
}

Jerry::~Jerry()
{
}

void Jerry::debugWrite( uint32_t address, std::span<uint32_t const> data )
{
  if ( address & 3 )
    throw Ex{ "Jerry::debugWrite: Unaligned address " } << std::hex << address;

  if ( address < RAM_BASE || address + data.size() * sizeof( uint32_t ) >= RAM_BASE + RAM_SIZE )
    throw Ex{ "Jerry::debugWrite: Unhandled address " } << std::hex << address;

  std::copy( data.begin(), data.end(), mLocalRAM.begin() + ( address - RAM_BASE ) / sizeof( uint32_t ) );
}

AdvanceResult Jerry::busCycle()
{
  if ( mCtrl.dspgo )
  {
    halfCycle();
    halfCycle();
  }
  return mBusGate;
}

uint16_t Jerry::busCycleRequestReadWord( uint32_t address )
{
  busCycle();

  mLastLocalRAMAccessCycle = mCycle;
  //TODO: read word from DSP RAM should work even od address not aligned to long word
  return readWord( address );
}

uint32_t Jerry::busCycleRequestReadLong( uint32_t address )
{
  busCycle();

  mLastLocalRAMAccessCycle = mCycle;
  return readLong( address );
}

void Jerry::busCycleRequestWriteWord( uint32_t address, uint16_t data )
{
  busCycle();

  mLastLocalRAMAccessCycle = mCycle;
  //TODO: write word to DSP RAM should be ignored
  writeWord( address, data );
}

void Jerry::busCycleRequestWriteLong( uint32_t address, uint32_t data )
{
  busCycle();

  mLastLocalRAMAccessCycle = mCycle;
  writeLong( address, data );
}

void Jerry::ackReadByteRequest( uint8_t value )
{
  assert( mBusGate );
  assert( mBusGate.getOperation() > 0 );
  assert( mBusGate.getSize() == 1 );
  assert( mBusGate.getAddress() <= 0xf10000 );

  if ( portWriteDst( mBusGate.getReg(), value ) )
  {
    LOG_LOADLONG( mBusGate.getAddress(), mPortWriteDstData );
    busGatePop();
  }
  else
  {
    throw Ex{} << "Jerry::io: Unhandled read ack at $" << std::hex << std::setfill( '0' ) << std::setw( 6 ) << mStageRead.address;
  }

}

void Jerry::ackReadWordRequest( uint16_t value )
{
  assert( mBusGate );
  assert( mBusGate.getOperation() > 0 );
  assert( mBusGate.getSize() == 2 );
  assert( mBusGate.getAddress() <= 0xf10000 );

  if ( portWriteDst( mBusGate.getReg(), std::byteswap( value ) ) )
  {
    LOG_LOADWORD( mBusGate.getAddress(), mPortWriteDstData );
    busGatePop();
  }
  else
  {
    throw Ex{} << "Jerry::io: Unhandled read ack at $" << std::hex << std::setfill( '0' ) << std::setw( 6 ) << mStageRead.address;
  }
}

void Jerry::ackReadLongRequest( uint32_t value )
{
  assert( mBusGate );
  assert( mBusGate.getOperation() > 0 );
  assert( mBusGate.getSize() == 3 );
  assert( mBusGate.getAddress() <= 0xf10000 );

  if ( portWriteDst( mBusGate.getReg(), std::byteswap( value ) ) )
  {
    LOG_LOADLONG( mBusGate.getAddress(), mPortWriteDstData );
    busGatePop();
  }
  else
  {
    throw Ex{} << "Jerry::io: Unhandled read ack at $" << std::hex << std::setfill( '0' ) << std::setw( 6 ) << mStageRead.address;
  }

}

uint32_t Jerry::getReg( int32_t index ) const
{
  return mRegs[GlobalReg{ index }];
}

uint64_t Jerry::clock() const
{
  return mClock;
}

void Jerry::ackWrite()
{
  assert( mBusGate );

  switch ( mBusGate.getSize() )
  {
  case 1:
    LOG_STOREBYTE( mBusGate.getAddress(), mBusGate.getValue() );
    break;
  case 2:
    LOG_STOREWORD( mBusGate.getAddress(), mBusGate.getValue() );
    break;
  case 3:
    LOG_STORELONG( mBusGate.getAddress(), mBusGate.getValue() );
    break;
  default:
    assert( !"Jerry::ackWrite: Unhandled size " );
  }
  busGatePop();
}

uint8_t Jerry::readByte( uint32_t address ) const
{
  assert( ( address & 0xff0000 ) == 0xf10000 );

  switch ( address )
  {
  case JPIT1:
    break;
  case JPIT2:
    break;
  case JPIT3:
    break;
  case JPIT4:
    break;
  case JPIT1R:
    return mJPIT1R >> 8;
  case JPIT1R+1:
    return mJPIT1R & 0xff;
  case JPIT2R:
    return mJPIT2R  >> 8;
  case JPIT2R+1:
    return mJPIT2R & 0xff;
  case JPIT3R:
    return mJPIT3R >> 8;
  case JPIT3R+1:
    return mJPIT3R & 0xff;
  case JPIT4R:
    return mJPIT4R >> 8;
  case JPIT4R+1:
    return mJPIT4R & 0xff;
  case J_INT:
    return mJIntCtrl.get() >> 8;
  case J_INT + 2:
    break;
  case JOYSTICK:
    return mJoystick.getJoy() >> 8;
  case JOYSTICK+1:
    return mJoystick.getJoy() & 0xff;
  case JOYBUTS:
    return mJoystick.getBut() >> 8;
  case JOYBUTS+1:
    return mJoystick.getBut() & 0xff;
  case SCLK:
    break;
  case SMODE:
    break;
  case L_I2S:
    break;
  case R_I2S:
    break;
  case ASICLK:
    LOG_WARN( WARN_UNIMPLEMENTED );
    break;
  case ASICTRL:
    LOG_WARN( WARN_UNIMPLEMENTED );
    break;
  case ASIDATA:
    LOG_WARN( WARN_UNIMPLEMENTED );
    break;
  case D_FLAGS:
  case D_MTXC:
  case D_MTXA:
  case D_END:
  case D_PC:
  case D_CTRL:
  case D_MOD:
  case D_DIVCTRL:
  case D_MACHI:
    LOG_WARN( WARN_BAD_ADDRESS );
    break;
  default:
    if ( address >= RAM_BASE && address < RAM_BASE + RAM_SIZE )
    {
      LOG_WARN( WARN_BAD_ADDRESS );
      break;
    }
    else if ( address >= ROM_BASE && address < ROM_BASE + ROM_SIZE )
    {
      LOG_WARN( WARN_BAD_ADDRESS );
      break;
    }
    else
    {
      LOG_WARN( WARN_BAD_ADDRESS );
    }
    break;
  }

  return 0;
}

uint16_t Jerry::readWord( uint32_t address ) const
{
  assert( ( address & 0xff0000 ) == 0xf10000 );
  if ( ( address & 1 ) != 0 )
  {
    LOG_WARN( WARN_BAD_ADDRESS );
    address &= ~1;
  }

  switch ( address )
  {
  case JPIT1:
    break;
  case JPIT2:
    break;
  case JPIT3:
    break;
  case JPIT4:
    break;
  case JPIT1R:
    return mJPIT1R;
  case JPIT2R:
    return mJPIT2R;
  case JPIT3R:
    return mJPIT3R;
  case JPIT4R:
    return mJPIT4R;
  case J_INT:
    return mJIntCtrl.get();
  case J_INT + 2:
    break;
  case JOYSTICK:
    return mJoystick.getJoy();
  case JOYBUTS:
    return mJoystick.getBut();
  case SCLK:
    break;
  case SMODE:
    break;
  case L_I2S:
    break;
  case R_I2S:
    break;
  case ASICLK:
    LOG_WARN( WARN_UNIMPLEMENTED );
    break;
  case ASICTRL:
    LOG_WARN( WARN_UNIMPLEMENTED );
    break;
  case ASIDATA:
    LOG_WARN( WARN_UNIMPLEMENTED );
    break;
  case D_FLAGS:
  case D_MTXC:
  case D_MTXA:
  case D_END:
  case D_PC:
  case D_CTRL:
  case D_MOD:
  case D_DIVCTRL:
  case D_MACHI:
    LOG_WARN( WARN_BAD_ADDRESS );
    break;
  default:
    if ( address >= RAM_BASE && address < RAM_BASE + RAM_SIZE )
    {
      LOG_WARN( WARN_BAD_ADDRESS );
      break;
    }
    else if ( address >= ROM_BASE && address < ROM_BASE + ROM_SIZE )
    {
      LOG_WARN( WARN_BAD_ADDRESS );
      break;
    }
    else
    {
      LOG_WARN( WARN_BAD_ADDRESS );
    }
    break;
  }

  return 0;
}

uint32_t Jerry::readLong( uint32_t address ) const
{
  assert( ( address & 0xff0000 ) == 0xf10000 );
  if ( ( address & 3 ) != 0 )
  {
    LOG_WARN( WARN_BAD_ADDRESS );
    address &= ~3;
  }

  switch ( address )
  {
  case JPIT1:
  case JPIT3:
  case J_INT:
  case SCLK:
  case SMODE:
  case L_I2S:
  case R_I2S:
  case ASICLK:
  case ASICTRL:
  case ASIDATA:
    return ( (uint32_t)readWord( address ) << 16 ) | readWord( address + 2 );
  case JOYSTICK:
    return mJIntCtrl.get();
  case D_FLAGS:
    return mFlags.get();
  case D_MTXC:
    return mMTXC;
  case D_MTXA:
    return mMTXA;
  case D_END:
    LOG_WARN( WARN_BAD_ADDRESS );
    break;
  case D_PC:
    return mPC;
  case D_CTRL:
    return mCtrl.get();
  case D_MOD:
    return mMod;
  case D_REMAIN:
    return mRemain;
  case D_MACHI:
    return mMachi;
  default:
    if ( address >= RAM_BASE && address < RAM_BASE + RAM_SIZE )
    {
      return std::byteswap( mLocalRAM[( address - RAM_BASE ) / sizeof( uint32_t )] );
    }
    else if ( address >= ROM_BASE && address < ROM_BASE + ROM_SIZE )
    {
      return gWaveTable[( address - ROM_BASE ) / sizeof( uint32_t )];
    }
    else
    {
      LOG_WARN( WARN_BAD_ADDRESS );
    }
    break;
  }

  return 0;
}

void Jerry::writeByte( uint32_t address, uint8_t data )
{
  assert( ( address & 0xff0000 ) == 0xf10000 );
  if ( ( address & 1 ) != 0 )
  {
    LOG_WARN( WARN_BAD_ADDRESS );
    address &= ~1;
  }

  switch ( address )
  {
  case JPIT1:
    mJPIT1R = mJPIT1 = data;
    reconfigureTimer1();
    break;
  case JPIT2:
    mJPIT2R = mJPIT2 = data;
    reconfigureTimer1();
    break;
  case JPIT3:
    mJPIT3R = mJPIT3 = data;
    reconfigureTimer1();
    break;
  case JPIT4:
    mJPIT4R = mJPIT4 = data;
    reconfigureTimer1();
    break;
  case J_INT:
    mJIntCtrl.set( data );
    break;
  case J_INT + 2:
    break;
  case JOYSTICK:
    mJoystick.setJoy( data );
    break;
  case JOYBUTS:
    break;
  case SCLK:
    mSCLK = data & 0xff;
    reconfigureDAC();
    break;
  case SCLK + 2:
    break;
  case SMODE:
    smodeSet( data );
    reconfigureDAC();
    break;
  case SMODE + 2:
    break;
  case L_I2S:
    mI2S.left = data;
    break;
  case L_I2S + 2:
    break;
  case R_I2S:
    mI2S.right = data;
    break;
  case R_I2S + 2:
    break;
  case ASIDATA:
    break;
  case ASICTRL:
    break;
  case ASICLK:
    break;
  case JPIT1R:
  case JPIT2R:
  case JPIT3R:
  case JPIT4R:
    LOG_WARN( WARN_BAD_ADDRESS );
    break;
  case D_FLAGS:
  case D_MTXC:
  case D_MTXA:
  case D_END:
  case D_PC:
  case D_CTRL:
  case D_MOD:
  case D_DIVCTRL:
  case D_MACHI:
    LOG_WARN( WARN_BAD_ADDRESS );
    break;
  default:
    if ( address >= RAM_BASE && address < RAM_BASE + RAM_SIZE )
    {
      LOG_WARN( WARN_BAD_ADDRESS );
    }
    else if ( address >= ROM_BASE && address < ROM_BASE + ROM_SIZE )
    {
      LOG_WARN( WARN_BAD_ADDRESS );
    }
    else
    {
      LOG_WARN( WARN_BAD_ADDRESS );
    }
    break;
  }
}

void Jerry::writeWord( uint32_t address, uint16_t data )
{
  assert( ( address & 0xff0000 ) == 0xf10000 );
  if ( ( address & 1 ) != 0 )
  {
    LOG_WARN( WARN_BAD_ADDRESS );
    address &= ~1;
  }

  switch ( address )
  {
  case JPIT1:
    mJPIT1R = mJPIT1 = data;
    reconfigureTimer1();
    break;
  case JPIT2:
    mJPIT2R = mJPIT2 = data;
    reconfigureTimer1();
    break;
  case JPIT3:
    mJPIT3R = mJPIT3 = data;
    reconfigureTimer1();
    break;
  case JPIT4:
    mJPIT4R = mJPIT4 = data;
    reconfigureTimer1();
    break;
  case J_INT:
    mJIntCtrl.set( data );
    break;
  case J_INT + 2:
    break;
  case JOYSTICK:
    mJoystick.setJoy( data );
    break;
  case JOYBUTS:
    break;
  case SCLK:
    mSCLK = data & 0xff;
    reconfigureDAC();
    break;
  case SCLK + 2:
    break;
  case SMODE:
    smodeSet( data );
    reconfigureDAC();
    break;
  case SMODE + 2:
    break;
  case L_I2S:
    mI2S.left = data;
    break;
  case L_I2S + 2:
    break;
  case R_I2S:
    mI2S.right = data;
    break;
  case R_I2S + 2:
    break;
  case ASIDATA:
    break;
  case ASICTRL:
    break;
  case ASICLK:
    break;
  case JPIT1R:
  case JPIT2R:
  case JPIT3R:
  case JPIT4R:
    LOG_WARN( WARN_BAD_ADDRESS );
    break;
  case D_FLAGS:
  case D_MTXC:
  case D_MTXA:
  case D_END:
  case D_PC:
  case D_CTRL:
  case D_MOD:
  case D_DIVCTRL:
  case D_MACHI:
    LOG_WARN( WARN_BAD_ADDRESS );
    break;
  default:
    if ( address >= RAM_BASE && address < RAM_BASE + RAM_SIZE )
    {
      LOG_WARN( WARN_BAD_ADDRESS );
    }
    else if ( address >= ROM_BASE && address < ROM_BASE + ROM_SIZE )
    {
      LOG_WARN( WARN_BAD_ADDRESS );
    }
    else
    {
      LOG_WARN( WARN_BAD_ADDRESS );
    }
    break;
  }
}

void Jerry::writeLong( uint32_t address, uint32_t data )
{
  assert( ( address & 0xff0000 ) == 0xf10000 );
  if ( ( address & 3 ) != 0 )
  {
    LOG_WARN( WARN_BAD_ADDRESS );
    address &= ~3;
  }

  switch ( address )
  {
  case JPIT1:
    mJPIT1R = mJPIT1 = data >> 16;
    mJPIT2R = mJPIT2 = data & 0xffff;
    reconfigureTimer1();
    break;
  case JPIT3:
    mJPIT3R = mJPIT3 = data >> 16;
    mJPIT4R = mJPIT4 = data & 0xffff;
    reconfigureTimer2();
    break;
  case J_INT:
    mJIntCtrl.set( data );
    break;
  case L_I2S:
    mI2S.left = data & 0xffff;
    break;
  case R_I2S:
    mI2S.right = data & 0xffff;
    break;
  case ASICLK:
  case ASICTRL:
  case ASIDATA:
    break;
  case JOYSTICK:
    mJoystick.set( data );
    break;
  case SCLK:
    mSCLK = data & 0xff;
    reconfigureDAC();
    break;
  case SMODE:
    smodeSet( data & 0xffff );
    reconfigureDAC();
    break;
  case D_FLAGS:
    flagsSet( data );
    break;
  case D_MTXC:
    mMTXC = data;
    break;
  case D_MTXA:
    mMTXA = data;
    break;
  case D_END:
    if ( ( data & 1 ) == 0 || ( data & 4 ) == 0 )
      LOG_WARN( WARN_UNIMPLEMENTED );
    break;
  case D_PC:
    if ( data < RAM_BASE || data >= RAM_BASE + RAM_SIZE )
      throw Ex{ "Jerry unsupported PC " } << std::hex << data;
    mPC = data;
    break;
  case D_CTRL:
    ctrlSet( data );
    break;
  case D_MOD:
    mMod = data;
    break;
  case D_DIVCTRL:
    mDivCtrl = data;
    break;
  case D_MACHI:
    LOG_WARN( WARN_BAD_ADDRESS );
    break;
  default:
    if ( address >= RAM_BASE && address < RAM_BASE + RAM_SIZE )
    {
      mLocalRAM[( address - RAM_BASE ) / sizeof( uint32_t )] = std::byteswap( data );
    }
    else if ( address >= ROM_BASE && address < ROM_BASE + ROM_SIZE )
    {
      LOG_WARN( WARN_BAD_ADDRESS );
    }
    else
    {
      LOG_WARN( WARN_BAD_ADDRESS );
    }
    break;
  }
}

void Jerry::localBus()
{
  switch ( mLocalBus.state )
  {
  case LocalBus::WRITE_LONG:
    writeLong( mLocalBus.address, mLocalBus.data );
    mLocalBus.state = LocalBus::IDLE;
    mStageIO.state = StageIO::IDLE;
    mLastLocalRAMAccessCycle = mCycle;
    LOG_STORELONG( mLocalBus.address, mLocalBus.data );
    break;
  case LocalBus::WRITE_WORD:
    if ( mLocalBus.address < D_FLAGS ){
      writeWord( mLocalBus.address & 0xfffffe, mLocalBus.data );
      mLocalBus.state = LocalBus::IDLE;
      mStageIO.state = StageIO::IDLE;
      mLastLocalRAMAccessCycle = mCycle;
      LOG_STOREWORD( mLocalBus.address, mLocalBus.data );
    } else {
      LOG_WARN( WARN_BAD_SIZE );
      writeLong( mLocalBus.address & 0xfffffc, mLocalBus.data << ( ( ( mLocalBus.address & 1 ) ^ 1 ) * 16 ) );
      mLocalBus.state = LocalBus::IDLE;
      mStageIO.state = StageIO::IDLE;
      mLastLocalRAMAccessCycle = mCycle;
      LOG_STORELONG( mLocalBus.address, mLocalBus.data );
    }
    break;
  case LocalBus::WRITE_BYTE:
    if ( mLocalBus.address < D_FLAGS ){
      writeByte( mLocalBus.address, mLocalBus.data );
      mLocalBus.state = LocalBus::IDLE;
      mStageIO.state = StageIO::IDLE;
      mLastLocalRAMAccessCycle = mCycle;
      LOG_STOREBYTE( mLocalBus.address, mLocalBus.data );
    } else {
      LOG_WARN( WARN_BAD_SIZE );
      writeLong( mLocalBus.address & 0xfffffc, mLocalBus.data << ( ( 3 - ( mLocalBus.address & 3 ) ) * 8 ) );
      mLocalBus.state = LocalBus::IDLE;
      mStageIO.state = StageIO::IDLE;
      mLastLocalRAMAccessCycle = mCycle;
      LOG_STORELONG( mLocalBus.address, mLocalBus.data );
    }

    break;
  case LocalBus::READ_LONG:
    mStageWrite.updateRegL( mLocalBus.reg, readLong( mLocalBus.address ) );
    mLocalBus.state = LocalBus::IDLE;
    mStageIO.state = StageIO::IDLE;
    mLastLocalRAMAccessCycle = mCycle;
    LOG_LOADLONG( mLocalBus.address, mStageWrite.dataL );
    break;
  case LocalBus::READ_WORD:
    if ( mLocalBus.address < D_FLAGS ){
      mStageWrite.updateRegL( mLocalBus.reg, readWord( mLocalBus.address & 0xfffffe ) );
      LOG_LOADWORD( mLocalBus.address, mStageWrite.dataL );
    } else {
      LOG_WARN( WARN_BAD_SIZE );
      mStageWrite.updateRegL( mLocalBus.reg, readLong( mLocalBus.address & 0xfffffc ) >> ( ( ( mLocalBus.address & 1 ) ^ 1 ) * 16 ) );
      LOG_LOADLONG( mLocalBus.address, mStageWrite.dataL );
    }
    mLocalBus.state = LocalBus::IDLE;
    mStageIO.state = StageIO::IDLE;
    mLastLocalRAMAccessCycle = mCycle;
    break;
  case LocalBus::READ_BYTE:
    if ( mLocalBus.address < D_FLAGS ){
      mStageWrite.updateRegL( mLocalBus.reg, readByte( mLocalBus.address ) );
      LOG_LOADBYTE( mLocalBus.address, mStageWrite.dataL );
    } else {
      LOG_WARN( WARN_BAD_SIZE );
      mStageWrite.updateRegL( mLocalBus.reg, readLong( mLocalBus.address & 0xfffffc ) >> ( ( 3 - ( mLocalBus.address & 3 ) ) * 8 ) );
      LOG_LOADLONG( mLocalBus.address, mStageWrite.dataL );
    }
    mLocalBus.state = LocalBus::IDLE;
    mStageIO.state = StageIO::IDLE;
    mLastLocalRAMAccessCycle = mCycle;
    break;
  default:
    break;
  }
}

void Jerry::storeByte( uint32_t address, uint8_t data )
{
  if ( ( address & 0x00ff0000 ) != 0x00f10000 )
  {
    busGatePush( AdvanceResult::writeByte( address, data ) );
  }
  else if ( mLocalBus.state == LocalBus::IDLE )
  {
    mLocalBus.state = LocalBus::WRITE_BYTE;
    mLocalBus.address = address;
    mLocalBus.data = data;
  }
}

void Jerry::storeWord( uint32_t address, uint16_t data )
{
  if ( ( address & 0x00ff0000 ) != 0x00f10000 )
  {
    busGatePush( AdvanceResult::writeWord( address, std::byteswap( data ) ) );
  }
  else
  {
    mLocalBus.state = LocalBus::WRITE_WORD;
    mLocalBus.address = address;
    mLocalBus.data = data;
  }
}

void Jerry::storeLong( uint32_t address, uint32_t data )
{
  if ( ( address & 0x00ff0000 ) != 0x00f10000 )
  {
    busGatePush( AdvanceResult::writeLong( address, std::byteswap( data ) ) );
  }
  else
  {
    mLocalBus.state = LocalBus::WRITE_LONG;
    mLocalBus.address = address;
    mLocalBus.data = data;
  }
}

void Jerry::loadByte( uint32_t address, GlobalReg reg )
{
  if ( ( address & 0x00ff0000 ) != 0x00f10000 )
  {
    busGatePush( AdvanceResult::readByte( address, reg ) );
  }
  else
  {
    mLocalBus.state = LocalBus::READ_BYTE;
    mLocalBus.address = address;
    mLocalBus.reg = reg;
  }
}

void Jerry::loadWord( uint32_t address, GlobalReg reg )
{
  if ( ( address & 0x00ff0000 ) != 0x00f10000 )
  {
    busGatePush( AdvanceResult::readWord( address, reg ) );
  }
  else
  {
    mLocalBus.state = LocalBus::READ_WORD;
    mLocalBus.address = address;
    mLocalBus.reg = reg;
  }
}

void Jerry::loadLong( uint32_t address, GlobalReg reg )
{
  if ( ( address & 0x00ff0000 ) != 0x00f10000 )
  {
    busGatePush( AdvanceResult::readLong( address, reg ) );
  }
  else
  {
    mLocalBus.state = LocalBus::READ_LONG;
    mLocalBus.address = address;
    mLocalBus.reg = reg;
  }
}

void Jerry::io()
{
  localBus();
  ioDispatch();
}

void Jerry::halfCycle()
{
  checkInterrupt();
  divideUnit();
  io();
  stageWrite();
  compute();
  stageRead();
  decode();
  prefetch();
  mCycle += 1;
}

bool Jerry::testCondition( uint32_t condition ) const
{
  switch ( condition )
  {
  case 0x00:
    return true;
  case 0x01:
    return !mFlags.z;
  case 0x02:
    return mFlags.z;
  case 0x04:
    return !mFlags.c;
  case 0x05:
    return !mFlags.c && !mFlags.z;
  case 0x06:
    return !mFlags.c && mFlags.z;
  case 0x08:
    return mFlags.c;
  case 0x09:
    return mFlags.c && !mFlags.z;
  case 0x0a:
    return mFlags.c && mFlags.z;
  case 0x14:
    return !mFlags.n;
  case 0x15:
    return !mFlags.n && !mFlags.z;
  case 0x16:
    return !mFlags.n && mFlags.z;
  case 0x18:
    return mFlags.n;
  case 0x19:
    return mFlags.n && !mFlags.z;
  case 0x1a:
    return mFlags.n && mFlags.z;
  case 0x1f:
    return false;
  default:
    LOG_WARN( WARN_ILLEGAL_CONDITION );
    return true;
  }
}

void Jerry::divideUnit()
{
  if ( mDivideUnit.cycle >= 0 )
  {
    if ( mDivideUnit.cycle > 0 )
    {
      // taken from VJ. conceived by SCPCD

      uint32_t sign = mDivideUnit.r & 0x80000000;
      mDivideUnit.r = ( mDivideUnit.r << 1 ) | ( ( mDivideUnit.q >> 31 ) & 0x01 );
      mDivideUnit.r += ( sign ? mDivideUnit.divisor : -mDivideUnit.divisor );
      mDivideUnit.q = ( mDivideUnit.q << 1 ) | ( ( ( ~mDivideUnit.r ) >> 31 ) & 0x01 );

      sign = mDivideUnit.r & 0x80000000;
      mDivideUnit.r = ( mDivideUnit.r << 1 ) | ( ( mDivideUnit.q >> 31 ) & 0x01 );
      mDivideUnit.r += ( sign ? mDivideUnit.divisor : -mDivideUnit.divisor );
      mDivideUnit.q = ( mDivideUnit.q << 1 ) | ( ( ( ~mDivideUnit.r ) >> 31 ) & 0x01 );

      mDivideUnit.cycle -= 1;
      LOG_DIV( mDivideUnit.cycle );
    }

    if ( mDivideUnit.cycle == 0 )
    {
      //holding the cycle count at 0 until successfull write to the destination register
      if ( !mDivideUnit.busy )
      {
        mDivideUnit.cycle = -1;
      }
      else if ( mStageWrite.canUpdateReg() )
      {
        mStageWrite.updateReg( mDivideUnit.reg, mDivideUnit.q );
        mRemain = mDivideUnit.r;
        mDivideUnit.busy = false;
      }
    }
  }
}

void Jerry::ioDispatch()
{
  switch ( mStageIO.state )
  {
    case StageIO::STORE_BYTE:
      storeByte( mStageIO.address, ( uint8_t )mStageIO.data );
      break;
    case StageIO::STORE_WORD:
      storeWord( mStageIO.address, ( uint16_t )mStageIO.data );
      break;
    case StageIO::STORE_LONG:
      storeLong( mStageIO.address, mStageIO.data );
      break;
    case StageIO::LOAD_BYTE:
      loadByte( mStageIO.address, mStageIO.reg );
      break;
    case StageIO::LOAD_WORD:
      loadWord( mStageIO.address, mStageIO.reg );
      break;
    case StageIO::LOAD_LONG:
      loadLong( mStageIO.address, mStageIO.reg );
      break;
    default:
      break;
  }
}

bool Jerry::stageWriteReg()
{
  assert( mStageWrite.regFlags.reg >= 0 );

  if ( portWriteDst( GlobalReg( mStageWrite.regFlags.reg ), mStageWrite.data ) )
  {
    mStageWrite.regFlags.reg = -1;
    return true;
  }
  else
  {
    return false;
  }
}

bool Jerry::stageWriteRegL()
{
  assert( mStageWrite.regL.idx >= 0 );

  return portWriteDst( mStageWrite.regL, mStageWrite.dataL );
}

void Jerry::stageWriteFlags()
{
  if ( mStageWrite.regFlags.z >= 0 )
  {
    mFlags.z = mStageWrite.regFlags.z > 0;
    mStageWrite.regFlags.z = -1;
  }

  if ( mStageWrite.regFlags.c >= 0 )
  {
    mFlags.c = mStageWrite.regFlags.c > 0;
    mStageWrite.regFlags.c = -1;
  }

  if ( mStageWrite.regFlags.n >= 0 )
  {
    mFlags.n = mStageWrite.regFlags.n > 0;
    mStageWrite.regFlags.n = -1;
  }

  mFlagsSemaphore -= 1;
  assert( mFlagsSemaphore >= 0 );
}

void Jerry::stageWritePC()
{
  mPC = mStageWrite.pc;
  mPrefetch.queueSize = 0;
}

void Jerry::stageWrite()
{
  switch ( mStageWrite.updateMask )
  {
  case StageWrite::UPDATE_REG:
    mStageWrite.updateMask = stageWriteReg() ? StageWrite::UPDATE_NONE : StageWrite::UPDATE_REG;
    break;
  case StageWrite::UPDATE_FLAGS:
    mStageWrite.updateMask = StageWrite::UPDATE_NONE;
    stageWriteFlags();
    break;
  case ( StageWrite::UPDATE_REG | StageWrite::UPDATE_FLAGS ):
    mStageWrite.updateMask = stageWriteReg() ? StageWrite::UPDATE_NONE : StageWrite::UPDATE_REG;
    stageWriteFlags();
    break;
  case ( StageWrite::UPDATE_REG_L ):
    mStageWrite.updateMask = stageWriteRegL() ? StageWrite::UPDATE_NONE : StageWrite::UPDATE_REG_L;
    break;
  case ( StageWrite::UPDATE_REG_L | StageWrite::UPDATE_REG ):
    mStageWrite.updateMask = stageWriteRegL() ? StageWrite::UPDATE_REG : StageWrite::UPDATE_REG_L;
    break;
  case ( StageWrite::UPDATE_REG_L | StageWrite::UPDATE_FLAGS ):
    mStageWrite.updateMask = stageWriteRegL() ? StageWrite::UPDATE_NONE : StageWrite::UPDATE_REG_L;
    stageWriteFlags();
    break;
  case ( StageWrite::UPDATE_REG_L | StageWrite::UPDATE_REG | StageWrite::UPDATE_FLAGS ):
    mStageWrite.updateMask = stageWriteRegL() ? StageWrite::UPDATE_REG : StageWrite::UPDATE_REG_L;
    stageWriteFlags();
    break;
  case StageWrite::UPDATE_PC:
    mStageWrite.updateMask = StageWrite::UPDATE_NONE;
    stageWritePC();
    break;
  case ( StageWrite::UPDATE_REG | StageWrite::UPDATE_PC ):
    mStageWrite.updateMask = stageWriteReg() ? StageWrite::UPDATE_NONE : StageWrite::UPDATE_REG;
    stageWritePC();
    break;
  case ( StageWrite::UPDATE_FLAGS | StageWrite::UPDATE_PC ):
    mStageWrite.updateMask = StageWrite::UPDATE_NONE;
    stageWriteFlags();
    stageWritePC();
    break;
  case ( StageWrite::UPDATE_REG | StageWrite::UPDATE_FLAGS | StageWrite::UPDATE_PC ):
    mStageWrite.updateMask = stageWriteReg() ? StageWrite::UPDATE_NONE : StageWrite::UPDATE_REG;
    stageWriteFlags();
    stageWritePC();
    break;
  case ( StageWrite::UPDATE_REG_L | StageWrite::UPDATE_PC ):
    mStageWrite.updateMask = stageWriteRegL() ? StageWrite::UPDATE_NONE : StageWrite::UPDATE_REG_L;
    stageWritePC();
    break;
  case ( StageWrite::UPDATE_REG_L | StageWrite::UPDATE_REG | StageWrite::UPDATE_PC ):
    mStageWrite.updateMask = stageWriteRegL() ? StageWrite::UPDATE_REG : StageWrite::UPDATE_REG_L;
    stageWritePC();
    break;
  case ( StageWrite::UPDATE_REG_L | StageWrite::UPDATE_FLAGS | StageWrite::UPDATE_PC ):
    mStageWrite.updateMask = stageWriteRegL() ? StageWrite::UPDATE_NONE : StageWrite::UPDATE_REG_L;
    stageWriteFlags();
    stageWritePC();
    break;
  case ( StageWrite::UPDATE_REG_L | StageWrite::UPDATE_REG | StageWrite::UPDATE_FLAGS | StageWrite::UPDATE_PC ):
    mStageWrite.updateMask = stageWriteRegL() ? StageWrite::UPDATE_REG : StageWrite::UPDATE_REG_L;
    stageWriteFlags();
    stageWritePC();
    break;
  default:
    break;
  }
}

void Jerry::compute()
{
  switch ( mStageCompute.instruction )
  {
  case DSPI::ADD:
  case DSPI::ADDQ:
    if ( mStageWrite.canUpdateReg() )
    {
      auto result = mStageCompute.dataSrc + mStageCompute.dataDst;
      mStageWrite.updateReg( mStageCompute.regDst, result );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = mStageWrite.data < mStageCompute.dataSrc ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::ADDC:
    if ( mStageWrite.canUpdateReg() )
    {
      auto result = mStageCompute.dataSrc + mStageCompute.dataDst + ( mFlags.c ? 1 : 0 );
      mStageWrite.updateReg( mStageCompute.regDst, result );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = mStageWrite.data < mStageCompute.dataSrc ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::ADDQT:
    if ( mStageWrite.canUpdateReg() )
    {
      auto result = mStageCompute.dataSrc + mStageCompute.dataDst;
      mStageWrite.updateReg( mStageCompute.regDst, result );
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREG( mStageWrite.regFlags );
    }
    break;
  case DSPI::SUB:
  case DSPI::SUBQ:
    if ( mStageWrite.canUpdateReg() )
    {
      auto result = mStageCompute.dataDst - mStageCompute.dataSrc;
      mStageWrite.updateReg( mStageCompute.regDst, result );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = mStageCompute.dataSrc < mStageCompute.dataDst ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::SUBC:
    if ( mStageWrite.canUpdateReg() )
    {
      uint64_t res = (uint64_t)mStageCompute.dataDst + (uint64_t)( mStageCompute.dataSrc ^ 0xffffffff ) + (uint64_t)( mFlags.c ? 0 : 1 );
      mStageWrite.updateReg( mStageCompute.regDst, (uint32_t)res );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = ( ( res >> 32 ) & 1 ) ^ 1;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::SUBQT:
    if ( mStageWrite.canUpdateReg() )
    {
      auto result = mStageCompute.dataDst - mStageCompute.dataSrc;
      mStageWrite.updateReg( mStageCompute.regDst, result );
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREG( mStageWrite.regFlags );
    }
    break;
  case DSPI::NEG:
    if ( mStageWrite.canUpdateReg() )
    {
      auto result = -mStageCompute.dataDst;
      mStageWrite.updateReg( mStageCompute.regDst, result );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = mStageCompute.dataDst != 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::AND:
    if ( mStageWrite.canUpdateReg() )
    {
      auto result = mStageCompute.dataSrc & mStageCompute.dataDst;
      mStageWrite.updateReg( mStageCompute.regDst, result );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      //"and" copies bit 31 of the result into C flag
      mStageWrite.regFlags.c = mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::OR:
    if ( mStageWrite.canUpdateReg() )
    {
      auto result = mStageCompute.dataSrc | mStageCompute.dataDst;
      mStageWrite.updateReg( mStageCompute.regDst, result );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.regFlags.c = ( mStageCompute.dataSrc & mStageCompute.dataDst ) >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::XOR:
    if ( mStageWrite.canUpdateReg() )
    {
      auto result = mStageCompute.dataSrc ^ mStageCompute.dataDst;
      mStageWrite.updateReg( mStageCompute.regDst, result );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.regFlags.c = ( mStageCompute.dataSrc & mStageCompute.dataDst ) >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::NOT:
    if ( mStageWrite.canUpdateReg() )
    {
      auto result = ~mStageCompute.dataDst;
      mStageWrite.updateReg( mStageCompute.regDst, result );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      //r0 is 1 as NOT sets C, r1 == 0 = > C == 0
      mStageWrite.regFlags.c = mStageWrite.regFlags.z ^ 1;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::BTST:
    if ( mStageWrite.canUpdateReg() )
    {
      mStageWrite.regFlags.z = ( mStageCompute.dataDst & ( 1 << mStageCompute.dataSrc ) ) == 0 ? 1 : 0;
      //BTST clears N bit, no matter if bit 31 is tested value is set or not.
      mStageWrite.regFlags.n = 0;
      //same for C flag
      mStageWrite.regFlags.c = 0;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::BSET:
    if ( mStageWrite.canUpdateReg() )
    {
      auto result = mStageCompute.dataDst | ( 1 << mStageCompute.dataSrc );
      mStageWrite.updateReg( mStageCompute.regDst, result );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      //bset clears C flag
      mStageWrite.regFlags.c = 0;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::BCLR:
    if ( mStageWrite.canUpdateReg() )
    {
      auto result = mStageCompute.dataDst & ~( 1 << mStageCompute.dataSrc );
      mStageWrite.updateReg( mStageCompute.regDst, result );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::MULT:
    if ( mStageWrite.canUpdateReg() )
    {
      auto result = (uint16_t)mStageCompute.dataSrc * (uint16_t)mStageCompute.dataDst;
      mStageWrite.updateReg( mStageCompute.regDst, result );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      // mStageWrite.regFlags.c not affected !
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::IMULT:
    if ( mStageWrite.canUpdateReg() )
    {
      auto result = (int16_t)mStageCompute.dataSrc * (int16_t)mStageCompute.dataDst;
      mStageWrite.updateReg( mStageCompute.regDst, result );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::IMULTN:
    mMacStage.acc = (int16_t)mStageCompute.dataSrc * (int16_t)mStageCompute.dataDst;
    mStageWrite.regFlags.z = mMacStage.acc == 0 ? 1 : 0;
    mStageWrite.regFlags.n = mStageWrite.data >> 31;
    mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
    mStageCompute.instruction = DSPI::EMPTY;
    LOG_COMPUTEMUL();
    LOG_COMPUTEFLAGS( mStageWrite.regFlags );
    break;
  case DSPI::RESMAC:
    if ( mStageWrite.canUpdateReg() )
    {
      mStageWrite.updateReg( mStageCompute.regDst, mMacStage.acc );
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREG( mStageWrite.regFlags );
    }
    break;
  case DSPI::IMACN:
    mMacStage.acc += ( int16_t )mStageCompute.dataSrc * ( int16_t )mStageCompute.dataDst;
    mStageCompute.instruction = DSPI::EMPTY;
    LOG_COMPUTEMAC();
    break;
  case DSPI::ABS:
    if ( mStageWrite.canUpdateReg() )
    {
      if ( mStageCompute.dataDst == 0x80000000 ){
        mStageWrite.regFlags.z = 0;
        mStageWrite.regFlags.n = 1;
        mStageWrite.regFlags.c = 1;
      } else {
        mStageWrite.updateReg( mStageCompute.regDst,(int32_t) mStageCompute.dataDst < 0 ? (~mStageCompute.dataDst)+1 : mStageCompute.dataDst );
        mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
        mStageWrite.regFlags.n = 0;
        mStageWrite.regFlags.c = (mStageCompute.dataDst >> 31) & 1 ? 1 : 0;
      }
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::SH:
    if ( mStageWrite.canUpdateReg() )
    {
      const bool shiftRight = ( int32_t )mStageCompute.dataSrc >= 0;
      const uint32_t absShift = shiftRight ? mStageCompute.dataSrc : -mStageCompute.dataSrc;
      const uint32_t shiftResult = shiftRight ? mStageCompute.dataDst >> absShift : mStageCompute.dataDst << absShift;
      //limit amount of shift to 31, shift of 32 and above will shift out all bits and we don't want to stumble upon a problem that CPU is using only 5 lsbs to do 32-bit shift
      mStageWrite.updateReg( mStageCompute.regDst, absShift < 32 ? shiftResult : 0 );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.regFlags.c = mStageCompute.dataSrc >= 0 ? ( mStageCompute.dataDst & 1 ) : ( mStageCompute.dataDst >> 31 );
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::SHLQ:
    if ( mStageWrite.canUpdateReg() )
    {
      mStageWrite.updateReg( mStageCompute.regDst, mStageCompute.dataDst << ( 32 - mStageCompute.dataSrc ) );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.regFlags.c = mStageCompute.dataDst >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::SHRQ:
    if ( mStageWrite.canUpdateReg() )
    {
      mStageWrite.updateReg( mStageCompute.regDst, mStageCompute.dataDst >> mStageCompute.dataSrc );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.regFlags.c = mStageCompute.dataDst & 1;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::SHA:
    if ( mStageWrite.canUpdateReg() )
    {
      if ( ( int32_t )mStageCompute.dataSrc >= 0 )
      {
        //shift right
        const uint32_t absShift = mStageCompute.dataSrc;
        const int32_t shiftArg = ( int32_t )mStageCompute.dataDst;
        //limit amount of shift to 31, shift of 32 and above will shift out all bits and we don't want to stumble upon a problem that CPU is using only 5 lsbs to do 32-bit shift
        mStageWrite.updateReg( mStageCompute.regDst, absShift > 31 ? ( shiftArg >> 31 ) : ( shiftArg >> absShift ) );
      }
      else
      {
        // shift left
        const uint32_t absShift = -mStageCompute.dataSrc;
        //limit amount of shift to 31, shift of 32 and above will shift out all bits and we don't want to stumble upon a problem that CPU is using only 5 lsbs to do 32-bit shift
        mStageWrite.updateReg( mStageCompute.regDst, absShift > 31 ? 0 : ( mStageCompute.dataDst << absShift ) );
      }
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.regFlags.c = mStageCompute.dataSrc >= 0 ? ( mStageCompute.dataDst & 1 ) : ( mStageCompute.dataDst >> 31 );
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::SHARQ:
    if ( mStageWrite.canUpdateReg() )
    {
      mStageWrite.updateReg( mStageCompute.regDst, ( uint32_t )( ((int64_t)(int32_t)mStageCompute.dataDst ) >> mStageCompute.dataSrc ) );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.regFlags.c = mStageCompute.dataDst & 1;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::ROR:
  case DSPI::RORQ:
    if ( mStageWrite.canUpdateReg() )
    {
      mStageWrite.updateReg( mStageCompute.regDst, std::rotr( mStageCompute.dataDst, mStageCompute.dataSrc ) );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.regFlags.c = mStageCompute.dataDst >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::CMP:
    {
      uint32_t data = mStageCompute.dataDst - mStageCompute.dataSrc;
      mStageWrite.regFlags.z = data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = (int32_t)mStageCompute.dataSrc < ( int32_t )mStageCompute.dataDst ? 1 : 0;
      mStageWrite.regFlags.n = data >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::CMPQ:
    {
      auto i0 = ( int8_t )( mStageCompute.dataSrc << 3 );
      auto i1 = ( int64_t )i0;
      auto i2 = i1 >> 3;
      auto i3 = ( uint32_t )i2;
      uint32_t data = mStageCompute.dataDst - i3;
      mStageWrite.regFlags.z = data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = (int32_t)mStageCompute.dataSrc < ( int32_t )mStageCompute.dataDst ? 1 : 0;
      mStageWrite.regFlags.n = data >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::SUBQMOD:
    if ( mStageWrite.canUpdateReg() )
    {
      mStageWrite.updateReg( mStageCompute.regDst, ( mStageCompute.dataDst & mMod ) | ( ( mStageCompute.dataDst - mStageCompute.dataSrc ) & ( ~mMod ) ) );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = mStageCompute.dataSrc < mStageCompute.dataDst ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::SAT16S:
    if ( mStageWrite.canUpdateReg() )
    {
      mStageWrite.updateReg( mStageCompute.regDst, ( ( int32_t )mStageCompute.dataDst < -32768 ) ? -32768 : ( ( int32_t )mStageCompute.dataDst > 32767 ) ? 32767 : ( int32_t )mStageCompute.dataDst );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.regFlags.c = 0;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::SAT32S:
    if ( mStageWrite.canUpdateReg() )
    {
      int32_t temp = mMacStage.acc >> 32;
      mStageWrite.updateReg( mStageCompute.regDst, ( temp < -1 ) ? ( int32_t )0x80000000 : ( temp > 0 ) ? ( int32_t )0x7FFFFFFF : mStageCompute.dataDst );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.regFlags.c = 0;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::LOAD14N:
  case DSPI::LOAD15N:
  case DSPI::LOAD14R:
  case DSPI::LOAD15R:
    if ( mStageIO.state == StageIO::IDLE )
    {
      mStageCompute.instruction = DSPI::EMPTY;
      mStageIO.state = StageIO::LOAD_LONG;
      mStageIO.address = mStageCompute.dataSrc + mStageCompute.regDst.idx;
      mStageIO.reg = mStageCompute.regSrc;
      LOG_COMPUTEINDEX();
    }
    break;
  case DSPI::MIRROR:
    if ( mStageWrite.canUpdateReg() )
    {
      uint8_t b0 = mStageCompute.dataDst & 0xff;
      uint8_t b1 = ( mStageCompute.dataDst >> 8 ) & 0xff;
      uint8_t b2 = ( mStageCompute.dataDst >> 16 ) & 0xff;
      uint8_t b3 = ( mStageCompute.dataDst >> 24 ) & 0xff;

      //https://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith64Bits
      uint8_t m0 = ( ( b0 * 0x80200802ULL ) & 0x0884422110ULL ) * 0x0101010101ULL >> 32;
      uint8_t m1 = ( ( b1 * 0x80200802ULL ) & 0x0884422110ULL ) * 0x0101010101ULL >> 32;
      uint8_t m2 = ( ( b2 * 0x80200802ULL ) & 0x0884422110ULL ) * 0x0101010101ULL >> 32;
      uint8_t m3 = ( ( b3 * 0x80200802ULL ) & 0x0884422110ULL ) * 0x0101010101ULL >> 32;

      mStageWrite.updateReg( mStageCompute.regDst, ( m0 << 24 ) | ( m1 << 16 ) | ( m2 << 8 ) | m3 );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::STORE14N:
  case DSPI::STORE15N:
  case DSPI::STORE14R:
  case DSPI::STORE15R:
    if ( mStageIO.state == StageIO::IDLE )
    {
      //Indexed store is not guarded by the scoreboard mechanism
      portReadDstAndHiddenCommit( mStageCompute.regSrc );
      mStageCompute.instruction = DSPI::EMPTY;
      mStageIO.state = StageIO::STORE_LONG;
      mStageIO.address = mStageCompute.dataSrc + mStageCompute.regDst.idx;
      mStageIO.data = mStageCompute.dataDst;
      LOG_COMPUTEINDEX();
    }
    break;
  case DSPI::JUMP:
    //JUMP can't be executed if no instruction is in the pipeline after it
    if ( mStageRead.instruction == DSPI::EMPTY )
      break;
    if ( testCondition( mStageCompute.regDst.idx ) )
    {
      mStageWrite.pc = mStageCompute.dataDst;
      mStageWrite.updateMask |= StageWrite::UPDATE_PC;
      LOG_JUMPT();
    }
    else
    {
      LOG_JUMPF();
    }
    mStageCompute.instruction = DSPI::EMPTY;
    break;
  case DSPI::JR:
    //JR can't be executed if no instruction is in the pipeline after it
    if ( mStageRead.instruction == DSPI::EMPTY )
      break;
    if ( testCondition( mStageCompute.regDst.idx ) )
    {
      mStageWrite.pc = mStageCompute.dataSrc;
      mStageWrite.updateMask |= StageWrite::UPDATE_PC;
      LOG_JUMPT();
    }
    else
    {
      LOG_JUMPF();
    }
    mStageCompute.instruction = DSPI::EMPTY;
    break;
  case DSPI::MMULT:
    if ( mStageIO.state != StageIO::IDLE )
      LOG_WARN( WARN_IO_PENDING ); //MMULT instruction executed while IO was in progress
    mStageCompute.instruction = DSPI::EMPTY;
    break;
  case DSPI::MTOI:
    if ( mStageWrite.canUpdateReg() )
    {
      if ( mRegStatus[mStageCompute.regDst] != FREE )
        LOG_WARN( WARN_REG_IN_USE );

      mStageWrite.updateReg( mStageCompute.regDst, ( ( ( int32_t )mStageCompute.dataSrc >> 8 ) & 0xFF800000 ) | ( mStageCompute.dataSrc & 0x007FFFFF ) );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::NORMI:
    if ( mStageWrite.canUpdateReg() )
    {
      if ( mRegStatus[mStageCompute.regDst] != FREE )
        LOG_WARN( WARN_REG_IN_USE );

      uint32_t data = 0;

      if ( mStageCompute.dataSrc )
      {
        while ( ( mStageCompute.dataSrc & 0xffc00000 ) == 0 )
        {
          mStageCompute.dataSrc <<= 1;
          data--;
        }
        while ( ( mStageCompute.dataSrc & 0xff800000 ) != 0 )
        {
          mStageCompute.dataSrc >>= 1;
          data++;
        }
      }
      mStageWrite.updateReg( mStageCompute.regDst, data );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::ADDQMOD:
    if ( mStageWrite.canUpdateReg() )
    {
      mStageWrite.updateReg( mStageCompute.regDst, ( mStageCompute.dataDst& mMod ) | ( ( mStageCompute.dataSrc + mStageCompute.dataDst ) & ( ~mMod ) ) );
      mStageWrite.regFlags.z = mStageWrite.data == 0 ? 1 : 0;
      mStageWrite.regFlags.c = mStageWrite.data < mStageCompute.dataSrc ? 1 : 0;
      mStageWrite.regFlags.n = mStageWrite.data >> 31;
      mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
      mStageCompute.instruction = DSPI::EMPTY;
      LOG_COMPUTEREGFLAGS( mStageWrite.regFlags );
    }
    break;
  case DSPI::MM_IMULTN:
    mMacStage.acc = ( int16_t )mStageCompute.dataSrc * ( int16_t )mStageWrite.data;
    mStageCompute.instruction = DSPI::EMPTY;
    LOG_COMPUTEMUL();
    break;
  case DSPI::MM_IMACN:
  {
    int32_t add = ( int16_t )mStageCompute.dataSrc * ( int16_t )mStageWrite.data;
    int32_t old = mMacStage.acc;
    mMacStage.acc += add;
    mStageCompute.instruction = DSPI::EMPTY;
    mStageWrite.regFlags.z = mMacStage.acc == 0 ? 1 : 0;
    mStageWrite.regFlags.c = ( int32_t )mMacStage.acc < ( int32_t )old ? 1 : 0;
    mStageWrite.regFlags.n = mMacStage.acc >> 31;
    mStageWrite.updateMask |= StageWrite::UPDATE_FLAGS;
    LOG_COMPUTEMACARG( mStageWrite.regFlags );
  }
    break;
  default:
    break;
  }
}

void Jerry::stageRead()
{
  if ( mStageCompute.instruction != DSPI::EMPTY )
  {
    dualPortCommit();
    return;
  }

  auto regSrc = mStageRead.regSrc.translate( mStageRead.regFile );
  auto regDst = mStageRead.regDst.translate( mStageRead.regFile );

  switch ( mStageRead.instruction )
  {
  case DSPI::EMPTY:
    dualPortCommit();
    break;
  case DSPI::NOP:
    dualPortCommit();
    mStageRead.instruction = DSPI::EMPTY;
    break;
  case DSPI::ADD:
  case DSPI::SUB:
  case DSPI::AND:
  case DSPI::OR:
  case DSPI::XOR:
  case DSPI::MULT:
  case DSPI::IMULT:
  case DSPI::SH:
  case DSPI::SHA:
  case DSPI::ROR:
    if ( portReadBoth( regSrc, regDst ) )
    {
      dualPortCommit();
      mFlagsSemaphore += 1;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = regDst;
      mStageCompute.dataSrc = mStageRead.dataSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::ADDC:
  case DSPI::SUBC:
    if ( mFlagsSemaphore == 0 && portReadBoth( regSrc, regDst ) )
    {
      dualPortCommit();
      mFlagsSemaphore += 1;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = regDst;
      mStageCompute.dataSrc = mStageRead.dataSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::ADDQ:
  case DSPI::SUBQ:
  case DSPI::SUBQMOD:
  case DSPI::ADDQMOD:
  case DSPI::SHRQ:
  case DSPI::SHARQ:
  case DSPI::RORQ:

    if ( portReadDst( regDst ) )
    {
      dualPortCommit();
      LOG_PORTIMM( mStageRead.regSrc.idx );
      mFlagsSemaphore += 1;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = regDst;
      mStageCompute.dataSrc = mStageRead.regSrc.idx == 0 ? 32 : mStageRead.regSrc.idx;
      mStageCompute.dataDst = mStageRead.dataDst;
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::BSET:
  case DSPI::BCLR:
    if ( portReadDst( regDst ) )
    {
      dualPortCommit();
      LOG_PORTIMM( mStageRead.regSrc.idx );
      mFlagsSemaphore += 1;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = regDst;
      mStageCompute.dataSrc = mStageRead.regSrc.idx;
      mStageCompute.dataDst = mStageRead.dataDst;
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::SHLQ:
    if ( portReadDst( regDst ) )
    {
      dualPortCommit();
      LOG_PORTIMM( 32 - mStageRead.regSrc.idx );
      mFlagsSemaphore += 1;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = regDst;
      mStageCompute.dataSrc = mStageRead.regSrc.idx == 0 ? 32 : mStageRead.regSrc.idx;
      mStageCompute.dataDst = mStageRead.dataDst;
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::NEG:
  case DSPI::NOT:
  case DSPI::ABS:
  case DSPI::SAT16S:
  case DSPI::SAT32S:
  case DSPI::MIRROR:
    if ( portReadDst( regDst ) )
    {
      dualPortCommit();
      mFlagsSemaphore += 1;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = regDst;
      mStageCompute.dataDst = mStageRead.dataDst;
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::ADDQT:
  case DSPI::SUBQT:
    if ( portReadDst( regDst ) )
    {
      dualPortCommit();
      LOG_PORTIMM( mStageRead.regSrc.idx );
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = regDst;
      mStageCompute.dataSrc = mStageRead.regSrc.idx == 0 ? 32 : mStageRead.regSrc.idx;
      mStageCompute.dataDst = mStageRead.dataDst;
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::BTST:
  case DSPI::CMPQ:
    if ( portReadDst( regDst ) )
    {
      dualPortCommit();
      LOG_PORTIMM( mStageRead.regSrc.idx );
      mFlagsSemaphore += 1;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.regSrc.idx;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::IMULTN:
    //TODO: make instruction after it atomic
    if ( portReadBoth( regSrc, regDst ) )
    {
      dualPortCommit();
      mFlagsSemaphore += 1;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::IMACN:
    //TODO: make instruction after it atomic
    if ( portReadBoth( regSrc, regDst ) )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::CMP:
    if ( portReadBoth( regSrc, regDst ) )
    {
      dualPortCommit();
      mFlagsSemaphore += 1;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::MTOI:
  case DSPI::NORMI:
    if ( portReadSrc( regSrc ) )
    {
      dualPortCommit();
      mFlagsSemaphore += 1;
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = regDst;
      mStageCompute.dataSrc = mStageRead.dataSrc;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::RESMAC:
    if ( mStageWrite.canUpdateReg() )
    {
      if ( mRegStatus[regDst] != FREE )
        LOG_WARN( WARN_REG_IN_USE );
      dualPortCommit();
      mStageWrite.updateReg( regDst, ( uint32_t )mMacStage.acc );
      mStageRead.instruction = DSPI::EMPTY;
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::MOVEQ:
    if ( mStageWrite.canUpdateReg() )
    {
      if ( mRegStatus[regDst] != FREE )
        LOG_WARN( WARN_REG_IN_USE );
      dualPortCommit();
      LOG_PORTIMM( mStageRead.regSrc.idx );
      mStageWrite.updateReg( regDst, mStageRead.regSrc.idx );
      mStageRead.instruction = DSPI::EMPTY;
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::MOVEFA:
    if ( mStageWrite.canUpdateReg() && portReadSrc( mStageRead.regSrc.translate( mStageRead.regFile ^ 32 ) ) )
    {
      if ( mRegStatus[regDst] != FREE )
        LOG_WARN( WARN_REG_IN_USE );
      dualPortCommit();
      mStageWrite.updateReg( regDst, mStageRead.dataSrc );
      mStageRead.instruction = DSPI::EMPTY;
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::DIV:
    if ( mDivideUnit.cycle <= 0 && portReadBoth( regSrc, regDst ) )
    {
      if ( mDivideUnit.cycle == 0 && ( mDivideUnit.reg == regDst || mDivideUnit.reg == regSrc ) )
        LOG_WARN( WARN_DIV_TOO_CLOSE ); //Two consecutive DIV instructions are too close to each other

      dualPortCommit();
      mStageRead.instruction = DSPI::EMPTY;
      mDivideUnit.cycle = 16;
      mDivideUnit.reg = regDst;
      mDivideUnit.divisor = mStageRead.dataSrc;
      mDivideUnit.q = mStageRead.dataDst;
      mDivideUnit.r = 0;
      mDivideUnit.busy = true;
      if ( mDivCtrl & 0x01 )
      {
        mDivideUnit.q <<= 16;
        mDivideUnit.r = mStageCompute.dataDst >> 16;
      }
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::MOVE:
    if ( mStageWrite.canUpdateReg() && portReadSrc( regSrc ) )
    {
      if ( mRegStatus[regDst] != FREE )
        LOG_WARN( WARN_REG_IN_USE );

      dualPortCommit();
      mStageWrite.updateReg( regDst, mStageRead.dataSrc );
      mStageRead.instruction = DSPI::EMPTY;
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::MOVETA:
    if ( mStageWrite.canUpdateReg() && portReadSrc( regSrc ) )
    {
      dualPortCommit();
      auto altReg = mStageRead.regDst.translate( mStageRead.regFile ^ 32 );
      mStageWrite.updateReg( altReg, mStageRead.dataSrc );
      mStageRead.instruction = DSPI::EMPTY;
      lockReg( altReg );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::MOVEI:
    if ( mStageWrite.canUpdateReg() )
    {
      if ( mRegStatus[regDst] != FREE )
        LOG_WARN( WARN_REG_IN_USE );
      dualPortCommit();
      mStageRead.instruction = DSPI::EMPTY;
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::LOADB:
    if ( mStageIO.state == StageIO::IDLE && portReadSrc( regSrc ) )
    {
      dualPortCommit();
      mStageRead.instruction = DSPI::EMPTY;
      mStageIO.state = StageIO::LOAD_BYTE;
      mStageIO.address = mStageRead.dataSrc;
      mStageIO.reg = regDst;
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::LOADW:
    if ( mStageIO.state == StageIO::IDLE && portReadSrc( regSrc ) )
    {
      dualPortCommit();
      mStageRead.instruction = DSPI::EMPTY;
      mStageIO.state = StageIO::LOAD_WORD;
      mStageIO.address = mStageRead.dataSrc;
      mStageIO.reg = regDst;
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::LOAD:
    if ( mStageIO.state == StageIO::IDLE && portReadSrc( regSrc ) )
    {
      dualPortCommit();
      mStageRead.instruction = DSPI::EMPTY;
      mStageIO.state = StageIO::LOAD_LONG;
      mStageIO.address = mStageRead.dataSrc;
      mStageIO.reg = regDst;
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::LOAD14R:
    if ( portReadBoth( LocalReg{ 14 }.translate( mStageRead.regFile ), regSrc ) )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;   //base address to store
      mStageCompute.regSrc = regDst;                //register to store to
      mStageCompute.regDst = GlobalReg( mStageRead.dataDst );    //offset
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::LOAD15R:
    if ( portReadBoth( LocalReg{ 15 }.translate( mStageRead.regFile ), regSrc) )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;   //base address to store
      mStageCompute.regSrc = regDst;     //register to store to
      mStageCompute.regDst = GlobalReg( mStageRead.dataDst );    //offset
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::LOAD14N:
    if ( portReadSrc( LocalReg{ 14 }.translate( mStageRead.regFile ) ) )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;   //base address to store
      mStageCompute.regSrc = regDst;     //register to store
      mStageCompute.regDst = GlobalReg( (mStageRead.regSrc.idx == 0 ? 32 : mStageRead.regSrc.idx) * 4 );     //offset
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::LOAD15N:
    if ( portReadSrc( LocalReg{ 15 }.translate( mStageRead.regFile ) ) )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;   //base address to store
      mStageCompute.regSrc = regDst;     //register to store
      mStageCompute.regDst = GlobalReg( (mStageRead.regSrc.idx == 0 ? 32 : mStageRead.regSrc.idx) * 4 );     //offset
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::STOREB:
    if ( mStageIO.state == StageIO::IDLE && portReadBoth( regDst, regSrc ) )
    {
      dualPortCommit();
      mStageRead.instruction = DSPI::EMPTY;
      mStageIO.state = StageIO::STORE_BYTE;
      mStageIO.address = mStageRead.dataDst;
      mStageIO.data = mStageRead.dataSrc;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::STOREW:
    if ( mStageIO.state == StageIO::IDLE && portReadBoth( regDst, regSrc ) )
    {
      dualPortCommit();
      mStageRead.instruction = DSPI::EMPTY;
      mStageIO.state = StageIO::STORE_WORD;
      mStageIO.address = mStageRead.dataDst;
      mStageIO.data = mStageRead.dataSrc;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::STORE:
    if ( mStageIO.state == StageIO::IDLE && portReadBoth( regDst, regSrc ) )
    {
      dualPortCommit();
      mStageRead.instruction = DSPI::EMPTY;
      mStageIO.state = StageIO::STORE_LONG;
      mStageIO.address = mStageRead.dataDst;
      mStageIO.data = mStageRead.dataSrc;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::STORE14R:
    if ( portReadBoth( LocalReg{ 14 }.translate( mStageRead.regFile ), regSrc ) )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;                 //base address to store
      mStageCompute.regSrc = regDst;                              //register to store
      mStageCompute.regDst = GlobalReg( mStageRead.dataDst );     //offset
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::STORE15R:
    if ( portReadBoth( LocalReg{ 15 }.translate( mStageRead.regFile ), regSrc ) )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;                 //base address to store
      mStageCompute.regSrc = regDst;                              //register to store
      mStageCompute.regDst = GlobalReg( mStageRead.dataDst );     //offset
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::STORE14N:
    if ( portReadSrc( LocalReg{ 14 }.translate( mStageRead.regFile ) ) )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;                                   //base address to store
      mStageCompute.regSrc = regDst;                                                //register to store
      mStageCompute.regDst = GlobalReg( (mStageRead.regSrc.idx == 0 ? 32 : mStageRead.regSrc.idx) * 4 );    //offset
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::STORE15N:
    if ( portReadSrc( LocalReg{ 15 }.translate( mStageRead.regFile ) ) )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;                                   //base address to store
      mStageCompute.regSrc = regDst;                                                //register to store
      mStageCompute.regDst = GlobalReg( (mStageRead.regSrc.idx == 0 ? 32 : mStageRead.regSrc.idx) * 4 );    //offset
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::MOVEPC:
    if ( mStageWrite.canUpdateReg() )
    {
      if ( mRegStatus[regDst] != FREE )
        LOG_WARN( WARN_REG_IN_USE );
      dualPortCommit();
      mStageWrite.updateReg( regDst, mPC - ( mPrefetch.queueSize + 1 ) * 2 );
      mStageRead.instruction = DSPI::EMPTY;
      lockReg( regDst );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::JUMP:
    //JUMP w/o condition does not wait for flags
    if ( ( mFlagsSemaphore == 0 || mStageRead.regDst.idx == 0 ) && portReadDst( regSrc ) )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      LOG_PORTCOND( mStageRead.regDst.idx );
      mStageCompute.regDst = GlobalReg{ mStageRead.regDst.idx };
      mStageCompute.dataDst = mStageRead.dataDst;
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::JR:
    if ( mFlagsSemaphore == 0 )
    {
      dualPortCommit();
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.regDst = GlobalReg{ mStageRead.regDst.idx };
      int32_t off = ( ( int8_t )( mStageRead.regSrc.idx << 3 ) >> 3 );
      mStageCompute.dataSrc = mPC + ( off - mPrefetch.queueSize ) * 2;
      LOG_JR( mStageRead.regDst.idx, mStageCompute.dataSrc );
    }
    else
    {
      dualPortCommit();
    }
    break;
  case DSPI::MMULT:
    dualPortCommit();
    std::swap( mStageRead.instruction, mStageCompute.instruction );
    mStageCompute.regSrc = mStageRead.regSrc.translate( 32 );
    mStageCompute.regDst = regDst;
    break;
  case DSPI::MM_IMACN:
    mFlagsSemaphore += 1;
    [[fallthrough]];
  case DSPI::MM_IMULTN:
    if ( portReadSrc( GlobalReg( mStageCompute.regSrc.idx + ( mMacStage.cnt >> 1 ) ) ) )
    {
      dualPortCommitMMULT( ( mMacStage.cnt & 1 ) == 1 );
      std::swap( mStageRead.instruction, mStageCompute.instruction );
      mStageCompute.dataSrc = mStageRead.dataSrc;
      mStageIO.state = StageIO::LOAD_LONG;
      mStageIO.address = mMTXA + mMacStage.cnt * 4 * ( ( mMTXC & 16 ) ? mMacStage.size : 1 );
      //destination register used as a temporary register
      mStageIO.reg = mStageCompute.regDst;
      mMacStage.cnt += 1;
    }
    break;
  case DSPI::MM_RESMAC:
    //resetting write flag of internal memory load
    mPortWriteDstReg = GlobalReg{};
    mStageWrite.updateReg( regDst, ( uint32_t )mMacStage.acc );

    mStageRead.instruction = DSPI::EMPTY;
    break;
  }
}

void Jerry::decode()
{
  if ( mStageRead.instruction != DSPI::EMPTY )
  {
    mPrefetch.doPrefetch = mPrefetch.needsPrefetching() && mPrefetch.status < Prefetch::NO_PREFETCH;
    return;
  }

  auto pull = prefetchPull();

  switch ( pull.status() )
  {
  case Prefetch::OPCODE:
  case Prefetch::MACSEQ:
    mStageRead.address = pull.address();
    mStageRead.instruction = pull.opcode();
    mStageRead.regSrc = pull.regSrc();
    mStageRead.regDst = pull.regDst();
    mStageRead.regFile = mRegisterFile;
    LOG_DECODEDSP( mStageRead.instruction, mStageRead.regSrc.idx, mStageRead.regDst.idx );
    if ( mStageRead.address )
      LOG_INSTRADDR( mStageRead.address );
    break;
  case Prefetch::MOVEI1:
    mStageRead.dataSrc = pull.data();
    LOG_DECODEMOVEI( 0, mStageRead.dataSrc );
    break;
  case Prefetch::MOVEI2:
    mStageRead.dataSrc |= pull.data() << 16;
    mStageWrite.updateReg( mStageRead.regDst.translate( mStageRead.regFile ), mStageRead.dataSrc );
    LOG_DECODEMOVEI( 1, mStageRead.dataSrc );
    break;
  case Prefetch::IMULTN:
    mStageRead.address = pull.address();
    mStageRead.instruction = DSPI::MM_IMULTN;
    LOG_DECODEIMULTN( mStageCompute.regSrc.idx, 4 * ( ( mMTXC & 16 ) ? mMacStage.size : 1 ), mMacStage.cnt );
    break;
  case Prefetch::IMACN:
    mStageRead.instruction = DSPI::MM_IMACN;
    LOG_DECODEIMACN( mStageCompute.regSrc.idx, 4 * ( ( mMTXC & 16 ) ? mMacStage.size : 1 ), mMacStage.cnt );
    break;
  case Prefetch::RESMAC:
    mStageRead.instruction = DSPI::MM_RESMAC;
    LOG_DECODERESMAC( mStageCompute.regDst.idx );
    break;
  default:
    break;
  }
}

void Jerry::prefetch()
{
  //Only local RAM for now
  assert( ( mPC >= RAM_BASE ) && ( mPC < RAM_BASE + RAM_SIZE ) );

  if ( mPrefetch.doPrefetch && mLastLocalRAMAccessCycle != mCycle )
  {
    if ( int off = prefetchFill() )
    {
      LOG_PREFETCH( mPC );
      mLastLocalRAMAccessCycle = mCycle;
      mPC += off;
    }
  }

  LOG_FLUSH();
}

Jerry::Prefetch::Pull Jerry::prefetchPull()
{
  switch ( mPrefetch.status )
  {
  case Prefetch::OPCODE:
    if ( mPrefetch.queueSize == 0 )
    {
      mPrefetch.doPrefetch = true;
      return { Prefetch::EMPTY, 0, 0 };
    }
    else
    {
      uint16_t fetch = mPrefetch.queue & 0xffff;

      switch ( ( DSPI )( fetch >> 10 ) )
      {
      case DSPI::MOVEI:
        mPrefetch.status = Prefetch::MOVEI1;
        break;
      case DSPI::MMULT:
        mPrefetch.status = Prefetch::IMULTN;
        mMacStage.size = mMTXC & 15;
        mMacStage.cnt = 0;
        mMacStage.addr = mMTXA;
        if ( mMacStage.addr < 0xf1b000 || mMacStage.addr > 0xf1bffc )
        {
          mMacStage.addr = 0xf1b000;
          LOG_WARN( WARN_BAD_MTXA ); //Invalid D_MTXA address
        }
        if ( mMacStage.size < 3 )
        {
          mMacStage.size = 3;
          LOG_WARN( WARN_BAD_MATRIX_SIZE ); //MMULT with invalid size
        }
        break;
      case DSPI::IMULTN:
        //Only difference between MACSEQ and OPCODE is that MACSEQ is uniterruptible
        mPrefetch.status = Prefetch::MACSEQ;
        break;
      case DSPI::JUMP:
      case DSPI::JR:
        mPrefetch.status = Prefetch::JUMP1;
        break;
      default:
        break;
      }
      mPrefetch.queue >>= 16;
      mPrefetch.queueSize -= 1;
      mPrefetch.doPrefetch = mPrefetch.needsPrefetching();
      mPrefetch.decodedAddress += 2;
      return { Prefetch::OPCODE, mPrefetch.decodedAddress - 2, fetch };
    }
  case Prefetch::MACSEQ:
    if ( mPrefetch.queueSize == 0 )
    {
      mPrefetch.doPrefetch = true;
      return { Prefetch::EMPTY, 0, 0 };
    }
    else
    {
      uint16_t fetch = mPrefetch.queue & 0xffff;
      mPrefetch.queue >>= 16;
      mPrefetch.queueSize -= 1;
      mPrefetch.doPrefetch = mPrefetch.needsPrefetching();
      mPrefetch.decodedAddress += 2;
      Prefetch::Pull result{ Prefetch::OPCODE, mPrefetch.decodedAddress - 2, fetch };
      if ( result.opcode() == DSPI::RESMAC )
      {
        mPrefetch.status = mInterruptVector ? Prefetch::INT0 : Prefetch::OPCODE;
      }
      return result;
    }
  case Prefetch::JUMP1:
    if ( mPrefetch.queueSize == 0 )
    {
      mPrefetch.doPrefetch = true;
      return { Prefetch::EMPTY, 0, 0 };
    }
    else
    {
      uint16_t fetch = mPrefetch.queue & 0xffff;
      mPrefetch.queue >>= 16;
      mPrefetch.queueSize -= 1;
      mPrefetch.doPrefetch = mPrefetch.needsPrefetching();
      mPrefetch.decodedAddress += 2;
      Prefetch::Pull result{ Prefetch::OPCODE, mPrefetch.decodedAddress - 2, fetch };
      mPrefetch.status = Prefetch::JUMP2;
      return result;
    }
    case Prefetch::JUMP2:
      mPrefetch.doPrefetch = false;
      mPrefetch.status = mInterruptVector ? Prefetch::INT0 : Prefetch::OPCODE;
      return { Prefetch::EMPTY, 0, 0 };
    case Prefetch::MOVEI1:
    if ( mPrefetch.queueSize == 0 )
    {
      mPrefetch.doPrefetch = true;
      return { Prefetch::EMPTY, 0, 0 };
    }
    else
    {
      uint16_t fetch = mPrefetch.queue & 0xffff;
      mPrefetch.status = Prefetch::MOVEI2;
      mPrefetch.queue >>= 16;
      mPrefetch.queueSize -= 1;
      mPrefetch.decodedAddress += 2;
      mPrefetch.doPrefetch = mPrefetch.needsPrefetching();
      return { Prefetch::MOVEI1, 0, fetch };
    }
  case Prefetch::MOVEI2:
    if ( mPrefetch.queueSize == 0 )
    {
      mPrefetch.doPrefetch = true;
      return { Prefetch::EMPTY, 0, 0 };
    }
    else
    {
      uint16_t fetch = mPrefetch.queue & 0xffff;
      mPrefetch.status = mInterruptVector ? Prefetch::INT0 : Prefetch::OPCODE;
      mPrefetch.queue >>= 16;
      mPrefetch.queueSize -= 1;
      mPrefetch.doPrefetch = mPrefetch.needsPrefetching();
      mPrefetch.decodedAddress += 2;
      return { Prefetch::MOVEI2, 0, fetch };
    }
  case Prefetch::IMULTN:
    mPrefetch.doPrefetch = false;
    mPrefetch.status = Prefetch::IMACN;
    return { Prefetch::IMULTN, 0, 0 };
  case Prefetch::IMACN:
    if ( mMacStage.cnt < mMacStage.size )
    {
      mPrefetch.doPrefetch = false;
      return { Prefetch::IMACN, 0, 0 };
    }
    else
    {
      mPrefetch.status = mInterruptVector ? Prefetch::INT0 : Prefetch::OPCODE;
      mPrefetch.doPrefetch = mPrefetch.needsPrefetching() && mInterruptVector == 0;
      return { Prefetch::RESMAC, 0, 0 };
    }
  case Prefetch::INT0:
    mPrefetch.doPrefetch = false;
    mPrefetch.status = Prefetch::INT1;
    LOG_TAGUNINTERRUPTIBLESEQUENCE();
    mRegisterFile = 0;
    return { Prefetch::OPCODE, 0, ( ( uint16_t )DSPI::SUBQT << 10 ) | ( 4 << 5 ) | 31 };
  case Prefetch::INT1:
    mPrefetch.doPrefetch = false;
    mPrefetch.status = Prefetch::INT2;
    LOG_TAGUNINTERRUPTIBLESEQUENCE();
    return { Prefetch::OPCODE, 0, ( ( uint16_t )DSPI::MOVEPC << 10 ) | 30 };
  case Prefetch::INT2:
    mPrefetch.doPrefetch = false;
    mPrefetch.status = Prefetch::INT3;
    LOG_TAGUNINTERRUPTIBLESEQUENCE();
    return { Prefetch::OPCODE, 0, ( ( uint16_t )DSPI::STORE << 10 ) | ( 31 << 5 ) | 30 };
  case Prefetch::INT3:
    mPrefetch.doPrefetch = false;
    mPrefetch.status = Prefetch::INT4;
    LOG_TAGUNINTERRUPTIBLESEQUENCE();
    return { Prefetch::OPCODE, 0, ( ( uint16_t )DSPI::MOVEI << 10 ) | 30 };
  case Prefetch::INT4:
    mPrefetch.doPrefetch = false;
    mPrefetch.status = Prefetch::INT5;
    LOG_TAGUNINTERRUPTIBLESEQUENCE();
    return { Prefetch::MOVEI1, 0, mInterruptVector & 0xffff };
  case Prefetch::INT5:
    mPrefetch.doPrefetch = false;
    mPrefetch.status = Prefetch::INT6;
    LOG_TAGUNINTERRUPTIBLESEQUENCE();
    return { Prefetch::MOVEI2, 0, mInterruptVector >> 16 };
  case Prefetch::INT6:
    mPrefetch.doPrefetch = false;
    mPrefetch.status = Prefetch::INT7;
    LOG_TAGUNINTERRUPTIBLESEQUENCE();
    mInterruptVector = 0;
    return { Prefetch::OPCODE, 0, ( ( uint16_t )DSPI::JUMP << 10 ) | ( 30 << 5 ) };
  case Prefetch::INT7:
    mPrefetch.doPrefetch = false;
    mPrefetch.status = Prefetch::JUMP2;
    LOG_TAGUNINTERRUPTIBLESEQUENCE();
    return { Prefetch::OPCODE, 0, ( ( uint16_t )DSPI::NOP << 10 ) };
  default:
    mPrefetch.doPrefetch = mPrefetch.needsPrefetching();
    assert( false );
    return { Prefetch::EMPTY, 0, 0 };
  }
}

int Jerry::prefetchFill()
{
  assert( mPrefetch.queueSize < 4 );
  assert( ( mPC & 1 ) == 0 );

  mPrefetch.decodedAddress = mPC;

  if ( mPC & 2 )
  {
    auto pCode = ( uint16_t const* )mLocalRAM.data() + ( ( mPC - RAM_BASE ) >> 1 );

    //fetching word from odd address
    mPrefetch.queue = std::byteswap( *pCode );
    mPrefetch.queueSize = 1;
    return 2;
  }
  else
  {
    auto pCode = (uint16_t const*)( ( uint8_t const* )mLocalRAM.data() + ( mPC - RAM_BASE ) );

    mPrefetch.queue = std::byteswap( pCode[0] );
    mPrefetch.queue |= ( uint32_t )std::byteswap( pCode[1] ) << 16;

    mPrefetch.queueSize = 2;
    return 4;
  }

  return 0;
}


bool Jerry::portWriteDst( GlobalReg reg, uint32_t data )
{
  if ( mPortWriteDstReg.idx >= 0 )
    return false;

  mPortWriteDstReg = reg;
  mPortWriteDstData = data;
  assert( mPortWriteDstReg.idx >= 0 );
  return true;
}

bool Jerry::portReadSrc( GlobalReg regSrc )
{
  assert( regSrc.idx >= 0 );

  if ( mRegStatus[regSrc] != FREE && mPortWriteDstReg != regSrc )
    return false;

  if ( mPortReadSrcReg.idx >= 0 )
    return false;

  mPortReadSrcReg = regSrc;

  return true;
}

bool Jerry::portReadDst( GlobalReg regDst )
{
  assert( regDst.idx >= 0 );

  if ( mRegStatus[regDst] != FREE && mPortWriteDstReg != regDst )
    return false;

  if ( mPortReadDstReg.idx >= 0 )
    return false;

  mPortReadDstReg = regDst;

  return true;
}

void Jerry::portReadDstAndHiddenCommit( GlobalReg regDst )
{
  assert( mPortReadDstReg.idx < 0 );
  assert( regDst.idx >= 0 );

  mPortReadDstReg = regDst;

  if ( mRegStatus[mPortReadDstReg] != FREE )
  {
    if ( mPortWriteDstReg != mPortReadDstReg )
    {
      LOG_WARN( WARN_REG_IN_USE );
    }
    else
    {
      //a hack for indexed addressing done in compute stage
      mStageCompute.dataDst = mPortWriteDstData;
    }
  }
  else
  {
    //a hack for indexed addressing done in compute stage
    mStageCompute.dataDst = mRegs[mPortReadDstReg];
  }
}

bool Jerry::portReadBoth( GlobalReg regSrc, GlobalReg regDst )
{
  assert( regSrc.idx >= 0 );
  assert( regDst.idx >= 0 );

  if ( mRegStatus[regSrc] != FREE && mPortWriteDstReg != regSrc )
    return false;

  if ( mRegStatus[regDst] != FREE && mPortWriteDstReg != regDst )
    return false;

  if ( mPortWriteDstReg.idx >= 0 && mPortWriteDstReg != regSrc && mPortWriteDstReg != regDst )
    return false;

  if ( mPortReadSrcReg.idx >= 0 || mPortReadDstReg.idx >= 0 )
    return false;

  mPortReadSrcReg = regSrc;
  mPortReadDstReg = regDst;

  return true;
}

void Jerry::dualPortCommit()
{
  if ( mPortWriteDstReg.idx >= 0 )
  {
    if ( mRegStatus[mPortWriteDstReg] != mRegisterFile && mRegStatus[GlobalReg{mPortWriteDstReg.idx ^ 32}] == ( mRegisterFile ^ 32 ) )
    {
      //regpage has been changed after register lock and before write-back
      mRegStatus[GlobalReg{ mPortWriteDstReg.idx ^ 32 }] = FREE;
      LOG_WARN( WARN_REGPAGE_HAZARD );
    }
    else
    {
      mRegStatus[mPortWriteDstReg] = FREE;
    }
    mRegs[mPortWriteDstReg] = mPortWriteDstData;
    LOG_PORTWRITEDST( mPortWriteDstReg.idx, mPortWriteDstData );
    mPortWriteDstReg = GlobalReg{};
  }

  if ( mPortReadSrcReg.idx >= 0 )
  {
    mStageRead.dataSrc = mRegs[mPortReadSrcReg];
    LOG_PORTREADSRC( mPortReadSrcReg.idx, mStageRead.dataSrc );
    mPortReadSrcReg = GlobalReg{};
  }

  if ( mPortReadDstReg.idx >= 0 )
  {
    mStageRead.dataDst = mRegs[mPortReadDstReg];
    LOG_PORTREADDST( mPortReadDstReg.idx, mStageRead.dataDst );
    mPortReadDstReg = GlobalReg{};
  }
}

void Jerry::lockReg( GlobalReg reg )
{
  if ( mRegStatus[reg] != FREE )
    LOG_WARN( WARN_REG_IN_USE );

  mRegStatus[reg] = mRegisterFile;
}

void Jerry::dualPortCommitMMULT( bool high )
{
  if ( mPortReadSrcReg.idx >= 0 )
  {
    mStageRead.dataSrc = high ? mRegs[mPortReadSrcReg] >> 16 : mRegs[mPortReadSrcReg] & 0xffff;
    LOG_PORTREADSRCMMULT( mPortReadSrcReg.idx, high, mStageRead.dataSrc );
    mPortReadSrcReg = GlobalReg{};
  }
}

void Jerry::busGatePush( AdvanceResult result )
{
  if ( !mBusGate )
  {
    mBusGate = result;
  }
}

void Jerry::busGatePop()
{
  mBusGate = AdvanceResult::nop();
  mStageIO.state = StageIO::IDLE;
}

uint16_t Jerry::JINTCTRL::get() const
{
  return
    ( extpend ? J_EXTENA : 0 ) |
    ( dsppend ? J_DSPENA : 0 ) |
    ( tim1pend ? J_TIM1ENA : 0 ) |
    ( tim2pend ? J_TIM2ENA : 0 ) |
    ( asynpend ? J_ASYNENA : 0 ) |
    ( synpend ? J_SYNENA : 0 );
}

void Jerry::JINTCTRL::set( uint16_t value )
{
  extena = ( value & J_EXTENA ) != 0;
  dspena = ( value & J_DSPENA ) != 0;
  tim1ena = ( value & J_TIM1ENA ) != 0;
  tim2ena = ( value & J_TIM2ENA ) != 0;
  asynena = ( value & J_ASYNENA ) != 0;
  synena = ( value & J_SYNENA ) != 0;
  extpend &= ( value & J_EXTCLR ) == 0;
  dsppend &= ( value & J_DSPCLR ) == 0;
  tim1pend &= ( value & J_TIM1CLR ) == 0;
  tim2pend &= ( value & J_TIM2CLR ) == 0;
  asynpend &= ( value & J_ASYNCLR ) == 0;
  synpend &= ( value & J_SYNCLR ) == 0;
}

uint16_t Jerry::CTRL::get() const
{
  return
    0x00002000 |  //version
    ( dspgo ? DSPGO : 0 ) |
    intLatches;
}

void Jerry::ctrlSet( uint16_t value )
{
  mCtrl.dspgo = ( value & CTRL::DSPGO ) != 0;

  if ( value & CTRL::CPUINT )
    cpuint();

  if ( value & CTRL::FORCEINT0 )
    forceint0();

  if ( value & ( CTRL::SINGLE_STEP | CTRL::SINGLE_GO ) )
    LOG_WARN( WARN_UNIMPLEMENTED );

  if ( value & CTRL::BUS_HOG )
    LOG_WARN( WARN_UNIMPLEMENTED );

  if ( !mCtrl.dspgo )
    mBusGate = AdvanceResult::finish();
}

void Jerry::flagsSet( uint16_t value )
{
  mFlags.z = ( value & FLAGS::ZERO_FLAG ) != 0;
  mFlags.c = ( value & FLAGS::CARRY_FLAG ) != 0;
  mFlags.n = ( value & FLAGS::NEGA_FLAG ) != 0;
  mFlags.imask &= ( value & FLAGS::IMASK ) != 0;
  mFlags.cpuena = ( value & FLAGS::D_CPUENA ) != 0;
  mFlags.i2sena = ( value & FLAGS::D_I2SENA ) != 0;
  mFlags.tim1ena = ( value & FLAGS::D_TIM1ENA ) != 0;
  mFlags.tim2ena = ( value & FLAGS::D_TIM2ENA ) != 0;
  mFlags.ext0ena = ( value & FLAGS::D_EXT0ENA ) != 0;
  mFlags.ext1ena = ( value & FLAGS::D_EXT1ENA ) != 0;
  mCtrl.intLatches &= ( value & FLAGS::D_CPUCLR ) != 0 ? ~CTRL::D_CPULAT : 0xffff;
  mCtrl.intLatches &= ( value & FLAGS::D_I2SCLR ) != 0 ? ~CTRL::D_I2SLAT : 0xffff;
  mCtrl.intLatches &= ( value & FLAGS::D_TIM1CLR ) != 0 ? ~CTRL::D_TIM1LAT : 0xffff;
  mCtrl.intLatches &= ( value & FLAGS::D_TIM2CLR ) != 0 ? ~CTRL::D_TIM2LAT : 0xffff;
  mCtrl.intLatches &= ( value & FLAGS::D_EXT0CLR ) != 0 ? ~CTRL::D_EXT0LAT : 0xffff;
  mCtrl.intLatches &= ( value & FLAGS::D_EXT1CLR ) != 0 ? ~CTRL::D_EXT1LAT : 0xffff;
  mFlags.regpage = ( value & FLAGS::REGPAGE ) != 0;

  if ( value & FLAGS::DMAEN )
    LOG_WARN( WARN_UNIMPLEMENTED );

  mRegisterFile = mFlags.regpage ? 32 : 0;

  if ( !mFlags.imask )
    prioritizeInt();
}

void Jerry::smodeSet( uint16_t value )
{
  mSMODE.internal = ( value & SSMODE::INTERNAL ) != 0;
  mSMODE.wsen = ( value & SSMODE::WSEN ) != 0;
  mSMODE.rising = ( value & SSMODE::RISING ) != 0;
  mSMODE.falling = ( value & SSMODE::FALLING ) != 0;
  mSMODE.everyword = ( value & SSMODE::EVERYWORD ) != 0;
}

void Jerry::doInt( uint32_t mask )
{
  if ( ( mask & FLAGS::D_I2SENA ) != 0 && mFlags.i2sena )
    mCtrl.intLatches |= CTRL::D_I2SLAT;

  if ( ( mask & FLAGS::D_TIM1ENA ) != 0 && mFlags.tim1ena )
    mCtrl.intLatches |= CTRL::D_TIM1LAT;

  if ( ( mask & FLAGS::D_TIM2ENA ) != 0 && mFlags.tim2ena )
    mCtrl.intLatches |= CTRL::D_TIM2LAT;

  if ( ( mask & FLAGS::D_CPUENA ) != 0 && mFlags.cpuena )
    mCtrl.intLatches |= CTRL::D_CPULAT;

  if ( ( mask & FLAGS::D_EXT0ENA ) != 0 && mFlags.ext0ena )
    mCtrl.intLatches |= CTRL::D_EXT0LAT;

  if ( ( mask & FLAGS::D_EXT1ENA ) != 0 && mFlags.ext1ena )
    mCtrl.intLatches |= CTRL::D_EXT1LAT;

  if ( !mFlags.imask )
    prioritizeInt();
}

void Jerry::prioritizeInt()
{
  if ( mCtrl.intLatches == 0 )
    return;

  if ( mCtrl.intLatches & CTRL::D_EXT1LAT )
  {
    launchInt( 5 );
  }
  else if ( mCtrl.intLatches & CTRL::D_EXT0LAT )
  {
    mJIntCtrl.extpend = mJIntCtrl.extena;
    launchInt( 4 );
  }
  else if ( mCtrl.intLatches & CTRL::D_TIM2LAT )
  {
    mJIntCtrl.tim2pend = mJIntCtrl.tim2ena;
    launchInt( 3 );
  }
  else if ( mCtrl.intLatches & CTRL::D_TIM1LAT )
  {
    mJIntCtrl.tim1pend = mJIntCtrl.tim1ena;
    launchInt( 2 );
  }
  else if ( mCtrl.intLatches & CTRL::D_I2SLAT )
  {
    mJIntCtrl.synpend = mJIntCtrl.synena;
    launchInt( 1 );
  }
  else if ( mCtrl.intLatches & CTRL::D_CPULAT )
  {
    launchInt( 0 );
  }
  else
  {
    assert( false );
    mCtrl.intLatches = 0;
    return;
  }

  mFlags.imask = true;
}

void Jerry::launchInt( int priority )
{
  mInterruptVector = RAM_BASE + priority * 16;
  //when status isn't OPCODE it means that we are in the middle of an uninterruptible sequence
  //the transition to OPCODE will set it to INT0 when the interrupt vector is not 0
  if ( mPrefetch.status == Prefetch::OPCODE )
    mPrefetch.status = Prefetch::INT0;
}

void Jerry::cpuint()
{
  mJIntCtrl.dsppend = mJIntCtrl.dspena;
  LOG_WARN( WARN_UNIMPLEMENTED );
}

void Jerry::forceint0()
{
  doInt( FLAGS::D_CPUENA );
}

void Jerry::checkInterrupt()
{
  if ( mCycle < mInterruptor.cycleMin )
    return;

  if ( mInterruptor.cycleMin == mInterruptor.cycleI2S )
  {
    mInterruptor.cycleI2S += mInterruptor.periodI2S;
    sample();
  }
  if ( mInterruptor.cycleMin == mInterruptor.cycleTimer1 )
  {
    mInterruptor.cycleTimer1 += mInterruptor.periodTimer1;
    doInt( FLAGS::D_TIM1ENA );
  }
  if ( mInterruptor.cycleMin == mInterruptor.cycleTimer2 )
  {
    mInterruptor.cycleTimer2 += mInterruptor.periodTimer2;
    doInt( FLAGS::D_TIM2ENA );
  }

  mInterruptor.cycleMin = std::min( std::min( mInterruptor.cycleI2S, mInterruptor.cycleTimer1 ), mInterruptor.cycleTimer2 );
}

void Jerry::setI2S( uint32_t period )
{
  mInterruptor.periodI2S = period;
  mInterruptor.cycleI2S = mCycle + period - mCycle % period;

  mInterruptor.cycleMin = std::min( std::min( mInterruptor.cycleI2S, mInterruptor.cycleTimer1 ), mInterruptor.cycleTimer2 );
}

void Jerry::setTimer1( uint32_t period )
{
  mInterruptor.periodTimer1 = period;
  mInterruptor.cycleTimer1 = mCycle + period - mCycle % period;

  mInterruptor.cycleMin = std::min( std::min( mInterruptor.cycleI2S, mInterruptor.cycleTimer1 ), mInterruptor.cycleTimer2 );
}

void Jerry::setTimer2( uint32_t period )
{
  mInterruptor.periodTimer2 = period;
  mInterruptor.cycleTimer2 = mCycle + period - mCycle % period;

  mInterruptor.cycleMin = std::min( std::min( mInterruptor.cycleI2S, mInterruptor.cycleTimer1 ), mInterruptor.cycleTimer2 );
}

void Jerry::reconfigureDAC()
{
  if ( !mSMODE.internal || !mSMODE.wsen || mWavePath.empty() )
    return;

  if ( !mSMODE.rising || mSMODE.everyword || mSMODE.falling )
    LOG_WARN( WARN_UNIMPLEMENTED );

  int period  = ( 64 * ( mSCLK + 1 ) );
  int serialClockFrequency = mClock / period;

  mWaveOut = std::make_unique<WaveOut>( mWavePath, serialClockFrequency );

  setI2S( period );
}

void Jerry::reconfigureTimer1()
{
  int period = ( mJPIT1 + 1 ) * ( mJPIT2 + 1 );
  setTimer1( period );
}

void Jerry::reconfigureTimer2()
{
  int period = ( mJPIT3 + 1 ) * ( mJPIT4 + 1 );
  setTimer2( period );
}

void Jerry::sample()
{
  doInt( FLAGS::D_I2SENA );
  if ( mJoystick.audioEnable && mWaveOut )
    mWaveOut->put( std::bit_cast<int16_t*>( &mI2S ) );
}

uint16_t Jerry::FLAGS::get() const
{
  return
    ( z ? ZERO_FLAG : 0 ) |
    ( c ? CARRY_FLAG : 0 ) |
    ( n ? NEGA_FLAG : 0 ) |
    ( imask ? IMASK : 0 ) |
    ( cpuena ? D_CPUENA : 0 ) |
    ( i2sena ? D_I2SENA : 0 ) |
    ( tim1ena ? D_TIM1ENA : 0 ) |
    ( tim2ena ? D_TIM2ENA : 0 ) |
    ( ext0ena ? D_EXT0ENA : 0 ) |
    ( ext1ena ? D_EXT1ENA : 0 ) |
    ( regpage ? REGPAGE : 0 );
}

void Jerry::StageWrite::updateReg( GlobalReg reg, uint32_t value )
{
  updateMask |= UPDATE_REG;
  regFlags.reg = (int8_t)reg.idx;
  assert( regFlags.reg >= 0 );
  data = value;
}

void Jerry::StageWrite::updateRegL( GlobalReg reg, uint32_t data )
{
  updateMask |= UPDATE_REG_L;
  regL = reg;
  dataL = data;
}


bool Jerry::StageWrite::canUpdateReg() const
{
  return ( updateMask & UPDATE_REG ) == 0;
}

uint32_t Jerry::Joystick::get() const
{
  return ntsc ? 16 : 0;
}

uint16_t Jerry::Joystick::getJoy() const
{
  return 0;
}

uint16_t Jerry::Joystick::getBut() const
{
  return ntsc ? 16 : 0;
}

void Jerry::Joystick::set( uint32_t data )
{
  audioEnable = ( data & 0x1000000 ) != 0;
}

void Jerry::Joystick::setJoy( uint16_t data )
{
  audioEnable = ( data & 0x100 ) != 0;
}
