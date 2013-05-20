#ifndef ECC_PORT_CLIENT_HPP
#define ECC_PORT_CLIENT_HPP

#include <cc++/socket.h>
#include "Port.hpp"

using ecc::Port;

/**
 *
 */
namespace ecc 
{
  /**
   *
   */
  class PortClient : public virtual Port
  {
  public:
	PortClient(string,string, int);
	bool connect();
	void init();
  };//class PortClient
}//namespace ecc

#endif
