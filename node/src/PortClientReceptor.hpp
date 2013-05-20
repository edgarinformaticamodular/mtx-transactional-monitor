#ifndef ECC_PORT_CLIENT_RECEPTOR_HPP
#define ECC_PORT_CLIENT_RECEPTOR_HPP

#include "PortClient.hpp"

using ecc::PortClient;

/**
 *
 */
namespace ecc 
{
  class PortClientReceptor : public virtual PortClient
  {
  public:
	PortClientReceptor(string,string, int);
	void push(Message* a_message);
  };//
}//

#endif
