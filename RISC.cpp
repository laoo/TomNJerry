#include "RISC.hpp"
#include "Ex.hpp"

struct RISCOpcode
{
  enum Opcode : uint16_t
  {
    ABS,
    ADD,
    ADDC,
    ADDQ,
    ADDQMOD,
    ADDQT,
    AND,
    BCLR,
    BSET,
    BTST,
    CMP,
    CMPQ,
    DIV,
    IMACN,
    IMULT,
    IMULTN,
    JR,
    JUMP,
    LOAD,
    LOAD14N,
    LOAD14R,
    LOAD15N,
    LOAD15R,
    LOADB,
    LOADW,
    MIRROR,
    MMULT,
    MOVE,
    MOVEFA,
    MOVEI,
    MOVEPC,
    MOVEQ,
    MOVETA,
    MTOI,
    MULT,
    NEG,
    NOP,
    NORMI,
    NOT,
    OR,
    RESMAC,
    ROR,
    RORQ,
    SAT16S,
    SAT32S,
    SH,
    SHA,
    SHARQ,
    SHLQ,
    SHRQ,
    STORE,
    STORE14N,
    STORE14R,
    STORE15N,
    STORE15R,
    STOREB,
    STOREW,
    SUB,
    SUBC,
    SUBQ,
    SUBQMOD,
    SUBQT,
    XOR
  };

  Opcode opcode;
  uint8_t src;
  uint8_t dst;
};

class Coroutine
{
  struct promise_type;

  std::coroutine_handle<promise_type> mCoro;

  Coroutine( std::coroutine_handle<promise_type> c ) : mCoro{ c }
  {
  }

  struct Decode
  {
  };

  struct ReadAlternateSrcGetDst
  {
    uint8_t src;
    uint8_t dst;
  };

  struct ReadSrcGetDst
  {
    uint8_t src;
    uint8_t dst;
  };

  struct ReadSrcGetAlternateDst
  {
    uint8_t src;
    uint8_t dst;
  };

  struct ReadSrcLockReadDstLockFlags
  {
    uint8_t src;
    uint8_t dst;
  };

  struct ReadSrcsGetDst
  {
    uint8_t src1;
    uint8_t src2;
  };

  struct ReadSrcLockReadDstLockReadFlags
  { 
    uint8_t src;
    uint8_t dst;
  };

  struct ReadSrcReadDst
  {
    uint8_t src;
    uint8_t dst;
  };

  struct LockReadDstLockFlags
  {
    uint8_t dst;
  };

  struct ReadDstLockFlags
  {
    uint8_t dst;
  };

  struct LockReadDst
  {
    uint8_t dst;
  };

  struct GetDst
  { 
    uint8_t dst;
  };

  struct Compute
  {
  };

  struct NOP
  {
  };

  struct MemoryLoadLong
  {
    uint32_t src;
  };

  struct MemoryStoreLong
  {
    uint32_t addr;
    uint32_t value;
  };

  struct UnlockWriteDstUnlockWriteFlags
  {
    uint32_t valuenz;
    uint16_t c;
    uint16_t reg;
  };

  struct UnlockWriteDst
  {
    uint32_t value;
    uint16_t reg;
  };

  struct UnlockWriteFlags
  {
    uint32_t nz;
    uint16_t c;
  };

  struct WriteDst
  {
    uint32_t value;
    uint16_t reg;
  };

  struct WriteAlternateDst
  {
    uint32_t value;
    uint16_t reg;
  };

public:
  struct promise_type
  {
    promise_type()
    {
    }
    auto get_return_object() { return Coroutine{ std::coroutine_handle<promise_type>::from_promise( *this ) }; }
    auto initial_suspend() noexcept { return std::suspend_never{}; }
    auto final_suspend() noexcept { return std::suspend_always{}; }
    void return_void() {}
    void unhandled_exception() { std::terminate(); }

    struct BaseAwaiter
    {
      bool await_ready() { return false; }
      void await_suspend( std::coroutine_handle<> c ) {}
      void await_resume() {}
    };

    struct Awaiter
    {
      enum struct Type
      {
         DECODE,
         READ_SRC_GET_DST,
         READ_ALTERNATE_SRC_GET_DST,
         READ_SRC_GET_ALTERNATE_DST,
         READ_SRC_LOCK_READ_DST_LOCK_FLAGS,
         READ_SRC_LOCK_READ_DST_LOCK_READ_FLAGS,
         READ_SRC_READ_DST,
         READ_DST_LOCK_FLAGS,
         READ_SRCS_GET_DST,
         LOCK_READ_DST_LOCK_FLAGS,
         LOCK_READ_DST,
         GET_DST,
         COMPUTE,
         NOP,
         MEMORY_LOAD_LONG,
         MEMORY_STORE_LONG,
         UNLOCK_WRITE_DST_UNLOCK_WRITE_FLAGS,
         UNLOCK_WRITE_DST,
         UNLOCK_WRITE_FLAGS,
         WRITE_DST,
         WRITE_ALTERNATE_DST
      } type;

