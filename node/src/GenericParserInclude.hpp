#ifndef ECC_GENERIC_PARSER_INCLUDE_HPP
#define ECC_GENERIC_PARSER_INCLUDE_HPP

#include <sstream>
#include <string>
#include "BasicParser.hpp"

namespace ecc
{

  /**
   * 
   */
  template< int t_id, int t_maxFieldSize, int t_headerSize>
  class GenericParserInclude : public BasicParser<t_id,t_maxFieldSize,t_headerSize>
  {
  public:
	GenericParserInclude();
	virtual ~GenericParserInclude();
	virtual void parse(stringstream&);
  };//class GenericParserInclude
  
}//namespace ecc

#include "GenericParserInclude.cpp"

#endif
