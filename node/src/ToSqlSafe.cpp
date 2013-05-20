#ifndef ECC_TO_SQL_SAFE_CPP
#define ECC_TO_SQL_SAFE_CPP

#include "ToSqlSafe.hpp"

using namespace ecc;

/**
 *
 */
char ToSqlSafe::operator()(char a_char)
{
  switch(a_char){
  case '\'':
	return '"';
  case '\x002':
  case '\x003':
  case '\x000':
	return ' ';
  default:
	return a_char;
  }
}//operator()

#endif
