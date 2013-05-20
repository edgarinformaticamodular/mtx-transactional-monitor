#ifndef ECC_DISCONNECTOR_HPP
#define ECC_DISCONNECTOR_HPP

#include <cc++/thread.h>

#include "Port.hpp"
#include "PortManager.hpp"

using ost::Thread;

/**
 *
 */
namespace ecc
{
  //  class Port;
  //class PortManager;

  class Disconnector :public virtual Thread
  {
  protected: //todo mandar el logoff !!!!!!!!!!!!!!!
	static int defaultTimeToDisconnect;
	static int defaultInitialLogCountdownToDisconnect;
	Port* d_port_p;
	PortManager* d_portManager_p;
	int d_timeToReconnect;
	int d_initialLogCountdownToReconnect;
	static bool d_isQuit;
  public:
	Disconnector( PortManager*, Port* );
	virtual ~Disconnector();
	void run(void);
	void loadContext();
	void final();
	static bool isQuit(bool);
  };//class Disconnector

}//namespace ecc

#endif
