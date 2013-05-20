#ifndef ECC_WT0200_RT0210A_HPP
#define ECC_WT0200_RT0210A_HPP

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
  class WT0200RT0210A : virtual public Rule {
  public:
	WT0200RT0210A();
	virtual void apply(Tx&);
  };//class

}//namespace ecc

#endif
