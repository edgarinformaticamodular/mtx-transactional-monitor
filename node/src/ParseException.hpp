#ifndef ECC_PARSE_EXCEPTION_HPP
#define ECC_PARSE_EXCEPTION_HPP

#include <string>
#include "Exception.hpp"

namespace ecc
{
  /**
   *
   */
  class ParseException : public virtual ecc::Exception
  {
  protected:
	string d_field;
	string d_muestra;

  public:
	ParseException(string, string);
	ParseException(string, string, string);
	string getField() const;
	virtual string getMessage() const;
  };//class ParseException

}//namespace ecc

#endif
