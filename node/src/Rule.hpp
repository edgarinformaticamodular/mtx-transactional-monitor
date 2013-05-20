#ifndef ECC_RULE_HPP
#define ECC_RULE_HPP

#include <string>
#include <sstream>
#include "Tx.hpp"
#include "MessageFactory.hpp"
#include "QueryManager.hpp"

using namespace std;
using ecc::Tx;
using ecc::Message;

namespace ecc
{

  /**
   *
   */
  class Rule {
  protected:
	Tx::TxState d_txState;
	Tx::Event d_event;
  public:
	Rule();
	virtual ~Rule();	
	virtual bool isMatch(Tx&);
	virtual void apply(Tx&) = 0;
	void cach(MessageFactory&, QueryManager&);
	void wt0200(Tx&);
	void wc0210r(Tx&,string);
	void wc0210a(Tx&);	
	void wt0430(Tx&);
	void wt0420(Tx&);
	void wt0421(Tx&);
	void wt042x(Tx&,Message*&);
	void reject(Tx&);	
	static void setupEvent(Tx& a_tx);
  };//class Rule

}//namespace ecc

#endif
