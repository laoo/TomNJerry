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
    XOR,
    _ILL
  };

  Opcode opcode;
  uint8_t src;
  uint8_t dst;
  uint8_t translatedSrc;
  uint8_t translatedDst;
};

class ExecutionUnit
{
  struct promise_type;

  std::coroutine_handle<promise_type> mCoro;

  ExecutionUnit( std::coroutine_handle<promise_type> c ) : mCoro{ c }
  {
  }


public:

  struct Decode
  {
    struct Res
    {
      RISCOpcode opcode;
    };
  };

  struct GetCode
  {
    struct Res
    {
      uint16_t code;
    };
  };

  struct ReadSrc
  {
    uint8_t src;

    struct Res
    {
      uint32_t src;
    };
  };

  struct ReadSrcLockReadDstLockFlags
  {
    uint8_t src;
    uint8_t dst;

    struct Res
    {
      uint32_t src;
      uint32_t dst;
    };
  };

  struct ReadSrcs
  {
    uint8_t src1;
    uint8_t src2;

    struct Res
    {
      uint32_t src1;
      uint32_t src2;
    };
  };

  struct ReadSrcLockReadDstLockReadFlags
  {
    uint8_t src;
    uint8_t dst;

    struct Res
    {
      uint32_t src;
      uint32_t dst;
      uint16_t dstReg;
      uint16_t c;
    };
  };

  struct ReadSrcReadDst
  {
    uint8_t src;
    uint8_t dst;

    struct Res
    {
      uint32_t src;
      uint32_t dst;
    };
  };

  struct LockReadDstLockFlags
  {
    uint8_t dst;

    struct Res
    {
      uint32_t dst;
    };
  };

  struct ReadDstLockFlags
  {
    uint8_t dst;

    struct Res
    {
      uint32_t dst;
      uint16_t dstReg;
    };
  };

  struct LockReadDst
  {
    uint8_t dst;

    struct Res
    {
      uint32_t dst;
    };
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

    struct Res
    {
      uint32_t value;
    };
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

  enum struct Type
  {
    DECODE,
    GET_CODE,
    READ_SRC,
    READ_SRC_LOCK_READ_DST_LOCK_FLAGS,
    READ_SRC_LOCK_READ_DST_LOCK_READ_FLAGS,
    READ_SRC_READ_DST,
    READ_DST_LOCK_FLAGS,
    READ_SRCS,
    LOCK_READ_DST_LOCK_FLAGS,
    LOCK_READ_DST,
    COMPUTE,
    NOP,
    MEMORY_LOAD_LONG,
    MEMORY_STORE_LONG,
    UNLOCK_WRITE_DST_UNLOCK_WRITE_FLAGS,
    UNLOCK_WRITE_DST,
    UNLOCK_WRITE_FLAGS,
    WRITE_DST,
    WRITE_ALTERNATE_DST
  };

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
      BaseAwaiter() = default;
      BaseAwaiter( BaseAwaiter const& ) = delete;
      BaseAwaiter& operator=( BaseAwaiter const& ) = delete;
      bool await_ready() { return false; }
      void await_suspend( std::coroutine_handle<> c ) {}
      void await_resume() {}
    };

    struct Awaiter
    {
      Type type;

      struct U
      {
        struct DecodeAwaiter : BaseAwaiter
        {
          Decode::Res res;
          auto await_resume() { return res; }
        } decodeAwaiter;

        struct GetCodeAwaiter : BaseAwaiter
        {
          GetCode::Res res;
          auto await_resume()
          {
            return res;
          }
        } getCodeAwaiter;

        struct ReadSrcAwaiter : BaseAwaiter
        {
          ReadSrc arg;
          ReadSrc::Res res;
          auto await_resume() { return res; }
        } readSrcAwaiter;

        struct ReadSrcLockReadDstLockFlagsAwaiter : BaseAwaiter
        {
          ReadSrcLockReadDstLockFlags arg;
          ReadSrcLockReadDstLockFlags::Res res;
          auto await_resume() { return res; }
        } readSrcLockDstLockFlagsAwaiter;

        struct ReadSrcLockReadDstLockReadFlagsAwaiter : BaseAwaiter
        {
          ReadSrcLockReadDstLockReadFlags arg;
          ReadSrcLockReadDstLockReadFlags::Res res;
          auto await_resume() { return res; }
        } readSrcLockDstLockReadFlagsAwaiter;

