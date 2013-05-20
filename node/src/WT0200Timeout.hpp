#ifndef ECC_WT0200_TIME_OUT_HPP
#define ECC_WT0200_TIME_OUT_HPP

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
  class WT0200Timeout : virtual public Rule {
  public:
	WT0200Timeout();
	virtual void apply(Tx&);
  };//class

}//namespace ecc

#endif
