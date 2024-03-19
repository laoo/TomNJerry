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
    uint16_t pass;
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

  struct GetMod
  {
    struct Res
    {
      uint32_t mod;
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
    GET_MOD,
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
          GetCode arg;
          GetCode::Res res;
          auto await_resume() { return res; }
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

        struct GetModAwaiter : BaseAwaiter
        {
          GetMod::Res res;
          auto await_resume() { return res; }
        } getModAwaiter;

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
          UnlockWriteDstUnlockWriteFlags arg;
        } unlockWriteDstUnlockWriteFlagsAwaiter;

        struct UnlockWriteFlagsAwaiter : BaseAwaiter
        {
          UnlockWriteFlags arg;
        } unlockWriteFlagsAwaiter;

        struct UnlockWriteDstAwaiter : BaseAwaiter
        {
          UnlockWriteDst arg;
        } unlockWriteDstAwaiter;

        struct WriteDstAwaiter : BaseAwaiter
        {
          WriteDst arg;
        } writeDstAwaiter;
      } u{};
    } await{};

    Awaiter::U::DecodeAwaiter& await_transform( Decode )
    {
      await.type = ExecutionUnit::Type::DECODE;
      return await.u.decodeAwaiter;
    }

    Awaiter::U::GetCodeAwaiter& await_transform( GetCode arg )
    {
      await.type = ExecutionUnit::Type::GET_CODE;
      await.u.getCodeAwaiter.arg = arg;
      return await.u.getCodeAwaiter;
    }

    GetCode getCode() const
    {
      assert( await.type == Type::GET_CODE );
      return await.u.getCodeAwaiter.arg;
    }

    void getCode( GetCode::Res code )
    {
      assert( await.type == Type::READ_SRC );
      await.u.getCodeAwaiter.res = code;
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

    ReadSrcLockReadDstLockFlags readSrcLockReadDstLockFlags() const
    {
      assert( await.type == Type::READ_SRC_LOCK_READ_DST_LOCK_FLAGS );
      return await.u.readSrcLockDstLockFlagsAwaiter.arg;
    }

    void readSrcLockReadDstLockFlags( ReadSrcLockReadDstLockFlags::Res res )
    {
      assert( await.type == Type::READ_SRC_LOCK_READ_DST_LOCK_FLAGS );
      await.u.readSrcLockDstLockFlagsAwaiter.res = res;
    }

    Awaiter::U::ReadSrcLockReadDstLockReadFlagsAwaiter& await_transform( ReadSrcLockReadDstLockReadFlags const& arg )
    {
      await.type = ExecutionUnit::Type::READ_SRC_LOCK_READ_DST_LOCK_READ_FLAGS;
      await.u.readSrcLockDstLockReadFlagsAwaiter.arg = arg;
      return await.u.readSrcLockDstLockReadFlagsAwaiter;
    }

    ReadSrcLockReadDstLockReadFlags readSrcLockReadDstLockReadFlags() const
    {
      assert( await.type == Type::READ_SRC_LOCK_READ_DST_LOCK_READ_FLAGS );
      return await.u.readSrcLockDstLockReadFlagsAwaiter.arg;
    }

    void readSrcLockReadDstLockReadFlags( ReadSrcLockReadDstLockReadFlags::Res res )
    {
      assert( await.type == Type::READ_SRC_LOCK_READ_DST_LOCK_READ_FLAGS );
      await.u.readSrcLockDstLockReadFlagsAwaiter.res = res;
    }

    Awaiter::U::ReadSrcReadDstAwaiter& await_transform( ReadSrcReadDst const& arg )
    {
      await.type = ExecutionUnit::Type::READ_SRC_READ_DST;
      await.u.readSrcReadDstAwaiter.arg = arg;
      return await.u.readSrcReadDstAwaiter;
    }

    ReadSrcReadDst readSrcReadDst() const
    {
      assert( await.type == Type::READ_SRC_READ_DST );
      return await.u.readSrcReadDstAwaiter.arg;
    }

    void readSrcReadDst( ReadSrcReadDst::Res res )
    {
      assert( await.type == Type::READ_SRC_READ_DST );
      await.u.readSrcReadDstAwaiter.res = res;
    }

    Awaiter::U::ReadDstLockFlagsAwaiter& await_transform( ReadDstLockFlags const& arg )
    {
      await.type = ExecutionUnit::Type::READ_DST_LOCK_FLAGS;
      await.u.readDstLockFlagsAwaiter.arg = arg;
      return await.u.readDstLockFlagsAwaiter;
    }

    ReadDstLockFlags readDstLockFlags() const
    {
      assert( await.type == Type::READ_DST_LOCK_FLAGS );
      return await.u.readDstLockFlagsAwaiter.arg;
    }

    void readDstLockFlags( ReadDstLockFlags::Res res )
    {
      assert( await.type == Type::READ_DST_LOCK_FLAGS );
      await.u.readDstLockFlagsAwaiter.res = res;
    }

    Awaiter::U::ReadSrcsAwaiter& await_transform( ReadSrcs const& arg )
    {
      await.type = ExecutionUnit::Type::READ_SRCS;
      await.u.readSrcsAwaiter.arg = arg;
      return await.u.readSrcsAwaiter;
    }

    ReadSrcs readSrcs() const
    {
      assert( await.type == Type::READ_SRCS );
      return await.u.readSrcsAwaiter.arg;
    }

    void readSrcs( ReadSrcs::Res res )
    {
      assert( await.type == Type::READ_SRCS );
      await.u.readSrcsAwaiter.res = res;
    }

    Awaiter::U::LockReadDstLockFlagsAwaiter& await_transform( LockReadDstLockFlags const& arg )
    {
      await.type = ExecutionUnit::Type::LOCK_READ_DST_LOCK_FLAGS;
      await.u.lockReadDstLockFlagsAwaiter.arg = arg;
      return await.u.lockReadDstLockFlagsAwaiter;
    }

    LockReadDstLockFlags lockReadDstLockFlags() const
    {
      assert( await.type == Type::LOCK_READ_DST_LOCK_FLAGS );
      return await.u.lockReadDstLockFlagsAwaiter.arg;
    }

    void lockReadDstLockFlags( LockReadDstLockFlags::Res res )
    {
      assert( await.type == Type::LOCK_READ_DST_LOCK_FLAGS );
      await.u.lockReadDstLockFlagsAwaiter.res = res;
    }

    Awaiter::U::LockReadDstAwaiter& await_transform( LockReadDst const& arg )
    {
      await.type = ExecutionUnit::Type::LOCK_READ_DST;
      await.u.lockReadDstAwaiter.arg = arg;
      return await.u.lockReadDstAwaiter;
    }

    LockReadDst lockReadDst() const
    {
      assert( await.type == Type::LOCK_READ_DST );
      return await.u.lockReadDstAwaiter.arg;
    }

    void lockReadDst( LockReadDst::Res res )
    {
      assert( await.type == Type::LOCK_READ_DST );
      await.u.lockReadDstAwaiter.res = res;
    }

    Awaiter::U::GetModAwaiter& await_transform( GetMod )
    {
      await.type = ExecutionUnit::Type::GET_MOD;
      return await.u.getModAwaiter;
    }

    void getMod( GetMod::Res mod )
    {
      assert( await.type == Type::GET_MOD );
      await.u.getModAwaiter.res = mod;
    }

    Awaiter::U::ComputeAwaiter& await_transform( Compute )
    {
      await.type = ExecutionUnit::Type::COMPUTE;
      return await.u.computeAwaiter;
    }

    Awaiter::U::NOPAwaiter& await_transform( NOP )
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

    MemoryLoadLong memoryLoadLong() const
    {
      assert( await.type == Type::MEMORY_LOAD_LONG );
      return await.u.memoryLoadLongAwaiter.arg;
    }

    void memoryLoadLong( MemoryLoadLong::Res res )
    {
      assert( await.type == Type::MEMORY_LOAD_LONG );
      await.u.memoryLoadLongAwaiter.res = res;
    }

    Awaiter::U::MemoryStoreLongAwaiter& await_transform( MemoryStoreLong const& arg )
    {
      await.type = ExecutionUnit::Type::MEMORY_LOAD_LONG;
      await.u.memoryStoreLongAwaiter.arg = arg;
      return await.u.memoryStoreLongAwaiter;
    }

    MemoryStoreLong memoryStoreLong() const
    {
      assert( await.type == Type::MEMORY_STORE_LONG );
      return await.u.memoryStoreLongAwaiter.arg;
    }

    Awaiter::U::UnlockWriteDstUnlockWriteFlagsAwaiter& await_transform( UnlockWriteDstUnlockWriteFlags const& arg )
    {
      await.type = ExecutionUnit::Type::UNLOCK_WRITE_DST_UNLOCK_WRITE_FLAGS;
      await.u.unlockWriteDstUnlockWriteFlagsAwaiter.arg = arg;
      return await.u.unlockWriteDstUnlockWriteFlagsAwaiter;
    }

    UnlockWriteDstUnlockWriteFlags unlockWriteDstUnlockWriteFlags() const
    {
      assert( await.type == Type::UNLOCK_WRITE_DST_UNLOCK_WRITE_FLAGS );
      return await.u.unlockWriteDstUnlockWriteFlagsAwaiter.arg;
    }

    Awaiter::U::UnlockWriteDstAwaiter& await_transform( UnlockWriteDst const& arg )
    {
      await.type = ExecutionUnit::Type::UNLOCK_WRITE_DST;
      await.u.unlockWriteDstAwaiter.arg = arg;
      return await.u.unlockWriteDstAwaiter;
    }

    UnlockWriteDst unlockWriteDst() const
    {
      assert( await.type == Type::UNLOCK_WRITE_DST );
      return await.u.unlockWriteDstAwaiter.arg;
    }

    Awaiter::U::UnlockWriteFlagsAwaiter& await_transform( UnlockWriteFlags const& arg )
    {
      await.type = ExecutionUnit::Type::UNLOCK_WRITE_FLAGS;
      await.u.unlockWriteFlagsAwaiter.arg = arg;
      return await.u.unlockWriteFlagsAwaiter;
    }

    UnlockWriteFlags unlockWriteFlags() const
    {
      assert( await.type == Type::UNLOCK_WRITE_FLAGS );
      return await.u.unlockWriteFlagsAwaiter.arg;
    }

    Awaiter::U::WriteDstAwaiter& await_transform( WriteDst const& arg )
    {
      await.type = ExecutionUnit::Type::WRITE_DST;
      await.u.writeDstAwaiter.arg = arg;
      return await.u.writeDstAwaiter;
    }

    WriteDst writeDst() const
    {
      assert( await.type == Type::WRITE_DST );
      return await.u.writeDstAwaiter.arg;
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

  Type getType() const
  {
    return mCoro.promise().await.type;
  }

  void decode( RISCOpcode opcode )
  {
    mCoro.promise().await.u.decodeAwaiter.res.opcode = opcode;
    mCoro();
  }

  GetCode getCode() const
  {
    return mCoro.promise().getCode();
  }

  void getCode( uint16_t code )
  {
    mCoro.promise().getCode( { code } );
    mCoro();
  }

  ReadSrc readSrc() const
  {
    return mCoro.promise().readSrc();
  }

  void readSrc( ReadSrc::Res res )
  {
    mCoro.promise().readSrc();
    mCoro();
  }

  ReadSrcLockReadDstLockFlags readSrcLockReadDstLockFlags() const
  {
    return mCoro.promise().readSrcLockReadDstLockFlags();
  }

  void readSrcLockReadDstLockFlags( ReadSrcLockReadDstLockFlags::Res res )
  {
    mCoro.promise().readSrcLockReadDstLockFlags( res );
    mCoro();
  }

  ReadSrcLockReadDstLockReadFlags readSrcLockReadDstLockReadFlags() const
  {
    return mCoro.promise().readSrcLockReadDstLockReadFlags();
  }

  void readSrcLockReadDstLockReadFlags( ReadSrcLockReadDstLockReadFlags::Res res )
  {
    mCoro.promise().readSrcLockReadDstLockReadFlags( res );
    mCoro();
  }

  ReadSrcReadDst readSrcReadDst() const
  {
    return mCoro.promise().readSrcReadDst();
  }

  void readSrcReadDst( ReadSrcReadDst::Res res )
  {
    mCoro.promise().readSrcReadDst( res );
    mCoro();
  }

  ReadDstLockFlags readDstLockFlags() const
  {
    return mCoro.promise().readDstLockFlags();
  }

  void readDstLockFlags( ReadDstLockFlags::Res res )
  {
    mCoro.promise().readDstLockFlags( res );
    mCoro();
  }

  ReadSrcs readSrcs() const
  {
    return mCoro.promise().readSrcs();
  }

  void readSrcs( ReadSrcs::Res res )
  {
    mCoro.promise().readSrcs( res );
    mCoro();
  }

  LockReadDstLockFlags lockReadDstLockFlags() const
  {
    return mCoro.promise().lockReadDstLockFlags();
  }

  void lockReadDstLockFlags( LockReadDstLockFlags::Res res )
  {
    mCoro.promise().lockReadDstLockFlags( res );
    mCoro();
  }

  LockReadDst lockReadDst() const
  {
    return mCoro.promise().lockReadDst();
  }

  void lockReadDst( LockReadDst::Res res )
  {
    mCoro.promise().lockReadDst( res );
    mCoro();
  }

  void getMod( GetMod::Res mod )
  {
    mCoro.promise().getMod( mod );
    mCoro();
  }

  MemoryLoadLong memoryLoadLong() const
  {
    return mCoro.promise().memoryLoadLong();
  }

  void memoryLoadLong( MemoryLoadLong::Res res )
  {
    mCoro.promise().memoryLoadLong( res );
    mCoro();
  }

  MemoryStoreLong memoryStoreLong() const
  {
    return mCoro.promise().memoryStoreLong();
  }

  UnlockWriteDstUnlockWriteFlags unlockWriteDstUnlockWriteFlags() const
  {
    return mCoro.promise().unlockWriteDstUnlockWriteFlags();
  }

  UnlockWriteDst unlockWriteDst() const
  {
    return mCoro.promise().unlockWriteDst();
  }

  UnlockWriteFlags unlockWriteFlags() const
  {
    return mCoro.promise().unlockWriteFlags();
  }

  WriteDst writeDst() const
  {
    return mCoro.promise().writeDst();
  }

  void advance()
  {
    mCoro();
  }

};

