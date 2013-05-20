#ifndef ECC_PORT_HPP
#define ECC_PORT_HPP

#include <cc++/thread.h> 

#include "Message.hpp"

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

using std::string;
using ost::Mutex;

namespace ecc
{

  class Port 
  {
  protected:
	typedef enum {pendingInput, pendingOutput, pendingError} Pending;
	typedef enum {timeoutFlushData, overflowFlushData} FlushData;
	static unsigned long timeoutCompletePacket; //time out para que se complete un paquete
	static unsigned long  timeoutDisconnect;//timeout para la desconeccion
	static int maxBufferSize;//maximo tamanno de buffer
	string d_hostName;
	int d_portNumber;	
	timeval d_referenceTime_timeval;
	int d_fileDescriptor;
	bool d_isActive;
	bool d_isLogon;
	bool d_isOpen;
	bool d_doDisconnect;
	bool d_isTimeout;
	bool d_isReceptionStarted;
	bool d_lastBytesAvail;
	char* d_buffer_p;
	struct sockaddr_in d_remote__sockaddr_in;//siempre el cliente
	int d_bytesToSend;
	string d_name;
	bool d_isReconnect;
  public:
	static int ID_DEFAULT;
	Port(string,string, int);
	~Port();
	virtual void onConnectionClosed(void);
	virtual void onDataReceived(char*, unsigned int);
	virtual void push(Message*) = 0;
	virtual bool connect() = 0;
	virtual void endSocket(void);
	int fileDescriptor() const { return d_fileDescriptor; }
	void setId(int);
	unsigned long relativeTime(void);
	void setReferenceTime(unsigned long);
	bool isActive(void) const { return d_isActive; }
	bool isLogon(void) const { return d_isLogon; }
	bool isLogon(bool);
	void isTimeout(bool a_isTimeout){ d_isTimeout = a_isTimeout; }
	bool isTimeout() const { return d_isTimeout; }
	bool isReceptionStarted() const {return d_isReceptionStarted; }
	string getHostName() const { return d_hostName; }
	int getPortNumber() const { return d_portNumber; }
	string getName();
	void setName(string a_name) { d_name = a_name; }
	bool doDisconnect(bool);
  public:
	void onDisconnect(void);
	void onExpired(void);
	bool writeData(const char*, const int, bool a_isRequiredLogon = true);
	bool isActive(bool);
	bool closeSocket(void);
	bool onPending();
	void onOutput();
	bool isReconnect(bool);
	bool isReconnect();
	void virtual init() = 0;
  protected:
	void resetReadTimeout(unsigned long); //Clear the timeout flag
	void onRxTimeout(void);
	void flushRxData(FlushData);
	int ensureSend(void *, int);
	bool isPending(Port::Pending , unsigned long);
	ost::Mutex& Port::getSendMutex_r();
  };//class Port
  //  using namespace Loki;
}//namespace ecc

#endif
