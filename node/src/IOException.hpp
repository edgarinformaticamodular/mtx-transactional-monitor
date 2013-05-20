#ifndef ECC_IOEXCEPTION_HPP
#define ECC_IOEXCEPTION_HPP

#include <string>
#include "Exception.hpp"

namespace ecc
{

  /**
   *
   */
  class IOException : public virtual ecc::Exception {
  public:
	IOException();
	IOException(string);
	virtual ~IOException();
  };//class IOException

}//namespace ecc

#endif