        struct ReadSrcReadDstAwaiter : BaseAwaiter
        {
          ReadSrcReadDst arg;
          ReadSrcReadDst::Res res;
          auto await_resume() { return res; }
        } readSrcReadDstAwaiter;

        struct ReadDstLockFlagsAwaiter : BaseAwaiter
        {
          ReadDstLockFlags arg;
          ReadDstLockFlags::Res res;
          auto await_resume() { return res; }
        } readDstLockFlagsAwaiter;

        struct ReadSrcsAwaiter : BaseAwaiter
        {
          ReadSrcs arg;
          ReadSrcs::Res res;
          auto await_resume() { return res; }
        } readSrcsAwaiter;

        struct LockReadDstLockFlagsAwaiter : BaseAwaiter
        {
          LockReadDstLockFlags arg;
          LockReadDstLockFlags::Res res;
          auto await_resume() { return res; }
        } lockReadDstLockFlagsAwaiter;

        struct LockReadDstAwaiter : BaseAwaiter
        {
          LockReadDst arg;
          LockReadDst::Res res;
          auto await_resume() { return res; }
        } lockReadDstAwaiter;

        struct ComputeAwaiter : BaseAwaiter
        {
        } computeAwaiter;

        struct NOPAwaiter : BaseAwaiter
        {
        } nopAwaiter;

        struct MemoryLoadLongAwaiter : BaseAwaiter
        {
          MemoryLoadLong arg;
          MemoryLoadLong::Res res;
          auto await_resume() { return res; }
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
      } u{};
    } await{};

    Awaiter::U::DecodeAwaiter& await_transform( Decode )
    {
      await.type = ExecutionUnit::Type::DECODE;
      return await.u.decodeAwaiter;
    }

    Awaiter::U::GetCodeAwaiter& await_transform( GetCode )
    {
      await.type = ExecutionUnit::Type::GET_CODE;
      return await.u.getCodeAwaiter;
    }

    void getCode( uint16_t code )
    {
      await.u.getCodeAwaiter.res.code = code;
    }

    Awaiter::U::ReadSrcAwaiter& await_transform( ReadSrc const& arg )
    {
      await.type = ExecutionUnit::Type::READ_SRC;
      await.u.readSrcAwaiter.arg = arg;
      return await.u.readSrcAwaiter;
    }

    ReadSrc readSrc() const
    {
      assert( await.type == Type::READ_SRC );
      return await.u.readSrcAwaiter.arg;
    }

    void readSrc( ReadSrc::Res res )
    {
      assert( await.type == Type::READ_SRC );
      await.u.readSrcAwaiter.res = res;
    }

    Awaiter::U::ReadSrcLockReadDstLockFlagsAwaiter& await_transform( ReadSrcLockReadDstLockFlags const& arg )
    {
      await.type = ExecutionUnit::Type::READ_SRC_LOCK_READ_DST_LOCK_FLAGS;
      await.u.readSrcLockDstLockFlagsAwaiter.arg = arg;
      return await.u.readSrcLockDstLockFlagsAwaiter;
    }

    Awaiter::U::ReadSrcLockReadDstLockReadFlagsAwaiter& await_transform( ReadSrcLockReadDstLockReadFlags const& arg )
    {
      await.type = ExecutionUnit::Type::READ_SRC_LOCK_READ_DST_LOCK_READ_FLAGS;
      await.u.readSrcLockDstLockReadFlagsAwaiter.arg = arg;
      return await.u.readSrcLockDstLockReadFlagsAwaiter;
    }

    Awaiter::U::ReadSrcReadDstAwaiter& await_transform( ReadSrcReadDst const& arg )
    {
      await.type = ExecutionUnit::Type::READ_SRC_READ_DST;
      await.u.readSrcReadDstAwaiter.arg = arg;
      return await.u.readSrcReadDstAwaiter;
    }

    Awaiter::U::ReadDstLockFlagsAwaiter& await_transform( ReadDstLockFlags const& arg )
    {
      await.type = ExecutionUnit::Type::READ_DST_LOCK_FLAGS;
      await.u.readDstLockFlagsAwaiter.arg = arg;
      return await.u.readDstLockFlagsAwaiter;
    }

