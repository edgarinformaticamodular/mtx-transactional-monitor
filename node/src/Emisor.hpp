#ifndef ECC_EMISOR_HPP
#define ECC_EMISOR_HPP

#include <cc++/thread.h>
#include "Message.hpp"

using ost::Thread;

namespace ecc
{
  class Emisor : public virtual Thread
  {
  public:
	typedef enum {
	  ECHO
	  ,LOGON
	  ,LOGOFF
	} InformationType;
	static Emisor& instance();
	void run();
	void w0800(string,string);
	void w0800(string);//para enviar a todos los puertos
	void w0810(string,string,string);
	void loadContext();
	bool isQuit(bool a_isQuit);
  protected:
	void final();
  private:
	Emisor();
	Message* createMessage(string);
	virtual ~Emisor();
	static void create();
	static void onDeadReference();
	static ost::Mutex& getSingletonMutex_r();
  private:
	static Emisor* d_emisor_p;
	static bool d_isDestroyed;
	bool d_isEcosEmissionActive;
	int d_ecosEmissionInterval;
	bool d_isQuit;
  };//class Emisor
  
}//namespace ecc

#endif
