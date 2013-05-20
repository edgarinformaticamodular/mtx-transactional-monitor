#ifndef ECC_PORTS_FACTORY_CPP
#define ECC_PORTS_FACTORY_CPP

#include "PortsFactory.hpp"
#include "Context.hpp"
#include "BadArgumentException.hpp"
#include "Log.hpp"
#include "PortClientAcceptor.hpp"
#include "PortClientReceptor.hpp"
#include "PortServerAcceptor.hpp"
#include "PortServerReceptor.hpp"
#include "Port.hpp"

using ecc::PortsFactory;
using ecc::Context;
using ecc::BadArgumentException;
using ecc::Log;
using ecc::PortClientAcceptor;
using ecc::PortClientReceptor;
using ecc::PortServerAcceptor;
using ecc::PortServerReceptor;
using ecc::Port;

/**
 *
 */
PortsFactory::PortsFactory()
{
  loadContext();
}//

/**
 *
 */
PortsFactory::~PortsFactory()
{
}//

/**
 *
 */
void PortsFactory::loadContext()
{
  //obtengo todos las connecciones que seran activadas en la ejecucion actual
  string activeConnections_string 
	= Context::instance().getProperty_r("acceptors")
	+ ","
	+ Context::instance().getProperty_r("receptors")
	;
  const string delimiters(",");
  vector<string> activeConnections;
  string::size_type f_begin, f_end;
  f_begin = activeConnections_string.find_first_not_of(delimiters);
  while( f_begin != string::npos )
	{
	  f_end = activeConnections_string.find_first_of(delimiters,f_begin);
	  if( f_end == string::npos )
		  f_end = activeConnections_string.length();
	  activeConnections.push_back( activeConnections_string.substr(f_begin,f_end-f_begin) );
	  f_begin = activeConnections_string.find_first_not_of(delimiters, f_end);
	}
  //ahora relleno el d_context
  for(
	  vector<string>::iterator f_iterator = activeConnections.begin();
	  f_iterator != activeConnections.end();
	  f_iterator++
	  )
	{
	  ContextItem contextItem;
	  string connectionName = *f_iterator;
	  try
		{
		  //el nombre
		  contextItem.connectionName = connectionName;
		  //hostname
		  contextItem.hostName 
			= Context::instance().getProperty_r( connectionName + "_host_name" );
		  //networkRol
		  contextItem.networkRol 
			= Context::instance().getProperty_r( connectionName + "_network_rol" );
		  if( 
			 contextItem.networkRol != "server" 
			  && contextItem.networkRol != "client" 
			 )
			{
			  ecc::BadArgumentException e;
			  e
				<< "< MSG 001072 >: BadArgumentException::\n\tNo es un rol valido -"
				<< contextItem.networkRol
				<< "- validos:\"server\"- \"client\", en la variable -"
				<< connectionName + "_network_rol"
				<< "del archivo de configuracion."
				<< endl
				<< "throw in PortsFactory::loadContext\n";
			  throw e;
			}
		  //bussinesRol
		  contextItem.bussinesRol 
			= Context::instance().getProperty_r( connectionName + "_bussines_rol" );
		  if( 
			 contextItem.bussinesRol != "acceptor" 
			  && contextItem.bussinesRol != "receptor" 
			 )
			{
			  ecc::BadArgumentException e;
			  e
				<< "< MSG 001078 >:  BadArgumentException::\n\tNo es un rol valido -"
				<< contextItem.bussinesRol
				<< "- validos:\"acceptor\"- \"receptor\", en la variable -"
				<< connectionName + "_bussines_rol"
				<< "del archivo de configuracion."
				<< endl
				<< "throw in PortsFactory::loadContext\n";
			  throw e;
			}
		  //portNumber
		  int portNumber;
		  string portNumber_string = Context::instance().getProperty_r( connectionName + "_port_number" );
		  if( !(istringstream(portNumber_string) >> portNumber) )
			{
			  ecc::BadArgumentException e;
			  e
				<< "< MSG 001079 >:  BadArgumentException::\n\tNo es un numero de puerto valido -"
				<< portNumber
				<< "-, en la variable -"
				<< connectionName + "_port_number"
				<< "del archivo de configuracion."
				<< endl
				<< "throw in PortsFactory::loadContext\n";
			  throw e;		  
			}
		  contextItem.portNumber = portNumber;
		  d_context.push_back( contextItem );
		}catch(ecc::Exception& e){
		  e
			<< "catch in PortsFactory::loadContext\n\n"
			;
		  Log::instance().log( e.getMessage().c_str() );
		  stringstream sstream;
		  sstream
			<< "< MSG 001073 >: PortsFactory::loadContext: warning!: se no se creara la conexion de nombre: -"
			<< connectionName
			<< "-."
			<< endl;

		  continue;//ya no la insertamos
		}
	}//for
}//

/**
 *
 */
vector<Port*> PortsFactory::create()
{
  vector<Port*> ports;
  vector< ContextItem >::iterator f_iterator;
  for(
	  f_iterator = d_context.begin();
	  f_iterator != d_context.end();
	  f_iterator++
	  )
	{
	  ContextItem contextItem = *f_iterator;
	  Port* port_p; 
	  if( contextItem.networkRol == "client" && contextItem.bussinesRol == "acceptor" )
		{
		  port_p 
			= new PortClientAcceptor(
							 contextItem.connectionName
							 ,contextItem.hostName
							 ,contextItem.portNumber
							 );
		}
	  else if( contextItem.networkRol == "client" && contextItem.bussinesRol == "receptor" )
		{
		  port_p 
			= new PortClientReceptor(
							 contextItem.connectionName
							 ,contextItem.hostName
							 ,contextItem.portNumber
							 );
		}
	  else if( contextItem.networkRol == "server" && contextItem.bussinesRol == "acceptor" )
		{
		  port_p 
			= new PortServerAcceptor(
							 contextItem.connectionName
							 ,contextItem.hostName
							 ,contextItem.portNumber
							 );
		}
	  else if( contextItem.networkRol == "server" && contextItem.bussinesRol == "acceptor" )
		{
		  port_p 
			= new PortServerReceptor(
							 contextItem.connectionName
							 ,contextItem.hostName
							 ,contextItem.portNumber
							 );
		}
	  ports.push_back( port_p );
	}//for
  return ports;
}//

/**
 * esto se utiliza para crear clientes de nuestros servidores que estan desocupadoss parra que se desbloqueen
 */
void PortsFactory::createAutoclients()
{
  vector< ContextItem >::iterator f_iterator;
  for(
	  f_iterator = d_context.begin();
	  f_iterator != d_context.end();
	  f_iterator++
	  )
	{
	  ContextItem contextItem = *f_iterator;
	  if( contextItem.networkRol == "server" )
		{
		  PortClientAcceptor portClientAcceptor(
												contextItem.connectionName
												,contextItem.hostName
												,contextItem.portNumber
												);
		  portClientAcceptor.init();
		  portClientAcceptor.connect();
		}
	}//for
}//

#endif
