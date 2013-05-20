#ifndef ECC_INITIAL_RC0200_EXCEPTION_CPP
#define ECC_INITIAL_RC0200_EXCEPTION_CPP

#include "InitialRC0200Exception.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"

using ecc::Tx;
using ecc::Message;
using ecc::InitialRC0200Exception;
using ecc::QueryManager;
using ecc::Exception;

InitialRC0200Exception::InitialRC0200Exception()
{
  d_txState = Tx::INITIAL;
  d_event = Tx::RC0200_EXCEPTION;
}

void InitialRC0200Exception::apply(Tx& a_tx)
{
  a_tx.setState( Tx::END );//estos son importantes aqui si hay exepcion
  a_tx.setEvent( Tx::DONE );//y este tambien
  string responseCode = Message::RESPONSE_CODE_LOCAL_DENIED;
  //actualizo en base de datos
  {
	QueryManager queryManager;
	queryManager.begin();
	queryManager.updateTx(
						  a_tx.getId()
						  ,ecc::pp::txs::state::ABORT
						  ,ecc::pp::txs::evolve::PASIVE
						  ,responseCode
						  ,"------"
						  );
	cach( a_tx.messageFactory(), queryManager );
	queryManager.isCommit(true);
  }
  wc0210r( a_tx, responseCode );
}

#endif
