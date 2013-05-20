#ifndef ECC_INITIAL_RC0200_HPP
#define ECC_INITIAL_RC0200_HPP

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
  class InitialRC0200 : public virtual Rule {
  public:
	InitialRC0200();
	virtual void apply(Tx&);
  };//class

}//namespace ecc

#endif
