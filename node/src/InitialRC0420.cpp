#ifndef ECC_INITIAL_RC0420_CPP
#define ECC_INITIAL_RC0420_CPP

#include "InitialRC0420.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"

using ecc::Tx;
using ecc::Message;
using ecc::InitialRC0420;
using ecc::QueryManager;
using ecc::Exception;

InitialRC0420::InitialRC0420()
{
  d_txState = Tx::INITIAL;
  d_event = Tx::RC0420;
}

void InitialRC0420::apply(Tx& a_tx)
{
  a_tx.timedWait();//parra quitar el primer counter del semaforo
  try
	{
	  Message* message_p = a_tx.getCurrentIn();

	  string authorisationCode = message_p->getAuthorisationCode();
	  string login = message_p->getLogin();
	  string password = message_p->getPassword();
	  string telephone = message_p->getTelephone();
	  string id = message_p->getTxId();
	  bool result;
	  QueryManager::AcceptState acceptState;
	  {
		QueryManager queryManager;
		queryManager.begin();
		if( message_p->isRecover() )
		  acceptState = queryManager.delayReverseTx(
							    id
							    ,ecc::pp::txs::state::REVERSING
							    ,ecc::pp::txs::evolve::ACTIVE
							    ,authorisationCode
							    //,login
							    //,password
							    ,telephone
							    );
		else
		  acceptState = queryManager.delayReverseTx(
							    id
							    ,ecc::pp::txs::state::COMMIT
							    ,ecc::pp::txs::evolve::PASIVE
							    ,authorisationCode
							    ,login//todo:falta incluir el amount en validacion segun ISO
							    ,password
							    ,telephone
							    );
		a_tx.setAcceptState( acceptState );
		//aqui cacho los valores que voy a utilizar despues
		a_tx
		  .messageFactory()
		  .setCacheOriginalDataElements(
										queryManager
										.createOriginalDataElements( a_tx.getId() )
										);
		cach(a_tx.messageFactory(),queryManager);//cacho los otros datos
		//note como aqui no dejo el dinero disponible si no puedo comunicarme con la telefonica
		switch( acceptState )
		  {
		  case QueryManager::ACCEPTED:
			{
			  wt0420(a_tx);
			  a_tx.setState( Tx::WT0420 );
			  a_tx.setEvent( Tx::TIME_OUT );
			  //a_tx.timedWait();esto ya lo hago mas adelannte y no retengo una conexion ociosamente tanto tiempo
			}
			break;
		  default:
			a_tx.setState( Tx::END );
			a_tx.setEvent( Tx::DONE );
			break;
		  }
		queryManager.isCommit(true);
	  }//termino aqui el block parra que se haga el commit
	  if( acceptState == QueryManager::ACCEPTED )
		{
		  a_tx.timedWait();//este lo hago hasta aca para no entretener el uso de una conexion a la base de datos inutilmente y hacer el comit si se pudo mandar el mensaje bien y reversar localmente
		}
	}catch(Exception& e){
	  a_tx.setAcceptState( QueryManager::NO_ACCEPTED );
	  a_tx.setState( Tx::END );
	  a_tx.setEvent( Tx::EXCEPTION );
	  e 
		<< endl
		<< "no se aplico el reverso\n"
		<< "catch in InitialRC0420::apply."
		<< endl;
	  throw;
	  //Log::instance().log( e.getMessage().c_str() );//todo:esto ya lo hace quien lo cacha
	}catch(std::exception& e){
	  a_tx.setAcceptState( QueryManager::NO_ACCEPTED );
	  a_tx.setState( Tx::END );
	  a_tx.setEvent( Tx::EXCEPTION );
	  stringstream sstream;
	  sstream
		<< e.what()
		<< endl
		<< "< MSG 005110 >: no se aplico eel reverso\n"
		<< "catch in InitialRC0420::apply."
		<< endl;
	  throw;
	  //Log::instance().log( sstream.str().c_str() );
	}catch(...){
	  a_tx.setState( Tx::END );
	  a_tx.setEvent( Tx::EXCEPTION );
	  a_tx.setAcceptState( QueryManager::NO_ACCEPTED );
	  stringstream sstream;
	  sstream
		<< "< MSG 005109 >: ???::exception:\n\tOcurrio una excepcion no identificada en solicitud de reverso. no se aplica el reverso."
		<< "catch in InitialRC0420::apply."
		<< endl;
	  throw;
	  //Log::instance().log( sstream.str().c_str() );
	}//catch
}

#endif
