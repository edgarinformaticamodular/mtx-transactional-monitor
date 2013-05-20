#ifndef ECC_PORT_SERVER_RECEPTOR_HPP
#define ECC_PORT_SERVER_RECEPTOR_HPP

#include "PortServer.hpp"

using ecc::PortServer;

/**
 *
 */
namespace ecc 
{
  class PortServerReceptor : public virtual PortServer
  {
  public:
	PortServerReceptor(string,string, int);
	void push(Message* a_message);
  };//
}//

#endif