      union U
      {
        struct DecodeAwaiter : BaseAwaiter
        {
          RISCOpcode opcode;
          RISCOpcode await_resume() { return opcode; }
        } decodeAwaiter;

        struct ReadSrcGetDstAwaiter : BaseAwaiter
        {
          ReadSrcGetDst arg;
          struct Res
          {
            uint32_t src;
            uint16_t dstReg;
          } res;
          Res await_resume() { return res; }
        } readSrcGetDstAwaiter;

        struct ReadAlternateSrcGetDstAwaiter : BaseAwaiter
        {
          ReadAlternateSrcGetDst arg;
          struct Res
          {
            uint32_t src;
            uint16_t dstReg;
          } res;
          Res await_resume() { return res; }
        } readAlternateSrcGetDstAwaiter;

        struct ReadSrcGetAlternateDstAwaiter : BaseAwaiter
        {
          ReadSrcGetAlternateDst arg;
          struct Res
          {
            uint32_t src;
            uint16_t dstReg;
          } res;
          Res await_resume() { return res; }
        } readSrcGetAlternateDstAwaiter;

        struct ReadSrcLockReadDstLockFlagsAwaiter : BaseAwaiter
        {
          ReadSrcLockReadDstLockFlags arg;
          struct Res
          {
            uint32_t src;
            uint32_t dst;
            uint16_t dstReg;
          } res;
          Res await_resume() { return res; }
        } readSrcLockDstLockFlagsAwaiter;

        struct ReadSrcLockReadDstLockReadFlagsAwaiter : BaseAwaiter
        {
          ReadSrcLockReadDstLockReadFlags arg;
          struct Res
          {
            uint32_t src;
            uint32_t dst;
            uint16_t dstReg;
            uint16_t c;
          } res;
          Res await_resume() { return res; }
        } readSrcLockDstLockReadFlagsAwaiter;

        struct ReadSrcReadDstAwaiter : BaseAwaiter
        {
          ReadSrcReadDst arg;
          struct Res
          {
            uint32_t src;
            uint32_t dst;
          } res;
          Res await_resume() { return res; }
        } readSrcReadDstAwaiter;

        struct ReadDstLockFlagsAwaiter : BaseAwaiter
        {
          ReadDstLockFlags arg;
          struct Res
          {
            uint32_t dst;
            uint16_t dstReg;
          } res;
          Res await_resume() { return res; }
        } readDstLockFlagsAwaiter;

        struct ReadSrcsGetDstAwaiter : BaseAwaiter
        {
          ReadSrcsGetDst arg;
          struct Res
          {
            uint32_t src1;
            uint32_t src2;
            uint16_t dstReg;
          } res;
          Res await_resume() { return res; }
        } readSrcsGetDstAwaiter;

        struct LockReadDstLockFlagsAwaiter : BaseAwaiter
        {
          LockReadDstLockFlags arg;
          struct Res
          {
            uint32_t dst;
            uint16_t dstReg;
          } res;
          Res await_resume() { return res; }
        } lockReadDstLockFlagsAwaiter;

        struct LockReadDstAwaiter : BaseAwaiter
        {
          LockReadDst arg;
          struct Res
          {
            uint32_t dst;
            uint16_t dstReg;
          } res;
          Res await_resume() { return res; }
        } lockReadDstAwaiter;

        struct GetDstAwaiter : BaseAwaiter
        {
          GetDst arg;
          struct Res
          {
            uint16_t dstReg;
          } res;
          Res await_resume() { return res; }
        } getDstAwaiter;

        struct ComputeAwaiter : BaseAwaiter
        {
        } computeAwaiter;

        struct NOPAwaiter : BaseAwaiter
        {
        } nopAwaiter;

        struct MemoryLoadLongAwaiter : BaseAwaiter
        {
          MemoryLoadLong arg;
          struct Res
          {
            uint32_t value;
          } res;
          Res await_resume() { return res; }
        } memoryLoadLongAwaiter;

        struct MemoryStoreLongAwaiter : BaseAwaiter
        {
          MemoryStoreLong arg;
        } memoryStoreLongAwaiter;

