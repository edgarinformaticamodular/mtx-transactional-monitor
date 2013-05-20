#ifndef ECC_WT0200_RT0210R_HPP
#define ECC_WT0200_RT0210R_HPP

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
  class WT0200RT0210R : virtual public Rule {
  public:
	WT0200RT0210R();
	virtual void apply(Tx&);
  };//class

}//namespace ecc

#endif
