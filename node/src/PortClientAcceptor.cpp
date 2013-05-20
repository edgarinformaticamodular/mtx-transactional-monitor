#ifndef ECC_PORT_CLIENT_ACCEPTOR_CPP
#define ECC_PORT_CLIENT_ACCEPTOR_CPP

#include "PortClientAcceptor.hpp"
#include "SyncQueue.hpp"

using ecc::PortClientAcceptor;

/**
 *
 */
PortClientAcceptor::PortClientAcceptor(
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
void PortClientAcceptor::push( Message* a_message_p )
{
  SyncQueueAcceptor::instance().push( a_message_p );  
}//

#endif
