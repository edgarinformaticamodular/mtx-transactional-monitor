#ifndef ECC_PORT_CPP
#define ECC_PORT_CPP

#include "Port.hpp"
#include "Message.hpp"
#include "Exception.hpp"
#include "ParseException.hpp"
#include "Context.hpp"
#include "Log.hpp"
#include "Asserter.hpp"

using ost::Mutex;
using std::string;
using ecc::Port;
using ecc::Message;
using ecc::Exception;
using ecc::ParseException;
using ecc::Context;
using ecc::Log;

unsigned long Port::timeoutCompletePacket = 10000;//milisegundos
int Port::maxBufferSize = 32768;//bytes
unsigned long Port::timeoutDisconnect = 500;//miliseconds
/** todo: revisar la asignacion de la variable d_isConnectionClosed al inicializarse no trae problemas
 *
 */
Port::Port(
		   string a_name
		   ,string a_hostName
		   ,int a_portNumber
		   )
  :d_name(a_name) 
  ,d_portNumber(a_portNumber)
  ,d_hostName(a_hostName)
  ,d_isActive(false)
  ,d_isLogon(false)
  ,d_isOpen(false)//checar esto si vale la pena inicializarlo asi
  ,d_doDisconnect(false)
  ,d_isTimeout(false)
  ,d_isReceptionStarted(false)
  ,d_lastBytesAvail(0)
{
  d_buffer_p = new char[Port::maxBufferSize];
}//Port

/**
 *
 */
Port::~Port()
{
  endSocket();
  delete [] d_buffer_p;
}//~Port

/**
 *
 */
void Port::endSocket()
{

}//

/**
 *
 */
void Port::onConnectionClosed(void)
{ 
  try
	{
	  endSocket();
	}catch(...){
	  Log::instance().log("???::exception:\n\n\tcatch in Port::OnConnectionClosed.");
	}//
}//OnConnectionClosed

/**
 *	cuando llega un paquete
 *	
 */
void Port::onDataReceived(char* a_data, unsigned int a_size)
{
  ofstream x("received");
  x.write(a_data,a_size);
  x.close();
  cout.write(a_data,a_size);
  cout << endl << a_size << endl;
  cout.flush();
  try
	{
	  if( a_size < 0 )
		{
		  ecc::Exception e;
		  e << "< MSG 100 >: ecc::Exception::\n\n\tTamanno invalido de trama por mandar: -"<<a_size<<"-."<<endl<<" throw in Port::onDataReceived.\n";
		  throw e;
		}//if

	  string trama;
	  trama.assign(a_data,a_size);// todo: le qute el -1: el menos uno es para eliminar el \x00 que manda

	  //todo:aqui deveria de dar de alta el nombre del puerto
	  ecc::Message_sp  message_sp( new Message(d_name, trama) );
	  //Context::log("Port::OnDataReceived: info\n\tAntes de push.");
	  //todo: aqui va si va a cola de servicios o a cola de clientes
	  push(message_sp);
	  //Context::log("Port::OnDataReceived: info\n\tDespues de push.");
	}catch(ecc::Exception& e){
	  e << "catch in Port::onDataReceived.";
	  Log::instance().log( e.getMessage().c_str() );
	}catch(std::exception& e){
	  stringstream sstream;
	  sstream 
		<< e.what()
		<< "catch in Port::onDataReceived.";
	  Log::instance().log( sstream.str().c_str() );
	}catch(...){
	  Log::instance().log( "??::exception::\n\n\tOcurrio una excepcion.\ncatch in Port::onDataReceived.");
	}
}//onDataReceived

/**
 *	Overridden from class Port.
 *	Called when data is available in the receive buffer.
 */
