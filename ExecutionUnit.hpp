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
  uint8_t srcValue;
  uint8_t dstValue;
  int8_t srcReg;
  int8_t dstReg;
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
  * ReadLockDst
  * ReadLockDstLockFlags
  * ReadLockDstReadFlags
  * ReadLockDstReadLockFlags
  * ReadSrc
  * ReadSrcLockFlags
  * ReadSrcReadFlags
  * ReadSrcReadLockFlags
  * ReadSrcReadDst
  * ReadSrcReadLockDst
  * ReadSrcReadDstLockFlags
  * ReadSrcReadDstReadFlags
  * ReadSrcReadDstReadLockFlags
  * ReadSrcReadLockDstLockFlags
  * ReadSrcReadLockDstReadFlags
  * ReadSrcReadLockDstReadLockFlags
  *
  * WriteDst
  * UnlockWriteDst
  * UnlockWriteFlags
  * UnlockWriteDstUnlockWriteFlags

  */

  //execute unit
  struct Decode { struct Res { RISCOpcode opcode; }; };
  struct GetCodeLo { struct Res { uint16_t code; }; };
  struct GetCodeHi { struct Res { uint16_t code; }; };
  struct GetMod { struct Res { uint32_t mod; }; };
  struct Compute {};
  struct DivideInit { struct Res { bool offset; }; };
  struct DivideCycle { int32_t cycle; };
  struct NOP {};
  struct IO {};
  struct MoveQ {};

  //internal memory unit
  struct MemoryLoadByte { uint32_t src; struct Res { uint8_t value; bool success; }; };
  struct MemoryLoadWord { uint32_t src; struct Res { uint16_t value; bool success; }; };
  struct MemoryLoadLong { uint32_t src; struct Res { uint32_t value; bool success; }; };

  struct MemoryStoreByte { uint32_t addr; uint8_t value; struct Res { bool success; }; };
  struct MemoryStoreWord { uint32_t addr; uint16_t value; struct Res { bool success; }; };
  struct MemoryStoreLong { uint32_t addr; uint32_t value; struct Res { bool success; }; };

  //external memory unit
  struct MemoryLoadByteExternal { uint32_t src; struct Res { uint8_t value; }; };
  struct MemoryLoadWordExternal { uint32_t src; struct Res { uint16_t value; }; };
  struct MemoryLoadLongExternal { uint32_t src; struct Res { uint32_t value; }; };

  struct MemoryStoreByteExternal { uint32_t addr; uint8_t value; };
  struct MemoryStoreWordExternal { uint32_t addr; uint16_t value; };
  struct MemoryStoreLongExternal { uint32_t addr; uint32_t value; };

  //read unit
  struct ReadDstNoScoreboard { int8_t dst; struct Res { uint32_t dst; }; };
  struct ReadDstLockFlags { int8_t dst; struct Res { uint32_t dst; }; };
  struct ReadLockDst { int8_t dst; struct Res { uint32_t dst; }; };
  struct ReadLockDstLockFlags { int8_t dst; struct Res { uint32_t dst; }; };
  struct ReadSrc { int8_t src; struct Res { uint32_t src; }; };
  struct ReadSrcReadDst { int8_t src; int8_t dst; struct Res { uint32_t src; uint32_t dst; }; };
  struct ReadSrcReadLockDst { int8_t src; int8_t dst; struct Res { uint32_t src; uint32_t dst; }; };
  struct ReadSrcReadLockDstLockFlags { int8_t src; int8_t dst; struct Res { uint32_t src; uint32_t dst; }; };
  struct ReadSrcReadLockDstReadLockFlags { int8_t src; int8_t dst; struct Res { uint32_t src; uint32_t dst; uint16_t c; }; };
  struct ReadPC { struct Res { uint32_t pc; }; };

  //write unit
  struct WriteDst { uint32_t value; int8_t reg; };
  struct WriteDstWriteFlags { uint32_t valuenz; uint16_t c; int8_t reg; };
  struct UnlockWriteDst { uint32_t value; int8_t reg; };
  struct UnlockWriteDstRemain { uint32_t value; uint32_t remain; int8_t reg; };
  struct UnlockWriteFlags { uint32_t nz;  uint16_t c; };
  struct UnlockWriteDstUnlockWriteFlags { uint32_t valuenz; uint16_t c; int8_t reg; };

  enum struct AwaiterType
  {
    DECODE,
    GET_CODE_LO,
    GET_CODE_HI,
    GET_MOD,
    COMPUTE,
    DIVIDE_INIT,
    DIVIDE_CYCLE,
    NOP,
    IO,
    MOVE_Q,

    MEMORY_LOAD_BYTE,
    MEMORY_LOAD_WORD,
    MEMORY_LOAD_LONG,
    MEMORY_STORE_BYTE,
    MEMORY_STORE_WORD,
    MEMORY_STORE_LONG,

    MEMORY_LOAD_BYTE_EXTERNAL,
    MEMORY_LOAD_WORD_EXTERNAL,
    MEMORY_LOAD_LONG_EXTERNAL,
    MEMORY_STORE_BYTE_EXTERNAL,
    MEMORY_STORE_WORD_EXTERNAL,
    MEMORY_STORE_LONG_EXTERNAL,

    READ_DST_NO_SCOREBOARD,
    READ_DST_LOCK_FLAGS,
    READ_LOCK_DST,
    READ_LOCK_DST_LOCK_FLAGS,
    READ_SRC,
    READ_SRC_READ_DST,
    READ_SRC_READ_LOCK_DST,
    READ_SRC_READ_LOCK_DST_LOCK_FLAGS,
    READ_SRC_READ_LOCK_DST_READ_LOCK_FLAGS,
    READ_PC,

    WRITE_DST,
    WRITE_DST_WRITE_FLAGS,
    UNLOCK_WRITE_DST,
    UNLOCK_WRITE_DST_REMAIN,
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

      struct GetCodeLoAwaiter : BaseAwaiter
      {
        GetCodeLo::Res res;
        auto await_resume() { return res; }
      } getCodeLoAwaiter;

      struct GetCodeHiAwaiter : BaseAwaiter
      {
        GetCodeHi::Res res;
        auto await_resume() { return res; }
      } getCodeHiAwaiter;

      struct GetModAwaiter : BaseAwaiter
      {
        GetMod::Res res;
        auto await_resume() { return res; }
      } getModAwaiter;

      struct ComputeAwaiter : BaseAwaiter
      {
      } computeAwaiter;

      struct DivideInitAwaiter : BaseAwaiter
      {
        DivideInit::Res res;
        auto await_resume() { return res; }
      } divideInitAwaiter;

      struct DivideCycleAwaiter : BaseAwaiter
      {
        DivideCycle arg;
      } divideCycleAwaiter;

      struct NOPAwaiter : BaseAwaiter
      {
      } nopAwaiter;

      struct IOAwaiter : BaseAwaiter
      {
      } ioAwaiter;

      struct MoveQAwaiter : BaseAwaiter
      {
      } moveQAwaiter;


      struct MemoryLoadByteAwaiter : BaseAwaiter
      {
        MemoryLoadByte arg;
        MemoryLoadByte::Res res;
        auto await_resume() { return res; }
      } memoryLoadByteAwaiter;

      struct MemoryLoadWordAwaiter : BaseAwaiter
      {
        MemoryLoadWord arg;
        MemoryLoadWord::Res res;
        auto await_resume() { return res; }
      } memoryLoadWordAwaiter;

      struct MemoryLoadLongAwaiter : BaseAwaiter
      {
        MemoryLoadLong arg;
        MemoryLoadLong::Res res;
        auto await_resume() { return res; }
      } memoryLoadLongAwaiter;

      struct MemoryStoreByteAwaiter : BaseAwaiter
      {
        MemoryStoreByte arg;
        MemoryStoreByte::Res res;
        auto await_resume() { return res; }
      } memoryStoreByteAwaiter;

      struct MemoryStoreWordAwaiter : BaseAwaiter
      {
        MemoryStoreWord arg;
        MemoryStoreWord::Res res;
        auto await_resume() { return res; }
      } memoryStoreWordAwaiter;

      struct MemoryStoreLongAwaiter : BaseAwaiter
      {
        MemoryStoreLong arg;
        MemoryStoreLong::Res res;
        auto await_resume() { return res; }
      } memoryStoreLongAwaiter;

      struct MemoryLoadByteExternalAwaiter : BaseAwaiter
      {
        MemoryLoadByteExternal arg;
        MemoryLoadByteExternal::Res res;
        auto await_resume() { return res; }
      } memoryLoadByteExternalAwaiter;

      struct MemoryLoadWordExternalAwaiter : BaseAwaiter
      {
        MemoryLoadWordExternal arg;
        MemoryLoadWordExternal::Res res;
        auto await_resume() { return res; }
      } memoryLoadWordExternalAwaiter;

      struct MemoryLoadLongExternalAwaiter : BaseAwaiter
      {
        MemoryLoadLongExternal arg;
        MemoryLoadLongExternal::Res res;
        auto await_resume() { return res; }
      } memoryLoadLongExternalAwaiter;

      struct MemoryStoreByteExternalAwaiter : BaseAwaiter
      {
        MemoryStoreByteExternal arg;
        Decode::Res res;
      } memoryStoreByteExternalAwaiter;

      struct MemoryStoreWordExternalAwaiter : BaseAwaiter
      {
        MemoryStoreWordExternal arg;
        Decode::Res res;
      } memoryStoreWordExternalAwaiter;

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

      struct ReadLockDstAwaiter : BaseAwaiter
      {
        ReadLockDst arg;
        ReadLockDst::Res res;
        auto await_resume() { return res; }
      } readLockDstAwaiter;

      struct ReadLockDstLockFlagsAwaiter : BaseAwaiter
      {
        ReadLockDstLockFlags arg;
        ReadLockDstLockFlags::Res res;
        auto await_resume() { return res; }
      } readLockDstLockFlagsAwaiter;

      struct ReadSrcAwaiter : BaseAwaiter
      {
        ReadSrc arg;
        ReadSrc::Res res;
        auto await_resume() { return res; }
      } readSrcAwaiter;

      struct ReadDstNoScoreboardAwaiter : BaseAwaiter
      {
        ReadDstNoScoreboard arg;
        ReadDstNoScoreboard::Res res;
        auto await_resume() { return res; }
      } readDstNoScoreboardAwaiter;

      struct ReadSrcReadDstAwaiter : BaseAwaiter
      {
        ReadSrcReadDst arg;
        ReadSrcReadDst::Res res;
        auto await_resume() { return res; }
      } readSrcReadDstAwaiter;

      struct ReadSrcReadLockDstLockFlagsAwaiter : BaseAwaiter
      {
        ReadSrcReadLockDstLockFlags arg;
        ReadSrcReadLockDstLockFlags::Res res;
        auto await_resume() { return res; }
      } readSrcReadLockDstLockFlagsAwaiter;

      struct ReadSrcReadLockDstAwaiter : BaseAwaiter
      {
        ReadSrcReadLockDst arg;
        ReadSrcReadLockDst::Res res;
        auto await_resume() { return res; }
      } readSrcReadLockDstAwaiter;

      struct ReadSrcReadLockDstReadLockFlagsAwaiter : BaseAwaiter
      {
        ReadSrcReadLockDstReadLockFlags arg;
        ReadSrcReadLockDstReadLockFlags::Res res;
        auto await_resume() { return res; }
      } readSrcReadLockDstReadLockFlagsAwaiter;

      struct ReadPCAwaiter : BaseAwaiter
      {
        ReadPC::Res res;
        auto await_resume() { return res; }
      } readPCAwaiter;


      struct WriteDstAwaiter : BaseAwaiter
      {
        WriteDst arg;
      } writeDstAwaiter;

      struct WriteDstWriteFlagsAwaiter : BaseAwaiter
      {
        WriteDstWriteFlags arg;
      } writeDstWriteFlagsAwaiter;

      struct UnlockWriteDstAwaiter : BaseAwaiter
      {
        UnlockWriteDst arg;
      } unlockWriteDstAwaiter;

      struct UnlockWriteDstRemainAwaiter : BaseAwaiter
      {
        UnlockWriteDstRemain arg;
      } unlockWriteDstRemainAwaiter;

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

    Awaiter::GetCodeLoAwaiter& await_transform( GetCodeLo arg )
    {
      type = AwaiterType::GET_CODE_LO;
      return awaiter.getCodeLoAwaiter;
    }

    void getCodeLo( GetCodeLo::Res code )
    {
      assert( type == AwaiterType::GET_CODE_LO );
      awaiter.getCodeLoAwaiter.res = code;
    }

    Awaiter::GetCodeHiAwaiter& await_transform( GetCodeHi arg )
    {
      type = AwaiterType::GET_CODE_HI;
      return awaiter.getCodeHiAwaiter;
    }

    void getCodeHi( GetCodeHi::Res code )
    {
      assert( type == AwaiterType::GET_CODE_HI );
      awaiter.getCodeHiAwaiter.res = code;
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

    Awaiter::DivideInitAwaiter& await_transform( DivideInit )
    {
      type = AwaiterType::DIVIDE_INIT;
      return awaiter.divideInitAwaiter;
    }

    void divideInit( DivideInit::Res offset )
    {
      assert( type == AwaiterType::DIVIDE_INIT );
      awaiter.divideInitAwaiter.res = offset;
    }

    Awaiter::DivideCycleAwaiter& await_transform( DivideCycle cycle )
    {
      type = AwaiterType::DIVIDE_CYCLE;
      awaiter.divideCycleAwaiter.arg = cycle;
      return awaiter.divideCycleAwaiter;
    }

    DivideCycle divideCycle()
    {
      assert( type == AwaiterType::DIVIDE_CYCLE );
      return awaiter.divideCycleAwaiter.arg;
    }

    Awaiter::NOPAwaiter& await_transform( NOP )
    {
      type = AwaiterType::NOP;
      return awaiter.nopAwaiter;
    }

    Awaiter::IOAwaiter& await_transform( IO )
    {
      type = AwaiterType::IO;
      return awaiter.ioAwaiter;
    }

    Awaiter::MoveQAwaiter& await_transform( MoveQ )
    {
      type = AwaiterType::MOVE_Q;
      return awaiter.moveQAwaiter;
    }

    Awaiter::MemoryLoadByteAwaiter& await_transform( MemoryLoadByte const& arg )
    {
      type = AwaiterType::MEMORY_LOAD_BYTE;
      awaiter.memoryLoadByteAwaiter.arg = arg;
      return awaiter.memoryLoadByteAwaiter;
    }

    Awaiter::MemoryLoadWordAwaiter& await_transform( MemoryLoadWord const& arg )
    {
      type = AwaiterType::MEMORY_LOAD_WORD;
      awaiter.memoryLoadWordAwaiter.arg = arg;
      return awaiter.memoryLoadWordAwaiter;
    }

    Awaiter::MemoryLoadLongAwaiter& await_transform( MemoryLoadLong const& arg )
    {
      type = AwaiterType::MEMORY_LOAD_LONG;
      awaiter.memoryLoadLongAwaiter.arg = arg;
      return awaiter.memoryLoadLongAwaiter;
    }

    MemoryLoadByte memoryLoadByte() const
    {
      assert( type == AwaiterType::MEMORY_LOAD_BYTE );
      return awaiter.memoryLoadByteAwaiter.arg;
    }

    MemoryLoadWord memoryLoadWord() const
    {
      assert( type == AwaiterType::MEMORY_LOAD_WORD );
      return awaiter.memoryLoadWordAwaiter.arg;
    }

    MemoryLoadLong memoryLoadLong() const
    {
      assert( type == AwaiterType::MEMORY_LOAD_LONG );
      return awaiter.memoryLoadLongAwaiter.arg;
    }

    void memoryLoadByte( MemoryLoadByte::Res res )
    {
      assert( type == AwaiterType::MEMORY_LOAD_BYTE );
      awaiter.memoryLoadByteAwaiter.res = res;
    }

    void memoryLoadWord( MemoryLoadWord::Res res )
    {
      assert( type == AwaiterType::MEMORY_LOAD_WORD );
      awaiter.memoryLoadWordAwaiter.res = res;
    }

    void memoryLoadLong( MemoryLoadLong::Res res )
    {
      assert( type == AwaiterType::MEMORY_LOAD_LONG );
      awaiter.memoryLoadLongAwaiter.res = res;
    }

    Awaiter::MemoryStoreByteAwaiter& await_transform( MemoryStoreByte const& arg )
    {
      type = AwaiterType::MEMORY_STORE_BYTE;
      awaiter.memoryStoreByteAwaiter.arg = arg;
      return awaiter.memoryStoreByteAwaiter;
    }

    Awaiter::MemoryStoreWordAwaiter& await_transform( MemoryStoreWord const& arg )
    {
      type = AwaiterType::MEMORY_STORE_WORD;
      awaiter.memoryStoreWordAwaiter.arg = arg;
      return awaiter.memoryStoreWordAwaiter;
    }

    Awaiter::MemoryStoreLongAwaiter& await_transform( MemoryStoreLong const& arg )
    {
      type = AwaiterType::MEMORY_LOAD_LONG;
      awaiter.memoryStoreLongAwaiter.arg = arg;
      return awaiter.memoryStoreLongAwaiter;
    }

    MemoryStoreByte memoryStoreByte() const
    {
      assert( type == AwaiterType::MEMORY_STORE_BYTE );
      return awaiter.memoryStoreByteAwaiter.arg;
    }

    MemoryStoreWord memoryStoreWord() const
    {
      assert( type == AwaiterType::MEMORY_STORE_WORD );
      return awaiter.memoryStoreWordAwaiter.arg;
    }

    MemoryStoreLong memoryStoreLong() const
    {
      assert( type == AwaiterType::MEMORY_STORE_LONG );
      return awaiter.memoryStoreLongAwaiter.arg;
    }

    void memoryStoreByte( MemoryStoreByte::Res res )
    {
      assert( type == AwaiterType::MEMORY_STORE_BYTE );
      awaiter.memoryStoreByteAwaiter.res = res;
    }

    void memoryStoreWord( MemoryStoreWord::Res res )
    {
      assert( type == AwaiterType::MEMORY_STORE_WORD );
      awaiter.memoryStoreWordAwaiter.res = res;
    }

    void memoryStoreLong( MemoryStoreLong::Res res )
    {
      assert( type == AwaiterType::MEMORY_STORE_LONG );
      awaiter.memoryStoreLongAwaiter.res = res;
    }

    Awaiter::MemoryLoadByteExternalAwaiter& await_transform( MemoryLoadByteExternal const& arg )
    {
      type = AwaiterType::MEMORY_LOAD_BYTE_EXTERNAL;
      awaiter.memoryLoadByteExternalAwaiter.arg = arg;
      return awaiter.memoryLoadByteExternalAwaiter;
    }

    Awaiter::MemoryLoadWordExternalAwaiter& await_transform( MemoryLoadWordExternal const& arg )
    {
      type = AwaiterType::MEMORY_LOAD_WORD_EXTERNAL;
      awaiter.memoryLoadWordExternalAwaiter.arg = arg;
      return awaiter.memoryLoadWordExternalAwaiter;
    }

    Awaiter::MemoryLoadLongExternalAwaiter& await_transform( MemoryLoadLongExternal const& arg )
    {
      type = AwaiterType::MEMORY_LOAD_LONG_EXTERNAL;
      awaiter.memoryLoadLongExternalAwaiter.arg = arg;
      return awaiter.memoryLoadLongExternalAwaiter;
    }

    MemoryLoadByteExternal memoryLoadByteExternal() const
    {
      assert( type == AwaiterType::MEMORY_LOAD_BYTE_EXTERNAL );
      return awaiter.memoryLoadByteExternalAwaiter.arg;
    }

    MemoryLoadWordExternal memoryLoadWordExternal() const
    {
      assert( type == AwaiterType::MEMORY_LOAD_WORD_EXTERNAL );
      return awaiter.memoryLoadWordExternalAwaiter.arg;
    }

    MemoryLoadLongExternal memoryLoadLongExternal() const
    {
      assert( type == AwaiterType::MEMORY_LOAD_LONG_EXTERNAL );
      return awaiter.memoryLoadLongExternalAwaiter.arg;
    }

    void memoryLoadByteExternal( MemoryLoadByteExternal::Res res )
    {
      assert( type == AwaiterType::MEMORY_LOAD_BYTE_EXTERNAL );
      awaiter.memoryLoadByteExternalAwaiter.res = res;
    }

    void memoryLoadWordExternal( MemoryLoadWordExternal::Res res )
    {
      assert( type == AwaiterType::MEMORY_LOAD_WORD_EXTERNAL );
      awaiter.memoryLoadWordExternalAwaiter.res = res;
    }

    void memoryLoadLongExternal( MemoryLoadLongExternal::Res res )
    {
      assert( type == AwaiterType::MEMORY_LOAD_LONG_EXTERNAL );
      awaiter.memoryLoadLongExternalAwaiter.res = res;
    }

    Awaiter::MemoryStoreByteExternalAwaiter& await_transform( MemoryStoreByteExternal const& arg )
    {
      type = AwaiterType::MEMORY_STORE_BYTE_EXTERNAL;
      awaiter.memoryStoreByteExternalAwaiter.arg = arg;
      return awaiter.memoryStoreByteExternalAwaiter;
    }

    Awaiter::MemoryStoreWordExternalAwaiter& await_transform( MemoryStoreWordExternal const& arg )
    {
      type = AwaiterType::MEMORY_STORE_WORD_EXTERNAL;
      awaiter.memoryStoreWordExternalAwaiter.arg = arg;
      return awaiter.memoryStoreWordExternalAwaiter;
    }

    Awaiter::MemoryStoreLongExternalAwaiter& await_transform( MemoryStoreLongExternal const& arg )
    {
      type = AwaiterType::MEMORY_STORE_LONG_EXTERNAL;
      awaiter.memoryStoreLongExternalAwaiter.arg = arg;
      return awaiter.memoryStoreLongExternalAwaiter;
    }

    MemoryStoreByteExternal memoryStoreByteExternal() const
    {
      assert( type == AwaiterType::MEMORY_STORE_BYTE_EXTERNAL );
      return awaiter.memoryStoreByteExternalAwaiter.arg;
    }

    MemoryStoreWordExternal memoryStoreWordExternal() const
    {
      assert( type == AwaiterType::MEMORY_STORE_WORD_EXTERNAL );
      return awaiter.memoryStoreWordExternalAwaiter.arg;
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

    Awaiter::ReadLockDstAwaiter& await_transform( ReadLockDst const& arg )
    {
      type = AwaiterType::READ_LOCK_DST;
      awaiter.readLockDstAwaiter.arg = arg;
      return awaiter.readLockDstAwaiter;
    }

    ReadLockDst readLockDst() const
    {
      assert( type == AwaiterType::READ_LOCK_DST );
      return awaiter.readLockDstAwaiter.arg;
    }

    void readLockDst( ReadLockDst::Res res )
    {
      assert( type == AwaiterType::READ_LOCK_DST );
      awaiter.readLockDstAwaiter.res = res;
    }

    Awaiter::ReadLockDstLockFlagsAwaiter& await_transform( ReadLockDstLockFlags const& arg )
    {
      type = AwaiterType::READ_LOCK_DST_LOCK_FLAGS;
      awaiter.readLockDstLockFlagsAwaiter.arg = arg;
      return awaiter.readLockDstLockFlagsAwaiter;
    }

    ReadLockDstLockFlags readLockDstLockFlags() const
    {
      assert( type == AwaiterType::READ_LOCK_DST_LOCK_FLAGS );
      return awaiter.readLockDstLockFlagsAwaiter.arg;
    }

    void readLockDstLockFlags( ReadLockDstLockFlags::Res res )
    {
      assert( type == AwaiterType::READ_LOCK_DST_LOCK_FLAGS );
      awaiter.readLockDstLockFlagsAwaiter.res = res;
    }

    Awaiter::ReadDstNoScoreboardAwaiter& await_transform( ReadDstNoScoreboard const& arg )
    {
      type = AwaiterType::READ_DST_NO_SCOREBOARD;
      awaiter.readDstNoScoreboardAwaiter.arg = arg;
      return awaiter.readDstNoScoreboardAwaiter;
    }

    ReadDstNoScoreboard readDstNoScoreboard() const
    {
      assert( type == AwaiterType::READ_DST_NO_SCOREBOARD );
      return awaiter.readDstNoScoreboardAwaiter.arg;
    }

    void readDstNoScoreboard( ReadDstNoScoreboard::Res res )
    {
      assert( type == AwaiterType::READ_DST_NO_SCOREBOARD );
      awaiter.readDstNoScoreboardAwaiter.res = res;
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

    Awaiter::ReadSrcReadLockDstAwaiter& await_transform( ReadSrcReadLockDst const& arg )
    {
      type = AwaiterType::READ_SRC_READ_LOCK_DST;
      awaiter.readSrcReadLockDstAwaiter.arg = arg;
      return awaiter.readSrcReadLockDstAwaiter;
    }

    ReadSrcReadLockDst readSrcReadLockDst() const
    {
      assert( type == AwaiterType::READ_SRC_READ_LOCK_DST );
      return awaiter.readSrcReadLockDstAwaiter.arg;
    }

    void readSrcReadLockDst( ReadSrcReadLockDst::Res res )
    {
      assert( type == AwaiterType::READ_SRC_READ_LOCK_DST );
      awaiter.readSrcReadLockDstAwaiter.res = res;
    }

    Awaiter::ReadSrcReadLockDstLockFlagsAwaiter& await_transform( ReadSrcReadLockDstLockFlags const& arg )
    {
      type = AwaiterType::READ_SRC_READ_LOCK_DST_LOCK_FLAGS;
      awaiter.readSrcReadLockDstLockFlagsAwaiter.arg = arg;
      return awaiter.readSrcReadLockDstLockFlagsAwaiter;
    }

    ReadSrcReadLockDstLockFlags readSrcReadLockDstLockFlags() const
    {
      assert( type == AwaiterType::READ_SRC_READ_LOCK_DST_LOCK_FLAGS );
      return awaiter.readSrcReadLockDstLockFlagsAwaiter.arg;
    }

    void readSrcReadLockDstLockFlags( ReadSrcReadLockDstLockFlags::Res res )
    {
      assert( type == AwaiterType::READ_SRC_READ_LOCK_DST_LOCK_FLAGS );
      awaiter.readSrcReadLockDstLockFlagsAwaiter.res = res;
    }

    Awaiter::ReadSrcReadLockDstReadLockFlagsAwaiter& await_transform( ReadSrcReadLockDstReadLockFlags const& arg )
    {
      type = AwaiterType::READ_SRC_READ_LOCK_DST_READ_LOCK_FLAGS;
      awaiter.readSrcReadLockDstReadLockFlagsAwaiter.arg = arg;
      return awaiter.readSrcReadLockDstReadLockFlagsAwaiter;
    }

    ReadSrcReadLockDstReadLockFlags readSrcReadLockDstReadLockFlags() const
    {
      assert( type == AwaiterType::READ_SRC_READ_LOCK_DST_READ_LOCK_FLAGS );
      return awaiter.readSrcReadLockDstReadLockFlagsAwaiter.arg;
    }

    void readSrcReadLockDstReadLockFlags( ReadSrcReadLockDstReadLockFlags::Res res )
    {
      assert( type == AwaiterType::READ_SRC_READ_LOCK_DST_READ_LOCK_FLAGS );
      awaiter.readSrcReadLockDstReadLockFlagsAwaiter.res = res;
    }

    Awaiter::ReadPCAwaiter& await_transform( ReadPC )
    {
      type = AwaiterType::READ_PC;
      return awaiter.readPCAwaiter;
    }

    void readPC( ReadPC::Res res )
    {
      assert( type == AwaiterType::READ_PC );
      awaiter.readPCAwaiter.res = res;
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

    Awaiter::WriteDstWriteFlagsAwaiter& await_transform( WriteDstWriteFlags const& arg )
    {
      type = AwaiterType::WRITE_DST_WRITE_FLAGS;
      awaiter.writeDstWriteFlagsAwaiter.arg = arg;
      return awaiter.writeDstWriteFlagsAwaiter;
    }

    WriteDstWriteFlags writeDstUnlockWriteFlags() const
    {
      assert( type == AwaiterType::WRITE_DST_WRITE_FLAGS );
      return awaiter.writeDstWriteFlagsAwaiter.arg;
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

    Awaiter::UnlockWriteDstRemainAwaiter& await_transform( UnlockWriteDstRemain const& arg )
    {
      type = AwaiterType::UNLOCK_WRITE_DST_REMAIN;
      awaiter.unlockWriteDstRemainAwaiter.arg = arg;
      return awaiter.unlockWriteDstRemainAwaiter;
    }

    UnlockWriteDstRemain unlockWriteDstRemain() const
    {
      assert( type == AwaiterType::UNLOCK_WRITE_DST_REMAIN );
      return awaiter.unlockWriteDstRemainAwaiter.arg;
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

  void getCodeLo( uint16_t code )
  {
    mCoro.promise().getCodeLo( { code } );
    mCoro();
  }

  void getCodeHi( uint16_t code )
  {
    mCoro.promise().getCodeHi( { code } );
    mCoro();
  }

  void getMod( uint32_t mod )
  {
    mCoro.promise().getMod( { mod } );
    mCoro();
  }

  void divideInit( bool offset )
  {
    mCoro.promise().divideInit( { offset } );
    mCoro();
  }

  DivideCycle divideCycle() const
  {
    return mCoro.promise().divideCycle();
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

  ReadLockDst readLockDst() const
  {
    return mCoro.promise().readLockDst();
  }

  void readLockDst( ReadLockDst::Res res )
  {
    mCoro.promise().readLockDst( res );
    mCoro();
  }

  ReadLockDstLockFlags readLockDstLockFlags() const
  {
    return mCoro.promise().readLockDstLockFlags();
  }

  void readLockDstLockFlags( ReadLockDstLockFlags::Res res )
  {
    mCoro.promise().readLockDstLockFlags( res );
    mCoro();
  }

  ReadDstNoScoreboard readDstNoScoreboard() const
  {
    return mCoro.promise().readDstNoScoreboard();
  }

  void readDstNoScoreboard( ReadDstNoScoreboard::Res res )
  {
    mCoro.promise().readDstNoScoreboard( res );
    mCoro();
  }

  ReadSrc readSrc() const
  {
    return mCoro.promise().readSrc();
  }

  void readSrc( ReadSrc::Res res )
  {
    mCoro.promise().readSrc( res );
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

  ReadSrcReadLockDst readSrcReadLockDst() const
  {
    return mCoro.promise().readSrcReadLockDst();
  }

  void readSrcReadLockDst( ReadSrcReadLockDst::Res res )
  {
    mCoro.promise().readSrcReadLockDst( res );
    mCoro();
  }

  ReadSrcReadLockDstLockFlags readSrcReadLockDstLockFlags() const
  {
    return mCoro.promise().readSrcReadLockDstLockFlags();
  }

  void readSrcReadLockDstLockFlags( ReadSrcReadLockDstLockFlags::Res res )
  {
    mCoro.promise().readSrcReadLockDstLockFlags( res );
    mCoro();
  }

  ReadSrcReadLockDstReadLockFlags readSrcReadLockDstReadLockFlags() const
  {
    return mCoro.promise().readSrcReadLockDstReadLockFlags();
  }

  void readSrcReadLockDstReadLockFlags( ReadSrcReadLockDstReadLockFlags::Res res )
  {
    mCoro.promise().readSrcReadLockDstReadLockFlags( res );
    mCoro();
  }

  void readPC( uint32_t pc )
  {
    mCoro.promise().readPC( { pc } );
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

  UnlockWriteDstRemain unlockWriteDstRemain() const
  {
    return mCoro.promise().unlockWriteDstRemain();
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

