#ifndef ECC_GENERIC_PARSER_CPP
#define ECC_GENERIC_PARSER_CPP

#include <sstream>
#include <string>
#include "ParseException.hpp"
#include "GenericParser.hpp"

using namespace ecc;

/**
 * GenericParser
 */
template< int t_id, int t_maxFieldSize, int t_headerSize>
GenericParser<t_id,t_maxFieldSize,t_headerSize>
::GenericParser<t_id,t_maxFieldSize,t_headerSize>()
{
}//GenericParser

/**
 * falta definirlo cual deve ser como virtual
 */
template< int t_id, int t_maxFieldSize, int t_headerSize>
GenericParser<t_id,t_maxFieldSize,t_headerSize>
::~GenericParser<t_id,t_maxFieldSize,t_headerSize>()
{
}//~GenericParser

/**
 * parse
 */
template< int t_id, int t_maxFieldSize, int t_headerSize>
void GenericParser<t_id,t_maxFieldSize,t_headerSize>
::parse(stringstream& a_stringstream)
{
  static const int ASCII_MIN = 32;//espacio en SPC
  static const int ASCII_MAX = 126;//antes del DEL
  BasicParser<t_id,t_maxFieldSize,t_headerSize>::parse(a_stringstream);
  //valido que sean caracteres validos
  string::iterator itr;
  for( itr = d_data.begin(); itr != d_data.end(); itr++ )
	if( 
	   static_cast<int>(*itr) < ASCII_MIN 
	   || 
	   static_cast<int>(*itr) > ASCII_MAX 
	   )
	  throw ParseException(getId_str(),"< MSG 095 >: ParseException:\n\n\tinvalido: caracter especial inesperado\nthrow in GenericParser::parse.\n",d_data);
}//parse

#endif
