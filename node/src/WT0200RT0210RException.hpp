#ifndef ECC_WT0200_RT0210R_EXCEPTION_HPP
#define ECC_WT0200_RT0210R_EXCEPTION_HPP

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
  class WT0200RT0210RException : virtual public Rule {
  public:
	WT0200RT0210RException();
	virtual void apply(Tx&);
  };//class

}//namespace ecc

#endif
