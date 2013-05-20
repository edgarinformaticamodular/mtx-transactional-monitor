#ifndef ECC_INITIAL_RC0200_EXCEPTION_HPP
#define ECC_INITIAL_RC0200_EXCEPTION_HPP

#include <string>
#include <sstream>
#include "Tx.hpp"
#include "Rule.hpp"

using namespace std;
using ecc::Tx;
using ecc::Rule;

namespace ecc
{

  /**
   *
   */
  class InitialRC0200Exception : virtual public Rule {
  public:
	InitialRC0200Exception();
	virtual void apply(Tx&);
  };//class

}//namespace ecc

#endif
