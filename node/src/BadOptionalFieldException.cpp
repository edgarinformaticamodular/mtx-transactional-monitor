#ifndef ECC_BADOPTIONALFIELDEXCEPTION_CPP
#define ECC_BADOPTIONALFIELDEXCEPTION_CPP

#include "BadOptionalFieldException.hpp"
#include <string>
#include <sstream>

using namespace ecc;

/**
 *
 */
BadOptionalFieldException::BadOptionalFieldException()
  :Exception()
{
}//BadOptionalFieldException

/**
 *
 */
BadOptionalFieldException::BadOptionalFieldException(string a_explanation)
  :Exception(a_explanation)
{
}//BadOptionalFieldException

/**
 *
 */
BadOptionalFieldException::~BadOptionalFieldException()
{
}//~BadOptionalFieldException

#endif
