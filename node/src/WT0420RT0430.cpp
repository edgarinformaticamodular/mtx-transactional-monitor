#ifndef ECC_WT0420_RT0430_CPP
#define ECC_WT0420_RT0430_CPP

#include "WT0420RT0430.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"

using ecc::Tx;
using ecc::Message;
using ecc::WT0420RT0430;
using ecc::QueryManager;
using ecc::Exception;

WT0420RT0430::WT0420RT0430()
{
  d_txState = Tx::WT0420;
  d_event = Tx::RT0430;
}


bool WT0420RT0430::isMatch(Tx& a_tx)
{
  return (a_tx.getState() == Tx::WT0421 || a_tx.getState() == Tx::WT0420) && (a_tx.getEvent() == d_event);
}


void WT0420RT0430::apply(Tx& a_tx)
{
  string id = a_tx.getId();
  try
	{
	  Message* currentIn_sp = a_tx.getCurrentIn();
	  {
		QueryManager queryManager;
		queryManager.begin();
		queryManager.updateTx(
							  id
							  ,ecc::pp::txs::state::ABORT
							  ,ecc::pp::txs::evolve::PASIVE
							  );
		queryManager.isCommit(true);
	  }
	  a_tx.setState( Tx::END );
	  a_tx.setEvent( Tx::UNDEFINED );
	}catch(Exception& e){
	  e
		<< "catch in WT0420RT0430::apply\n"
		;
	  a_tx.setState( Tx::END );
	  a_tx.setEvent( Tx::EXCEPTION );
	  throw;
	}catch(...){
	  a_tx.setState( Tx::END );
	  a_tx.setEvent( Tx::EXCEPTION );
	  throw;
	}
}

#endif
