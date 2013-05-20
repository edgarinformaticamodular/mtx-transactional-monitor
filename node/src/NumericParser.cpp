#ifndef ECC_NUMERIC_PARSER_CPP
#define ECC_NUMERIC_PARSER_CPP

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include "ParseException.hpp"

using namespace ecc;

/**
 * GenericParser
 */
template< int t_id, int t_maxFieldSize, int t_headerSize>
NumericParser<t_id,t_maxFieldSize,t_headerSize>
::NumericParser<t_id,t_maxFieldSize,t_headerSize>(){
}//GenericParser

/** no
 * falta def. cual deve ser como virtual
 */
template< int t_id, int t_maxFieldSize, int t_headerSize>
NumericParser<t_id,t_maxFieldSize,t_headerSize>
::~NumericParser<t_id,t_maxFieldSize,t_headerSize>(){
}//~NumericParser

/**
 * parse
 */
template< int t_id, int t_maxFieldSize, int t_headerSize>
void NumericParser<t_id,t_maxFieldSize,t_headerSize>
::parse(stringstream& a_stringstream)
{
  static const int ASCII_MIN = 48;// 0
  static const int ASCII_MAX = 57;// 9
  BasicParser<t_id,t_maxFieldSize,t_headerSize>::parse(a_stringstream);
  //valido que sean caracteres validos
  string::iterator itr;
  for( itr = d_data.begin(); itr != d_data.end(); itr++ )
	if( 
	   static_cast<int>(*itr) < ASCII_MIN 
	   || 
	   static_cast<int>(*itr) > ASCII_MAX 
	   )
	  {
		throw ParseException(getId_str(),"< MSG 079 >: ParseException\n\n\tInvalido caracter no numerico inesperado.\nthrow in NumericParser\n",d_data);
	  }
}//parse

#endif
