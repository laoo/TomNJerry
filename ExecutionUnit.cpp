#include "ExecutionUnit.hpp"
#include "Ex.hpp"


ExecutionUnit ExecutionUnit::create()
{
  for ( ;; )
  {
    auto [opcode] = co_await Decode{};
    switch ( opcode.opcode ) {
    case RISCOpcode::ABS:
    {
      auto [dst] = co_await ReadLockDstLockFlags{ .dst = opcode.dstReg };
      co_await Compute{};
      if ( dst == 0x80000000 )
      {
        co_await UnlockWriteDstUnlockWriteFlags{
          .valuenz = 0x80000000,
          .c = 1,
          .reg = opcode.dstReg
        };
      }
      else
      {
        uint32_t result = ( int32_t )dst < 0 ? ( ~dst ) + 1 : dst;

        co_await UnlockWriteDstUnlockWriteFlags{
          .valuenz = result,
          .c = ( uint16_t )( ( result >> 31 ) & 1 ? 1 : 0 ),
          .reg = opcode.dstReg
        };
      }
      break;
    }
    case RISCOpcode::ADD:
    {
      auto [src, dst] = co_await ReadSrcReadLockDstLockFlags{ .src = opcode.srcReg, .dst = opcode.dstReg };
      uint32_t result = src + dst;
      co_await Compute{};
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( result < src ? 1 : 0 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::ADDC:
    {
      auto [src, dst, c] = co_await ReadSrcReadLockDstReadLockFlags{ .src = opcode.srcReg, .dst = opcode.dstReg };
      uint32_t result = src + dst + c;
      co_await Compute{};
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( result < src ? 1 : 0 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::ADDQ:
    {
      uint32_t src = opcode.srcValue == 0 ? 32 : opcode.srcValue;
      auto [dst] = co_await ReadLockDstLockFlags{ .dst = opcode.dstReg };
      uint32_t result = src + dst;
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( result < src ? 1 : 0 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::ADDQMOD:
    {
      uint32_t src = opcode.srcValue == 0 ? 32 : opcode.srcValue;
      auto [dst] = co_await ReadLockDstLockFlags{ .dst = opcode.dstReg };
      auto [mod] = co_await GetMod{};
      uint32_t result = ( dst & mod ) | ( ( src + dst ) & ( ~mod ) );
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( result < src ? 1 : 0 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::ADDQT:
    {
      uint32_t src = opcode.srcValue == 0 ? 32 : opcode.srcValue;
      auto [dst] = co_await ReadLockDst{ .dst = opcode.dstReg };
      uint32_t result = src + dst;
      co_await Compute{};
      co_await UnlockWriteDst{
        .value = result,
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::AND:
    {
      auto [src, dst] = co_await ReadSrcReadLockDstLockFlags{ .src = opcode.srcReg, .dst = opcode.dstReg };
      uint32_t result = src & dst;
      co_await Compute{};
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        //"and" copies bit 31 of the result into C flag
        .c = ( uint16_t )( result >> 31 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::BCLR:
    {
      auto [dst] = co_await ReadLockDstLockFlags{ .dst = opcode.dstReg };
      uint32_t result = dst & ~( 1 << opcode.srcValue );
      co_await Compute{};
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( result >> 31 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::BSET:
    {
      auto [dst] = co_await ReadLockDstLockFlags{ .dst = opcode.dstReg };
      uint32_t result = dst | ( 1 << opcode.srcValue );
      co_await Compute{};
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = 0,
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::BTST:
    {
      auto [dst] = co_await ReadDstLockFlags{ .dst = opcode.dstReg };
      uint32_t result = dst & ( 1 << opcode.srcValue );
      co_await Compute{};
      co_await UnlockWriteFlags{
        //BTST clears N bit, no matter if bit 31 is tested value is set or not.
        .nz = result ? 1u : 0u,
        .c = 0
      };
      break;
    }
    case RISCOpcode::CMP:
    {
      auto [src, dst] = co_await ReadSrcReadLockDstLockFlags{ .src = opcode.srcReg, .dst = opcode.dstReg };
      uint32_t result = src + dst;
      co_await Compute{};
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( result < src ? 1 : 0 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::CMPQ:
    {
      int8_t i0 = ( int8_t )( opcode.srcValue << 3 );
      int64_t i1 = ( int64_t )i0;
      int64_t i2 = i1 >> 3;
      uint32_t src = ( uint32_t )i2;
      auto [dst] = co_await ReadLockDstLockFlags{ .dst = opcode.dstReg };
      uint32_t result = dst - src;
      co_await Compute{};
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( result < src ? 1 : 0 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::DIV:
    {
      auto [divisor, dst] = co_await ReadSrcReadLockDst{ .src = opcode.srcReg, .dst = opcode.dstReg };

      int32_t cycle = 16;
      uint32_t q = dst;
      uint32_t r = 0;
      if ( auto [offset] = co_await DivideInit{}; offset )
      {
        q <<= 16;
        r = dst >> 16;
      }

      do
      {
        uint32_t sign = r & 0x80000000;
        r = ( r << 1 ) | ( ( q >> 31 ) & 0x01 );
        r += ( sign ? divisor : -divisor );
        q = ( q << 1 ) | ( ( ( ~r ) >> 31 ) & 0x01 );

        sign = r & 0x80000000;
        r = ( r << 1 ) | ( ( q >> 31 ) & 0x01 );
        r += ( sign ? divisor : -divisor );
        q = ( q << 1 ) | ( ( ( ~r ) >> 31 ) & 0x01 );

        cycle -= 1;
        co_await DivideCycle{ .cycle = cycle };
      } while ( cycle );

      co_await UnlockWriteDstRemain{
        .value = q,
        .remain = r,
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::IMACN:
      throw Ex{} << "NYI";
    case RISCOpcode::IMULT:
    {
      auto [src, dst] = co_await ReadSrcReadLockDstLockFlags{ .src = opcode.srcReg, .dst = opcode.dstReg };
      int32_t result = ( int16_t )src * ( int16_t )dst;
      co_await Compute{};
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = (uint32_t)result,
        .c = ( uint16_t )( result < 0 ? 1 : 0 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::IMULTN:
      throw Ex{} << "NYI";
    case RISCOpcode::JR:
      throw Ex{} << "NYI";
    case RISCOpcode::JUMP:
      throw Ex{} << "NYI";
    case RISCOpcode::LOAD:
    {
      auto [src] = co_await ReadSrc{ .src = opcode.srcReg };
      auto [value, success] = co_await MemoryLoadLong{ .src = src };
      if ( success )
      {
        co_await WriteDst{
          .value = value,
          .reg = opcode.dstReg
        };
      }
      else
      {
        auto [value] = co_await MemoryLoadLongExternal{ .src = src };
        co_await WriteDst{
          .value = value,
          .reg = opcode.dstReg
        };
      }
      break;
    }
    case RISCOpcode::LOAD14N:
    {
      auto [base] = co_await ReadSrc{ .src = 14 };
      uint32_t addr = base + ( opcode.srcValue == 0 ? 32 : opcode.srcValue ) * 4;
      co_await IO{};
      auto [value,success] = co_await MemoryLoadLong{ .src = addr };
      if ( success )
      {
        co_await WriteDst{
          .value = value,
          .reg = opcode.dstReg
        };
      }
      else
      {
        auto [value] = co_await MemoryLoadLongExternal{ .src = addr };
        co_await WriteDst{
          .value = value,
          .reg = opcode.dstReg
        };
      }
      break;
    }
    case RISCOpcode::LOAD14R:
    {
      auto [base, off] = co_await ReadSrcReadDst{ .src = 14, .dst = opcode.srcReg };
      uint32_t addr = base + off;
      co_await IO{};
      auto [value, success] = co_await MemoryLoadLong{ .src = addr };
      if ( success )
      {
        co_await WriteDst{
          .value = value,
          .reg = opcode.dstReg
        };
      }
      else
      {
        auto [value] = co_await MemoryLoadLongExternal{ .src = addr };
        co_await WriteDst{
          .value = value,
          .reg = opcode.dstReg
        };
      }
      break;
    }
    case RISCOpcode::LOAD15N:
    {
      auto [base] = co_await ReadSrc{ .src = 15 };
      uint32_t addr = base + ( opcode.srcValue == 0 ? 32 : opcode.srcValue ) * 4;
      co_await IO{};
      auto [value, success] = co_await MemoryLoadLong{ .src = addr };
      if ( success )
      {
        co_await WriteDst{
          .value = value,
          .reg = opcode.dstReg
        };
      }
      else
      {
        auto [value] = co_await MemoryLoadLongExternal{ .src = addr };
        co_await WriteDst{
          .value = value,
          .reg = opcode.dstReg
        };
      }
      break;
    }
    case RISCOpcode::LOAD15R:
    {
      auto [base, off] = co_await ReadSrcReadDst{ .src = 15, .dst = opcode.srcReg };
      uint32_t addr = base + off;
      co_await IO{};
      auto [value, success] = co_await MemoryLoadLong{ .src = addr };
      if ( success )
      {
        co_await WriteDst{
          .value = value,
          .reg = opcode.dstReg
        };
      }
      else
      {
        auto [value] = co_await MemoryLoadLongExternal{ .src = addr };
        co_await WriteDst{
          .value = value,
          .reg = opcode.dstReg
        };
      }
      break;
    }
    case RISCOpcode::LOADB:
    {
      auto [src] = co_await ReadSrc{ .src = opcode.srcReg };
      auto [value, success] = co_await MemoryLoadByte{ .src = src };
      if ( success )
      {
        co_await WriteDst{
          .value = value,
          .reg = opcode.dstReg
        };
      }
      else
      {
        auto [value] = co_await MemoryLoadByteExternal{ .src = src };
        co_await WriteDst{
          .value = value,
          .reg = opcode.dstReg
        };
      }
      break;
    }
    case RISCOpcode::LOADW:
    {
      auto [src] = co_await ReadSrc{ .src = opcode.srcReg };
      auto [value, success] = co_await MemoryLoadWord{ .src = src };
      if ( success )
      {
        co_await WriteDst{
          .value = value,
          .reg = opcode.dstReg
        };
      }
      else
      {
        auto [value] = co_await MemoryLoadWordExternal{ .src = src };
        co_await WriteDst{
          .value = value,
          .reg = opcode.dstReg
        };
      }
      break;
    }
    case RISCOpcode::MIRROR:
    {
      auto [dst] = co_await ReadLockDstLockFlags{ .dst = opcode.dstReg };

      uint8_t b0 = dst & 0xff;
      uint8_t b1 = ( dst >> 8 ) & 0xff;
      uint8_t b2 = ( dst >> 16 ) & 0xff;
      uint8_t b3 = ( dst >> 24 ) & 0xff;

      //https://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith64Bits
      uint32_t m0 = ( ( b0 * 0x80200802ULL ) & 0x0884422110ULL ) * 0x0101010101ULL >> 32;
      uint32_t m1 = ( ( b1 * 0x80200802ULL ) & 0x0884422110ULL ) * 0x0101010101ULL >> 32;
      uint32_t m2 = ( ( b2 * 0x80200802ULL ) & 0x0884422110ULL ) * 0x0101010101ULL >> 32;
      uint32_t m3 = ( ( b3 * 0x80200802ULL ) & 0x0884422110ULL ) * 0x0101010101ULL >> 32;

      co_await Compute{};

      uint32_t result = ( m0 << 24 ) | ( m1 << 16 ) | ( m2 << 8 ) | m3;

      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        //TODO: check if C flag is set correctly
        .c = ( uint16_t )( ( result >> 31 ) & 1 ? 1 : 0 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::MMULT:
      throw Ex{} << "NYI";
    case RISCOpcode::MOVE:
    {
      auto [src] = co_await ReadSrc{ .src = opcode.srcReg };
      co_await WriteDst{
        .value = src,
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::MOVEFA:
    {
      auto [src] = co_await ReadSrc{ .src = (int8_t)( opcode.srcReg ^ 31 ) };
      co_await WriteDst{
        .value = src,
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::MOVEI:
    {
      auto [low] = co_await GetCodeLo{};
      auto [high] = co_await GetCodeHi{};
      co_await WriteDst{
        .value = ( uint32_t )low | ( ( uint32_t )high << 16 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::MOVEPC:
    {
      auto [pc] = co_await ReadPC{};
      co_await WriteDst{
        .value = pc,
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::MOVEQ:
    {
      co_await MoveQ{};
      co_await WriteDst{
        .value = opcode.srcValue,
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::MOVETA:
    {
      auto [src] = co_await ReadSrc{ .src = opcode.srcReg };
      co_await WriteDst{
        .value = src,
        .reg = (int8_t)( opcode.dstReg ^ 32 )
      };
      break;
    }
    case RISCOpcode::MTOI:
      throw Ex{} << "NYI";
    case RISCOpcode::MULT:
    {
      auto [src, dst] = co_await ReadSrcReadLockDstLockFlags{ .src = opcode.srcReg, .dst = opcode.dstReg };
      uint32_t result = (uint16_t)src * ( uint16_t )dst;
      co_await Compute{};
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( result ? 1 : 0 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::NEG:
    {
      auto [dst] = co_await ReadLockDstLockFlags{ .dst = opcode.dstReg };
      co_await Compute{};
      uint32_t result = -dst;
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( dst ? 1 : 0 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::NOP:
    {
      co_await NOP{};
      break;
    }
    case RISCOpcode::NORMI:
    {
      auto [src] = co_await ReadSrc{ .src = ( int8_t )( opcode.srcReg ^ 31 ) };

      uint32_t result = 0;

      if ( src )
      {
        while ( ( src & 0xffc00000 ) == 0 )
        {
          src <<= 1;
          result--;
        }
        while ( ( src & 0xff800000 ) != 0 )
        {
          src >>= 1;
          result++;
        }
      }
      co_await Compute{};
      co_await WriteDstWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( src < 0x10 ? 1 : 0 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::NOT:
    {
      auto [dst] = co_await ReadLockDstLockFlags{ .dst = opcode.dstReg };
      co_await Compute{};
      uint32_t result = ~dst;
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( result ? 1 : 0 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::OR:
    {
      auto [src, dst] = co_await ReadSrcReadLockDstLockFlags{ .src = opcode.srcReg, .dst = opcode.dstReg };
      uint32_t result = src | dst;
      co_await Compute{};
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( ( src & dst ) >> 31 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::RESMAC:
      throw Ex{} << "NYI";
    case RISCOpcode::ROR:
      throw Ex{} << "NYI";
    case RISCOpcode::RORQ:
      throw Ex{} << "NYI";
    case RISCOpcode::SAT16S:
      throw Ex{} << "NYI";
    case RISCOpcode::SAT32S:
      throw Ex{} << "NYI";
    case RISCOpcode::SH:
      throw Ex{} << "NYI";
    case RISCOpcode::SHA:
      throw Ex{} << "NYI";
    case RISCOpcode::SHARQ:
      throw Ex{} << "NYI";
    case RISCOpcode::SHLQ:
      throw Ex{} << "NYI";
    case RISCOpcode::SHRQ:
      throw Ex{} << "NYI";
    case RISCOpcode::STORE:
    {
      auto [src, dst] = co_await ReadSrcReadDst{ .src = opcode.srcReg, .dst = opcode.dstReg };
      co_await IO{};
      if ( auto [success] = co_await MemoryStoreLong{ .addr = dst, .value = src }; !success )
      {
        co_await MemoryStoreLongExternal{ .addr = dst, .value = src };
      }
      break;
    }
    case RISCOpcode::STORE14N:
    {
      auto [base] = co_await ReadSrc{ .src = 14 };
      auto [dst] = co_await ReadDstNoScoreboard{ .dst = opcode.srcReg };
      uint32_t addr = base + ( opcode.srcValue == 0 ? 32 : opcode.srcValue ) * 4;
      co_await IO{};
      if ( auto [success] = co_await MemoryStoreLong{ .addr = addr, .value = dst }; !success )
      {
        co_await MemoryStoreLongExternal{ .addr = addr, .value = dst };
      }
      break;
    }
    case RISCOpcode::STORE14R:
    {
      auto [base, off] = co_await ReadSrcReadDst{ .src = 14, .dst = opcode.srcReg };
      auto [dst] = co_await ReadDstNoScoreboard{ .dst = opcode.srcReg };
      uint32_t addr = base + off;
      co_await IO{};
      if ( auto [success] = co_await MemoryStoreLong{ .addr = addr, .value = dst }; !success )
      {
        co_await MemoryStoreLongExternal{ .addr = addr, .value = dst };
      }
      break;
    }
    case RISCOpcode::STORE15N:
    {
      auto [base] = co_await ReadSrc{ .src = 15 };
      auto [dst] = co_await ReadDstNoScoreboard{ .dst = opcode.srcReg };
      uint32_t addr = base + ( opcode.srcValue == 0 ? 32 : opcode.srcValue ) * 4;
      co_await IO{};
      if ( auto [success] = co_await MemoryStoreLong{ .addr = addr, .value = dst }; !success )
      {
        co_await MemoryStoreLongExternal{ .addr = addr, .value = dst };
      }
      break;
    }
    case RISCOpcode::STORE15R:
    {
      auto [base, off] = co_await ReadSrcReadDst{ .src = 15, .dst = opcode.srcReg };
      auto [dst] = co_await ReadDstNoScoreboard{ .dst = opcode.srcReg };
      uint32_t addr = base + off;
      co_await IO{};
      if ( auto [success] = co_await MemoryStoreLong{ .addr = addr, .value = dst }; !success )
      {
        co_await MemoryStoreLongExternal{ .addr = addr, .value = dst };
      }
      break;
    }
    case RISCOpcode::STOREB:
    {
      auto [src, dst] = co_await ReadSrcReadDst{ .src = opcode.srcReg, .dst = opcode.dstReg };
      co_await IO{};
      if ( auto [success] = co_await MemoryStoreByte{ .addr = dst, .value = ( uint8_t )src }; !success )
      {
        co_await MemoryStoreByteExternal{ .addr = dst, .value = ( uint8_t )src };
      }
      break;
    }
    case RISCOpcode::STOREW:
    {
      auto [src, dst] = co_await ReadSrcReadDst{ .src = opcode.srcReg, .dst = opcode.dstReg };
      co_await IO{};
      if ( auto [success] = co_await MemoryStoreWord{ .addr = dst, .value = ( uint16_t )src }; !success )
      {
        co_await MemoryStoreWordExternal{ .addr = dst, .value = ( uint16_t )src };
      }
      break;
    }
    case RISCOpcode::SUB:
    {
      auto [src, dst] = co_await ReadSrcReadLockDstLockFlags{ .src = opcode.srcReg, .dst = opcode.dstReg };
      uint32_t result = dst - src;
      co_await Compute{};
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( src < dst ? 1 : 0 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::SUBC:
    {
      auto [src, dst, c] = co_await ReadSrcReadLockDstReadLockFlags{ .src = opcode.srcReg, .dst = opcode.dstReg };
      uint64_t result = ( uint64_t )dst + ( uint64_t )( src ^ 0xffffffff ) + ( uint64_t )c;
      co_await Compute{};
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = ( uint32_t )result,
        .c = ( uint16_t )( ( ( result >> 32 ) & 1 ) ^ 1 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::SUBQ:
    {
      uint32_t src = opcode.srcValue == 0 ? 32 : opcode.srcValue;
      auto [dst] = co_await ReadLockDstLockFlags{ .dst = opcode.dstReg };
      uint32_t result = dst - src;
      co_await Compute{};
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( src < dst ? 1 : 0 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::SUBQMOD:
    {
      uint32_t src = opcode.srcValue == 0 ? 32 : opcode.srcValue;
      auto [dst] = co_await ReadLockDstLockFlags{ .dst = opcode.dstReg };
      auto [mod] = co_await GetMod{};
      uint32_t result = ( dst & mod ) | ( ( dst - src ) & ( ~mod ) );
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( src < dst ? 1 : 0 ),
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::SUBQT:
    {
      uint32_t src = opcode.srcValue == 0 ? 32 : opcode.srcValue;
      auto [dst] = co_await ReadLockDst{ .dst = opcode.dstReg };
      uint32_t result = dst - src;
      co_await Compute{};
      co_await UnlockWriteDst{
        .value = result,
        .reg = opcode.dstReg
      };
      break;
    }
    case RISCOpcode::XOR:
    {
      auto [src, dst] = co_await ReadSrcReadLockDstLockFlags{ .src = opcode.srcReg, .dst = opcode.dstReg };
      uint32_t result = src ^ dst;
      co_await Compute{};
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( ( src & dst ) >> 31 ),
        .reg = opcode.dstReg
      };
      break;
    }
    default:
      throw Ex{} << "NYI";
    }
  }
  co_return;
}
