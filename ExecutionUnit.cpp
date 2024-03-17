#include "ExecutionUnit.hpp"
#include "Ex.hpp"


ExecutionUnit ExecutionUnit::create()
{
  for ( ;; )
  {
    RISCOpcode opcode = co_await Decode{};
    switch ( opcode.opcode ) {
    case RISCOpcode::ABS:
    {
      auto [dst, reg] = co_await LockReadDstLockFlags{ .dst = opcode.dst };
      co_await Compute{};
      if ( dst == 0x80000000 )
      {
        co_await UnlockWriteDstUnlockWriteFlags{
          .valuenz = 0x80000000,
          .c = 1,
          .reg = reg
        };
      }
      else
      {
        auto result = ( int32_t )dst < 0 ? ( ~dst ) + 1 : dst;

        co_await UnlockWriteDstUnlockWriteFlags{
          .valuenz = result,
          .c = ( uint16_t )( ( result >> 31 ) & 1 ? 1 : 0 ),
          .reg = reg
        };
      }
      break;
    }
    case RISCOpcode::ADD:
    {
      auto [src, dst, reg] = co_await ReadSrcLockReadDstLockFlags{ .src = opcode.src, .dst = opcode.dst };
      auto result = src + dst;
      co_await Compute{};
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( result < src ? 1 : 0 ),
        .reg = reg
      };
      break;
    }
    case RISCOpcode::ADDC:
    {
      auto [src, dst, reg, c] = co_await ReadSrcLockReadDstLockReadFlags{ .src = opcode.src, .dst = opcode.dst };
      auto result = src + dst + c;
      co_await Compute{};
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( result < src ? 1 : 0 ),
        .reg = reg
      };
      break;
    }
    case RISCOpcode::ADDQ:
    {
      auto src = opcode.src == 0 ? 32 : opcode.src;
      auto [dst, reg] = co_await LockReadDstLockFlags{ .dst = opcode.dst };
      auto result = src + dst;
      co_await UnlockWriteDstUnlockWriteFlags{
        .valuenz = result,
        .c = ( uint16_t )( result < src ? 1 : 0 ),
        .reg = reg
      };
      break;
    }
    case RISCOpcode::ADDQMOD:
      throw Ex{} << "NYI";
    case RISCOpcode::ADDQT:
    {
      auto src = opcode.src == 0 ? 32 : opcode.src;
      auto [dst, reg] = co_await LockReadDst{ .dst = opcode.dst };
      auto result = src + dst;
      co_await Compute{};
      co_await UnlockWriteDst{
        .value = result,
        .reg = reg
      };
      break;
    }
    case RISCOpcode::AND:
      throw Ex{} << "NYI";
    case RISCOpcode::BCLR:
      throw Ex{} << "NYI";
    case RISCOpcode::BSET:
      throw Ex{} << "NYI";
    case RISCOpcode::BTST:
    {
      auto [dst, reg] = co_await ReadDstLockFlags{ .dst = opcode.dst };
      co_await Compute{};
      co_await UnlockWriteFlags{
        //BTST clears N bit, no matter if bit 31 is tested value is set or not.
        .nz = dst & ( 1 << opcode.src ),
        .c = reg
      };
      break;
    }
    case RISCOpcode::CMP:
      throw Ex{} << "NYI";
    case RISCOpcode::CMPQ:
      throw Ex{} << "NYI";
    case RISCOpcode::DIV:
      throw Ex{} << "NYI";
    case RISCOpcode::IMACN:
      throw Ex{} << "NYI";
    case RISCOpcode::IMULT:
      throw Ex{} << "NYI";
    case RISCOpcode::IMULTN:
      throw Ex{} << "NYI";
    case RISCOpcode::JR:
      throw Ex{} << "NYI";
    case RISCOpcode::JUMP:
      throw Ex{} << "NYI";
    case RISCOpcode::LOAD:
    {
      auto [src, reg] = co_await ReadSrcGetDst{ .src = opcode.src, .dst = opcode.dst };
      auto [value] = co_await MemoryLoadLong{ .src = src };
      co_await WriteDst{
        .value = value,
        .reg = reg
      };
      break;
    }
    case RISCOpcode::LOAD14N:
    {
      auto [base, reg] = co_await ReadSrcGetDst{ .src = 14, .dst = opcode.dst };
      auto addr = base + ( opcode.src == 0 ? 32 : opcode.src ) * 4;
      co_await Compute{};
      auto [value] = co_await MemoryLoadLong{ .src = addr };
      co_await WriteDst{
        .value = value,
        .reg = reg
      };
      break;
    }
    case RISCOpcode::LOAD14R:
    {
      auto [base, off, reg] = co_await ReadSrcsGetDst{ .src1 = 14, .src2 = opcode.src };
      auto addr = base + off;
      co_await Compute{};
      auto [value] = co_await MemoryLoadLong{ .src = addr };
      co_await WriteDst{
        .value = value,
        .reg = reg
      };
      break;
    }
    case RISCOpcode::LOAD15N:
    {
      auto [base, reg] = co_await ReadSrcGetDst{ .src = 15, .dst = opcode.dst };
      auto addr = base + ( opcode.src == 0 ? 32 : opcode.src ) * 4;
      co_await Compute{};
      auto [value] = co_await MemoryLoadLong{ .src = addr };
      co_await WriteDst{
        .value = value,
        .reg = reg
      };
      break;
    }
    case RISCOpcode::LOAD15R:
    {
      auto [base, off, reg] = co_await ReadSrcsGetDst{ .src1 = 15, .src2 = opcode.src };
      auto addr = base + off;
      co_await Compute{};
      auto [value] = co_await MemoryLoadLong{ .src = addr };
      co_await WriteDst{
        .value = value,
        .reg = reg
      };
      break;
    }
    case RISCOpcode::LOADB:
      throw Ex{} << "NYI";
    case RISCOpcode::LOADW:
      throw Ex{} << "NYI";
    case RISCOpcode::MIRROR:
      throw Ex{} << "NYI";
    case RISCOpcode::MMULT:
      throw Ex{} << "NYI";
    case RISCOpcode::MOVE:
    {
      auto [src, reg] = co_await ReadSrcGetDst{ .src = opcode.src, .dst = opcode.dst };
      co_await WriteDst{
        .value = src,
        .reg = reg
      };
      break;
    }
    case RISCOpcode::MOVEFA:
    {
      auto [src, reg] = co_await ReadAlternateSrcGetDst{ .src = opcode.src, .dst = opcode.dst };
      co_await WriteDst{
        .value = src,
        .reg = reg
      };
      break;
    }
    case RISCOpcode::MOVEI:
      throw Ex{} << "NYI";
    case RISCOpcode::MOVEPC:
      throw Ex{} << "NYI";
    case RISCOpcode::MOVEQ:
    {
      auto [reg] = co_await GetDst{ .dst = opcode.dst };
      co_await WriteDst{
        .value = opcode.src,
        .reg = reg
      };
      break;
    }
    case RISCOpcode::MOVETA:
    {
      auto [src,reg] = co_await ReadSrcGetAlternateDst{ .src = opcode.src, .dst = opcode.dst };
      co_await WriteAlternateDst{
        .value = src,
        .reg = reg
      };
      break;
    }
    case RISCOpcode::MTOI:
      throw Ex{} << "NYI";
    case RISCOpcode::MULT:
      throw Ex{} << "NYI";
    case RISCOpcode::NEG:
      throw Ex{} << "NYI";
    case RISCOpcode::NOP:
    {
      co_await NOP{};
      break;
    }
    case RISCOpcode::NORMI:
      throw Ex{} << "NYI";
    case RISCOpcode::NOT:
      throw Ex{} << "NYI";
    case RISCOpcode::OR:
      throw Ex{} << "NYI";
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
      auto [src, dst] = co_await ReadSrcReadDst{ .src = opcode.src, .dst = opcode.dst };
      co_await NOP{};
      co_await MemoryStoreLong{ .addr = dst, .value = src };
      break;
    }
    case RISCOpcode::STORE14N:
      throw Ex{} << "NYI";
    case RISCOpcode::STORE14R:
      throw Ex{} << "NYI";
    case RISCOpcode::STORE15N:
      throw Ex{} << "NYI";
    case RISCOpcode::STORE15R:
      throw Ex{} << "NYI";
    case RISCOpcode::STOREB:
      throw Ex{} << "NYI";
    case RISCOpcode::STOREW:
      throw Ex{} << "NYI";
    case RISCOpcode::SUB:
      throw Ex{} << "NYI";
    case RISCOpcode::SUBC:
      throw Ex{} << "NYI";
    case RISCOpcode::SUBQ:
      throw Ex{} << "NYI";
    case RISCOpcode::SUBQMOD:
      throw Ex{} << "NYI";
    case RISCOpcode::SUBQT:
      throw Ex{} << "NYI";
    case RISCOpcode::XOR:
      throw Ex{} << "NYI";
    default: throw Ex{} << "NYI";
    }
  }
  co_return;
}
