#ifndef ECC_PARSE_EXCEPTION_CPP
#define ECC_PARSE_EXCEPTION_CPP

#include "ParseException.hpp"
#include <string>
#include <sstream>
using namespace ecc;
/**
 *
 */
ParseException::ParseException(string a_field, string a_explanation)
  :ecc::Exception(a_explanation)
  ,d_field(a_field)
  ,d_muestra("")
{
}//ParseException

/**
 *
 */
ParseException::ParseException(
							   string a_field, 
							   string a_explanation,
							   string a_muestra
							   )
  :ecc::Exception(a_explanation)
  ,d_field(a_field)
  ,d_muestra(a_muestra)
{
}//ParseException

/**
 *
 */
string ParseException::getField() const
{
  return d_field;
}//getField

/**
 *
 */
string ParseException::getMessage() const
{
  stringstream sstream;
  sstream << d_sstream.str() << "\nEn el campo -"<<d_field << "-. dato: -" << d_muestra <<"-.";
  return sstream.str();
}//getMessage

#endif
