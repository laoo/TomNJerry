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
  //decode unit
  struct Decode { struct Res { RISCOpcode opcode; }; }; //->read unit

  //read unit
  struct ReadReg { int8_t reg; struct Res { uint32_t value; }; }; //->compute unit
  struct ReadRegMove { int8_t reg; struct Res { uint32_t value; }; }; //->write unit
  struct ReadRegReadFlags { int8_t reg; struct Res { uint32_t value; bool n; bool z; bool c; }; }; //->compute unit
  struct ReadRegLockFlags { int8_t reg; struct Res { uint32_t value; }; }; //->compute unit
  struct ReadLockReg { int8_t reg; struct Res { uint32_t value; }; }; //->compute unit
  struct ReadLockRegLockFlags { int8_t reg; struct Res { uint32_t value; }; };  //->compute unit
  struct ReadSrcReadDst { int8_t src; int8_t dst; struct Res { uint32_t src; uint32_t dst; }; }; //->compute unit
  struct ReadSrcReadLockDst { int8_t src; int8_t dst; struct Res { uint32_t src; uint32_t dst; }; }; //->compute unit
  struct ReadSrcReadDstLockFlags { int8_t src; int8_t dst; struct Res { uint32_t src; uint32_t dst; }; }; //->compute unit
  struct ReadSrcReadLockDstLockFlags { int8_t src; int8_t dst; struct Res { uint32_t src; uint32_t dst; }; }; //->compute unit
  struct ReadSrcReadLockDstReadLockFlags { int8_t src; int8_t dst; struct Res { uint32_t src; uint32_t dst; uint16_t c; }; }; //->compute unit
  struct GetCodeLo { struct Res { uint16_t code; }; }; //->read unit
  struct GetCodeHi { struct Res { uint16_t code; }; }; //->write unit
  struct ReadPC { struct Res { uint32_t pc; }; }; //->write unit
  struct GetAcc { struct Res { int8_t acc; }; }; //->write unit
  struct MoveQ {}; //->write unit
  struct NOP {}; //->decode unit
  struct JRInit {}; //->compute unit

  //compute unit
  struct ComputePending {};  //->compute unit
  struct Compute {};  //->write unit
  struct GetMod { struct Res { uint32_t mod; }; }; //->write unit
  struct PrepareJump { struct Res { uint32_t pc; }; }; //->write unit
  struct DivideInit { struct Res { bool offset; }; }; //->divide unit
  struct ReadRegNoScoreboard { int8_t reg; struct Res { uint32_t value; }; }; //->compute unit
  struct MemoryLoadByte { uint32_t src; struct Res { uint32_t value; bool success; }; }; //->memory unit, write unit
  struct MemoryLoadWord { uint32_t src; struct Res { uint32_t value; bool success; }; }; //->memory unit, write unit
  struct MemoryLoadLong { uint32_t src; struct Res { uint32_t value; bool success; }; }; //->memory unit, write unit
  struct MemoryStoreByte { uint32_t addr; uint8_t value; struct Res { bool success; }; }; //->decode unit, memory unit
  struct MemoryStoreWord { uint32_t addr; uint16_t value; struct Res { bool success; }; }; //->decode unit, memory unit
  struct MemoryStoreLong { uint32_t addr; uint32_t value; struct Res { bool success; }; }; //->decode unit, memory unit

  //memory unit
  struct MemoryLoadByteExternal { uint32_t src; struct Res { uint32_t value; }; }; //->write unit
  struct MemoryLoadWordExternal { uint32_t src; struct Res { uint32_t value; }; }; //->write unit
  struct MemoryLoadLongExternal { uint32_t src; struct Res { uint32_t value; }; }; //->write unit
  struct MemoryStoreByteExternal { uint32_t addr; uint8_t value; }; //->decode unit
  struct MemoryStoreWordExternal { uint32_t addr; uint16_t value; }; //->decode unit
  struct MemoryStoreLongExternal { uint32_t addr; uint32_t value; }; //->decode unit

  //write unit
  struct WritePC { uint32_t pc; }; //->decode unit
  struct WriteDst { uint32_t value; int8_t reg; }; //->decode unit
  struct WriteDstWriteFlags { uint32_t valuenz; uint16_t c; int8_t reg; }; //->decode unit
  struct UnlockWriteFlags { uint32_t nz;  uint16_t c; }; //->decode unit
  struct UnlockWriteDst { uint32_t value; int8_t reg; }; //->decode unit
  struct UnlockWriteDstUnlockWriteFlags { uint32_t valuenz; uint16_t c; int8_t reg; }; //->decode unit
  struct UnlockWriteDstRemain { uint32_t value; uint32_t remain; int8_t reg; }; //->decode unit

  //divide unit
  struct DivideCycle { int32_t cycle; }; //-> divide unit, write unit

  enum struct AwaiterType
  {
    DECODE,

    READ_REG,
    READ_REG_MOVE,
    READ_REG_READ_FLAGS,
    READ_REG_LOCK_FLAGS,
    READ_LOCK_REG,
    READ_LOCK_REG_LOCK_FLAGS,
    READ_SRC_READ_DST,
    READ_SRC_READ_LOCK_DST,
    READ_SRC_READ_DST_LOCK_FLAGS,
    READ_SRC_READ_LOCK_DST_LOCK_FLAGS,
    READ_SRC_READ_LOCK_DST_READ_LOCK_FLAGS,
    GET_CODE_LO,
    GET_CODE_HI,
    READ_PC,
    GET_ACC,
    MOVE_Q,
    NOP,
    JR_INIT,

    COMPUTE_PENDING,
    COMPUTE,
    GET_MOD,
    PREPARE_JUMP,
    DIVIDE_INIT,
    READ_REG_NO_SCOREBOARD,
    MEMORY_LOAD_BYTE,
    MEMORY_LOAD_WORD,
    MEMORY_LOAD_LONG,
    MEMORY_STORE_BYTE,
    MEMORY_STORE_WORD,
    MEMORY_STORE_LONG,

    WRITE_PC,
    WRITE_DST,
    WRITE_DST_WRITE_FLAGS,
    UNLOCK_WRITE_FLAGS,
    UNLOCK_WRITE_DST,
    UNLOCK_WRITE_DST_UNLOCK_WRITE_FLAGS,
    UNLOCK_WRITE_DST_REMAIN,

    DIVIDE_CYCLE,

    MEMORY_LOAD_BYTE_EXTERNAL,
    MEMORY_LOAD_WORD_EXTERNAL,
    MEMORY_LOAD_LONG_EXTERNAL,
    MEMORY_STORE_BYTE_EXTERNAL,
    MEMORY_STORE_WORD_EXTERNAL,
    MEMORY_STORE_LONG_EXTERNAL
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

      struct GetAccAwaiter : BaseAwaiter
      {
        GetAcc::Res res;
        auto await_resume() { return res; }
      } getAccAwaiter;

      struct ComputeAwaiter : BaseAwaiter
      {
      } computeAwaiter;

      struct ComputePendingAwaiter : BaseAwaiter
      {
      } computePendingAwaiter;

      struct PrepareJumpAwaiter : BaseAwaiter
      {
        PrepareJump::Res res;
        auto await_resume() { return res; }
      } prepareJumpAwaiter;

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

      struct MoveQAwaiter : BaseAwaiter
      {
      } moveQAwaiter;

      struct JRInitAwaiter : BaseAwaiter
      {
      } jrInitAwaiter;

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

      struct ReadRegLockFlagsAwaiter : BaseAwaiter
      {
        ReadRegLockFlags arg;
        ReadRegLockFlags::Res res;
        auto await_resume() { return res; }
      } readRegLockFlagsAwaiter;

      struct ReadLockRegAwaiter : BaseAwaiter
      {
        ReadLockReg arg;
        ReadLockReg::Res res;
        auto await_resume() { return res; }
      } readLockRegAwaiter;

      struct ReadLockRegLockFlagsAwaiter : BaseAwaiter
      {
        ReadLockRegLockFlags arg;
        ReadLockRegLockFlags::Res res;
        auto await_resume() { return res; }
      } readLockRegLockFlagsAwaiter;

      struct ReadRegAwaiter : BaseAwaiter
      {
        ReadReg arg;
        ReadReg::Res res;
        auto await_resume() { return res; }
      } readRegAwaiter;

      struct ReadRegMoveAwaiter : BaseAwaiter
      {
        ReadRegMove arg;
        ReadRegMove::Res res;
        auto await_resume() { return res; }
      } readRegMoveAwaiter;

      struct ReadRegReadFlagsAwaiter : BaseAwaiter
      {
        ReadRegReadFlags arg;
        ReadRegReadFlags::Res res;
        auto await_resume() { return res; }
      } readRegReadFlagsAwaiter;

      struct ReadRegNoScoreboardAwaiter : BaseAwaiter
      {
        ReadRegNoScoreboard arg;
        ReadRegNoScoreboard::Res res;
        auto await_resume() { return res; }
      } readRegNoScoreboardAwaiter;

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

      struct ReadSrcReadDstLockFlagsAwaiter : BaseAwaiter
      {
        ReadSrcReadDstLockFlags arg;
        ReadSrcReadDstLockFlags::Res res;
        auto await_resume() { return res; }
      } readSrcReadDstLockFlagsAwaiter;

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

      struct WritePCAwaiter : BaseAwaiter
      {
        WritePC arg;
      } writePCAwaiter;

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

    Awaiter::GetAccAwaiter& await_transform( GetAcc )
    {
      type = AwaiterType::GET_ACC;
      return awaiter.getAccAwaiter;
    }

    void getAcc( GetAcc::Res acc )
    {
      assert( type == AwaiterType::GET_ACC );
      awaiter.getAccAwaiter.res = acc;
    }

    Awaiter::ComputeAwaiter& await_transform( Compute )
    {
      type = AwaiterType::COMPUTE;
      return awaiter.computeAwaiter;
    }

    Awaiter::ComputePendingAwaiter& await_transform( ComputePending )
    {
      type = AwaiterType::COMPUTE_PENDING;
      return awaiter.computePendingAwaiter;
    }

    Awaiter::PrepareJumpAwaiter& await_transform( PrepareJump )
    {
      type = AwaiterType::PREPARE_JUMP;
      return awaiter.prepareJumpAwaiter;
    }

    void prepareJump( PrepareJump::Res pc )
    {
      assert( type == AwaiterType::PREPARE_JUMP );
      awaiter.prepareJumpAwaiter.res = pc;
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

    Awaiter::MoveQAwaiter& await_transform( MoveQ )
    {
      type = AwaiterType::MOVE_Q;
      return awaiter.moveQAwaiter;
    }

    Awaiter::JRInitAwaiter& await_transform( JRInit )
    {
      type = AwaiterType::JR_INIT;
      return awaiter.jrInitAwaiter;
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

    Awaiter::ReadRegLockFlagsAwaiter& await_transform( ReadRegLockFlags const& arg )
    {
      type = AwaiterType::READ_REG_LOCK_FLAGS;
      awaiter.readRegLockFlagsAwaiter.arg = arg;
      return awaiter.readRegLockFlagsAwaiter;
    }

    ReadRegLockFlags readRegLockFlags() const
    {
      assert( type == AwaiterType::READ_REG_LOCK_FLAGS );
      return awaiter.readRegLockFlagsAwaiter.arg;
    }

    void readRegLockFlags( ReadRegLockFlags::Res res )
    {
      assert( type == AwaiterType::READ_REG_LOCK_FLAGS );
      awaiter.readRegLockFlagsAwaiter.res = res;
    }

    Awaiter::ReadLockRegAwaiter& await_transform( ReadLockReg const& arg )
    {
      type = AwaiterType::READ_LOCK_REG;
      awaiter.readLockRegAwaiter.arg = arg;
      return awaiter.readLockRegAwaiter;
    }

    ReadLockReg readLockReg() const
    {
      assert( type == AwaiterType::READ_LOCK_REG );
      return awaiter.readLockRegAwaiter.arg;
    }

    void readLockReg( ReadLockReg::Res res )
    {
      assert( type == AwaiterType::READ_LOCK_REG );
      awaiter.readLockRegAwaiter.res = res;
    }

    Awaiter::ReadLockRegLockFlagsAwaiter& await_transform( ReadLockRegLockFlags const& arg )
    {
      type = AwaiterType::READ_LOCK_REG_LOCK_FLAGS;
      awaiter.readLockRegLockFlagsAwaiter.arg = arg;
      return awaiter.readLockRegLockFlagsAwaiter;
    }

    ReadLockRegLockFlags readLockRegLockFlags() const
    {
      assert( type == AwaiterType::READ_LOCK_REG_LOCK_FLAGS );
      return awaiter.readLockRegLockFlagsAwaiter.arg;
    }

    void readLockRegLockFlags( ReadLockRegLockFlags::Res res )
    {
      assert( type == AwaiterType::READ_LOCK_REG_LOCK_FLAGS );
      awaiter.readLockRegLockFlagsAwaiter.res = res;
    }

    Awaiter::ReadRegNoScoreboardAwaiter& await_transform( ReadRegNoScoreboard const& arg )
    {
      type = AwaiterType::READ_REG_NO_SCOREBOARD;
      awaiter.readRegNoScoreboardAwaiter.arg = arg;
      return awaiter.readRegNoScoreboardAwaiter;
    }

    ReadRegNoScoreboard readRegNoScoreboard() const
    {
      assert( type == AwaiterType::READ_REG_NO_SCOREBOARD );
      return awaiter.readRegNoScoreboardAwaiter.arg;
    }

    void readRegNoScoreboard( ReadRegNoScoreboard::Res res )
    {
      assert( type == AwaiterType::READ_REG_NO_SCOREBOARD );
      awaiter.readRegNoScoreboardAwaiter.res = res;
    }

    Awaiter::ReadRegAwaiter& await_transform( ReadReg const& arg )
    {
      type = AwaiterType::READ_REG;
      awaiter.readRegAwaiter.arg = arg;
      return awaiter.readRegAwaiter;
    }

    ReadReg readReg() const
    {
      assert( type == AwaiterType::READ_REG );
      return awaiter.readRegAwaiter.arg;
    }

    void readReg( ReadReg::Res res )
    {
      assert( type == AwaiterType::READ_REG );
      awaiter.readRegAwaiter.res = res;
    }

    Awaiter::ReadRegMoveAwaiter& await_transform( ReadRegMove const& arg )
    {
      type = AwaiterType::READ_REG_MOVE;
      awaiter.readRegMoveAwaiter.arg = arg;
      return awaiter.readRegMoveAwaiter;
    }

    ReadRegMove readRegMove() const
    {
      assert( type == AwaiterType::READ_REG_MOVE );
      return awaiter.readRegMoveAwaiter.arg;
    }

    void readRegMove( ReadRegMove::Res res )
    {
      assert( type == AwaiterType::READ_REG_MOVE );
      awaiter.readRegMoveAwaiter.res = res;
    }

    Awaiter::ReadRegReadFlagsAwaiter& await_transform( ReadRegReadFlags const& arg )
    {
      type = AwaiterType::READ_REG_READ_FLAGS;
      awaiter.readRegReadFlagsAwaiter.arg = arg;
      return awaiter.readRegReadFlagsAwaiter;
    }

    ReadRegReadFlags readRegReadFlags() const
    {
      assert( type == AwaiterType::READ_REG_READ_FLAGS );
      return awaiter.readRegReadFlagsAwaiter.arg;
    }

    void readRegReadFlags( ReadRegReadFlags::Res res )
    {
      assert( type == AwaiterType::READ_REG_READ_FLAGS );
      awaiter.readRegReadFlagsAwaiter.res = res;
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

    Awaiter::ReadSrcReadDstLockFlagsAwaiter& await_transform( ReadSrcReadDstLockFlags const& arg )
    {
      type = AwaiterType::READ_SRC_READ_DST_LOCK_FLAGS;
      awaiter.readSrcReadDstLockFlagsAwaiter.arg = arg;
      return awaiter.readSrcReadDstLockFlagsAwaiter;
    }

    ReadSrcReadDstLockFlags readSrcReadDstLockFlags() const
    {
      assert( type == AwaiterType::READ_SRC_READ_DST_LOCK_FLAGS );
      return awaiter.readSrcReadDstLockFlagsAwaiter.arg;
    }

    void readSrcReadDstLockFlags( ReadSrcReadDstLockFlags::Res res )
    {
      assert( type == AwaiterType::READ_SRC_READ_DST_LOCK_FLAGS );
      awaiter.readSrcReadDstLockFlagsAwaiter.res = res;
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

    Awaiter::WritePCAwaiter& await_transform( WritePC const& arg )
    {
      type = AwaiterType::WRITE_PC;
      awaiter.writePCAwaiter.arg = arg;
      return awaiter.writePCAwaiter;
    }

    WritePC writePC() const
    {
      assert( type == AwaiterType::WRITE_PC );
      return awaiter.writePCAwaiter.arg;
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

  void getAcc( int8_t acc )
  {
    mCoro.promise().getAcc( { acc } );
    mCoro();
  }

  void prepareJump( uint32_t pc )
  {
    mCoro.promise().prepareJump( { pc } );
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

  MemoryLoadByte memoryLoadByte() const
  {
    return mCoro.promise().memoryLoadByte();
  }

  MemoryLoadWord memoryLoadWord() const
  {
    return mCoro.promise().memoryLoadWord();
  }

  MemoryLoadLong memoryLoadLong() const
  {
    return mCoro.promise().memoryLoadLong();
  }


  MemoryLoadByteExternal memoryLoadByteExternal() const
  {
    return mCoro.promise().memoryLoadByteExternal();
  }

  MemoryLoadWordExternal memoryLoadWordExternal() const
  {
    return mCoro.promise().memoryLoadWordExternal();
  }

  MemoryLoadLongExternal memoryLoadLongExternal() const
  {
    return mCoro.promise().memoryLoadLongExternal();
  }

  void memoryLoadByte( uint32_t value )
  {
    mCoro.promise().memoryLoadByte( { value } );
    mCoro();
  }

  void memoryLoadWord( uint32_t value )
  {
    mCoro.promise().memoryLoadWord( { value } );
    mCoro();
  }

  void memoryLoadByteExternal( uint32_t value )
  {
    mCoro.promise().memoryLoadByteExternal( { value } );
    mCoro();
  }

  void memoryLoadLongExternal( uint32_t value )
  {
    mCoro.promise().memoryLoadLongExternal( { value } );
    mCoro();
  }

  void memoryLoadWordExternal( uint32_t value )
  {
    mCoro.promise().memoryLoadWordExternal( { value } );
    mCoro();
  }

  void memoryLoadLong( uint32_t value )
  {
    mCoro.promise().memoryLoadLong( { value } );
    mCoro();
  }

  MemoryStoreByte memoryStoreByte() const
  {
    return mCoro.promise().memoryStoreByte();
  }

  MemoryStoreWord memoryStoreWord() const
  {
    return mCoro.promise().memoryStoreWord();
  }

  MemoryStoreLong memoryStoreLong() const
  {
    return mCoro.promise().memoryStoreLong();
  }

  MemoryStoreByteExternal memoryStoreByteExternal() const
  {
    return mCoro.promise().memoryStoreByteExternal();
  }

  MemoryStoreWordExternal memoryStoreWordExternal() const
  {
    return mCoro.promise().memoryStoreWordExternal();
  }

  MemoryStoreLongExternal memoryStoreLongExternal() const
  {
    return mCoro.promise().memoryStoreLongExternal();
  }

  ReadRegLockFlags readRegLockFlags() const
  {
    return mCoro.promise().readRegLockFlags();
  }

  void readRegLockFlags( uint32_t value )
  {
    mCoro.promise().readRegLockFlags( { value } );
    mCoro();
  }

  ReadLockReg readLockReg() const
  {
    return mCoro.promise().readLockReg();
  }

  void readLockReg( uint32_t value )
  {
    mCoro.promise().readLockReg( { value } );
    mCoro();
  }

  ReadLockRegLockFlags readLockRegLockFlags() const
  {
    return mCoro.promise().readLockRegLockFlags();
  }

  void readLockRegLockFlags( uint32_t value )
  {
    mCoro.promise().readLockRegLockFlags( { value } );
    mCoro();
  }

  ReadRegNoScoreboard readRegNoScoreboard() const
  {
    return mCoro.promise().readRegNoScoreboard();
  }

  void readRegNoScoreboard( uint32_t value )
  {
    mCoro.promise().readRegNoScoreboard( { value } );
    mCoro();
  }

  ReadReg readReg() const
  {
    return mCoro.promise().readReg();
  }

  void readReg( uint32_t value )
  {
    mCoro.promise().readReg( { value } );
    mCoro();
  }

  ReadRegMove readRegMove() const
  {
    return mCoro.promise().readRegMove();
  }

  void readRegMove( uint32_t value )
  {
    mCoro.promise().readRegMove( { value } );
    mCoro();
  }

  ReadRegReadFlags readRegReadFlags() const
  {
    return mCoro.promise().readRegReadFlags();
  }

  void readRegReadFlags( ReadRegReadFlags::Res res )
  {
    mCoro.promise().readRegReadFlags( res );
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

  ReadSrcReadDstLockFlags readSrcReadDstLockFlags() const
  {
    return mCoro.promise().readSrcReadDstLockFlags();
  }

  void readSrcReadDstLockFlags( ReadSrcReadDstLockFlags::Res res )
  {
    mCoro.promise().readSrcReadDstLockFlags( res );
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

  WritePC writePC() const
  {
    return mCoro.promise().writePC();
  }

  WriteDst writeDst() const
  {
    return mCoro.promise().writeDst();
  }

  WriteDstWriteFlags writeDstUnlockWriteFlags() const
  {
    return mCoro.promise().writeDstUnlockWriteFlags();
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

