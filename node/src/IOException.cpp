#ifndef ECC_IOEXCEPTION_CPP
#define ECC_IOEXCEPTION_CPP

#include <string>
#include "IOException.hpp"

using namespace ecc;

/**
 *
 */
IOException::IOException()
  :Exception::Exception() 
{
}//IOException

/**
 *
 */
IOException::IOException(string a_string)
  :Exception::Exception(a_string) 
{
}//IOException

/**
 *
 */
IOException::~IOException()
{ 
}//~IOException

#endif
