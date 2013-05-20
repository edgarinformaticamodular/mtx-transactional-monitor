#ifndef ECC_BADARGUMENTEXCEPTION_CPP
#define ECC_BADARGUMENTEXCEPTION_CPP

#include "BadArgumentException.hpp"
#include <string>
#include <sstream>

using namespace ecc;

/**
 *
 */
BadArgumentException::BadArgumentException()
  :Exception()
{
}//BadArgumentException

/**
 *
 */
BadArgumentException::BadArgumentException(string a_explanation)
  :Exception(a_explanation)
{
}//BadArgumentException

/**
 *
 */
BadArgumentException::~BadArgumentException()
{
}//~BadArgumentException

#endif
