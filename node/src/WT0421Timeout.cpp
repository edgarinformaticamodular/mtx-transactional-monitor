#ifndef ECC_WT0421_TIME_OUT_CPP
#define ECC_WT0421_TIME_OUT_CPP

#include "WT0421Timeout.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"

using ecc::Tx;
using ecc::Message;
using ecc::WT0421Timeout;
using ecc::QueryManager;
using ecc::Exception;

WT0421Timeout::WT0421Timeout()
{
  d_txState = Tx::WT0421;
  d_event = Tx::TIME_OUT;
}



void WT0421Timeout::apply(Tx& a_tx)
{
  try
	{
	  if( a_tx.getTry() < Tx::MAX_TRY )
		{
		  a_tx.incrementTry();
		  a_tx.setEvent( Tx::TIME_OUT );
		  a_tx.setState( Tx::WT0421 );
		  {
			QueryManager queryManager;
			queryManager.begin();
			cach(a_tx.messageFactory(),queryManager);//cacho los otros datos
			queryManager.isCommit(true);
		  }
		  wt0421(a_tx);
		  a_tx.timedWait();
		}
	  else
		{
		  a_tx.setEvent( Tx::TRY_LIMIT_REACHED );
		  a_tx.isTryAgain( false );
		  a_tx.setState( Tx::END );
		}
	}catch(Exception& e){
	  a_tx.setState( Tx::WT0421 );
	  a_tx.setEvent( Tx::TIME_OUT_EXCEPTION );
	  e
		<< "catch in WT0421Timeout::apply\n"
		;
	  throw;
	}catch(...){
	  a_tx.setState( Tx::WT0421 );
	  a_tx.setEvent( Tx::TIME_OUT_EXCEPTION );
	  throw;//todo: aqui en el cacheo en el principal deve dee darse el siguiente sleep !!!	  
	}
}

#endif
