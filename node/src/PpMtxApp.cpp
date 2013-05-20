#ifndef ECC_PPMTX_APP_CPP
#define ECC_PPMTX_APP_CPP

#include <vector>
#include <string>
#include <iostream>

using std::vector;
using std::string;

//#include "mpatrol.h"
#include "PpMtxApp.hpp"
#include "Emisor.hpp"
#include "Context.hpp"
#include "IOException.hpp"
#include "Exception.hpp"
#include "NotFoundException.hpp"
#include "BadArgumentException.hpp"
#include "Tx.hpp"
#include "DbQueueMonitor.hpp"
#include "SyncMap.hpp"
#include "SyncQueue.hpp"
#include "Log.hpp"
#include "Performer.hpp"
#include "QueryManager.hpp"
#include "Message.hpp"
#include "MessageFactory.hpp"
#include "DbQueueMonitor.hpp"
#include "DbPool.hpp"

using ::ecc::PpMtxApp;
using ::ecc::Emisor;
using ::ecc::Context;
using ::ecc::Exception;
using ::ecc::IOException;
using ::ecc::NotFoundException;
using ::ecc::BadArgumentException;
using ::ecc::Tx;
using ::ecc::SyncMap;
using ::ecc::SyncQueueAcceptor;
using ::ecc::SyncQueueReceptor;
using ::ecc::Log;
using ::ecc::PerformerAcceptor;
using ::ecc::PerformerReceptor;
using ::ecc::QueryManager;
using ::ecc::Message;
using ::ecc::MessageFactory;
using ::ecc::DbQueueMonitor;
using ::db::DbPool;

PpMtxApp* PpMtxApp::d_ppMtxApp_p = 0;

/**
 *
 */
PpMtxApp& PpMtxApp::instance()
{
  if(!d_ppMtxApp_p)
	{
	  ost::MutexLock mutexLock( getMutex_r() );
	  if(!d_ppMtxApp_p)
		create();
	}
  return *d_ppMtxApp_p;
}//instance


/**
 *
 */
void PpMtxApp::create()
{
  static PpMtxApp ppMtxApp;
  d_ppMtxApp_p = &ppMtxApp;
}//create



/**
 *
 */
PpMtxApp::PpMtxApp()
  :d_isReady(false)//al inicio no esta listo para responder aceptando logons de otras maquinas
{
}//EccApp

/**
 *
 */
PpMtxApp::~PpMtxApp()
{
  d_ppMtxApp_p = 0;
  //deveria borrar los servidores o se lo dejo al smart pointer?
}//~PpMtxApp

/**
 *
 */
void PpMtxApp::main()
{
  startup();
  d_semaphore.wait();//espera a desbloquearse esto lo maneja en el main.cpp mediante el SIGUSR1 handler
  shutdown();
}//

/**
 *
 */
void PpMtxApp::startup()
  // throws IOException 
{
  try
	{
	  Log::instance().log("< MSG 005306 >: PpMtxApp::startup:\n\n\tInitial Recover...");
	  recoverInitial();

	  //preparando fabrica de mensajes
	  Log::instance().log("< MSG 005307 >: PpMtxApp::startup:\n\n\tStartup Connectors...");
	  d_isReady = true;//este es un control global, de control de panico, si es establecido a false, en por ejemplo una situacion de falla en la base de datos o una falla de sistema global deberia ponerse en false, no esta totalmente implementado, solo a nivel de respuesta a ecos de peticion de logon, si pidieron logon y esta es false, se les responde con un logon de denegada peticion. Ademas justo aqui debe de establecerse como true, porque el portManager establece arranca los connectores y si no es ready = true entonces se contestara como false las peticiones de logon
	  startupPortManager();

	  Log::instance().log("< MSG 005308 >: PpMtxApp::startup:\n\n\tStartup Network Managment Code Manager...");
	  startupEcos();

	  Log::instance().log("< MSG 005309 >: PpMtxApp::startup:\n\n\tStartup Performers...");
	  startupPerformers();

	  Log::instance().log("< MSG 005310 >: PpMtxApp::startup:\n\n\tStartup DB queue monitor ...");
	  startupDbQueueMonitor();

	  Log::instance().log( "< MSG 005311 >: PpMtxApp::startup:\n\n\tCompletada secuencia de arranque.");

	}catch(Exception e){
	  e
		<< "catch in PpMtxApp::startup\n"
		;
	  cerr << e.getMessage();
	  throw e;
	}//catch
}//startup