void Port::onPending(void)
{
  if(!d_isOpen)
	{
	  // return;
	}
  // Read all available bytes into our buffer todo: no deveria ser maxbuffer - d_lastbytesavail ?
  int bytesAvail = ::recv(d_fileDescriptor, (char *)d_buffer_p, Port::maxBufferSize, MSG_PEEK);
  
  if( d_isReceptionStarted )
	{
	  if(d_isTimeout)	//The receive timer has expired...this is a timeout condition
		{
		  resetReadTimeout(Port::timeoutCompletePacket); //Clear the timeout flag
		  d_lastBytesAvail = 0;		//Reset the flags
		  d_isReceptionStarted = false;
		  onRxTimeout();	//Do whatever 'we' do for a timeout (probably a flush or disconnect)...
		  return;
		}
	}
  else
	{	//Start the receive timer
	  resetReadTimeout(Port::timeoutCompletePacket);	//Got 'n' seconds to get all the data else we timeout
	  d_isReceptionStarted = true;
	}
  if(d_lastBytesAvail == bytesAvail)	//Check if any more data has been received since last time
	{										//No point in parsing unless this has changed!
	  //Maybe yield in here!
	  //Thread::yield();
	  if(bytesAvail == 0)		//If we have been called with 0 bytes available (twice now)
		{							//a disconnection has occurred
		  if(!d_doDisconnect)
			{
			  closeSocket();	//Force the close
			}
		}
	  return;
	}
  //Depending on your application you may want to attempt to process the extra data 
  //(or change your MAX_RXBUF).
  //
  //Here I just flush the whole lot, because I assume a 'legal' client wont send more than
  //we can receive....maybe someone is trying to flood / overrun us!
  if(bytesAvail > Port::maxBufferSize)	
	{
	  Log::instance().log( "Port::pending: fail:\n\tTCP/IP overflow." );
	  flushRxData();
	  d_lastBytesAvail = 0;
	  d_isReceptionStarted = false;
	  return;
	}
  d_lastBytesAvail = bytesAvail;
  //In this loop you may parse the received data to determine whether a whole
  //'packet' has arrived. What you do in here depends on what data you are sending.
  //Here we will just look for a /r/n terminator sequence.
  //  for(int i=0; i < nBytesAvail; i++)
  //d_ofstream << m_pBuf[0];	  
  //d_ofstream << m_pBuf[1];	  
  //d_ofstream << m_pBuf[2];

  for(int i=0; i < bytesAvail; i++)//empiezo desde 2 para que llege header pre iso todo: segun yo ya se deveria empesar directamente desde 1, nomas para saltarse el x03 i fuera necesario
	{
	  // d_ofstream << m_pBuf[i];	  
	  /***************************SHOULD BE C*******************/
	  
	  if(d_buffer_p[i] == '\x03') //caracter terminador de la secuencia
		{
		  /**************************************************************/
		  // COMPULSORY ... Clear the flag and count..
		  // do this when you have received a good packet
		  d_lastBytesAvail = 0;
		  d_isReceptionStarted = false;
		  /**************************************************************/
		  
		  // Now receive the data into a buffer and call our receive function
		  //int nLen = i+2;
		  int nLen = i+1;//de seguro es 2 por el \x02 y el \x0 que pone al final todo: checar este 2
		  //char *pszRxData = new char[nLen+1];	//Allow space for terminator
		  char *pszRxData = new char[nLen];	//Allow space for terminator
		  ::recv(d_fileDescriptor, (char *)pszRxData, nLen, 0);
		  //		  pszRxData[nLen] = '\0';		//Terminate it
		  /******************************************************/
		  //aqui viene la parte que puede lanzar excepciones

		  //		  ofstream xxx("lastReceived");
		  //xxx.write(pszRxData,nLen);
		  //xxx.close();
		  try
			{
			  onDataReceived(pszRxData, nLen);
			}catch(...){
			  Log::instance().log("Port::pending: ???::exception::\n\t::Ocurrio una excepcion al querer procesar los datos recibidos con la funcion OnDataReceived.");
			}//catch
		  /***********************************************/
		  delete [] pszRxData;
		  return;
		}//if
	}//for
}//pending
/***************************END C*******************/

/**
 *
 */
void Port::onOutput()
{
  if(d_doDisconnect)
	return(a_size);
  
  //int nSent = send(buf, len);
  int nSent = ::send(d_fileDescriptor, (const char *)a_buffer_p, a_size, 0);
  //  struct pollfd f_pollfd;
  //f_pollfd.fd = d_fileDescriptor;
  //f_pollfd.revents = 0;
  //f_pollfd.events = POLLOUT;
  while(!isPending(pendingOutput, 0))		//Wait for output to complete
	{
	  if(d_doDisconnect || !d_isOpen)
		{
		  //If we are disconnecting, just pretend all the bytes were sent
		  return a_size;
		}
	  //I like to yield whenever waiting for things...
	  //this is optional and may not suit your implementation!
	  // Thread::yield();
	}
  //  Log::instance().log( "< MSG 001081 >: Port::onOutput: info:" );
}//

