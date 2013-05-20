#ifndef ECC_PORT_SERVER_RECEPTOR_CPP
#define ECC_PORT_SERVER_RECEPTOR_CPP

#include "PortServerReceptor.hpp"
#include "SyncQueue.hpp"

using ecc::PortServerReceptor;

/**
 *
 */
PortServerReceptor::PortServerReceptor(
									   string a_name
									   ,string a_hostName
									   ,int a_portNumber
									   )
  :PortServer(a_name, a_hostName, a_portNumber)
  ,Port(a_name, a_hostName, a_portNumber)
{
}//

/**
 *
 */
void PortServerReceptor::push( Message* a_message_p )
{
  SyncQueueReceptor::instance().push( a_message_p );  
}//

#endif
