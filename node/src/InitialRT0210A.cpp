#ifndef ECC_INITIAL_RT0210A_CPP
#define ECC_INITIAL_RT0210A_CPP

#include "InitialRT0210A.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"
#include "NotFoundException.hpp"
#include <iostream>
using namespace std;
using ecc::Tx;
using ecc::Message;
using ecc::InitialRT0210A;
using ecc::QueryManager;
using ecc::Exception;
using ecc::NotFoundException;

InitialRT0210A::InitialRT0210A()
{
  d_txState = Tx::INITIAL;
  d_event = Tx::RT0210A;
}

void InitialRT0210A::apply(Tx& a_tx)
{
  a_tx.timedWait();//parra quitar el primer counter del semaforo
  try
	{
	  {
		Message* currentIn_sp = a_tx.getCurrentIn();

		string authorisationCode = currentIn_sp->getAuthorisationCode();
		string responseCode = currentIn_sp->getResponseCode();
		string id = currentIn_sp->getTxId();
		//db::revTx(d_id,"ACTIVE",responseCode,authorisationCode);
		QueryManager queryManager;
		queryManager.begin();
		queryManager.updateTx(
							  id
							  ,ecc::pp::txs::state::REVERSING
							  ,ecc::pp::txs::evolve::ACTIVE
							  ,responseCode
							  ,authorisationCode
							  );
		//aqui cacho los valores que voy a utilizar despues
		a_tx
		  .messageFactory()
		  .setCacheOriginalDataElements(
										queryManager
										.createOriginalDataElements( a_tx.getId() )
										);
		cach(a_tx.messageFactory(),queryManager);
		wt0420(a_tx);//checar que esta funcion solo puede mandar excepciones antes de mandar el mensaje	
		a_tx.setState( Tx::WT0420 );
		a_tx.setEvent( Tx::TIME_OUT );
		queryManager.isCommit(true);
	  }
	  a_tx.timedWait();
	}catch(NotFoundException& e){//si no esta registro alguno de la transaccion
	  a_tx.setState( Tx::END);
	  a_tx.setEvent( Tx::EXCEPTION );
	  e
		<< "catch in InitialRT0210A::apply\n"
		;
	  throw;
	}catch(Exception& e){
	  a_tx.setState( Tx::WT0421 );
	  a_tx.setEvent( Tx::TIME_OUT );
	  e
		<< "catch in InitialRT0210A::apply\n"
		;
	  throw;
	}catch(...){
	  a_tx.setState( Tx::WT0421 );
	  a_tx.setEvent( Tx::TIME_OUT );
	  throw;
	}
}

#endif
