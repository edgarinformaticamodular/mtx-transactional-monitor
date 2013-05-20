#ifndef ECC_WT0420_TIME_OUT_CPP
#define ECC_WT0420_TIME_OUT_CPP

#include "WT0420Timeout.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"

using ecc::Tx;
using ecc::Message;
using ecc::WT0420Timeout;
using ecc::QueryManager;
using ecc::Exception;

WT0420Timeout::WT0420Timeout()
{
  d_txState = Tx::WT0420;
  d_event = Tx::TIME_OUT;
}


void WT0420Timeout::apply(Tx& a_tx)
{
  try
	{
	  a_tx.setState( Tx::WT0421 );
	  a_tx.incrementTry();
	  a_tx.setEvent( Tx::TIME_OUT );
	  {
		QueryManager queryManager;
		queryManager.begin();
		cach(a_tx.messageFactory(),queryManager);//cacho los otros datos
		queryManager.isCommit(true);
	  }
	  wt0421(a_tx);
	  a_tx.timedWait();
	}catch(Exception& e){
	  e
		<< "catch in WT0420Timeout::apply\n"
		;
	  a_tx.setState( Tx::WT0420 );
	  a_tx.setEvent( Tx::TIME_OUT_EXCEPTION );
	  throw;
	}catch(...){
	  a_tx.setState( Tx::WT0420 );
	  a_tx.setEvent( Tx::TIME_OUT_EXCEPTION );
	  throw;//todo: aqui en el cacheo en el principal deve dee darse el siguiente sleep !!!
	}
}

#endif
