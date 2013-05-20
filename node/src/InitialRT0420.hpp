#ifndef ECC_INITIAL_RT0420_HPP
#define ECC_INITIAL_RT0420_HPP

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
  class InitialRT0420 : virtual public Rule {
  public:
	InitialRT0420();
	virtual void apply(Tx&);
  };//class

}//namespace ecc

#endif