/**
 *	Overridden from class Port.
 *	Called when the socket has been disconnected from the client-side.
 *	Under some conditions this function is NOT called, which is why we have
 *	some additional disconnection functionality within pending().
 */
void Port::onDisconnect(void)
{
  d_isActive = false;
  d_isLogon = false;
  if(d_isOpen)
	{
	  d_doDisconnect = true;
	  closeSocket();
	  //aqui deveria de ir la de connect();
	}
}//

/**
 *	Overridden from class Port.
 *	This function is called by the system when our timer expires.
 *	We use the timer for 2 things:
 *	1) To determine whether reception has timed out. (Timer started in pending())
 *	2) To call CloseInterface to safely destroy the port.
 */
void Port::onExpired(void)
{
  d_isActive = false;//mejor decirle a el exterior que ya no cuenten con el puerto desde el principio y luego darlo de baja
  if(d_doDisconnect && d_isOpen)
	{
	  closeSocket();
	}
  else if(d_isOpen && d_isReceptionStarted)
	{
	  //Timer must have expired because the rx data has not all been received
	  d_isTimeout = true;
	}
}//expired

/**
 * sincroniza el acceso a el mapa de puertos, para que run y los hilos Connect no accesen al mismo tiempo
 */
ost::Mutex& Port::getSendMutex_r()
{
  static ost::Mutex mutex; // para evitar race conditions en constructor
  return mutex;
}//


/**
 *	This function will send the specified number of bytes, or the whole string
 *	(without the terminating '\0')
 */
bool Port::writeData(
					 const char *a_data_p
					 ,const int a_byteCount
					 ,bool a_isIgnoredLogon
					 )
{
  MutexLock mutexLock( getSendMutex_r() ); 
  if( ! d_isActive )
	{
	  ecc::Exception e;
	  e
		<< "< MSG 001062 >:  ecc::Exception\n\n\tLa conexión -"
		<< d_hostName << "-,-"
		<< d_portNumber <<"-, Esta cerrada!.\nthrow in Port::writeData.\n";
	  throw e;
	}
  if( ! a_isIgnoredLogon )
	if( ! d_isLogon )
	  {
		ecc::Exception e;
		e 
		  << "< MSG 001063 >:  ecc::Exception\n\n\tLa conexión -"
		  << d_hostName << "-,-"
		  << d_portNumber <<"-, No ha anunciado por mensaje LOGON\n que esta lista para recibir mensajes!."
		  << "\nthrow in Port::writeData.\n";
		throw e;
	  }
  //First calculate how many bytes we are to send
  int nLen = a_byteCount;
  
  if(nLen == -1)
  	nLen = strlen(a_data_p);
  
  int nBytesToSend = nLen;
  while(d_isOpen && nLen)//esto seria por el non-bloking socket, que hay que iterar
	{
	  nLen -= ensureSend((void *)&(a_data_p[nBytesToSend - nLen]), nLen);
	}
  return(true);
}//writeData

/**
 *	Our function to provide uniform closure of the Socket.
 *	Can be called from the outside!
 */
bool Port::closeSocket(void)
{
  if(d_isOpen && d_doDisconnect)
	{
	  d_isOpen = false;				//If m_bDoDisconnect == true we know this has been called 
	  onConnectionClosed();			//through the timer, so 'delete this' is safe!
	  // delete this;// orita no lo borro... porque lo pueden utilizar otros hilos... mejor eso se lo dejo a el connnector qque lo creo
	}
  else if(d_isOpen)
	{
	  d_doDisconnect = true;			//Just set the timer and the flag so we can 
	  //Thread::sleep(Port::timeoutDisconnect);
	  setReferenceTime(Port::timeoutDisconnect);		//disconnect safely, in DISCONNECT_MS
	}
  ::close(d_fileDescriptor);
  return(true);
}//


/**
 *	This function should be called from pending() when the first bytes of our 
 *	data has been received. If the complete data has not been received by the time
 *	this expires we consider this an error.
 *	
 */
void Port::resetReadTimeout(unsigned long a_timeout)
{
  d_isTimeout = false;
  setReferenceTime(a_timeout);
}//ResetReadTimeOut


/**
 *	This function should be use in the event of a reception error, to flush out
 *	the receive buffer.
 */
