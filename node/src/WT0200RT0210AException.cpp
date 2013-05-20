#ifndef ECC_WT0200_RT0210A_EXCEPTION_CPP
#define ECC_WT0200_RT0210A_EXCEPTION_CPP

#include "WT0200RT0210AException.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"

using ecc::Tx;
using ecc::Message;
using ecc::WT0200RT0210AException;
using ecc::QueryManager;
using ecc::Exception;

WT0200RT0210AException::WT0200RT0210AException()
{
  d_txState = Tx::WT0200;
  d_event = Tx::RT0210A_EXCEPTION;
}



void WT0200RT0210AException::apply(Tx& a_tx)
{
  try
	{
	  {
		QueryManager queryManager;	  
		queryManager.begin();
		a_tx
		  .messageFactory()
		  .setCacheOriginalDataElements(
										queryManager
										.createOriginalDataElements( a_tx.getId() )
										);
		cach(a_tx.messageFactory(), queryManager);
		queryManager.isCommit(true);
	  }
	
	  a_tx.setState( Tx::WT0420 );
	  a_tx.setEvent( Tx::TIME_OUT );
	  wt0420(a_tx);//checar que esta funcion solo puede mandar excepciones antes de mandar el mensaje
	  a_tx.timedWait();
	}catch(...){
	  a_tx.setState( Tx::END );
	  a_tx.setEvent( Tx::EXCEPTION );
	  throw;
	}
}

#endif
