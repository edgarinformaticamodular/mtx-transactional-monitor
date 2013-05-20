#ifndef ECC_WT0200_RT0210R_CPP
#define ECC_WT0200_RT0210R_CPP

#include "WT0200RT0210R.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"

using ecc::Tx;
using ecc::Message;
using ecc::WT0200RT0210R;
using ecc::QueryManager;
using ecc::Exception;

WT0200RT0210R::WT0200RT0210R()
{
  d_txState = Tx::WT0200;
  d_event = Tx::RT0210R;
}



void WT0200RT0210R::apply(Tx& a_tx)
{
  try
	{
	  Message* currentIn_sp = a_tx.getCurrentIn();
	  
	  Message* firstIn_sp = a_tx.getFirstIn();
	  //lo quito porque no hay tal !
	  //string authorisationCode = currentIn_sp->getAuthorisationCode();
	  string responseCode = currentIn_sp->getResponseCode();
	  string amount = firstIn_sp->getAmount();
	  string login = firstIn_sp->getLogin();
	  string id = firstIn_sp->getTxId();
	  {
		QueryManager queryManager;
		queryManager.begin();
		queryManager.plusCredit(login,amount);
		queryManager.updateTx(
							  id
							  ,ecc::pp::txs::state::ABORT
							  ,ecc::pp::txs::evolve::PASIVE
							  ,responseCode
							  ,"------"
							  );
		cach(a_tx.messageFactory(), queryManager);
		queryManager.isCommit(true);
	  }
	  wc0210r(a_tx, responseCode);
	  a_tx.setEvent( Tx::DONE );
	  a_tx.setState( Tx::END );
	}catch(...){
	  a_tx.setState( Tx::WT0200 );
	  a_tx.setEvent( Tx::RT0210R_EXCEPTION );
	  throw;
	}

}

#endif
