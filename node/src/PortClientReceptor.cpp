#ifndef ECC_PORT_CLIENT_RECEPTOR_CPP
#define ECC_PORT_CLIENT_RECEPTOR_CPP

#include "PortClientReceptor.hpp"
#include "SyncQueue.hpp"

using ecc::PortClientReceptor;

/**
 *
 */
PortClientReceptor::PortClientReceptor(
									   string a_name
									   ,string a_hostName
									   ,int a_portNumber
									   )
  :PortClient(a_name, a_hostName, a_portNumber)
  ,Port(a_name, a_hostName, a_portNumber)
{
}//

/**
 *
 */
void PortClientReceptor::push(Message* a_message_p )
{
  SyncQueueReceptor::instance().push( a_message_p );  
}//

#endif
