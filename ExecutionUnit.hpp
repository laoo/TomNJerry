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

  /*

  * ReadDst
  * ReadDstLockFlags
  * LockReadDst
  * LockReadDstLockFlags
  * LockReadDstReadFlags
  * LockReadDstLockReadFlags
  * ReadSrc
  * ReadSrcLockFlags
  * ReadSrcReadFlags
  * ReadSrcLockReadFlags
  * ReadSrcReadDst
  * ReadSrcLockReadDst
  * ReadSrcReadDstLockFlags
  * ReadSrcReadDstReadFlags
  * ReadSrcReadDstLockReadFlags
  * ReadSrcLockReadDstLockFlags
  * ReadSrcLockReadDstReadFlags
  * ReadSrcLockReadDstLockReadFlags
  *
  * WriteDst
  * UnlockWriteDst
  * UnlockWriteFlags
  * UnlockWriteDstUnlockWriteFlags

  */

  //execute unit
  struct Decode { struct Res { RISCOpcode opcode; }; };
  struct GetCode { uint16_t pass; struct Res { uint16_t code; }; };
  struct GetMod { struct Res { uint32_t mod; }; };
  struct Compute {};
  struct NOP {};

  //internal memory unit
  struct MemoryLoadLong { uint32_t src; struct Res { uint32_t value; bool success; }; };
  struct MemoryStoreLong { uint32_t addr; uint32_t value; struct Res { bool success; }; };

  //external memory unit
  struct MemoryLoadLongExternal { uint32_t src; struct Res { uint32_t value; }; };
  struct MemoryStoreLongExternal { uint32_t addr; uint32_t value; };

  //read unit
  struct ReadDstLockFlags { uint8_t dst; struct Res { uint32_t dst; }; };
  struct LockReadDst { uint8_t dst; struct Res { uint32_t dst; }; };
  struct LockReadDstLockFlags { uint8_t dst; struct Res { uint32_t dst; }; };
  struct ReadSrc { uint8_t src; struct Res { uint32_t src; }; };
  struct ReadSrcReadDst { uint8_t src; uint8_t dst; struct Res { uint32_t src; uint32_t dst; }; };
  struct ReadSrcLockReadDstLockFlags { uint8_t src; uint8_t dst; struct Res { uint32_t src; uint32_t dst; }; };
  struct ReadSrcLockReadDstLockReadFlags { uint8_t src; uint8_t dst; struct Res { uint32_t src; uint32_t dst; uint16_t c; }; };

  //write unit
  struct WriteDst { uint32_t value; uint8_t reg; };
  struct UnlockWriteDst { uint32_t value; uint8_t reg; };
  struct UnlockWriteFlags { uint32_t nz;  uint16_t c; };
  struct UnlockWriteDstUnlockWriteFlags { uint32_t valuenz; uint16_t c; uint8_t reg; };

  enum struct AwaiterType
  {
    DECODE,
    GET_CODE,
    GET_MOD,
    COMPUTE,
    NOP,

    MEMORY_LOAD_LONG,
    MEMORY_STORE_LONG,

    MEMORY_LOAD_LONG_EXTERNAL,
    MEMORY_STORE_LONG_EXTERNAL,

    READ_DST_LOCK_FLAGS,
    LOCK_READ_DST,
    LOCK_READ_DST_LOCK_FLAGS,
    READ_SRC,
    READ_SRC_READ_DST,
    READ_SRC_LOCK_READ_DST_LOCK_FLAGS,
    READ_SRC_LOCK_READ_DST_LOCK_READ_FLAGS,

    WRITE_DST,
    UNLOCK_WRITE_DST,
    UNLOCK_WRITE_FLAGS,
    UNLOCK_WRITE_DST_UNLOCK_WRITE_FLAGS,
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

    AwaiterType type;

    struct Awaiter
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
        MemoryStoreLong::Res res;
        auto await_resume() { return res; }
      } memoryStoreLongAwaiter;

      struct MemoryLoadLongExternalAwaiter : BaseAwaiter
      {
        MemoryLoadLongExternal arg;
        MemoryLoadLongExternal::Res res;
        auto await_resume() { return res; }
      } memoryLoadLongExternalAwaiter;

      struct MemoryStoreLongExternalAwaiter : BaseAwaiter
      {
        MemoryStoreLongExternal arg;
        Decode::Res res;
      } memoryStoreLongExternalAwaiter;


      struct ReadDstLockFlagsAwaiter : BaseAwaiter
      {
        ReadDstLockFlags arg;
        ReadDstLockFlags::Res res;
        auto await_resume() { return res; }
      } readDstLockFlagsAwaiter;

      struct LockReadDstAwaiter : BaseAwaiter
      {
        LockReadDst arg;
        LockReadDst::Res res;
        auto await_resume() { return res; }
      } lockReadDstAwaiter;

      struct LockReadDstLockFlagsAwaiter : BaseAwaiter
      {
        LockReadDstLockFlags arg;
        LockReadDstLockFlags::Res res;
        auto await_resume() { return res; }
      } lockReadDstLockFlagsAwaiter;

      struct ReadSrcAwaiter : BaseAwaiter
      {
        ReadSrc arg;
        ReadSrc::Res res;
        auto await_resume() { return res; }
      } readSrcAwaiter;

      struct ReadSrcReadDstAwaiter : BaseAwaiter
      {
        ReadSrcReadDst arg;
        ReadSrcReadDst::Res res;
        auto await_resume() { return res; }
      } readSrcReadDstAwaiter;

      struct ReadSrcLockReadDstLockFlagsAwaiter : BaseAwaiter
      {
        ReadSrcLockReadDstLockFlags arg;
        ReadSrcLockReadDstLockFlags::Res res;
        auto await_resume() { return res; }
      } readSrcLockReadDstLockFlagsAwaiter;

      struct ReadSrcLockReadDstLockReadFlagsAwaiter : BaseAwaiter
      {
        ReadSrcLockReadDstLockReadFlags arg;
        ReadSrcLockReadDstLockReadFlags::Res res;
        auto await_resume() { return res; }
      } readSrcLockReadDstLockReadFlagsAwaiter;


      struct WriteDstAwaiter : BaseAwaiter
      {
        WriteDst arg;
      } writeDstAwaiter;

      struct UnlockWriteDstAwaiter : BaseAwaiter
      {
        UnlockWriteDst arg;
      } unlockWriteDstAwaiter;

      struct UnlockWriteFlagsAwaiter : BaseAwaiter
      {
        UnlockWriteFlags arg;
      } unlockWriteFlagsAwaiter;

      struct UnlockWriteDstUnlockWriteFlagsAwaiter : BaseAwaiter
      {
        UnlockWriteDstUnlockWriteFlags arg;
      } unlockWriteDstUnlockWriteFlagsAwaiter;
    } awaiter{};

    Awaiter::DecodeAwaiter& await_transform( Decode )
    {
      type = AwaiterType::DECODE;
      return awaiter.decodeAwaiter;
    }

    Awaiter::GetCodeAwaiter& await_transform( GetCode arg )
    {
      type = AwaiterType::GET_CODE;
      awaiter.getCodeAwaiter.arg = arg;
      return awaiter.getCodeAwaiter;
    }
    GetCode getCode() const
    {
      assert( type == AwaiterType::GET_CODE );
      return awaiter.getCodeAwaiter.arg;
    }

    void getCode( GetCode::Res code )
    {
      assert( type == AwaiterType::READ_SRC );
      awaiter.getCodeAwaiter.res = code;
    }

    Awaiter::GetModAwaiter& await_transform( GetMod )
    {
      type = AwaiterType::GET_MOD;
      return awaiter.getModAwaiter;
    }

    void getMod( GetMod::Res mod )
    {
      assert( type == AwaiterType::GET_MOD );
      awaiter.getModAwaiter.res = mod;
    }

    Awaiter::ComputeAwaiter& await_transform( Compute )
    {
      type = AwaiterType::COMPUTE;
      return awaiter.computeAwaiter;
    }

    Awaiter::NOPAwaiter& await_transform( NOP )
    {
      type = AwaiterType::NOP;
      return awaiter.nopAwaiter;
    }

    Awaiter::MemoryLoadLongAwaiter& await_transform( MemoryLoadLong const& arg )
    {
      type = AwaiterType::MEMORY_LOAD_LONG;
      awaiter.memoryLoadLongAwaiter.arg = arg;
      return awaiter.memoryLoadLongAwaiter;
    }

    MemoryLoadLong memoryLoadLong() const
    {
      assert( type == AwaiterType::MEMORY_LOAD_LONG );
      return awaiter.memoryLoadLongAwaiter.arg;
    }

    void memoryLoadLong( MemoryLoadLong::Res res )
    {
      assert( type == AwaiterType::MEMORY_LOAD_LONG );
      awaiter.memoryLoadLongAwaiter.res = res;
    }

    Awaiter::MemoryStoreLongAwaiter& await_transform( MemoryStoreLong const& arg )
    {
      type = AwaiterType::MEMORY_LOAD_LONG;
      awaiter.memoryStoreLongAwaiter.arg = arg;
      return awaiter.memoryStoreLongAwaiter;
    }

    MemoryStoreLong memoryStoreLong() const
    {
      assert( type == AwaiterType::MEMORY_STORE_LONG );
      return awaiter.memoryStoreLongAwaiter.arg;
    }

    void memoryStoreLong( MemoryStoreLong::Res res )
    {
      assert( type == AwaiterType::MEMORY_STORE_LONG );
      awaiter.memoryStoreLongAwaiter.res = res;
    }

    Awaiter::MemoryLoadLongExternalAwaiter& await_transform( MemoryLoadLongExternal const& arg )
    {
      type = AwaiterType::MEMORY_LOAD_LONG_EXTERNAL;
      awaiter.memoryLoadLongExternalAwaiter.arg = arg;
      return awaiter.memoryLoadLongExternalAwaiter;
    }

    MemoryLoadLongExternal memoryLoadLongExternal() const
    {
      assert( type == AwaiterType::MEMORY_LOAD_LONG_EXTERNAL );
      return awaiter.memoryLoadLongExternalAwaiter.arg;
    }

    void memoryLoadLongExternal( MemoryLoadLongExternal::Res res )
    {
      assert( type == AwaiterType::MEMORY_LOAD_LONG_EXTERNAL );
      awaiter.memoryLoadLongExternalAwaiter.res = res;
    }

    Awaiter::MemoryStoreLongExternalAwaiter& await_transform( MemoryStoreLongExternal const& arg )
    {
      type = AwaiterType::MEMORY_LOAD_LONG_EXTERNAL;
      awaiter.memoryStoreLongExternalAwaiter.arg = arg;
      return awaiter.memoryStoreLongExternalAwaiter;
    }

    MemoryStoreLongExternal memoryStoreLongExternal() const
    {
      assert( type == AwaiterType::MEMORY_STORE_LONG_EXTERNAL );
      return awaiter.memoryStoreLongExternalAwaiter.arg;
    }

    Awaiter::ReadDstLockFlagsAwaiter& await_transform( ReadDstLockFlags const& arg )
    {
      type = AwaiterType::READ_DST_LOCK_FLAGS;
      awaiter.readDstLockFlagsAwaiter.arg = arg;
      return awaiter.readDstLockFlagsAwaiter;
    }

    ReadDstLockFlags readDstLockFlags() const
    {
      assert( type == AwaiterType::READ_DST_LOCK_FLAGS );
      return awaiter.readDstLockFlagsAwaiter.arg;
    }

    void readDstLockFlags( ReadDstLockFlags::Res res )
    {
      assert( type == AwaiterType::READ_DST_LOCK_FLAGS );
      awaiter.readDstLockFlagsAwaiter.res = res;
    }

    Awaiter::LockReadDstAwaiter& await_transform( LockReadDst const& arg )
    {
      type = AwaiterType::LOCK_READ_DST;
      awaiter.lockReadDstAwaiter.arg = arg;
      return awaiter.lockReadDstAwaiter;
    }

    LockReadDst lockReadDst() const
    {
      assert( type == AwaiterType::LOCK_READ_DST );
      return awaiter.lockReadDstAwaiter.arg;
    }

    void lockReadDst( LockReadDst::Res res )
    {
      assert( type == AwaiterType::LOCK_READ_DST );
      awaiter.lockReadDstAwaiter.res = res;
    }

    Awaiter::LockReadDstLockFlagsAwaiter& await_transform( LockReadDstLockFlags const& arg )
    {
      type = AwaiterType::LOCK_READ_DST_LOCK_FLAGS;
      awaiter.lockReadDstLockFlagsAwaiter.arg = arg;
      return awaiter.lockReadDstLockFlagsAwaiter;
    }

    LockReadDstLockFlags lockReadDstLockFlags() const
    {
      assert( type == AwaiterType::LOCK_READ_DST_LOCK_FLAGS );
      return awaiter.lockReadDstLockFlagsAwaiter.arg;
    }

    void lockReadDstLockFlags( LockReadDstLockFlags::Res res )
    {
      assert( type == AwaiterType::LOCK_READ_DST_LOCK_FLAGS );
      awaiter.lockReadDstLockFlagsAwaiter.res = res;
    }

    Awaiter::ReadSrcAwaiter& await_transform( ReadSrc const& arg )
    {
      type = AwaiterType::READ_SRC;
      awaiter.readSrcAwaiter.arg = arg;
      return awaiter.readSrcAwaiter;
    }

    ReadSrc readSrc() const
    {
      assert( type == AwaiterType::READ_SRC );
      return awaiter.readSrcAwaiter.arg;
    }

    void readSrc( ReadSrc::Res res )
    {
      assert( type == AwaiterType::READ_SRC );
      awaiter.readSrcAwaiter.res = res;
    }

    Awaiter::ReadSrcReadDstAwaiter& await_transform( ReadSrcReadDst const& arg )
    {
      type = AwaiterType::READ_SRC_READ_DST;
      awaiter.readSrcReadDstAwaiter.arg = arg;
      return awaiter.readSrcReadDstAwaiter;
    }

    ReadSrcReadDst readSrcReadDst() const
    {
      assert( type == AwaiterType::READ_SRC_READ_DST );
      return awaiter.readSrcReadDstAwaiter.arg;
    }

    void readSrcReadDst( ReadSrcReadDst::Res res )
    {
      assert( type == AwaiterType::READ_SRC_READ_DST );
      awaiter.readSrcReadDstAwaiter.res = res;
    }

    Awaiter::ReadSrcLockReadDstLockFlagsAwaiter& await_transform( ReadSrcLockReadDstLockFlags const& arg )
    {
      type = AwaiterType::READ_SRC_LOCK_READ_DST_LOCK_FLAGS;
      awaiter.readSrcLockReadDstLockFlagsAwaiter.arg = arg;
      return awaiter.readSrcLockReadDstLockFlagsAwaiter;
    }

    ReadSrcLockReadDstLockFlags readSrcLockReadDstLockFlags() const
    {
      assert( type == AwaiterType::READ_SRC_LOCK_READ_DST_LOCK_FLAGS );
      return awaiter.readSrcLockReadDstLockFlagsAwaiter.arg;
    }

    void readSrcLockReadDstLockFlags( ReadSrcLockReadDstLockFlags::Res res )
    {
      assert( type == AwaiterType::READ_SRC_LOCK_READ_DST_LOCK_FLAGS );
      awaiter.readSrcLockReadDstLockFlagsAwaiter.res = res;
    }

    Awaiter::ReadSrcLockReadDstLockReadFlagsAwaiter& await_transform( ReadSrcLockReadDstLockReadFlags const& arg )
    {
      type = AwaiterType::READ_SRC_LOCK_READ_DST_LOCK_READ_FLAGS;
      awaiter.readSrcLockReadDstLockReadFlagsAwaiter.arg = arg;
      return awaiter.readSrcLockReadDstLockReadFlagsAwaiter;
    }

    ReadSrcLockReadDstLockReadFlags readSrcLockReadDstLockReadFlags() const
    {
      assert( type == AwaiterType::READ_SRC_LOCK_READ_DST_LOCK_READ_FLAGS );
      return awaiter.readSrcLockReadDstLockReadFlagsAwaiter.arg;
    }

    void readSrcLockReadDstLockReadFlags( ReadSrcLockReadDstLockReadFlags::Res res )
    {
      assert( type == AwaiterType::READ_SRC_LOCK_READ_DST_LOCK_READ_FLAGS );
      awaiter.readSrcLockReadDstLockReadFlagsAwaiter.res = res;
    }

    Awaiter::WriteDstAwaiter& await_transform( WriteDst const& arg )
    {
      type = AwaiterType::WRITE_DST;
      awaiter.writeDstAwaiter.arg = arg;
      return awaiter.writeDstAwaiter;
    }

    WriteDst writeDst() const
    {
      assert( type == AwaiterType::WRITE_DST );
      return awaiter.writeDstAwaiter.arg;
    }

    Awaiter::UnlockWriteDstAwaiter& await_transform( UnlockWriteDst const& arg )
    {
      type = AwaiterType::UNLOCK_WRITE_DST;
      awaiter.unlockWriteDstAwaiter.arg = arg;
      return awaiter.unlockWriteDstAwaiter;
    }

    UnlockWriteDst unlockWriteDst() const
    {
      assert( type == AwaiterType::UNLOCK_WRITE_DST );
      return awaiter.unlockWriteDstAwaiter.arg;
    }

    Awaiter::UnlockWriteFlagsAwaiter& await_transform( UnlockWriteFlags const& arg )
    {
      type = AwaiterType::UNLOCK_WRITE_FLAGS;
      awaiter.unlockWriteFlagsAwaiter.arg = arg;
      return awaiter.unlockWriteFlagsAwaiter;
    }

    UnlockWriteFlags unlockWriteFlags() const
    {
      assert( type == AwaiterType::UNLOCK_WRITE_FLAGS );
      return awaiter.unlockWriteFlagsAwaiter.arg;
    }

    Awaiter::UnlockWriteDstUnlockWriteFlagsAwaiter& await_transform( UnlockWriteDstUnlockWriteFlags const& arg )
    {
      type = AwaiterType::UNLOCK_WRITE_DST_UNLOCK_WRITE_FLAGS;
      awaiter.unlockWriteDstUnlockWriteFlagsAwaiter.arg = arg;
      return awaiter.unlockWriteDstUnlockWriteFlagsAwaiter;
    }

    UnlockWriteDstUnlockWriteFlags unlockWriteDstUnlockWriteFlags() const
    {
      assert( type == AwaiterType::UNLOCK_WRITE_DST_UNLOCK_WRITE_FLAGS );
      return awaiter.unlockWriteDstUnlockWriteFlagsAwaiter.arg;
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

  AwaiterType getType() const
  {
    return mCoro.promise().type;
  }

  void decode( RISCOpcode opcode )
  {
    mCoro.promise().awaiter.decodeAwaiter.res.opcode = opcode;
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

  void getMod( GetMod::Res mod )
  {
    mCoro.promise().getMod( mod );
    mCoro();
  }

  void advance()
  {
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

  ReadDstLockFlags readDstLockFlags() const
  {
    return mCoro.promise().readDstLockFlags();
  }

  void readDstLockFlags( ReadDstLockFlags::Res res )
  {
    mCoro.promise().readDstLockFlags( res );
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

  LockReadDstLockFlags lockReadDstLockFlags() const
  {
    return mCoro.promise().lockReadDstLockFlags();
  }

  void lockReadDstLockFlags( LockReadDstLockFlags::Res res )
  {
    mCoro.promise().lockReadDstLockFlags( res );
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

  ReadSrcReadDst readSrcReadDst() const
  {
    return mCoro.promise().readSrcReadDst();
  }

  void readSrcReadDst( ReadSrcReadDst::Res res )
  {
    mCoro.promise().readSrcReadDst( res );
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

  WriteDst writeDst() const
  {
    return mCoro.promise().writeDst();
  }

  UnlockWriteDst unlockWriteDst() const
  {
    return mCoro.promise().unlockWriteDst();
  }

  UnlockWriteFlags unlockWriteFlags() const
  {
    return mCoro.promise().unlockWriteFlags();
  }

  UnlockWriteDstUnlockWriteFlags unlockWriteDstUnlockWriteFlags() const
  {
    return mCoro.promise().unlockWriteDstUnlockWriteFlags();
  }

};

