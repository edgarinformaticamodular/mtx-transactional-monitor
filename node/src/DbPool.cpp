#ifndef ECC_DB_POOL_CPP
#define ECC_DB_POOL_CPP

#include <cc++/thread.h> 

#include <stdlib.h>
#include <new>
#include <set>
#include <algorithm>
#include <iostream>

using namespace std;

#include "DbPool.hpp"
#include "LockingPtr.hpp"
#include "Exception.hpp"
#include "Context.hpp"
#include "Log.hpp"
#include "BadArgumentException.hpp"

using namespace db;
using ecc::LockingPtr;
using ecc::Exception;
using ecc::Context;
using ecc::Log;
using ecc::BadArgumentException;

DbPool* DbPool::d_dbPool_p = 0;
bool DbPool::d_isDestroyed = false;
int DbPool::MAX_CONNECTIONS = 4;
/**
 *
 */
DbPool& DbPool::instance()
{
  if(!d_dbPool_p)
	{
	  ost::MutexLock mutexLock( getMutex_r() );
	  if(!d_dbPool_p)
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
  return *d_dbPool_p;
}//

/**
 *
 */
void DbPool::create()
{
  static DbPool dbPool;
  d_dbPool_p = &dbPool;
}//

/**
 *
 */
void DbPool::onDeadReference()
{
  create();
  new (d_dbPool_p) DbPool;
  d_isDestroyed = false;
}//

/**
 *
 */
DbPool::~DbPool()
{
  d_dbPool_p = 0;
  LockingPtr<Set> set_lp(d_set, d_mutex);
  for( //si no se han cerrado se cierran ahora
	  Set::iterator itr = set_lp->begin();
	  itr != set_lp->end();
	  itr++
	  )
	{
	  ::PQfinish(*itr);
	}
  d_isDestroyed = true;
}//~DbPool

/**
 *
 */
void DbPool::insert(db::Connection_sp a_connection_sp)
{
  LockingPtr<Set> set_lp(d_set, d_mutex);
  Connection_sp chequed_sp = check(a_connection_sp);
  if( chequed_sp == a_connection_sp )
	{
	  if( set_lp->find(a_connection_sp) != set_lp->end() )
		{
		  return;
		}
	  else
		{
		  set_lp->insert(a_connection_sp);
		}
	}
  else
	{
	  set_lp->erase( a_connection_sp );	  
	  ::PQfinish( a_connection_sp );
	  set_lp->insert( chequed_sp );
	}
  d_semaphore.post();
}//

/**
 *
 */
void DbPool::closeAll()
{
  LockingPtr<Set> set_lp(d_set, d_mutex);
  for(
	  Set::iterator f_iterator = set_lp->begin();
	  f_iterator != set_lp->end();
	  f_iterator++
	  )
	{
	  d_semaphore.wait();
	  Connection_sp connection_sp = *f_iterator;
	  set_lp->erase( connection_sp );	  
	  ::PQfinish( connection_sp );
	}
}//

/**
 *
 */
db::Connection_sp DbPool::erase()
{
  //notese que antes que tome el candado, checa el semaforo
  d_semaphore.wait();
  LockingPtr<Set> set_lp(d_set, d_mutex);
  if( set_lp->empty() )
	{
	  ecc::Exception e("< MSG 054 >: ecc::Exception:\n\tSet vacia !!! no hay conexiones disponibles.\nthrow in DbPool::pop.\n");
	  throw e;
	}
  Connection_sp connection_sp = *( set_lp->begin() );
  set_lp->erase( connection_sp );
  return connection_sp;
}//

/**
 *
 */
ost::Semaphore& DbPool::getSemaphore_r()
{
  return d_semaphore;
}//

/**
 *
 */
DbPool::DbPool()
  :d_dbPoolSize(1)
{
  loadContext();
  LockingPtr<Set> set_lp(d_set, d_mutex);
  for(int i=0;i<d_dbPoolSize;i++)
	{
	  db::Connection_sp connection_sp = createConnection_sp();
	  set_lp->insert( connection_sp );
	  d_semaphore.post();
	}
}//

/**
 *
 */
Connection_sp DbPool::createConnection_sp()
{
  ::PGconn   *conn_p = 0;

  string connInfo;

  if( d_dbHost.size() > 0 )
	connInfo += "host='" + d_dbHost + "' ";
  if( d_dbHostAddr.size() > 0 )
	connInfo += "hostaddr='" + d_dbHostAddr + "' ";
  if( d_dbPort.size() > 0 )
	connInfo += "port='" + d_dbPort + "' ";
  if( d_dbName.size() > 0 )
	connInfo += "dbname='" + d_dbName + "' ";
  if( d_dbUser.size() > 0 )
	connInfo += "user='" + d_dbUser + "' ";
  if( d_dbConnectTimeout.size() > 0 )
	connInfo += "connect_timeout='" + d_dbConnectTimeout + "' ";
  if( d_dbOptions.size() > 0 )
  	connInfo += "options='" + d_dbOptions + "' ";
  if( d_dbTty.size() > 0 )
	connInfo += "tty='" + d_dbTty + "' ";
  if( d_dbRequireSsl.size() > 0 )
  	connInfo += "requiressl='" + d_dbRequireSsl + "' ";

  {
	stringstream sstream;
	sstream
	  << "< MSG 005296 >: DbPool::createConnection_sp:info:\n\n\tCreando conexion a base de datos con los siguientes parametros (no se incluye password):\n"
	  << connInfo
	  << endl
	  ;
	Log::instance().log( sstream.str().c_str() );
  }

  if( d_dbPassword.size() > 0 )
	connInfo += "password='" + d_dbPassword + "' ";


  conn_p
	= ::PQconnectdb( connInfo.c_str() );
  //  cout << conn_p<< ::PQstatus(conn_p) << "  " << ::CONNECTION_OK << " " << ::CONNECTION_BAD << endl;
  if (::PQstatus(conn_p) == ::CONNECTION_BAD)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 055 >: ecc::Exception: Connection to database -"
		<< d_dbName
		<< "- failed."
			<< endl
			<< "Server report: -"
			<< ::PQerrorMessage(conn_p)
			<< "-."
			<< "throw in pGConn.\n"
			<< endl;
		} 
  return conn_p;
}//createConnection_p

