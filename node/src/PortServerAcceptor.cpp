#ifndef ECC_PORT_SERVER_ACCEPTOR_CPP
#define ECC_PORT_SERVER_ACCEPTOR_CPP

#include "PortServerAcceptor.hpp"
#include "SyncQueue.hpp"

using ecc::PortServerAcceptor;

/**
 *
 */
PortServerAcceptor::PortServerAcceptor(
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
void PortServerAcceptor::push( Message* a_message_p )
{
  SyncQueueAcceptor::instance().push( a_message_p );  
}//

#endif
