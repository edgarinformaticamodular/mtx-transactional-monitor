#ifndef ECC_NOTFOUND_EXCEPTION_CPP
#define ECC_NOTFOUND_EXCEPTION_CPP

#include "NotFoundException.hpp"
#include <string>
#include <sstream>

using namespace ecc;

/**
 *
 */
NotFoundException::NotFoundException()
  :Exception()
{
}//NotFoundException

/**
 *
 */
NotFoundException::NotFoundException(string a_explanation)
  :Exception(a_explanation)
{
}//NotFoundException

/**
 *
 */
NotFoundException::~NotFoundException()
{
}//~NotFoundException

#endif