/**
 *
 */
void PpMtxApp::startupEcos()
{
  Emisor::instance().loadContext();
  Emisor::instance().detach();
}//

/**
 *
 */
void PpMtxApp::startupPerformers()
{
  PerformerAcceptor::instance().detach();
  PerformerReceptor::instance().detach();
}//startupPerformers


/**
 *
 */
void PpMtxApp::startupDbQueueMonitor()
{
  //  DbQueueMonitor::instance().detach();//todo:detach o start???
  DbQueueMonitor::instance().detach();//todo:detach o start???
  DbQueueMonitor::instance().isReady(true);
}//startupDbQueueMonitor

/**
 *
 */
void PpMtxApp::startupPortManager()
{
  MessageFactory::loadContext();
  Tx::loadContext();
  PortManager::loadContext();
  PortManager::instance().activeAllPorts();
  PortManager::instance().detach();
  //////// creacion del connector con la telefonica
  //cargo los valores para las Tx de config
}//startup

/**
 *
 *
 void PpMtxApp::clear()
 {
 //limpiando connectores
  Client::instance().clear();
  PerformerT::instance().clear();
  PerformerC::instance().clear();
  DbPool::instance().clear();
  }//
*/  

/**
 *
 */
void PpMtxApp::shutdown()
{
  Log::instance().log("< MSG 005297 >: PpMtxApp::shutdown:\n\n\tIniciando secuencia de shutdown...");
	  
  //
  Log::instance().log("< MSG 005298 >: PpMtxApp::shutdown:\n\n\tApagando Emisor de Ecos");
  Emisor::instance().isQuit(true);
  Emisor::instance().join();
  //
  Log::instance().log("< MSG 005299 >: PpMtxApp::shutdown:\n\n\tDesactivando PortManager");
  PortManager::instance().isQuit(true);
  PortManager::instance().join();

  //
  Log::instance().log("< MSG 005300 >: PpMtxApp::shutdown:\n\n\tDesactivando Connectors...");
  try
	{
	  PortManager::instance().desactiveAllPorts();
	}catch(Exception& e){//para cachar cuando no se pudo conectar y que de todas maneras se termine adecuadamente la secuencia de shutdown
	  e
		<< "catch in PpMtxApp::shutdown\n"
		;
	  Log::instance().log( e.getMessage().c_str() );
	}
  //los disconnectors no son ciclicos assi que no hace falta su quit

  //
  Log::instance().log("< MSG 005301 >: PpMtxApp::shutdown:\n\n\tDesactivando DbQueueMonitor...");
  DbQueueMonitor::instance().isReady(false);
  DbQueueMonitor::instance().isQuit(true);
  DbQueueMonitor::instance().join();

  //
  Log::instance().log("< MSG 005302 >: PpMtxApp::shutdown:\n\n\tCerrando conexiones con base de datos...");
  DbPool::instance().closeAll();

  //
  Log::instance().log("< MSG 005303 >: PpMtxApp::shutdown:\n\n\tDesactivando PerformerAcceptor");
  PerformerAcceptor::instance().isQuit(true);
  PerformerAcceptor::instance().join();

  //
  Log::instance().log("PpMtxApp::shutdown:\n\n\tDesactivando PerformerReceptor");
  PerformerReceptor::instance().isQuit(true);
  PerformerReceptor::instance().join();

  //
  Log::instance().log("< MSG 005304 >: PpMtxApp::shutdown:\n\n\tSe esta dando tiempo para que terminen las transacciones pendientes...");

  int trialN = 0;

  int MAX_TRIALS_FREE_TX = 5;
  string maxTrialsFreeTx = Context::instance().getProperty_r("max_trials_free_tx");
  int maxTrialsFreeTx_int;
  if( !( stringstream( maxTrialsFreeTx ) >> maxTrialsFreeTx_int) )
	maxTrialsFreeTx_int = MAX_TRIALS_FREE_TX;

  int INTERVAL_TRIALS_FREE_TX = 3000;
  string intervalTrialsFreeTx = Context::instance().getProperty_r("interval_trials_free_tx");
  int intervalTrialsFreeTx_int;
  if( !( stringstream( intervalTrialsFreeTx ) >> intervalTrialsFreeTx_int) )
	intervalTrialsFreeTx_int = INTERVAL_TRIALS_FREE_TX;


  for(;;)
	{
	  trialN++;
	  if( trialN > maxTrialsFreeTx_int ) 
		{
		  break;
		}
	  int txPendingN = SyncMap::instance().size();
	  if( txPendingN > 0 )
		{
		  stringstream sstream;
		  sstream 
			<< "< MSG 005305 >: PpMtxApp:shutdown:\n\n\tEsperando a que se liberen -"
			<< txPendingN
			<< "- transacciones pendientes. Intento -"
			<< trialN
			<< "- de -"
			<< MAX_TRIALS_FREE_TX
			<< "-."
			<< endl
			;
		  Thread::sleep( intervalTrialsFreeTx_int );//espera 3 segundos
		}
	  else
		{
		  break;
		}
	}//while
}//shutdown

