#pragma once

#include <memory>
#include <sstream>
#include <stdexcept>


class Ex : public std::exception
{
public:

  Ex() : exception{}, mSS{ std::make_shared<std::stringstream>() }, mStr{}
  {
  }

  Ex( char const* str ) : Ex{}
  {
    *mSS << str;
  }


  template<typename T>
  Ex& operator<<( T const& t )
  {
    *mSS << t;
    return *this;
  }

  char const* what() const noexcept override
  {
    mStr = mSS->str();
    return mStr.c_str();
  }

private:
  std::shared_ptr<std::stringstream> mSS;
  mutable std::string mStr;
};

class EmulationViolation : public Ex
{
public:
  EmulationViolation() : Ex{}
  {
  }

  EmulationViolation( char const* str ) : Ex{ str }
  {
  }
};
