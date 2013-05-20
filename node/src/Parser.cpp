#ifndef ECC_PARSER_CPP
#define ECC_PARSER_CPP

#include "Parser.hpp"
#include <sstream>
#include <string>

using namespace ecc;

/**
 *
 */
Parser::Parser()
{
}//Parser

/**
 *
 */
Parser::~Parser()
{
}//~Parser

/**
 * como se usa mucho la puse inline
 *
string Parser::getData() const
{ 
  return d_data; 
}//getData
*/
/**
 *
 */
string Parser::getHeader() const
{ 
  return d_header; 
}//getHeader

/**
 *
 */
void Parser::setData(const string a_data)
{
  d_data = a_data;
}//setData

/**
 *
 */
string Parser::display() const
{
  return "";
}//display

/**
 *
 */
bool Parser::isIncludeHeader() const
{
  return false;
}//isIncludeHeader

#endif
