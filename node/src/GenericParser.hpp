#ifndef ECC_GENERIC_PARSER_HPP
#define ECC_GENERIC_PARSER_HPP

#include <sstream>
#include <string>
#include "BasicParser.hpp"

namespace ecc
{

  /**
   * GenericParser
   */
  template< int t_id, int t_maxFieldSize, int t_headerSize>
  class GenericParser : public BasicParser<t_id,t_maxFieldSize,t_headerSize>
  {
  public:
	GenericParser();
	virtual ~GenericParser();	
	virtual void parse(stringstream&);
  };//class GenericParser
  
}//namespace ecc

#include "GenericParser.cpp"

#endif