/**
 *
 */
db::Connection_sp DbPool::check(db::Connection_sp a_connection_sp)
{
  //cout <<"check:"<< ::PQstatus(a_connection_sp) << "  " << ::CONNECTION_OK << " " << ::CONNECTION_BAD << endl;
  if( ::PQstatus(a_connection_sp) == ::CONNECTION_BAD )
	{
	  return createConnection_sp();
	}
  else
	{
	  return a_connection_sp;
	}
}//check

/**
 *
 */
ost::Mutex& DbPool::getMutex_r()
{
  static ost::Mutex mutex;
  return mutex;
}//getMutex_r

/**
 *
 */
void DbPool::loadContext()
{
  try
	{
	  d_dbHost = Context::instance().getProperty_r("db_host");
	  d_dbHostAddr = Context::instance().getProperty_r("db_host_addr");
	  d_dbPort = Context::instance().getProperty_r("db_port");
	  d_dbName = Context::instance().getProperty_r("db_name");
	  d_dbUser = Context::instance().getProperty_r("db_user");
	  d_dbPassword = Context::instance().getProperty_r("db_password");
	  d_dbConnectTimeout = Context::instance().getProperty_r("db_connect_timeout");
	  d_dbOptions = Context::instance().getProperty_r("db_options");
	  d_dbTty = Context::instance().getProperty_r("db_tty");
	  d_dbRequireSsl = Context::instance().getProperty_r("db_require_ssl");

	  string dbPoolSize_string
		= Context::instance().getProperty_r("db_pool_size");
	  BadArgumentException e;
	  e
		<< "< MSG 005200 >: BadArgumentException:\n\tValor invalido en archivo de configuracion, para el numero de conexiones disponibles en el pool de conexiones a la base de datos -db_pool_size- : -" << dbPoolSize_string << "-, se utilizara valor por default: -"<< MAX_CONNECTIONS <<"-.";
	  
	  if( !(stringstream( dbPoolSize_string ) >> d_dbPoolSize) )
		throw e;
	  if( d_dbPoolSize < 1 )
		throw e;
	}catch(Exception& e){
	  d_dbPoolSize = MAX_CONNECTIONS;
	  e
		<< endl
		<< "catch in DbPool::loadContext.";
	  Log::instance().log( e.getMessage().c_str() );
	}
}//


#endif
