#ifndef ECC_DISCONNECTOR_CPP
#define ECC_DISCONNECTOR_CPP

#include <cc++/thread.h>

#include <sstream>
#include <string>
#include <map>

#include "Disconnector.hpp"
#include "Port.hpp"
#include "PortManager.hpp"
#include "Context.hpp"
#include "Exception.hpp"
#include "BadArgumentException.hpp"
#include "Log.hpp"
#include "Emisor.hpp"
#include "Asserter.hpp"
#include "Message.hpp"

using ecc::Disconnector; 
using ecc::Port; 
using ecc::Log;
using ecc::Emisor;
using ecc::Message;
using std::string;

/**
 *
 */
Disconnector::Disconnector(
					 PortManager* a_portManager_p
					 ,Port* a_port_p
					 ) 
  :d_portManager_p( a_portManager_p )
  ,d_port_p( a_port_p )
{
}//


/**
 *
 */
Disconnector::~Disconnector()
{
  terminate();
}//

/**
 *
 */
void Disconnector::final()
{
  //estos los vamos a borrar manualmeente
  // delete this;
}//

/**
 *
 */
void Disconnector::run(void)
{
  try
	{
	  d_port_p->isReconnect(false);//para que ya no lo vuelva a reconectar el connector
	  d_port_p->isLogon(false);
	  Emisor::instance().w0800( d_port_p->getName(), Message::LOGOFF_NETWORK_MANAGEMENT_INFORMATION_CODE );
	  d_port_p->doDisconnect(true);
	  d_port_p->isActive(false);
	  d_port_p->closeSocket();
	  d_portManager_p->remove( d_port_p );
	  //delete d_port_p;//todo: checar la liberacion de memoria
	}catch(...){
	  stringstream sstream;
	  sstream
		<< "< MSG 001080 >: ???exception: Disconnector::run: ocurrio una exepcion al tratar de desconectar : -"
		<< d_port_p->getHostName()
		<< ":"
		<< d_port_p->getPortNumber()
		<< "-."
		<< endl
		<< "catch in Disconnector::run.\n"
		;
	  Log::instance().log(sstream.str().c_str());
	}
}//

#endif
