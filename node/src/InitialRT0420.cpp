#ifndef ECC_INITIAL_RT0420_CPP
#define ECC_INITIAL_RT0420_CPP

#include "InitialRT0420.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"

using ecc::Tx;
using ecc::Message;
using ecc::InitialRT0420;
using ecc::QueryManager;
using ecc::Exception;

InitialRT0420::InitialRT0420()
{
  d_txState = Tx::INITIAL;
  d_event = Tx::RT0420;
}

void InitialRT0420::apply(Tx& a_tx)
{
  a_tx.timedWait();//parra quitar el primer counter del semaforo
  try
	{
	  bool result;
	  Message* message_p = a_tx.getCurrentIn();

	  string authorisationCode = message_p->getAuthorisationCode();
	  string responseCode = message_p->getResponseCode();
	  string telephone = message_p->getTelephone();
	  string id = message_p->getTxId();
	  {
		QueryManager queryManager;
		queryManager.begin();
		QueryManager::AcceptState acceptState;
		acceptState = queryManager.delayReverseTx(
												  id
												  ,ecc::pp::txs::state::ABORT
												  ,ecc::pp::txs::evolve::PASIVE
												  ,authorisationCode
												  ,telephone
												  );
		//aqui cacho los valores que voy a utilizar despues
		a_tx
		  .messageFactory()
		  .setCacheOriginalDataElements(
										queryManager
										.createOriginalDataElements( a_tx.getId() )
										);
		cach( a_tx.messageFactory(), queryManager );
		
		if( acceptState = QueryManager::ACCEPTED )
		  wt0430(a_tx);//checar que esta funcion solo puede mandar excepciones antes de mandar el mensaje
		else
		  {
			stringstream sstring;
			sstring
			  << "InitialRT0420::info:\n\t< MSG 522 >: La peticion de reverso, para la transaccion:-"
			  << id
			  << "-, y codigo de authorisacion: -"
			  << authorisationCode
			  << "-; no corresponden a ninguna transaccion autorizada.";
			Log::instance().log( sstring.str().c_str() );
		  }
		queryManager.isCommit(true);
		a_tx.setEvent( Tx::DONE );
		a_tx.setState( Tx::END );
	  }
	}catch(Exception& e){
	  a_tx.setState( Tx::END );
	  a_tx.setEvent( Tx::EXCEPTION );
	  e
		<< "catch in InitialRT0420::apply\n"
		;
	  throw;
	}catch(...){
	  a_tx.setState( Tx::END );
	  a_tx.setEvent( Tx::EXCEPTION );
	  throw;
	}
}

#endif
