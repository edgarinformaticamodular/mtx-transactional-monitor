#ifndef ECC_PORT_CLIENT_ACCEPTOR_HPP
#define ECC_PORT_CLIENT_ACCEPTOR_HPP

#include "PortClient.hpp"

using ecc::PortClient;

/**
 *
 */
namespace ecc 
{
  class PortClientAcceptor : public virtual PortClient
  {
  public:
	PortClientAcceptor(string,string, int);
	void push(Message* a_message);
  };//
}//

#endif
