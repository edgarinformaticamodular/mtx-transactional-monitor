#ifndef ECC_ALPHANUMERIC_PARSER_HPP
#define ECC_ALPHANUMERIC_PARSER_HPP

#include <sstream>
#include <string>
#include "BasicParser.hpp"

namespace ecc
{

  /**
   * 
   */
  template< int t_id, int t_maxFieldSize, int t_headerSize>
  class AlphanumericParser : public BasicParser<t_id,t_maxFieldSize,t_headerSize>
  {
  public:
	AlphanumericParser();
	virtual ~AlphanumericParser();
	virtual void parse(stringstream&);
  };//class AlphanumericParser
  
}//namespace ecc

#include "AlphanumericParser.cpp"

#endif
