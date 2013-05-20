#ifndef ECC_TX_CPP
#define ECC_TX_CPP

#include "Asserter.hpp"
#include "Tx.hpp"
#include "Exception.hpp"
#include "Message.hpp"
#include "Context.hpp"
//#include "Asserter.hpp"
#include "SyncVector.hpp"
#include "SyncMap.hpp"
#include "QueryManager.hpp"
#include <sstream>
#include "Log.hpp"
#include "NotFoundException.hpp"
#include "ScopeGuard.h"
#include "PortManager.hpp"
#include "Evolver.hpp"
#include "PortManager.hpp"
#include "Rule.hpp"

#include <iostream>

#define x(a) cout << d_id<<","<<a << endl;

using ::ecc::Tx;
using ::ecc::Exception;
using ::ecc::Message;
using ::ecc::Context;
using ::ecc::SyncVector;
using ::ecc::QueryManager;
using ::ecc::Log;
using ::ecc::PortManager;
using ::ecc::NotFoundException;
using ::ecc::SyncMap;
using ::ecc::PortManager;
using ::ecc::Rule;

using std::cout;
using std::endl;
using std::string;
using std::stringstream;

int Tx::MAX_TRY = 5;
int Tx::MAX_VARCHAR_ESTADO = 10;
int Tx::MAX_VARCHAR_TRAMA = 2000;
int Tx::WAIT_TIME_FIRST = 5000;
int Tx::WAIT_TIME =  5000;
int Tx::MAX_ITERATIONS =  15;

/**
 *
 */
void Tx::loadContext()
{
  string maxReverseTrialsT 
	= Context::instance().getProperty_r("max_reverse_trials");
  if( !( stringstream( maxReverseTrialsT ) >> MAX_TRY ) )
	MAX_TRY = 5;
  
  string txTimeoutT 
	= Context::instance().getProperty_r("tx_timeout");
  if( !( stringstream( txTimeoutT ) >> WAIT_TIME ) )
	WAIT_TIME = 5000;
}//

/** 
 *
 */
Tx::Tx()
  :Thread(2)
  ,d_try(0)
  ,d_messageIn(-1)//-1 porque es el indice del primer elemento y cuando no hay nadie, no seria ni siquiera 0 puesto que cero ya es el indice el primer elemento
  ,d_iterations(0)
  ,d_isTryAgain(true)
  ,d_acceptState( QueryManager::NO_ACCEPTED )
  ,d_cursor(-1)
   {
	 d_state = Tx::INITIAL;
	 d_event = Tx::UNDEFINED;
	 d_eventOld = Tx::UNDEFINED;
   }//Tx


/**
 *
 */
void Tx::final()
{
  stringstream sstream;
  sstream << "Tx::final: Tx: -"<<d_id<<"- deleted.";
  Log::instance().log( sstream.str().c_str() );
  delete this;
}//

/**
 *
 */
Tx::~Tx()
{
  terminate();
}//

/**todo: aqui devo de meter un semaforo 
 * para indicar  cuando no meter nuevos mensajes
 * en sincronia con el hilo de evolve
 *
 */
bool Tx::addIn(Message* a_message_p)
{
  bool aux = d_messagesIn.pushBack( a_message_p );
  return aux;
}//addIn

/** 
 *
 */
void Tx::addOut(Message* a_message_p)
{
  d_messagesOut.pushBack( a_message_p );
}//addOut


/**
 *
 */
string Tx::getId()
{
  return d_id;
}//getId

/**
 *
 */
void Tx::setId(string a_id)
{
  d_id = a_id;
}//setId

/**
 *
 */
void Tx::display() 
{
  cout << "================================================"<< endl;
  cout << "Tx::display\n\t Transaccion No. -"<<d_id<<"-"<<endl;
  cout << " Estado: -"<<d_state<<"-, evento: -"<< d_event<<"-."<<endl;
  cout << "Mensajes In:"<<endl;
  d_messagesIn.display();
  cout << "Mensajes Out:"<<endl;
  d_messagesOut.display();
}//display

/**
 *
 */
void Tx::run(void)
{
  setCancel( cancelDefault );
  try
	{
	  try
		{
		  //sleep(18000);
		  //el preproces se dio en el Performer onMessage
		  process();
		  postprocess();
		}catch(Exception& e){
		  e	<< "\ncatch in Tx::run.\n";
		  Log::instance().log( e.getMessage().c_str() );
		}catch(std::exception& e){
		  stringstream sstream;
		  sstream
			<< e.what()
			<< endl
			<< "catch in Tx::run."
			<< endl;
		  Log::instance().log( sstream.str().c_str() );
		}catch(...){
		  Log::instance().log("Tx::run: ???::exception\ncatch in Tx::run.");
		}
	}catch(...){}
  exit();

}//run

