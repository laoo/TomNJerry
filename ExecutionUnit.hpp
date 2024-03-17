#pragma once

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

class ExecutionUnit
{
  struct promise_type;

  std::coroutine_handle<promise_type> mCoro;

  ExecutionUnit( std::coroutine_handle<promise_type> c ) : mCoro{ c }
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
    auto get_return_object() { return ExecutionUnit{ std::coroutine_handle<promise_type>::from_promise( *this ) }; }
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

  static ExecutionUnit create();

  ExecutionUnit() : mCoro{}
  {
  }

  ~ExecutionUnit()
  {
    if ( mCoro )
      mCoro.destroy();
  }

  ExecutionUnit( ExecutionUnit&& other ) noexcept
  {
    mCoro = std::exchange( other.mCoro, nullptr );
  }

  ExecutionUnit& operator=( ExecutionUnit&& other ) noexcept
  {
    mCoro = std::exchange( other.mCoro, nullptr );
    return *this;
  }

  ExecutionUnit( ExecutionUnit const& ) = delete;
  ExecutionUnit& operator=( ExecutionUnit const& ) = delete;
};

