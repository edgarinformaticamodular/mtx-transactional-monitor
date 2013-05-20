#ifndef ECC_PORT_CLIENT_CPP
#define ECC_PORT_CLIENT_CPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/tcp.h>

#include "PortClient.hpp"
#include "Context.hpp"
#include "Exception.hpp"
#include "Log.hpp"

#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

using ecc::PortClient;
using ecc::Context;
using ecc::Log;
using ecc::Exception;

using std::stringstream;

/**
 *
 */
PortClient::PortClient(
					   string a_name
					   ,string a_hostName
					   ,int a_portNumber
					   )
  :Port(a_name, a_hostName, a_portNumber)
{
}//PortClient

/**
 *
 */
void PortClient::init()
{
  struct hostent *f_hostent_p;
  if ((f_hostent_p=gethostbyname(d_hostName.c_str())) == NULL) {  // obtener información de máquina 
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 001049 >: Exception:"
		<< endl
		<< "\tNo se pudo obtener mas informacion de la maquina para \n-"
		<< d_name
		<< "- en -"
		<< d_hostName
		<< "-."
		<< endl
		<< "throw in ecc::PortClient::PortClient.\n"
		<< endl;
	  throw e;
  }
  if ((d_fileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 001051 >:  Exception:"
		<< endl
		<< "\tNo se pudo obtener un descriptor de archivo para \n-"
		<< d_name
		<< "- en -"
		<< d_hostName
		<< "-."
		<< endl
		<< "throw in ecc::PortClient::PortClient: socket.\n"
		<< endl;
	  throw e;
  }
  d_remote__sockaddr_in.sin_family = AF_INET;    // Ordenación de bytes de la máquina 
  d_remote__sockaddr_in.sin_port = htons(d_portNumber);  // short, Ordenación de bytes de la red 
  d_remote__sockaddr_in.sin_addr = *((struct in_addr *)f_hostent_p->h_addr);
  memset(&(d_remote__sockaddr_in.sin_zero), '\0', 8);  // poner a cero el resto de la estructura 

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
  		<< "< MSG 005448 >: PortClient::init: Exception:" << endl
  		<< "\n\t\tNo se pudo establecer la opcion TCP_NODELAY" << endl
  		;
  	  throw e;
  	}
  //bloqueante
  //  int fflags = fcntl(d_fileDescriptor, F_GETFL);
  //fflags &= ~O_NONBLOCK;
  //fflags &= O_NDELAY;
  //fflags |= O_NONBLOCK;

  //fcntl(d_fileDescriptor, F_SETFL, fflags);

  //todo:checar O_NDELAY si es conveniente y tambien O_NONBLOCK checar concurrencia
  // long opts = fcntl(so, F_GETFL);
  // fcntl(so, F_SETFL, opts | O_NDELAY);
}//

/**
 *
 */
bool PortClient::connect()
{
  if(
	 ::connect(
			 d_fileDescriptor
			 ,(struct sockaddr *)&d_remote__sockaddr_in
			 ,sizeof(struct sockaddr)
			 ) == -1
	 ) 
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 001052 >: Exception:"
		<< endl
		<< "\tNo se pudo connectar para \n-"
		<< d_name
		<< "- en -"
		<< d_hostName
		//<< ":"
		//<< d_remote__sockaddr_in.sin_port
		<< "-"
		<< endl
		<< "throw in ecc::PortClient::PortClient: connect.\n"
		<< endl;
	  throw e;
	}
  {
	stringstream sstream;
	sstream
	  << "< MSG 001071 >: ecc::PortClient::connect: info:\n\n\tConeccion exitosa con la conexion de nombre:\n\n\t-"
	  << d_name
	  << "-, en -"
	  << d_hostName
	  << ":"
	  << d_remote__sockaddr_in.sin_port
	  << "-."
	  << endl;
	Log::instance().log( sstream.str().c_str() );
  }
  //  int fflags = fcntl(d_fileDescriptor, F_GETFL);
  //fflags &= ~O_NONBLOCK;
  //fflags &= O_NDELAY;
  //fflags |= O_NONBLOCK;

  //fcntl(d_fileDescriptor, F_SETFL, fflags);

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