/**
 *
 */
void PpMtxApp::recoverInitial()
{
  //los que no se alcanzaron a mandar
  Log::instance().log("< MSG 005312 >: PpMtxApp::recoverInitial\n\n\tAbortando transacciones INITIAL y WT0200.");
  std::vector<Message*> messages;
  {
	QueryManager queryManager;
	queryManager.begin();
	queryManager.abortUnfinishedTx();//INITIAL Y ACTIVE, y borra encoladas en  queue
	queryManager.isCommit(true);
  }
}//

/** todo: checar si esta biien esto... y limitarlo solo para las no reversadas del dia
 *
 */
void PpMtxApp::recoverReversing(string a_receptor)
{
  //los que no se alcanzaron a mandar
  Log::instance().log("< MSG 005313 >: PpMtxApp::recoverReversing\n\n\tAbortando transacciones REVERSING.");
  std::vector<Message*> messages;
  {
	QueryManager queryManager;
	queryManager.begin();
	queryManager.loadForReverse(a_receptor, messages);//reinicia las que se estaban REVERSANDO
	queryManager.isCommit(true);
  }
  std::vector<Message*>::iterator itr;
  
  for(itr=messages.begin(); itr!=messages.end(); itr++)
	{
	  Message* message_p = *itr;
	  SyncQueueAcceptor::instance().push( message_p );
	}
}//

/**
 * regresa cuando de la entrada estandar se obtiene una linea que diga 'quit'
 */
void PpMtxApp::whileQuit()
{
  string input;

  cout << "PpMtx [quit,txs,queueReceptor,queueAcceptor]>>"<<endl;
  do
	{
	  getline(cin,input);
	  if( input == "txs" )
		{
		  SyncMap::instance().display();
		}
	  if( input == "queueReceptor" )
		{
		  SyncQueueReceptor::instance().display();
		}
	  if( input == "queueAcceptor" )
		{
		  SyncQueueAcceptor::instance().display();
		}
	}while( input != "quit" );
}//quitListener

/**
 *
 */
ost::Mutex& PpMtxApp::getMutex_r()
{
  static ost::Mutex mutex;
  return mutex;
}//getMutex_r

/**
 *
 */
bool PpMtxApp::isReady()
{
  return d_isReady;
}//

/**
 *
 */
bool PpMtxApp::isReady(bool a_isReady)
{
  if( ! a_isReady )
	d_semaphore.post();//para desbloquearlo y que se apague
  d_isReady = a_isReady;
}//


#endif
