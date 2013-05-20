#ifndef ECC_PORT_MANAGER_CPP
#define ECC_PORT_MANAGER_CPP

#include <cc++/thread.h>

#include <sstream>
#include <string>
#include <map>

#include "PortManager.hpp"
#include "PortsFactory.hpp"
#include "Port.hpp"
#include "Context.hpp"
#include "Exception.hpp"
#include "BadArgumentException.hpp"
#include "PortsFactory.hpp"
#include "Log.hpp"
#include "Connector.hpp"
#include "Disconnector.hpp"
#include "Asserter.hpp"

using ost::Thread;
using ost::MutexLock;

using std::stringstream;
using std::string;
using std::map;

using ecc::PortManager; 
using ecc::PortsFactory; 
using ecc::Port; 
using ecc::Context;
using ecc::Exception;
using ecc::BadArgumentException;
using ecc::PortsFactory;
using ecc::Log;
using ecc::Connector;
using ecc::Disconnector;

string PortManager::webPortNamePrefix = "WEB ";
set<string> PortManager::d_receptors;
PortManager* PortManager::d_portManager_p = 0;
bool PortManager::d_isDestroyed = false;
/**
 *
 */
PortManager& PortManager::instance()
{
  if(!d_portManager_p)
	{
	  ost::MutexLock mutexLock(getSingletonMutex_r());
	  if(!d_portManager_p)
		{
		  if(d_isDestroyed)
			{
			  onDeadReference();
			}
		  else
			{
			  create();
			}
		}
	}
  return *d_portManager_p;
}//instance

/**
 *
 */
void PortManager::create()
{
  static PortManager portManager;
  d_portManager_p = &portManager;
}//create

/**
 *
 */
void PortManager::onDeadReference()
{
  create();
  new (d_portManager_p) PortManager();
  d_isDestroyed = false;
}//

/**
 *
 */
PortManager::PortManager()
  :d_isQuit(false)
{

}//

/**
 *
 */
PortManager::~PortManager()
{
  d_portManager_p = 0;
  d_isDestroyed = true;//todo falta terminate
  terminate();
}//~PortManager

/**
 *
 */
bool PortManager::isQuit(bool a_isQuit)
{
  bool isQuitOld = a_isQuit;
  d_isQuit = a_isQuit;
  return isQuitOld;
}//

/**
 *
 */
ost::Mutex& PortManager::getSingletonMutex_r()
{
  static ost::Mutex mutex;
  return mutex;
}//getMutex_r

/**
 * sincroniza el acceso a el mapa de puertos, para que run y los hilos Connect no accesen al mismo tiempo
 */
ost::Mutex& PortManager::getMapMutex_r()
{
  static ost::Mutex mutex; // para evitar race conditions en constructor
  return mutex;
}//

/**
 *
 */
bool PortManager::send(string a_portName, string a_trama, bool a_isRequiredLogon)
{
  if( a_portName.substr(0,4) == PortManager::webPortNamePrefix )
	return true;
  map<string,Port*>::iterator f_iterator = d_ports.find(a_portName);
  if( f_iterator == d_ports.end() )
	{
	  ecc::Exception e;
	  e
		<< "< MSG 001061 >: ecc::Exception\n\n\tNo hay puerto con nombre -"
		<< a_portName
		<< "-. No se puede mandar el mensaje.\nthrow in PortManager::sendTo.\n";
	  throw e;
	}
  Port* port_p = f_iterator->second;
  bool result 
	= a_trama.size() == port_p->writeData( a_trama.data(), a_trama.size(), a_isRequiredLogon );
  return result;
}//

/**
 *
 */
void PortManager::isLogon(string a_portName, bool a_isLogon)
{
  map<string,Port*>::iterator f_iterator = d_ports.find(a_portName);
  if( f_iterator == d_ports.end() )
	{
	  ecc::Exception e;
	  e
		<< "< MSG 001100 >: ecc::Exception\n\n\tNo hay puerto con nombre -"
		<< a_portName
		<< "-. No se puede establecer logon status: -"
		<< a_isLogon
		<< "-.\nthrow in PortManager::isLogon.\n";
	  throw e;
	}
  Port* port_p = f_iterator->second;
  port_p->isLogon( a_isLogon );
  return;
}//