        struct UnlockWriteDstUnlockWriteFlagsAwaiter : BaseAwaiter
        {
          UnlockWriteDstUnlockWriteFlags res;
        } unlockWriteDstUnlockWriteFlagsAwaiter;

        struct UnlockWriteFlagsAwaiter : BaseAwaiter
        {
          UnlockWriteFlags res;
        } unlockWriteFlagsAwaiter;

        struct UnlockWriteDstAwaiter : BaseAwaiter
        {
          UnlockWriteDst res;
        } unlockWriteDstAwaiter;

        struct WriteDstAwaiter : BaseAwaiter
        {
          WriteDst res;
        } writeDstAwaiter;

        struct WriteAlternateDstAwaiter : BaseAwaiter
        {
          WriteAlternateDst res;
        } writeAlternateDstAwaiter;

      } u;
    } await;

    Awaiter::U::DecodeAwaiter& await_transform( Decode )
    {
      await.type = Awaiter::Type::DECODE;
      return await.u.decodeAwaiter;
    }

    Awaiter::U::ReadSrcGetDstAwaiter& await_transform( ReadSrcGetDst const& arg )
    {
      await.type = Awaiter::Type::READ_SRC_GET_DST;
      await.u.readSrcGetDstAwaiter.arg = arg;
      return await.u.readSrcGetDstAwaiter;
    }

    Awaiter::U::ReadAlternateSrcGetDstAwaiter& await_transform( ReadAlternateSrcGetDst const& arg )
    {
      await.type = Awaiter::Type::READ_ALTERNATE_SRC_GET_DST;
      await.u.readAlternateSrcGetDstAwaiter.arg = arg;
      return await.u.readAlternateSrcGetDstAwaiter;
    }

    Awaiter::U::ReadSrcGetAlternateDstAwaiter& await_transform( ReadSrcGetAlternateDst const& arg )
    {
      await.type = Awaiter::Type::READ_SRC_GET_ALTERNATE_DST;
      await.u.readSrcGetAlternateDstAwaiter.arg = arg;
      return await.u.readSrcGetAlternateDstAwaiter;
    }

    Awaiter::U::ReadSrcLockReadDstLockFlagsAwaiter& await_transform( ReadSrcLockReadDstLockFlags const& arg )
    {
      await.type = Awaiter::Type::READ_SRC_LOCK_READ_DST_LOCK_FLAGS;
      await.u.readSrcLockDstLockFlagsAwaiter.arg = arg;
      return await.u.readSrcLockDstLockFlagsAwaiter;
    }

    Awaiter::U::ReadSrcLockReadDstLockReadFlagsAwaiter& await_transform( ReadSrcLockReadDstLockReadFlags const& arg )
    {
      await.type = Awaiter::Type::READ_SRC_LOCK_READ_DST_LOCK_READ_FLAGS;
      await.u.readSrcLockDstLockReadFlagsAwaiter.arg = arg;
      return await.u.readSrcLockDstLockReadFlagsAwaiter;
    }

    Awaiter::U::ReadSrcReadDstAwaiter& await_transform( ReadSrcReadDst const& arg )
    {
      await.type = Awaiter::Type::READ_SRC_READ_DST;
      await.u.readSrcReadDstAwaiter.arg = arg;
      return await.u.readSrcReadDstAwaiter;
    }

    Awaiter::U::ReadDstLockFlagsAwaiter& await_transform( ReadDstLockFlags const& arg )
    {
      await.type = Awaiter::Type::READ_DST_LOCK_FLAGS;
      await.u.readDstLockFlagsAwaiter.arg = arg;
      return await.u.readDstLockFlagsAwaiter;
    }

    Awaiter::U::ReadSrcsGetDstAwaiter& await_transform( ReadSrcsGetDst const& arg )
    {
      await.type = Awaiter::Type::READ_SRCS_GET_DST;
      await.u.readSrcsGetDstAwaiter.arg = arg;
      return await.u.readSrcsGetDstAwaiter;
    }

    Awaiter::U::LockReadDstLockFlagsAwaiter& await_transform( LockReadDstLockFlags const& arg )
    {
      await.type = Awaiter::Type::LOCK_READ_DST_LOCK_FLAGS;
      await.u.lockReadDstLockFlagsAwaiter.arg = arg;
      return await.u.lockReadDstLockFlagsAwaiter;
    }

    Awaiter::U::LockReadDstAwaiter& await_transform( LockReadDst const& arg )
    {
      await.type = Awaiter::Type::LOCK_READ_DST;
      await.u.lockReadDstAwaiter.arg = arg;
      return await.u.lockReadDstAwaiter;
    }

