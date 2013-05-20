#ifndef ECC_ALPHANUMERIC_PARSER_CPP
#define ECC_ALPHANUMERIC_PARSER_CPP

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include "ParseException.hpp"

using namespace ecc;

/**
 * 
 */
template<int t_id, int t_maxFieldSize, int t_headerSize>
AlphanumericParser<t_id,t_maxFieldSize,t_headerSize>
::AlphanumericParser<t_id,t_maxFieldSize,t_headerSize>()
{
}//AlphanumericParser

/**
 * falta definirlo cual deve ser como virtual
 */
template< int t_id, int t_maxFieldSize, int t_headerSize>
AlphanumericParser<t_id,t_maxFieldSize,t_headerSize>
::~AlphanumericParser<t_id,t_maxFieldSize,t_headerSize>()
{
}//~AlphanumericParser

/**
 * parse
 */
template< int t_id, int t_maxFieldSize, int t_headerSize>
void AlphanumericParser<t_id,t_maxFieldSize,t_headerSize>
::parse(stringstream& a_stringstream)
{
  static const int ASCII_MIN_1 = 48;//0
  static const int ASCII_MAX_1 = 57;//9
  static const int ASCII_MIN_2 = 65;//A
  static const int ASCII_MAX_2 = 90;//Z
  static const int ASCII_MIN_3 = 97;//a
  static const int ASCII_MAX_3 = 122;//z
  static const int SPC = 32;//espacio blanco
  
  BasicParser<t_id,t_maxFieldSize,t_headerSize>::parse(a_stringstream);
  //valido que sean caracteres validos
  string::iterator itr;
  for( itr = d_data.begin(); itr != d_data.end(); itr++ )
	if( 
	   (//esto excluye el caso del SPC
		static_cast<int>(*itr) < ASCII_MIN_1 
		&&
		static_cast<int>(*itr) !=  SPC
		)
	   || (
		   static_cast<int>(*itr) > ASCII_MAX_1 
		   && 
		   static_cast<int>(*itr) < ASCII_MIN_2 
		   )
	   || (
		   static_cast<int>(*itr) > ASCII_MAX_2 
		   && 
		   static_cast<int>(*itr) < ASCII_MIN_3
		   )
	   ||
	   static_cast<int>(*itr) > ASCII_MAX_3
	   )
	  {
		throw ParseException(getId_str(),"< MSG 080 >: ParseException\n\n\tparse invalido caracter no alfanumerico inesparado\nthrow in AlphanumericParser.\n",d_data);
	  }
}//parse

#endif