/**
 *
 */
bool PortManager::sendAll(string a_trama, bool a_isRequiredLogon)
{
  bool isSendAll = true;
  map<string,Port*>::iterator f_iterator;
  for(
	  f_iterator = d_ports.begin();
	  f_iterator != d_ports.end();
	  f_iterator++
	  )
	{
	  isSendAll = 
		isSendAll 
		&& ( a_trama.size() == f_iterator->second->writeData( a_trama.data(), a_trama.size(), a_isRequiredLogon ) )
		;
	}
  return isSendAll;//solo exitosa si se mando a todos bien
}//

/**
 *
 */
void PortManager::activeAllPorts()
{
  Log::instance().log("< MSG 001074 >: PortManager::activeAllPorts: info:\n\n\tInicializando conexiones.");
  PortsFactory portsFactory;
  vector<Port*> ports_vector = portsFactory.create();
  vector<Port*>::iterator f_iterator;
  for(
	  f_iterator = ports_vector.begin();
	  f_iterator != ports_vector.end();
	  f_iterator++
	  )
	{
	  Port* port_p = *f_iterator;
	  Connector* connector_p = new Connector(this, port_p);//solito libera memoria el hilo al terminar
	  connector_p->start();//que busque reconectarse y cuando lo haga se adicionara al PortFactory
	}
}//

/**
 *
 */
void PortManager::desactiveAllPorts()
{
  Log::instance().log("< MSG 001077 >: PortManager:desactiveAllPorts: info :\n\n\tFinalizando conexiones.");

  vector<Thread*> disconnectors;
  map<string,Port*>::iterator f_iterator;
  for(
	  f_iterator = d_ports.begin();
	  f_iterator != d_ports.end();
	  f_iterator++
	  )
	{
	  Port* port_p = f_iterator->second;
	  Disconnector* disconnector_p = new Disconnector(this, port_p);//estos no liberan memoria al terminar
	  disconnector_p->detach();//que busque reconectarse y cuando lo haga se adicionara al PortFactory
	  disconnectors.push_back( disconnector_p );
	}
  //esto es parra desbloquear los Connecttores servidores que estan esperando bloqueadoss esperando cliientes
  PortsFactory portsFactory;
  portsFactory.createAutoclients();

  //esto es para asegurar que se terminan de desconectar todos
  vector<Thread*>::iterator f_iterator2;
  for(
	  f_iterator2 = disconnectors.begin();
	  f_iterator2 != disconnectors.end();
	  f_iterator2++
	  )
	{
	  Thread* thread_p = *f_iterator2;
	  thread_p->join();//todo: checar si funciona el join aun cuando alla terminado desde antes de que se llamara el join (yo creo que si porque es un semaforo al final de cuentas)
	}
  //ahora si los borro
  for(
	  f_iterator2 = disconnectors.begin();
	  f_iterator2 != disconnectors.end();
	  f_iterator2++
	  )
	{
	  Thread* thread_p = *f_iterator2;
	  delete thread_p;
	}  
}//

/**
 *
 */
void PortManager::add(string a_name, Port* a_port_p)
{
  MutexLock mutexLock( getMapMutex_r() );//deve sincronizarse con los hilos de Connector y el run
  d_ports[a_name] = a_port_p;//es importante que primero se adicione y luego se de de alta su file descriptor
  //puesto que en el peor de los casos sera ignorado muy poco tiempo por el select, asi que no cambiar el orden de esta instruccion respecto a la siguiente
  if( a_port_p->fileDescriptor() >= d_maxFileDescriptor )//esto es para el select
	d_maxFileDescriptor = a_port_p->fileDescriptor() + 1;
}//

/**
 * utilizar solo dentro de run !
 */
