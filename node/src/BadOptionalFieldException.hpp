#ifndef ECC_BADOPTIONALFIELDEXCEPTION_HPP
#define ECC_BADOPTIONALFIELDEXCEPTION_HPP

#include <string>
#include "Exception.hpp"

namespace ecc
{
  /**
   *
   */
  class BadOptionalFieldException : public virtual ecc::Exception
  {
  protected:

  public:
	BadOptionalFieldException();
	BadOptionalFieldException(string);
	~BadOptionalFieldException();
  };//class BadOptinalFieldException

}//namespace ecc

#endif