/**todo: revisar const y todo
 * ecc aqui me quede
 */
Message* Tx::getCurrentIn()
{
  Message* message_p = d_messagesIn.get( d_messageIn );
  if( ! message_p )
	{
	  NotFoundException e;
	  e.getStream() 
		<< "< MSG 077 >: NotFoundException:"
		<< endl
		<< "\tNo se ha encontrado el ultimo mensaje de entrada con -"
		<< d_messageIn
		<< "-."
		<< endl
		<< "throw in Tx::getCurrentIn.\n"
		<< endl;
	  throw e;
	}
  return message_p;
}//getCurrentIn

/**
 *
 */
Message* Tx::getFirstIn()
{
  Message* message_p = d_messagesIn.first();
  if( ! message_p )
	{
	  NotFoundException e;
	  e.getStream() 
		<< "< MSG 078 >: NotFoundException:"
		<< endl
		<< "\tNo se ha encontrado el ultimo mensaje de entrada con -"
		<< d_messageIn
		<< "-."
		<< endl
		<< "throw in Tx::getFirstIn.\n"
		<< endl;
	  throw e;
	}//if
  return message_p;
}//getFirstIn


/**
 *
 */
Tx::TxState Tx::getState()
{
  return d_state;
}//getState


/**
 *
 */
Tx::Event Tx::getEvent()
{
  return d_event;
}//

/**
 *
 */
void Tx::setEvent(Tx::Event a_event)
{
  d_event = a_event;
}//

/**
 *
 */
QueryManager::AcceptState Tx::getAcceptState()
{
  return d_acceptState;
}//

/**
 *
 */
void Tx::setAcceptState(QueryManager::AcceptState a_acceptState)
{
  d_acceptState = a_acceptState;
}//

/** todo: aqui falta ponerle el que cahce las exceptiones dentro del while para que rescate la iteraccion
 *
 */
void Tx::process(void)
{
  try
	{
	  //indica la pocicion tentativa del mensaje siguiente
	  int cursor = -1;
	  while( d_state != END )
		{
		  try
			{
			  d_iterations++;
			  //checo a ver si no se alcanzo el maximo numero de iteraciones
			  if( d_iterations > MAX_ITERATIONS )
				{
				  stringstream sstream;
				  sstream
					<< "Tx::process:\n\n\tMax Iterations Limit Reached in Tx: -"<<d_id<<"-. \nCancel procced." << endl;
				  Log::instance().log(sstream.str().c_str());
				  d_event = ITERATION_LIMIT_REACHED;
				  d_state = END;
				  break;
				}//considerando que d_messageIn sera -1 al inicio
			  else if( static_cast<unsigned int>(cursor+1) < d_messagesIn.size() )
				{
				  cursor++;
				  d_messageIn = cursor;
				  Rule::setupEvent(*this);//esto aplica para el mensaje actual
				}
			  logState();
			  Evolver::instance().evolve(*this);
			}catch(Exception& e){
			  e
				<< endl
				<< "catch in Tx::process."
				<< endl;
			  Log::instance().log( e.getMessage().c_str() );
			}catch(std::exception& e){
			  stringstream sstream;
			  sstream
				<< e.what()
				<< endl
				<< "catch in Tx::process."
				<< endl;
			  Log::instance().log( sstream.str().c_str() );
			}catch(...){
			  Log::instance().log( 
								  "???::exception:\n\tUn tipo no esperado de excepcion ocurrio mientras se prosesaba la transaccion.\ncatch in Tx::process. Canel proced.");
			  d_state = END;
			  d_event = EXCEPTION;
			}//catch
		}//while
	}catch(...){
	  Log::instance().log( 
						  "???::exception:\n\tUn tipo no esperado de excepcion ocurrio mientras se prosesaba la transaccion.\ncatch in Tx::process. fuera de while.");
	  d_state = END;
	  d_event = EXCEPTION;
	}//catch
}//process
  


/**
 *
 */
void Tx::postprocess(void)
{
  //el ultimo log step menos el final, 
  logState();
  //con lo siguiente ya no le pueden entrar mas mensajes
  SyncMap::instance().remove( d_id );//borro lista de transacciones
//  db::updateTx(d_id,"COMMIT",responseCode,authorisationCode);
  if( d_isTryAgain )
	d_messagesIn.purge(d_messageIn, d_messagesPurge);

  d_messagesIn.deleteAll();
  d_messagesOut.deleteAll();
  d_messagesPurge.purge();
}//postprocess

