#ifndef ECC_TESTED_DONE_EXCEPTION_CPP
#define ECC_TESTED_DONE_EXCEPTION_CPP

#include "TestedDoneException.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"
#include <cc++/socket.h>

using ecc::Tx;
using ecc::Message;
using ecc::TestedDoneException;
using ecc::QueryManager;
using ecc::Exception;

TestedDoneException::TestedDoneException()
{
  d_txState = Tx::TESTED;
  d_event = Tx::DONE_EXCEPTION;
}

void TestedDoneException::apply(Tx& a_tx)
{
  QueryManager::AcceptState acceptState = a_tx.getAcceptState();
  string id = a_tx.getId();
  switch( acceptState )
	{
	case QueryManager::ACCEPTED:
	  {
		try
		  {
			Message* currentIn_sp = a_tx.getCurrentIn();
			
			string login = currentIn_sp->getLogin();
			string password = currentIn_sp->getPassword();
			string amount = currentIn_sp->getAmount();

			string responseCode = Message::RESPONSE_CODE_AUTORIZADOR_NO_DISPONIBLE_MIDDLEWARE;
			stringstream sstream;
			sstream//si fue aceptada la excepcion fue al intentar mandar a la telefonica
			  << "< MSG 000575 >: TestedDoneException::apply\n\n\texepcion al intentar mandar transaccion Id: -"
			  << id
			  << "-. Se rembolsa al affiliado."
			  << endl;
			Log::instance().log(sstream.str().c_str());
			{
			  QueryManager queryManager;
			  queryManager.begin();
			  queryManager.plusCredit( login, amount );

			  //queryManager.isCommit(true); mejor juntos
			  //}//todo:checar si vale la pena juntarlos en una misma transaccion
			//actualizo en base de datos
			//{las junto facil:
			  //QueryManager queryManager;
			  //queryManager.begin();mejor juntos
			  queryManager.updateTx(
									a_tx.getId()
									,ecc::pp::txs::state::ABORT
									,ecc::pp::txs::evolve::PASIVE
									,responseCode
									,"------"
									);
			  queryManager.isCommit(true);
			  cach(a_tx.messageFactory(), queryManager);//nuevos valores
			}

			wc0210r(a_tx, responseCode);
			a_tx.setState( Tx::END );
			a_tx.setEvent( Tx::DONE );
		  }catch(Exception& e){
			e
			  << "catch in TestedDoneException::apply\n"
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
	  break;
	default://si la excepcion fue al intentar mandar mensaje al acceptor no importa
	  //porque de todas formas era para decirle que no se pudo, y si fue en la base de datos, quedo leida en la bitacora y se debera analizar que paso manualmentek
	  {
		a_tx.setState( Tx::END );
		a_tx.setEvent( Tx::EXCEPTION );
		break;
	  }
	}//switch
}

#endif
