#ifndef ECC_DEFAULT_HPP
#define ECC_DEFAULT_HPP

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
  class Default : virtual public Rule {
  public:
	Default();
	virtual bool isMatch(Tx&);
	virtual void apply(Tx&);
  };//class

}//namespace ecc

#endif
