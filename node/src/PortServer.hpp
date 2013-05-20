#ifndef ECC_PORT_SERVER_HPP
#define ECC_PORT_SERVER_HPP

#include <cc++/socket.h>
#include <fstream>
#include "Port.hpp"

using namespace std;

/**
 *
 */
namespace ecc 
{
  class PortServer : public virtual Port
  {
  public:
	PortServer(string, string, int);
	~PortServer();
	bool connect();
	void init();
  protected:
	static int maxClientsInQueue; // Cuántas conexiones pendientes se mantienen en cola
	int d_fileDescriptorServer;//el que genera hijos y que normalmente se le hace fork
	struct sockaddr_in d_local__sockaddr_in;//siempre el servidor
  };//class PortClient
}//namespace ecc
#endif
