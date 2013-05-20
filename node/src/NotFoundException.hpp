#ifndef ECC_NOTFOUND_EXCEPTION_HPP
#define ECC_NOTFOUND_EXCEPTION_HPP

#include <string>
#include "Exception.hpp"

namespace ecc
{
  /**
   *
   */
  class NotFoundException : public virtual ecc::Exception
  {
  protected:

  public:
	NotFoundException();
	NotFoundException(string);
	~NotFoundException();
  };//class NotFoundException

}//namespace ecc

#endif
