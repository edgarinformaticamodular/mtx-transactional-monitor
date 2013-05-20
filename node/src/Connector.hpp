#ifndef ECC_CONNECTOR_HPP
#define ECC_CONNECTOR_HPP

#include <cc++/thread.h>

using ost::Thread;

/**
 *
 */
namespace ecc
{
  class Port;
  class PortManager;

  class Connector :public virtual Thread
  {
  protected:
	static int defaultTimeToReconnect;
	static int defaultInitialLogCountdownToReconnect;
	Port* d_port_p;
	PortManager* d_portManager_p;
	int d_timeToReconnect;
	int d_initialLogCountdownToReconnect;
	bool d_isQuit;
  public:
	Connector( PortManager*, Port* );
	virtual ~Connector();
	void run(void);
	void loadContext();
	void final();
  };//class Connector

}//namespace ecc

#endif
