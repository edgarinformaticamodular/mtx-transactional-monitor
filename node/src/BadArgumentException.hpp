#ifndef ECC_BADARGUMENTEXCEPTION_HPP
#define ECC_BADARGUMENTEXCEPTION_HPP

#include <string>
#include "Exception.hpp"

namespace ecc
{
  /**
   *
   */
  class BadArgumentException : public virtual ecc::Exception
  {
  protected:

  public:
	BadArgumentException();
	BadArgumentException(string);
	~BadArgumentException();
  };//class BadArgumentException

}//namespace ecc

#endif
