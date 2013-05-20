#ifndef ECC_WT0200_RT0210A_CPP
#define ECC_WT0200_RT0210A_CPP

#include "WT0200RT0210A.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"

using ecc::Tx;
using ecc::Message;
using ecc::WT0200RT0210A;
using ecc::QueryManager;
using ecc::Exception;

WT0200RT0210A::WT0200RT0210A()
{
  d_txState = Tx::WT0200;
  d_event = Tx::RT0210A;
}



void WT0200RT0210A::apply(Tx& a_tx)
{
  try
	{
	  Message* currentIn_sp = a_tx.getCurrentIn();

	  //actualizo en base de datos
	  string responseCode = Message::RESPONSE_CODE_APPROVED;
	  string authorisationCode = currentIn_sp->getAuthorisationCode();
	  {
		QueryManager queryManager;	  
		queryManager.begin();
		queryManager.updateTx(
							  a_tx.getId()
							  ,ecc::pp::txs::state::COMMIT
							  ,ecc::pp::txs::evolve::PASIVE
							  ,responseCode
							  ,authorisationCode
							  );
		queryManager.isCommit(true);
		//aqui cacho los valores que voy a utilizar despues
		cach(a_tx.messageFactory(), queryManager);
	  }
	  //lo mando
	  wc0210a(a_tx);
	  a_tx.setState( Tx::END );
	  a_tx.setEvent( Tx::DONE );
	}catch(Exception& e){
	  a_tx.setState( Tx::WT0200 );//todo: antes estaba Tx::WC0210A y lo cambie a WT0200
	  a_tx.setEvent( Tx::RT0210A_EXCEPTION );
	  //string responseCode = Message::RESPONSE_CODE_LOCAL_DENIED;
	  e
		<< "Exception:\n\tSe rembolzara el monto correspondiente."
		<< endl
		<< "catch in WT0200RT0210A."
		<< endl;
	  //	  Log::instance().log( e.getMessage().c_str() );
	  throw e;
	}
} 

#endif
