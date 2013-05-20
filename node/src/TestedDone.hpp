#ifndef ECC_TESTED_DONE_HPP
#define ECC_TESTED_DONE_HPP

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
  class TestedDone : virtual public Rule {
  public:
	TestedDone();
	virtual void apply(Tx&);
  };//class

}//namespace ecc

#endif
