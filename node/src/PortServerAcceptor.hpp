#ifndef ECC_PORT_SERVER_ACCEPTOR_HPP
#define ECC_PORT_SERVER_ACCEPTOR_HPP

#include "PortServer.hpp"

using ecc::PortServer;

/**
 *
 */
namespace ecc 
{
  class PortServerAcceptor : public virtual PortServer
  {
  public:
	PortServerAcceptor(string,string, int);
	void push(Message* a_message);
  };//
}//

#endif