    Awaiter::U::GetDstAwaiter& await_transform( GetDst const& arg )
    {
      await.type = Awaiter::Type::GET_DST;
      await.u.getDstAwaiter.arg = arg;
      return await.u.getDstAwaiter;
    }

    Awaiter::U::ComputeAwaiter& await_transform( Compute const& arg )
    {
      await.type = Awaiter::Type::COMPUTE;
      return await.u.computeAwaiter;
    }

    Awaiter::U::NOPAwaiter& await_transform( NOP const& arg )
    {
      await.type = Awaiter::Type::NOP;
      return await.u.nopAwaiter;
    }

    Awaiter::U::MemoryLoadLongAwaiter& await_transform( MemoryLoadLong const& arg )
    {
      await.type = Awaiter::Type::MEMORY_LOAD_LONG;
      await.u.memoryLoadLongAwaiter.arg = arg;
      return await.u.memoryLoadLongAwaiter;
    }

    Awaiter::U::MemoryStoreLongAwaiter& await_transform( MemoryStoreLong const& arg )
    {
      await.type = Awaiter::Type::MEMORY_LOAD_LONG;
      await.u.memoryStoreLongAwaiter.arg = arg;
      return await.u.memoryStoreLongAwaiter;
    }

    Awaiter::U::UnlockWriteDstUnlockWriteFlagsAwaiter& await_transform( UnlockWriteDstUnlockWriteFlags const& arg )
    {
      await.type = Awaiter::Type::UNLOCK_WRITE_DST_UNLOCK_WRITE_FLAGS;
      await.u.unlockWriteDstUnlockWriteFlagsAwaiter.res = arg;
      return await.u.unlockWriteDstUnlockWriteFlagsAwaiter;
    }

    Awaiter::U::UnlockWriteDstAwaiter& await_transform( UnlockWriteDst const& arg )
    {
      await.type = Awaiter::Type::UNLOCK_WRITE_DST;
      await.u.unlockWriteDstAwaiter.res = arg;
      return await.u.unlockWriteDstAwaiter;
    }

    Awaiter::U::UnlockWriteFlagsAwaiter& await_transform( UnlockWriteFlags const& arg )
    {
      await.type = Awaiter::Type::UNLOCK_WRITE_FLAGS;
      await.u.unlockWriteFlagsAwaiter.res = arg;
      return await.u.unlockWriteFlagsAwaiter;
    }

    Awaiter::U::WriteDstAwaiter& await_transform( WriteDst const& arg )
    {
      await.type = Awaiter::Type::WRITE_DST;
      await.u.writeDstAwaiter.res = arg;
      return await.u.writeDstAwaiter;
    }

    Awaiter::U::WriteAlternateDstAwaiter& await_transform( WriteAlternateDst const& arg )
    {
      await.type = Awaiter::Type::WRITE_ALTERNATE_DST;
      await.u.writeAlternateDstAwaiter.res = arg;
      return await.u.writeAlternateDstAwaiter;
    }
  };

public:
  
  static Coroutine create();

  ~Coroutine()
  {
    if ( mCoro )
      mCoro.destroy();
  }

  Coroutine( Coroutine&& other ) noexcept
  {
    mCoro = std::exchange( other.mCoro, nullptr );
  }

  Coroutine& operator=( Coroutine&& other ) noexcept
  {
    mCoro = std::exchange( other.mCoro, nullptr );
    return *this;
  }

  Coroutine( Coroutine const& ) = delete;
  Coroutine& operator=( Coroutine const& ) = delete;

  void decode( RISCOpcode opcode )
  {
    assert( !mCoro.done() );

    mCoro.promise().await.u.decodeAwaiter.opcode = opcode;
    mCoro.resume();
  }
};


class CoroutinePool
{
public:
  CoroutinePool() = default;
  ~CoroutinePool() = default;

  Coroutine pull()
  {
    if ( mCoros.empty() )
      return Coroutine::create();
    else
    {
      Coroutine coro = std::move( mCoros.top() );
      mCoros.pop();
      return coro;
    }
  }

  void push( Coroutine coro )
  {
    mCoros.push( std::move( coro ) );
  }

private:
  std::stack<Coroutine> mCoros;
};


RISC::RISC() : mPool{ std::make_unique<CoroutinePool>() }
{
}

RISC::~RISC()
{
}

void RISC::run()
{
  auto coro = mPool->pull();

  coro.decode( { RISCOpcode::Opcode::ADD, 0, 0 } );
}

Coroutine Coroutine::create()
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
