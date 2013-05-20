#ifndef ECC_INITIAL_RC0420_HPP
#define ECC_INITIAL_RC0420_HPP

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
  class InitialRC0420 : virtual public Rule {
  public:
	InitialRC0420();
	virtual void apply(Tx&);
  };//class

}//namespace ecc

#endif