    Awaiter::U::ReadSrcsAwaiter& await_transform( ReadSrcs const& arg )
    {
      await.type = ExecutionUnit::Type::READ_SRCS;
      await.u.readSrcsAwaiter.arg = arg;
      return await.u.readSrcsAwaiter;
    }

    Awaiter::U::LockReadDstLockFlagsAwaiter& await_transform( LockReadDstLockFlags const& arg )
    {
      await.type = ExecutionUnit::Type::LOCK_READ_DST_LOCK_FLAGS;
      await.u.lockReadDstLockFlagsAwaiter.arg = arg;
      return await.u.lockReadDstLockFlagsAwaiter;
    }

    Awaiter::U::LockReadDstAwaiter& await_transform( LockReadDst const& arg )
    {
      await.type = ExecutionUnit::Type::LOCK_READ_DST;
      await.u.lockReadDstAwaiter.arg = arg;
      return await.u.lockReadDstAwaiter;
    }

    Awaiter::U::ComputeAwaiter& await_transform( Compute const& arg )
    {
      await.type = ExecutionUnit::Type::COMPUTE;
      return await.u.computeAwaiter;
    }

    Awaiter::U::NOPAwaiter& await_transform( NOP const& arg )
    {
      await.type = ExecutionUnit::Type::NOP;
      return await.u.nopAwaiter;
    }

    Awaiter::U::MemoryLoadLongAwaiter& await_transform( MemoryLoadLong const& arg )
    {
      await.type = ExecutionUnit::Type::MEMORY_LOAD_LONG;
      await.u.memoryLoadLongAwaiter.arg = arg;
      return await.u.memoryLoadLongAwaiter;
    }

    Awaiter::U::MemoryStoreLongAwaiter& await_transform( MemoryStoreLong const& arg )
    {
      await.type = ExecutionUnit::Type::MEMORY_LOAD_LONG;
      await.u.memoryStoreLongAwaiter.arg = arg;
      return await.u.memoryStoreLongAwaiter;
    }

    Awaiter::U::UnlockWriteDstUnlockWriteFlagsAwaiter& await_transform( UnlockWriteDstUnlockWriteFlags const& arg )
    {
      await.type = ExecutionUnit::Type::UNLOCK_WRITE_DST_UNLOCK_WRITE_FLAGS;
      await.u.unlockWriteDstUnlockWriteFlagsAwaiter.res = arg;
      return await.u.unlockWriteDstUnlockWriteFlagsAwaiter;
    }

    Awaiter::U::UnlockWriteDstAwaiter& await_transform( UnlockWriteDst const& arg )
    {
      await.type = ExecutionUnit::Type::UNLOCK_WRITE_DST;
      await.u.unlockWriteDstAwaiter.res = arg;
      return await.u.unlockWriteDstAwaiter;
    }

    Awaiter::U::UnlockWriteFlagsAwaiter& await_transform( UnlockWriteFlags const& arg )
    {
      await.type = ExecutionUnit::Type::UNLOCK_WRITE_FLAGS;
      await.u.unlockWriteFlagsAwaiter.res = arg;
      return await.u.unlockWriteFlagsAwaiter;
    }

    Awaiter::U::WriteDstAwaiter& await_transform( WriteDst const& arg )
    {
      await.type = ExecutionUnit::Type::WRITE_DST;
      await.u.writeDstAwaiter.res = arg;
      return await.u.writeDstAwaiter;
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
    if ( this != &other )
      mCoro = std::exchange( other.mCoro, nullptr );
  }

  ExecutionUnit& operator=( ExecutionUnit&& other ) noexcept
  {
    if ( this != &other )
      mCoro = std::exchange( other.mCoro, nullptr );
    return *this;
  }

  ExecutionUnit( ExecutionUnit const& ) = delete;
  ExecutionUnit& operator=( ExecutionUnit const& ) = delete;

  explicit operator bool() const
  {
    return ( bool )mCoro;
  }

  void decode( RISCOpcode opcode )
  {
    mCoro.promise().await.u.decodeAwaiter.res.opcode = opcode;
    mCoro();
  }

  void getCode( uint16_t code )
  {
    mCoro.promise().getCode( code );
    mCoro();
  }

  Type getType() const
  {
    return mCoro.promise().await.type;
  }

  ReadSrc readSrc() const
  {
    return mCoro.promise().readSrc();
  }

  void readSrc( ReadSrc::Res res ) const
  {
    mCoro.promise().readSrc();
    mCoro();
  }
};

