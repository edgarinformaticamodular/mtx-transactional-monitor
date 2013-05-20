#ifndef ECC_PARSER_HPP
#define ECC_PARSER_HPP

#include <sstream>
#include <string>

using namespace std;

namespace ecc
{

  /**
   * checar el correcto setup de destructores virtuales
   */
  class Parser
  {
  protected:
	string d_data;
	string d_header;
	
  public:
	Parser(void);
	virtual ~Parser(void) = 0;//checar los destructures virtuales
	virtual void parse(stringstream&) = 0;
	string getData() const { return d_data; }
	string getHeader() const;
	void setData(const string);
	virtual string display() const;
	virtual bool isIncludeHeader() const;

  };//class Parser

}//namespace ecc

#endif
