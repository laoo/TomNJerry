#pragma once


class CoroutinePool;

class RISC
{
public:
  
  RISC();
  ~RISC();

  void run();
private:

  std::unique_ptr<CoroutinePool> mPool;
};

void testCoroutine();
