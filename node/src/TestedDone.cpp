#ifndef ECC_TESTED_DONE_CPP
#define ECC_TESTED_DONE_CPP

#include "TestedDone.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"
#include <cc++/socket.h>

using ecc::Tx;
using ecc::Message;
using ecc::TestedDone;
using ecc::QueryManager;
using ecc::Exception;

TestedDone::TestedDone()
{
  d_txState = Tx::TESTED;
  d_event = Tx::DONE;
}

void TestedDone::apply(Tx& a_tx)
{
  QueryManager::AcceptState acceptState = a_tx.getAcceptState();
  string id = a_tx.getId();
  try
	{
	  switch( acceptState )
		{
		case QueryManager::ACCEPTED:
		  {
			wt0200(a_tx);
			a_tx.setState( Tx::WT0200 );
			a_tx.setEvent( Tx::TIME_OUT );
			a_tx.timedWait();
			break;
		  }
		default:
		  {
			string responseCode = QueryManager::toResponseCode(acceptState);
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
			  queryManager.isCommit(true);
			}

			wc0210r( a_tx, responseCode );
			a_tx.setEvent( Tx::DONE );
			a_tx.setState( Tx::END );
			break;
		  }
		}//switch
	}catch(Exception& e){
	  a_tx.setState( Tx::TESTED );
	  a_tx.setEvent( Tx::DONE_EXCEPTION );
	  stringstream sstream;
	  sstream 
		<< "< MSG 457 >: Exception:\n\t -Tx- -" << id << "- "
		<< endl
		<< e.getMessage()
		<< "catch in TestedDone::apply"
		<< endl;
	  Log::instance().log( sstream.str().c_str() );
	}
}

#endif
