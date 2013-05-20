#ifndef ECC_PORT_SERVER_CPP
#define ECC_PORT_SERVER_CPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/tcp.h>

#include "PortServer.hpp"
#include "Context.hpp"
#include "Log.hpp"
#include "Exception.hpp"

#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

using ecc::Exception;
using ecc::PortServer;
using ecc::Context;
using ecc::Log;

using std::stringstream;

int PortServer::maxClientsInQueue = 2; // Cuántas conexiones pendientes se mantienen en cola, todo: si conviene mejor dejarlo a 1 o incluso mejor a 0

/**
 *
 */
PortServer::PortServer(
					   string a_name
					   ,string a_hostName
					   ,int a_portNumber
					   )
  :Port(a_name, a_hostName, a_portNumber)
{
  struct hostent *he;
  int yes=1;

  if ((d_fileDescriptorServer = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 001053 >:  Exception:"
		<< endl
		<< "\tNo se pudo obtener un descriptor de archivo para \n\t-"
		<< d_hostName
		<< "-."
		<< endl
		<< "throw in ecc::PortServer::PortServer: socket.\n"
		<< endl;
	  throw e;
  }

  if (setsockopt(d_fileDescriptorServer, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 001054 >: Exception:"
		<< endl
		<< "\tsetsockopt fallo para el descriptor de archivo para \n\t-"
		<< d_hostName
		<< "-. puerto -"
		<< d_portNumber
		<< endl
		<< "throw in ecc::PortServer::PortServer: setsockopt.\n"
		<< endl;
	  throw e;
  }

  d_local__sockaddr_in.sin_family = AF_INET;         // Ordenación de bytes de la máquina
  d_local__sockaddr_in.sin_port = htons(d_portNumber);     // short, Ordenación de bytes de la red
  d_local__sockaddr_in.sin_addr.s_addr = INADDR_ANY; // Rellenar con mi dirección IP
  memset(&(d_local__sockaddr_in.sin_zero), '\0', 8); // Poner a cero el resto de la estructura

  if (::bind(d_fileDescriptorServer, (struct sockaddr *)&d_local__sockaddr_in, sizeof(struct sockaddr)) == -1)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 001055 >: Exception:"
		<< endl
		<< "\tbind fallo para \n\t-"
		<< d_name
		<< "- en -"
		<< d_hostName
		<< "-. puerto -"
		<< d_portNumber
		<< endl
		<< "throw in ecc::PortServer::PortServer: bind.\n"
		<< endl;
	  throw e;
	}
  
  if (::listen(d_fileDescriptorServer, PortServer::maxClientsInQueue) == -1) {
	ecc::Exception e;
	e.getStream() 
		<< "< MSG 001056 >: Exception:"
		<< endl
		<< "\tlisten fallo para \n\t-"
		<< d_name
		<< "- en -"
		<< d_hostName
		<< "-. puerto -"
		<< d_portNumber
		<< endl
		<< "throw in ecc::PortServer::PortServer: listen.\n"
		<< endl;
	  throw e;
  }

  //bloqueante
  //  int fflags = fcntl(d_fileDescriptorServer, F_GETFL);
  //fflags &= ~O_NONBLOCK;
  //fflags |= O_NONBLOCK;
  //fflags &= O_NDELAY;

  //fcntl(d_fileDescriptorServer, F_SETFL, fflags);

}//PortServer

/**
 *
 */
PortServer::~PortServer()
{
  close(d_fileDescriptorServer);
}//

/**
 *
 */
void PortServer::init()
{
}//

/**
 *
 */
bool PortServer::connect()
{
  /** todo: checar si es necesario eliminar procesos muertos
  struct sigaction sa;
  sa.sa_handler = sigchld_handler; // Eliminar procesos muertos
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 001057 >: Exception:"
		<< endl
		<< "\tfallo al eliminar procesos muertos en -"
		<< d_hostName
		<< "-. puerto -"
		<< d_portNumber
		<< "throw in ecc::PortServer::PortServer: sigaction.\n"
		<< endl;
	  throw e;
  }
  */
  //cout << "< MSG 001470 >: "; std::cout.flush();
  int sin_size = sizeof(struct sockaddr_in);
  if(
	 (
	  d_fileDescriptor = accept(//note que aqui ya es el file descriptor del puerto
								d_fileDescriptorServer
								,(struct sockaddr *)&d_remote__sockaddr_in
								,(socklen_t *)(&sin_size)
								)
	  ) == -1
	 ) 
	{
	  //cout << "< MSG 001442 >: "; std::cout.flush();
	  try
		{
		  ecc::Exception e;
		  e.getStream() 
			<< "< MSG 001057 >: Exception:"
			<< endl
			<< "\tfallo al eliminar procesos muertos en \n\t-"
			<< d_name
			<< "- en -"
			<< d_hostName
			<< ":"
			<< d_portNumber
			<< "-"
			<< endl
			<< "throw in ecc::PortServer::connect: sigaction.\n"
			<< endl;
		  throw e;
		}catch(ecc::Exception& e){
		  e
			<< "catch in PortServer::connect\n"
			<< "se ignora\n";
		  Log::instance().log( e.getMessage().c_str() );
		}
	  return false;
	}//if
  //bloqueante
  //  int fflags = fcntl(d_fileDescriptor, F_GETFL);
  //fflags |= O_NONBLOCK;
  //fflags &= O_NDELAY;
  //fcntl(d_fileDescriptor, F_SETFL, fflags);

    int flag = 1;
  int result = setsockopt(d_fileDescriptor,            /* socket affected */
                         IPPROTO_TCP,     /* set option at TCP level */
                         TCP_NODELAY,     /* name of option */
                         (char *) &flag,  /* the cast is historical 
  											  cruft */
                         sizeof(int));    /* length of option value */
  if (result < 0)
  	{
  	  ecc::Exception e;
  	  e
  		<< "< MSG 005450 >:  PortClient::init: Exception:" << endl
  		<< "\n\t\tNo se pudo establecer la opcion TCP_NODELAY" << endl
  		;
  	  throw e;
	}
  {
	stringstream sstream;
	sstream
	  << "ecc::PortServer::PortServer: \n\n\tConexion exitosa para \n\n\t-"
	  << d_name
	  << "-, en -"
	  << inet_ntoa(d_remote__sockaddr_in.sin_addr)
	  << ":"
	  << d_remote__sockaddr_in.sin_port
	  << "-."
	  << endl;
	Log::instance().log( sstream.str().c_str() );
  }
  //nosotros no hacemos fork, entonces: solo una conexion a la vez !!
  //	close(d_fileDescriptor);  // El proceso padre no lo necesita
  d_isActive = true;
  d_isLogon = false;
  d_isOpen = true;//checar esto si vale la pena inicializarlo asi
  d_doDisconnect = false;
  d_isTimeout = false;
  d_isReceptionStarted = false; 
  d_lastBytesAvail = 0;
  return true;
}//

#endif
