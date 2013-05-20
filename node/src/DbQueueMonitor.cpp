#ifndef ECC_DB_QUEUE_MONITOR_CPP
#define ECC_DB_QUEUE_MONITOR_CPP

#include <cc++/thread.h>
#include "Message.hpp"
#include "Context.hpp"
#include "DbQueueMonitor.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"
#include "SyncQueue.hpp"
#include "BadArgumentException.hpp"
#include "PpMtxApp.hpp"

#include <iostream>

using ecc::BadArgumentException;
using ecc::DbQueueMonitor;
using ecc::Log;
using ecc::SyncQueueAcceptor;
using ecc::Exception;
using ecc::Message;
using ecc::Context;
using ecc::QueryManager;
using ecc::PpMtxApp;

DbQueueMonitor* DbQueueMonitor::d_dbQueueMonitor_p = 0;

bool DbQueueMonitor::d_isDestroyed = false;

int DbQueueMonitor::INTERVAL_DEFAULT = 100;//100 milisegundos
/**
 *
 */

DbQueueMonitor& DbQueueMonitor::instance()
{
  if(!d_dbQueueMonitor_p)
	{
	  ost::MutexLock mutexLock( getMutex_r() );
	  if(!d_dbQueueMonitor_p)
		{
		  if(d_isDestroyed)
			{
			  onDeadReference();
			}
		  else
			{
			  create();
			}
		}
	}
  return *d_dbQueueMonitor_p;
}//instance

/**
 *
 */

void DbQueueMonitor::create()
{
  static DbQueueMonitor dbQueueMonitor;
  d_dbQueueMonitor_p = &dbQueueMonitor;
}//create

/**
 *
 */
void DbQueueMonitor::loadContext()
{
  try
	{
	  string interval_string 
		= Context::instance().getProperty_r("db_queue_interval");
	  if( !(stringstream( interval_string ) >> d_interval) )
		{
		  BadArgumentException e;
		  e
			<< "< MSG 023 >: Exception:\n\tValor invalido en archivo de configuracion, para el intervalo de tiempo en la variable -dbQueueInterval- : -" << interval_string << "-, se utilizara valor por default: -"<< INTERVAL_DEFAULT <<"-.";
		  throw e;
		}
	}catch(Exception& e){
	  Log::instance().log( e.getMessage().c_str() );
	  d_interval = INTERVAL_DEFAULT;
	}
}//

/**
 *
 */

void DbQueueMonitor::onDeadReference()
{
  create();
  new (d_dbQueueMonitor_p) DbQueueMonitor();
  //atexit(killPhoenix);
  d_isDestroyed = false;
}//onDeadReference

/**
 *
 */
DbQueueMonitor::~DbQueueMonitor()
{
  d_dbQueueMonitor_p = 0;
  d_isDestroyed = true;
  terminate();
}//~DbQueueMonitor


/**
 *
 */
void DbQueueMonitor::killPhoenix()
{
  d_dbQueueMonitor_p->~DbQueueMonitor();
}//killPhoenix

/**
 *
 */
DbQueueMonitor::DbQueueMonitor()
{
  loadContext();
}//DbQueueMonitor

/**
 *
 */
void DbQueueMonitor::run()
{
  vector<Message*> messages; 
  for(;;)
	{
	  try
		{
		  if( d_isQuit )
			break;
		  Thread::sleep(d_interval);
		  if( d_isReady )
		  {
			QueryManager queryManager;		 
			queryManager.begin();
			queryManager.loadMessages( messages );
			queryManager.isCommit(true);
		  }
		  vector<Message*>::iterator itr;
		  for(
			  itr = messages.begin();
			  itr != messages.end();
			  itr++
			  )
			{
			  push(*itr);
			}//for
		}catch(Exception& e){
		  e 
			<< "\ncatch int DbQueueMonitor::run.";
		  Log::instance().log(e.getMessage().c_str());
		  PpMtxApp::instance().isReady(false);
		}catch(std::exception& e){
		  stringstream sstream;
		  sstream
			<< e.what()
			<< "\ncatch int DbQueueMonitor::run.";
		  Log::instance().log( sstream.str().c_str());
		  PpMtxApp::instance().isReady(false);
		}catch(...){
		  stringstream sstream;
		  sstream
			<< "???::exception"
			<< "\ncatch int DbQueueMonitor::run.";
		  Log::instance().log( sstream.str().c_str());
		  PpMtxApp::instance().isReady(false);
		}
	  //purgo el vector para que no se encimen mensajes recibidos y se
	  //vuelvan a meter
	  messages.clear();
	}//while
}//run

/**
 *
 */
bool DbQueueMonitor::isQuit(bool a_isQuit)
{
  d_isQuit = a_isQuit;
}//

/**
 *
 */
void DbQueueMonitor::push(Message* a_message_p)
{
  SyncQueueAcceptor::instance().push( a_message_p );
}//push

/**
 *
 */
ost::Mutex& DbQueueMonitor::getMutex_r()
{
  static ost::Mutex mutex;
  return mutex;
}//getMutex_r

/**
 *
 */
bool DbQueueMonitor::isReady()
{
  return d_isReady;
}//

/**
 *
 */
bool DbQueueMonitor::isReady(bool a_isReady)
{
  bool old = d_isReady;
  d_isReady = a_isReady;
  return old;
}//

#endif
