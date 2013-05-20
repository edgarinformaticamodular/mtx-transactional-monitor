#ifndef ECC_WT0200_TIME_OUT_CPP
#define ECC_WT0200_TIME_OUT_CPP

#include "WT0200Timeout.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"

using ecc::Tx;
using ecc::Message;
using ecc::WT0200Timeout;
using ecc::QueryManager;
using ecc::Exception;

WT0200Timeout::WT0200Timeout()
{
  d_txState = Tx::WT0200;
  d_event = Tx::TIME_OUT;
}



void WT0200Timeout::apply(Tx& a_tx)
{
  try
	{//aqui no me importa recuperarme si no logro avisarle al call center  
	  Message* firstIn_sp = a_tx.getFirstIn();
	  string login = firstIn_sp->getLogin();
	  string amount = firstIn_sp->getAmount();
	  string responseCode = Message::RESPONSE_CODE_AUTORIZADOR_NO_DISPONIBLE_MIDDLEWARE;
	  {
		QueryManager queryManager;
		queryManager.begin();
		queryManager.plusCredit( login, amount );
		//queryManager.isCommit(true);
		//}//todo:checar si vale la pena juntar las dos como una sola transaccion
	  //actualizo en base de datos
	  //{las junte facil:
		//QueryManager queryManager;
		//queryManager.begin();
		queryManager.updateTx(
							  a_tx.getId()
							  ,ecc::pp::txs::state::ABORT
							  ,ecc::pp::txs::evolve::PASIVE
							  ,responseCode
							  ,"------"
							  );
		cach(a_tx.messageFactory(),queryManager);
		queryManager.isCommit(true);
	  }

	  wc0210r(a_tx, responseCode);
	  a_tx.setState( Tx::END );
	  a_tx.setEvent( Tx::DONE );
	}catch(Exception& e){
	  a_tx.setState( Tx::END );
	  a_tx.setEvent( Tx::EXCEPTION );
	  e
		<< "catch in WT0200Timeout::apply\n"
		;
	  throw e;//todo:cambie el log de lugar
	}catch(...){
	  a_tx.setState( Tx::END );
	  a_tx.setEvent( Tx::EXCEPTION );
	  throw;//todo: cambie el log de lugar
	}
}

#endif
