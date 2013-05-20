#ifndef ECC_PARSER_FUNCTOR_CPP
#define ECC_PARSER_FUNCTOR_CPP

#include <map>
#include <sstream>
#include "Parser.hpp"
#include "ParserFunctor.hpp"

using namespace std;
using namespace ecc;

/**
 *
 */
ParserFunctor::ParserFunctor(stringstream& a_stringstream)
  :d_stringstream(a_stringstream)
{
};//ParserFuctor

/**
 *
 */
ParserFunctor::~ParserFunctor()
{
};//~ParserFuctor

/**
 *
 */
void ParserFunctor::operator()(pair<int,Parser*> a_pair)
{
  Parser* parser_p = a_pair.second;
  if( parser_p ) parser_p->parse(d_stringstream);
  //todo: aqui deveria de ir un nullPointerException ?
}//operator()

#endif