void PortManager::remove(ecc::Port* a_port_p)
{
  //  MutexLock mutexLock( getSendMutex_r() ); //todo: checar:el remove no necesita el mutex porque esta sincronizado el acceso con run, puesto que run solamente es el que lo utiliza
  map<string,Port*>::iterator f_iterator = d_ports.find(a_port_p->getName());
  if( f_iterator != d_ports.end() )
	{
	  d_ports.erase( f_iterator );
	  if( a_port_p->fileDescriptor() == (d_maxFileDescriptor - 1) )//importantes los parentesis
		{
		  d_maxFileDescriptor = 0;//lo reseteo a cero para efectivamente encontrar el maximo actual
		  for(
			  map<string,Port*>::iterator f_iterator2 = d_ports.begin(); //todo: que ess peor eliminar el maximo file descriptor de select antes o despues de quitar el file descriptor de un  set ?
			  f_iterator2 != d_ports.end();
			  f_iterator2++
			  )
			{
			  Port* port_p = f_iterator2->second;
			  if( port_p->fileDescriptor() >= d_maxFileDescriptor )//esto es para el select
				d_maxFileDescriptor = a_port_p->fileDescriptor() + 1;
			}//for
		}//if
	}//if
}//

/**
 *
 */
void PortManager::run(void)
{
  unsigned long minimumRelativeTime = ~((unsigned long) 0);
  unsigned long relativeTime = 0;
  Port *port_p;
  unsigned char f_char;
  struct timeval timeout_timeval;
  struct timeval* timeout__timeval_p;
  fd_set inputSet, outputSet, errorSet;
  FD_ZERO(&inputSet);
  FD_ZERO(&outputSet);
  FD_ZERO(&errorSet);
  int fileDescriptor;
  setCancel(cancelDeferred);
  map<string,Port*>::iterator f_iterator;
  for(;;)
	{
	  if( d_isQuit )
		break;
	  //	  Thread::sleep(1);
	  try
		{
		  //while( d_ports.size() == 0 )//mientras no se agregen puertos a el connector que no ocupe procesador
		  //{
		  // Thread::sleep(1000);
		  //}
		  //talves aqui sea necesario un yield
		  minimumRelativeTime = ~((unsigned long) 0);//reseteo el minimo
		  {// principal para cada ciclo
			MutexLock mutexLock( getMapMutex_r() );//para que el map no sea alterado
			//		MutexLock _lock_(*this); //tendria las ventajas del singleton via _r() usar this?
			for(
				f_iterator = d_ports.begin();
				f_iterator != d_ports.end();
				f_iterator++
				)
			  {
				Port* port_p = f_iterator->second;
				if( port_p->isActive() )//solo si esta activo hacemos cosas
				  {
					fileDescriptor = port_p->fileDescriptor();
					//atender desconexion
					if( FD_ISSET(fileDescriptor, &errorSet) ) //hay messages out of band
					  {
						;
					  }
					//atender lectura
					if( FD_ISSET(fileDescriptor, &inputSet) )
					  {
						if( ! port_p->onPending() )
						  {
							port_p->isActive(false);
							port_p->isLogon(false);
							port_p->closeSocket();
							remove( port_p );
							Connector* connector_p = new Connector(this,port_p);
							connector_p->detach();
							break;//aqui es break porque ya modificamos el iterador al remover elementos del contenedor
						  }
					  }
					//atender escritura
					if( FD_ISSET(fileDescriptor, &outputSet) )
					  {
						port_p->onOutput();
					  }
					
					//atender timeout
				  retry:
					//tomo como minimo el mas bajo de todos los timers
					relativeTime = port_p->relativeTime();
					//		std::cout << port_p->getHostName()<<"\t"<<port_p->getPortNumber()<<"\t"<<relativeTime<<","<<(port_p->isTimeout()?"timeout":"")<<std::endl;
					if( relativeTime > 0 )
					  if( relativeTime < minimumRelativeTime )
						minimumRelativeTime = relativeTime;
					
					if( 
					   relativeTime == 0 
					   && (!port_p->isTimeout()) 
					   && (port_p->isReceptionStarted())
					   )
					  {
						//std::cout << port_p->getHostName()<<":"<<port_p->getPortNumber()<<"|TIME OUT:"<<std::endl;
						port_p->onExpired();//aqui se desactiva
						goto retry;//esto es porque el manejador (this) necesita recalcular el minimo talvez
					  }
					
				  }//if
			  }//for
			int f_size = d_ports.size();
			//reseteamos los sets para que nos avisen de nuevo		
			FD_ZERO(&inputSet);
			FD_ZERO(&outputSet);
			FD_ZERO(&errorSet);
			for(
				f_iterator = d_ports.begin();
				f_iterator != d_ports.end();
				f_iterator++
				)
			  {
				Port* port_p = f_iterator->second;
				fileDescriptor = port_p->fileDescriptor();
				FD_SET(fileDescriptor, &inputSet);
				//FD_SET(fileDescriptor, &outputSet);
				//FD_SET(fileDescriptor, &errorSet);
			  }//for
		  }//principal para cada cilco
		  if( minimumRelativeTime == ~((unsigned long) 0) )
			{//aqui en el caso de que no alla minimo lo hacemos que el minimo sea un segundo, otra alternativa seria poner NULL, pero esto podria bloquear el ciclo para siempre lo cual no es deseable para el apagado porque nunca se detecta la condicion d_isQuit, por otro lado no importa hacer otro ciclo si hay poca actividad, es decir si no regresa el select indica que hay poca actividad al menos en este servidor
			  timeout__timeval_p = &timeout_timeval;
			  timeout_timeval.tv_sec = 1;
			  timeout_timeval.tv_usec = 0;
			}
		  else
			{
			  timeout__timeval_p = &timeout_timeval;
			  timeout_timeval.tv_sec = minimumRelativeTime / 1000;
			  timeout_timeval.tv_usec = (minimumRelativeTime % 1000) * 1000;
			}
		  //unsigned long xxx = ~((unsigned long) 0);
		  //		  std::cout << "< MSG 005454 >: " << xxx << "|" << timeout__timeval_p->tv_sec << "," << timeout__timeval_p->tv_usec << std::endl;
		  if(
			 select(
					d_maxFileDescriptor
					,&inputSet
					,&outputSet
					,&errorSet
					,timeout__timeval_p
					) == -1
			 )
			{//algo fallo, asi que limpiamos los sets, porque sino va a creer que se desconectaron todos los file descriptors, y limpiando todos se ignorara esta iteracion y hasta la siguiente se recupera la funcion normal
			for(
				f_iterator = d_ports.begin();
				f_iterator != d_ports.end();
				f_iterator++
				)
			  {
				Port* port_p = f_iterator->second;
				fileDescriptor = port_p->fileDescriptor();
				FD_ZERO(&inputSet);
				FD_ZERO(&outputSet);
				FD_ZERO(&errorSet);

				FD_SET(fileDescriptor, &errorSet);
				if( 
				   select(
						  d_maxFileDescriptor
						  ,NULL
						  ,NULL
						  ,&errorSet
						  ,timeout__timeval_p
						  ) == -1
				   )
				  {
					FD_ZERO(&errorSet);
					FD_SET(fileDescriptor, &errorSet);
					perror("select");
					break;
				  }
			  }//for
			}
		  // cout.flush();
		}catch(ecc::Exception& e){
		  e
			<< "catch in PortManager::run\n";
			Log::instance().log( e.getMessage().c_str() );
		}catch(...){
		  stringstream sstream;
		  sstream
			<< "< MSG 001220 >: ???::exception: PortManager::run\n\n\tUna exception ha ocurrido\n";
		  Log::instance().log(sstream.str().c_str());
		}
	}//for(;;)
}//run

/**
 *
 */
bool PortManager::isReceptor(string a_identifyer)
{
  set<string>::iterator f_iterator = d_receptors.find(a_identifyer);
  if( f_iterator == d_receptors.end() )
	return false;
  else
	return true;
}//

/**
 *
 */
void PortManager::loadContext()
{
  //obtengo todos las connecciones que seran activadas en la ejecucion actual
  string receptors_string 
	= Context::instance().getProperty_r("receptors");

  const string delimiters(",");

  string::size_type f_begin, f_end;
  f_begin = receptors_string.find_first_not_of(delimiters);
  while( f_begin != string::npos )
	{
	  f_end = receptors_string.find_first_of(delimiters,f_begin);
	  if( f_end == string::npos )
		  f_end = receptors_string.length();
	  d_receptors.insert( receptors_string.substr(f_begin,f_end-f_begin) );
	  f_begin = receptors_string.find_first_not_of(delimiters, f_end);
	}
}//
  
#endif
