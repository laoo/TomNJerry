#pragma once

class Prefetcher
{
  struct promise_type;

  std::coroutine_handle<promise_type> mCoro;

  Prefetcher( std::coroutine_handle<promise_type> c ) : mCoro{ c }
  {
  }

public:

  struct Code {};

  struct promise_type
  {
    promise_type()
    {
    }
    auto get_return_object() { return Prefetcher{ std::coroutine_handle<promise_type>::from_promise( *this ) }; }
    auto initial_suspend() noexcept { return std::suspend_never{}; }
    auto final_suspend() noexcept { return std::suspend_always{}; }
    void return_void() {}
    void unhandled_exception() { std::terminate(); }

    enum State
    {
      UNKNOWN,
      NEEDS_INPUT,
      HAS_OUTPUT
    } mState;
    uint16_t mInput{};
    union
    {
      RISCOpcode mOutput{};
      uint16_t mRawOutput;
    };

    std::suspend_always yield_value( RISCOpcode opcode )
    {
      mOutput = opcode;
      mState = HAS_OUTPUT;
      return {};
    }

    std::suspend_always yield_value( uint16_t code )
    {
      mRawOutput = code;
      mState = HAS_OUTPUT;
      return {};
    }

    auto await_transform( Code )
    {
      struct InputAwaiter
      {
        uint16_t& opcode;
        bool await_ready() { return false; }
        void await_suspend( std::coroutine_handle<> c ) {}
        uint16_t await_resume()
        {
          return opcode;
        }
      };

      mState = NEEDS_INPUT;
      return InputAwaiter{ mInput };
    }
  };

public:

  template<typename OpcodeMapper>
  static Prefetcher create()
  {
    for ( ;; )
    {
      auto opcode = OpcodeMapper::mapOpcode( co_await Code{} );
      switch ( opcode.opcode )
      {
      case RISCOpcode::MOVEI:
      {
        co_yield opcode;
        co_yield co_await Code{};
        co_yield co_await Code{};
        break;
      }
      default:
        co_yield opcode;
      }
    }
  }

  Prefetcher() : mCoro{}
  {
  }

  ~Prefetcher()
  {
    if ( mCoro )
      mCoro.destroy();
  }

  Prefetcher( Prefetcher&& other ) noexcept
  {
    if ( this != &other )
      mCoro = std::exchange( other.mCoro, nullptr );
  }

  Prefetcher& operator=( Prefetcher&& other ) noexcept
  {
    if ( this != &other )
      mCoro = std::exchange( other.mCoro, nullptr );
    return *this;
  }

  Prefetcher( Prefetcher const& ) = delete;
  Prefetcher& operator=( Prefetcher const& ) = delete;

  explicit operator bool() const
  {
    return mCoro.promise().mState == promise_type::HAS_OUTPUT;
  }

  void put( uint16_t input )
  {
    assert( mCoro.promise().mState != promise_type::HAS_OUTPUT );
    mCoro.promise().mInput = input;
    mCoro();
    assert( mCoro.promise().mState == promise_type::HAS_OUTPUT );
  }

  RISCOpcode get() const
  {
    assert( mCoro.promise().mState == promise_type::HAS_OUTPUT );
    RISCOpcode result = mCoro.promise().mOutput;
    mCoro();
    return result;
  }

  uint16_t getCode() const
  {
    assert( mCoro.promise().mState == promise_type::HAS_OUTPUT );
    uint16_t result = mCoro.promise().mRawOutput;
    mCoro();
    return result;
  }
};

