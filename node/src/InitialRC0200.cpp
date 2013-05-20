#ifndef ECC_INITIAL_RC0200_CPP
#define ECC_INITIAL_RC0200_CPP

#include "InitialRC0200.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"
#include "PortManager.hpp"

using ecc::Tx;
using ecc::Message;
using ecc::InitialRC0200;
using ecc::QueryManager;
using ecc::Exception;
using ecc::PortManager;

InitialRC0200::InitialRC0200()
{
  d_txState = Tx::INITIAL;
  d_event = Tx::RC0200;
}

void InitialRC0200::apply(Tx& a_tx)
{
  a_tx.timedWait();//parra quitar el primer counter del semaforo
  try
	{
	  Message* currentIn_p = a_tx.getCurrentIn();

	  string login = currentIn_p->getLogin();
	  string password = currentIn_p->getPassword();
	  string amount = currentIn_p->getAmount();
	  string id = currentIn_p->getTxId();
	  QueryManager::AcceptState acceptState;
	  //cout << login << endl << password << endl << amount << endl;
	  {
		QueryManager queryManager;
		queryManager.begin();
		cach( a_tx.messageFactory(), queryManager );
		acceptState = queryManager.getAcceptState( 
												  login
												  ,password
												  ,amount 
												  ,id
												  );
		a_tx.setAcceptState( acceptState );
		//doy de alta la peticion de abono en la base de datos
		if( 
		   currentIn_p->getReceiverName().substr(0,4) 
		   != PortManager::webPortNamePrefix 
		   )
		  {
			queryManager.addTx(
							   id
							   ,login
							   ,amount
							   ,currentIn_p->getTelephone()
							   ,currentIn_p->getSystemsTraceAuditNumber()
							   ,currentIn_p->getAcceptor()
							   ,currentIn_p->getReceptor()
							   ,currentIn_p->getTransmissionDateTime()
							   );
		  }
		queryManager.isCommit(true);
	  }
	  stringstream sstream;
	  sstream<<"InitialRC0200::apply:\n\n\tTx: -"<<id<<"-, AcceptState = -"<< QueryManager::acceptStateToString(acceptState)<<"-.";
	  Log::instance().log( sstream.str().c_str() );
	  a_tx.setState( Tx::TESTED );
	  a_tx.setEvent( Tx::DONE );
	}catch(Exception& e){
	  a_tx.setAcceptState( QueryManager::NO_ACCEPTED );
	  a_tx.setState( Tx::INITIAL );
	  a_tx.setEvent( Tx::RC0200_EXCEPTION );
	  e 
		<< endl
		<< "catch in InitialRC0200::apply."
		<< endl;
	  Log::instance().log( e.getMessage().c_str() );
	}catch(std::exception& e){
	 a_tx.setAcceptState( QueryManager::NO_ACCEPTED );
	 a_tx.setState( Tx::INITIAL );
	 a_tx.setEvent( Tx::RC0200_EXCEPTION );
	 stringstream sstream;
	 sstream
	   << e.what()
	   << endl
	   << "< MSG 005111 >: catch in InitialRC0200::apply."
	   << endl;
	 Log::instance().log( sstream.str().c_str() );
	}catch(...){
	  a_tx.setState( Tx::INITIAL );
	  a_tx.setEvent( Tx::RC0200_EXCEPTION );
	  a_tx.setAcceptState( QueryManager::NO_ACCEPTED );
	  stringstream sstream;
	  sstream
		<< "< MSG 005112 >: ???::exception:\n\tOcurrio una excepcion no identificada en solicitud de aceptacion de abono. Se considerara como no aceptada."
		<< "catch in InitialRC0200::apply."
		<< endl;
	  Log::instance().log( sstream.str().c_str() );
	}//catch
}

#endif
