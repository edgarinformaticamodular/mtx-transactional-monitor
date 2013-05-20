#ifndef ECC_WT0421_TIME_OUT_EXCEPTION_HPP
#define ECC_WT0421_TIME_OUT_EXCEPTION_HPP

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
  class WT0421TimeoutException : virtual public Rule {
  public:
	WT0421TimeoutException();
	virtual void apply(Tx&);
  };//class

}//namespace ecc

#endif
