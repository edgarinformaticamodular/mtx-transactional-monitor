#ifndef ECC_INITIAL_RT0210A_HPP
#define ECC_INITIAL_RT0210A_HPP

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
  class InitialRT0210A : virtual public Rule {
  public:
	InitialRT0210A();
	virtual void apply(Tx&);
  };//class

}//namespace ecc

#endif
