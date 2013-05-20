#ifndef ECC_WT0420_RT0430_HPP
#define ECC_WT0420_RT0430_HPP

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
  class WT0420RT0430 : virtual public Rule {
  public:
	WT0420RT0430();
	bool isMatch(Tx& a_tx);
	virtual void apply(Tx&);
  };//class

}//namespace ecc

#endif
