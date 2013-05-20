#ifndef ECC_CONNECTOR_CPP
#define ECC_CONNECTOR_CPP

#include <cc++/thread.h>

#include <sstream>
#include <string>
#include <map>

#include "Connector.hpp"
#include "Port.hpp"
#include "PortManager.hpp"
#include "Context.hpp"
#include "Exception.hpp"
#include "BadArgumentException.hpp"
#include "Log.hpp"
#include "Emisor.hpp"
#include "Asserter.hpp"
#include "Message.hpp"

using ecc::Connector; 
using ecc::Port; 
using ecc::Context;
using ecc::Exception;
using ecc::BadArgumentException;
using ecc::Log;
using ecc::Emisor;
using ecc::Message;
using std::string;

int Connector::defaultTimeToReconnect = 10000;
int Connector::defaultInitialLogCountdownToReconnect = 3;

/**
 *
 */
Connector::Connector(
					 PortManager* a_portManager_p
					 ,Port* a_port_p
					 ) 
  :d_portManager_p( a_portManager_p )
  ,d_port_p( a_port_p )
  ,d_timeToReconnect( Connector::defaultTimeToReconnect )
  ,d_initialLogCountdownToReconnect( Connector::defaultInitialLogCountdownToReconnect )
{
  loadContext();
}//


/**
 *
 */
Connector::~Connector()
{
  terminate();
}//

/**
 *
 */
void Connector::final()
{
  delete this; //corroborar si en destructores virtuales se ejecuta el destructor base automaticamente
}//

/**
 *
 */
void Connector::loadContext()
{
  string timeToReconnect_string = Context::instance().getProperty_r("time_to_reconnect");
  try{  
	if( !(istringstream( timeToReconnect_string ) >> d_timeToReconnect ) )
	  {
		ecc::BadArgumentException e;
		e
		  << "< MSG 001065 >: BadArgumentException::\n\tProblemas para convertir la cadena -"
		  << timeToReconnect_string
		  << "- relativa al numero que indique intervalo para intentar reconectarse, en la variable time_to_reconnect, que esta en milisegundos, del archivo de configuracion."
		  << endl
		  << "throw in Connector::loadContext\n";
		throw e;
	  }
  }catch(ecc::Exception& e){
	e
	  << "Warning:\n\n\tUsing default time_to_reconnect: -"
	  << Connector::defaultTimeToReconnect
	  << endl
	  << "catch in Connector::loadConext\n"
	  << endl; 
	Log::instance().log( e.getMessage().c_str() );
  }

  string initialLogCountdownToReconnect_string = Context::instance().getProperty_r("initial_log_countdown_to_reconnect");
  try{  
	if( !(istringstream( initialLogCountdownToReconnect_string ) >> d_initialLogCountdownToReconnect ) )
	  {
		ecc::BadArgumentException e;
		e
		  << "< MSG 001067 >: BadArgumentException::\n\tProblemas para convertir la cadena -"
		  << initialLogCountdownToReconnect_string
		  << "- relativa al numero que indica cuantas veces maximo se reportara en el log qque se esta intentando reconectarse, (en milisegundos) en el archivo de configuracion."
		  << endl
		  << "throw in Connector::loadContext\n";
		throw e;
	  }
  }catch(ecc::Exception& e){
	e
	  << "Warning:\n\n\tUsing default initial_log_countdown_to_reconnect: -"
	  << Connector::defaultInitialLogCountdownToReconnect
	  << endl
	  << "catch in Connector::loadConext\n"
	  << endl; 
	Log::instance().log( e.getMessage().c_str() );
  }
  if( d_initialLogCountdownToReconnect < 0 )
	{
	stringstream sstream;
	sstream
	  << "< MSG 001068 >: Connector::loadContext: warning:\n\n\tinitial_log_countdown_to_reconnect in archivo de configuracion negativa!, realmente quiere que no se indique ni una sola vez en la bitacora que se esta intentando conectar los sockets?"
	  << endl;
	Log::instance().log( sstream.str().c_str() );
  }

}//

/**
 *
 */
void Connector::run(void)
{
  do{
	if( ! d_port_p->isReconnect() )
	  break;

	if( d_initialLogCountdownToReconnect > 0 )
	  {
		stringstream sstream;
		sstream
		  << "< MSG 001070 >: Connector::run:\n\n\tTriying to connect: -"
		  << d_port_p->getName()
		  << "- in -"
		  << d_port_p->getHostName()
		  << ":"
		  << d_port_p->getPortNumber()
		  << "-."
		  << endl;
		Log::instance().log( sstream.str().c_str() );
		d_initialLogCountdownToReconnect--;
	  }
	try//mandar el logon !!!!!
	  {
		d_port_p->init();
		if( d_port_p->connect() )
		  {
			if( ! d_port_p->isReconnect() )//esto es para apagar asi que no hace falta mandar logon ni nada
			  break;
			d_portManager_p->add( d_port_p->getName(), d_port_p );
			Emisor::instance().w0800( d_port_p->getName(), Message::LOGON_NETWORK_MANAGEMENT_INFORMATION_CODE ); 
			//d_port_p->isLogon(true);//aqui no va el logon, el logon es cuando se recive de la otra parte el mensaje
			//y luego se muere el hilo
			break;
		  }
	  }catch(ecc::Exception& e){
		e
		  << "catch in Connector::run"
		  << endl;
		if( d_initialLogCountdownToReconnect > 0 )
		  Log::instance().log( e.getMessage().c_str() );
	  }catch(...){
		Log::instance().log( "< MSG 001069 >: ???exception: \n\n\tExcepcion generica al intentar reconectarse");
	  }
	Thread::sleep( d_timeToReconnect );
  }while(true);
  //  Thread::sleep( d_timeToReconnect );//tiempo para que acabe de reconnectarse
}//

#endif
