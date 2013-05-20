#ifndef ECC_PARSER_FUNCTOR_HPP
#define ECC_PARSER_FUNCTOR_HPP

#include <map>
#include <sstream>
#include "Parser.hpp"

using namespace std;
namespace ecc
{

  /**
   *
   */
  class ParserFunctor
  {
  protected:
	stringstream& d_stringstream;
  public:
	ParserFunctor::ParserFunctor(stringstream& a_stringstream);
	~ParserFunctor();
	void ParserFunctor::operator()(pair<int,Parser*> a_pair);
  };//class ParserFunctor
  
}//namespace ecc

#endif