void Port::flushRxData(void)
{
  stringstream sstream;
  int received;
  sstream 
	<< "Port::FlushTxData:fail:\n\n\tflushed por timeout datos en buffer que no han llegado como trama completa. datos recibidos:\n\n";
  for(;;)
	{
	  //received = receive(m_pBuf, MAX_RXBUF);
	  received = ::recv(d_fileDescriptor, (char *)d_buffer_p, Port::maxBufferSize, 0);
	  if( received > 0 )
		sstream.write(d_buffer_p, received);
	  else
		break;
	}//for
  Log::instance().log( sstream.str().c_str() );
}//FlushRxData

/**
 *	Called when the receive timeout occurs
 */
void Port::onRxTimeout(void)
{
  flushRxData();
}//OnRxTimeout

/** 
 *	Little utility function for sending data to the client.
 *	@return Number of bytes sent to client
 */
int Port::ensureSend(void *a_buffer_p, int a_size)
{
  //If we are disconnecting, just pretend all the bytes were sent
  if(d_doDisconnect)
	return(a_size);
  
  //int nSent = send(buf, len);
  int nSent = ::send(d_fileDescriptor, (const char *)a_buffer_p, a_size, 0);
  //  struct pollfd f_pollfd;
  //f_pollfd.fd = d_fileDescriptor;
  //f_pollfd.revents = 0;
  //f_pollfd.events = POLLOUT;
  while(!isPending(pendingOutput, 0))		//Wait for output to complete
	{
	  if(d_doDisconnect || !d_isOpen)
		{
		  //If we are disconnecting, just pretend all the bytes were sent
		  return a_size;
		}
	  //I like to yield whenever waiting for things...
	  //this is optional and may not suit your implementation!
	  // Thread::yield();
	}
  return nSent;
}//DoSend

/**
 *
 */
bool Port::isPending(Pending a_pending, unsigned long a_timeout)
{
  struct timeval tv;
  struct timeval *tvp = &tv;
  fd_set grp;
  
  if(a_timeout == ~((unsigned long)0) )
	tvp = NULL;
  else
	{
	  tv.tv_usec = (a_timeout % 1000) * 1000;
	  tv.tv_sec = a_timeout / 1000;
	}
  
  int status;
  FD_ZERO(&grp);
  FD_SET(d_fileDescriptor, &grp);
  switch(a_pending)
	{
	case pendingInput:
	  status = select(d_fileDescriptor, &grp, NULL, NULL, tvp);
	  break;
	case pendingOutput:
	  status = select(d_fileDescriptor, NULL, &grp, NULL, tvp);
	  break;
	case pendingError:
	  status = select(d_fileDescriptor, NULL, NULL, &grp, tvp);
	  break;
	}
  if(status < 1)
	return false;
  if(FD_ISSET(d_fileDescriptor, &grp))
	return true;
  return false;
}//

/**
 *
 */
void Port::setReferenceTime(unsigned long a_timeout)
{
  struct timeval now;
  gettimeofday(&now, NULL);
  int secs = a_timeout / 1000;
  int usecs = (a_timeout % 1000) * 1000;
  
  d_referenceTime_timeval.tv_usec += usecs;
  if(d_referenceTime_timeval.tv_usec > 1000000l)
	{
	  ++d_referenceTime_timeval.tv_sec;
	  d_referenceTime_timeval.tv_usec %= 1000000l;
	}
  d_referenceTime_timeval.tv_sec += secs;
}//

/**
 *
 */
bool Port::isActive(bool a_isActive)
{
  bool isActiveOld = d_isActive;
  d_isActive = a_isActive;
  return isActiveOld;
}//

/**
 *
 */
bool Port::doDisconnect(bool a_doDisconnect)
{
  bool doDisconnectOld = d_doDisconnect;
  d_doDisconnect = a_doDisconnect;
  return doDisconnectOld;
}//

/**
 *
 */
unsigned long Port::relativeTime()
{
  struct timeval now;
  long diff;//porque puede tener signo

  if( d_isActive )
	{
	  gettimeofday(&now, NULL);
	  diff = 
		(now.tv_sec -d_referenceTime_timeval.tv_sec) * 1000l 
		+ (now.tv_usec - d_referenceTime_timeval.tv_usec) / 1000l;
	  if(diff < 0)
		return 0;
	  return diff;
	}
  else
	{
	  return ~((unsigned long) 0);//un numero muy muy grande
	}
}//

/**
 *
 */
bool Port::isLogon(bool a_isLogon)
{
  bool oldIsLogon = d_isLogon;
  d_isLogon = a_isLogon;
  return oldIsLogon;
}//
#endif
