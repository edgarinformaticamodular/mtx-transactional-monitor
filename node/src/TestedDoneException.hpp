#ifndef ECC_TESTED_DONE_EXCEPTION_HPP
#define ECC_TESTED_DONE_EXCEPTION_HPP

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
  class TestedDoneException : virtual public Rule {
  public:
	TestedDoneException();
	virtual void apply(Tx&);
  };//class

}//namespace ecc

#endif
