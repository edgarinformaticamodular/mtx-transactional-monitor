#ifndef ECC_NUMERIC_PARSER_HPP
#define ECC_NUMERIC_PARSER_HPP

#include <sstream>
#include <string>
#include "BasicParser.hpp"

namespace ecc
{
  
  /**
   * 
   */
  template< int t_id, int t_maxFieldSize, int t_headerSize>
  class NumericParser : public BasicParser<t_id,t_maxFieldSize,t_headerSize>
  {
  public:
	NumericParser();
	virtual ~NumericParser();
	virtual void parse(stringstream& a_stringstream);
  };//class NumericParser
  
}//namespace ecc

#include "NumericParser.cpp"

#endif
