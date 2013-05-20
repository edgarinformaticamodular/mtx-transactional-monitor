#ifndef ECC_BASIC_PARSER_HPP
#define ECC_BASIC_PARSER_HPP

#include <sstream>
#include <string>
#include "Parser.hpp"

namespace ecc
{
  /**
   * un parseo basico para todos los parseadores en template
   */
  template<int t_id, int t_maxFieldSize, int t_headerSize>
  class BasicParser : public Parser
  {
  public:
	static const int BASE;//esto simboliza la base 10

	BasicParser();
	~BasicParser();
	virtual bool isIncludeHeader() const;
	virtual string display() const;
	string getId_str() const;
	virtual void parse(stringstream&);
  };//class BasicParser
  
}//namespace ecc

#include "BasicParser.cpp"

#endif