/**
 * esta funcion siempre se logea al inicio de cada estado,osea que al
 * inspeccionarla en la bitacora se identifica que estado se procedio a evolucionar
 */
void Tx::logState()
{
  stringstream sstream;
  sstream<<"Tx::logState:\n\n\tTx: -"<<d_id<<"-, -"<< txStateToString(d_state)<<"-, -"<<eventToString(d_event)<<"-."<<endl;

  Log::instance().log(sstream.str().c_str() );
}//

/**
 * esta funcion siempre se logea al final de cada accion. osea que si se ve
 * en la bitacora significa que esa accion se completo
 */
void Tx::logAction(
				   string a_action
				   ,Message* a_message_p
				   ,TramaType a_tramaType
				   )
{
  stringstream sstream;
  sstream
	<< "Tx::logAction:\n\n\tTx: -"<<d_id<<"-, -" << a_action 
	<<"-, audit: -"<<a_message_p->getSystemsTraceAuditNumber()<<"-"<<endl;
  
  switch( a_tramaType )
	{
	case TRAMA_PARTITION:
	  sstream << a_message_p->getTramaPartition();
	  break;
	case TRAMA_IN:
	  sstream << a_message_p->getTramaIn();
	  break;
	case TRAMA_OUT:
	  sstream << a_message_p->getTramaOut();
	  break;
	}//switch
  Log::instance().log(sstream.str().c_str() );
}//

/**
 *
 */
const char* Tx::txStateToString(TxState a_txState)
{
  switch(a_txState)
	{
	case INITIAL:
	  return "INITIAL";
	case TESTED:
	  return "TESTED";
	case WT0200:
	  return "WT0200";
	case WT0420:
	  return "WT0420";
	case WT0421:
	  return "WT0421";
	case WC0210R:
	  return "WC0210R";
	case WC0210R_WT0420:
	  return "WC0210R_WT0420";
	case FLUSH:
	  return "FLUSH";
	case END:
	  return "END";
	default:
	  return "TxEstate???";
	}
}//txStateToString

/**
 *
 */
const char* Tx::eventToString(Event a_event)
{
  switch( a_event )
	{
	case UNDEFINED:
	  return "UNDEFINED";
	case EXCEPTION:
	  return "EXCEPTION";
	case TIME_OUT:
	  return "TIME_OUT";
	case TIME_OUT_EXCEPTION:
	  return "TIME_OUT_EXCEPTION";
	case DONE:
	  return "DONE";
	case DONE_EXCEPTION:
	  return "DONE_EXCEPTION";
	case RC0200:
	  return "RC0200";
	case RC0200_EXCEPTION:
	  return "RC0200_EXCEPTION";
	case RT0210A:
	  return "RT0210A";
	case RT0210A_EXCEPTION:
	  return "RT0210A_EXCEPTION";
	case RT0210R:
	  return "RT0210R";
	case RT0210R_EXCEPTION:
	  return "RT0210R_EXCEPTION";
	case RT0420:
	  return "RT0420";
	case RC0420:
	  return "RC0420";
	case RT0430:
	  return "RT0430";
	case RT9200:
	  return "RT9200";
	case RC0800:
	  return "RC0800";
	case RC0810:
	  return "RC0810";
	case RT0800:
	  return "RT0800";
	case RT0810:
	  return "RT0810";
	case ITERATION_LIMIT_REACHED:
	  return "ITERATION_LIMIT_REACHED";
	case TRY_LIMIT_REACHED:
	  return "TRY_LIMIT_REACHED";
	default:
	  return "event???";
	}
}//eventToString


/**
 *
 */
void Tx::timedWait()
{
  d_messagesIn.getTimedSemaphore_r().timedWait(WAIT_TIME);
}//


/**
 *
 */
void Tx::setState(TxState a_txState)
{
  d_state = a_txState;
}//

/**
 *
 */
void Tx::incrementTry()
{
  d_try++;
}//


/**
 *
 */
int Tx::getTry()
{
  return d_try;
}//


/**
 *
 */
bool Tx::isTryAgain(bool a_isTryAgain)
{
  bool old = d_isTryAgain;
  d_isTryAgain = a_isTryAgain;
  return old;
}//

/**
 *
 */
void Tx::setCursor(int a_cursor)
{
  d_cursor = a_cursor;
}//

/**
 *
 */
int Tx::getCursor()
{
  return d_cursor;
}//

#endif
