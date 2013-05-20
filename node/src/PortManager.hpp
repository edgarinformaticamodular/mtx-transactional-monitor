#ifndef ECC_PORT_MANAGER_HPP
#define ECC_PORT_MANAGER_HPP

#include <cc++/thread.h>

using ost::Thread;

#include <sstream>
#include <string>
#include <map>
#include <set>

using std::string;
using std::map;
using std::set;
using std::stringstream;

#include "Port.hpp"

using ecc::Port;

/**
 *
 */
namespace ecc
{
  class PortManager :public virtual Thread
  {
  public:
	static PortManager& instance();
  private:
	PortManager();
	virtual ~PortManager();
	static void create();
	static void onDeadReference();
	static ost::Mutex& getSingletonMutex_r();
  private:
	static PortManager* d_portManager_p;
	static bool d_isDestroyed;
  protected:
	map<string,Port*> d_ports;
	int d_maxFileDescriptor;
	bool d_isQuit;
  public:
	static bool isReceptor(string a_identifyer);
	//particulares
	static void loadContext();
	void activeAllPorts();
	void desactiveAllPorts();
	void run();
	void isLogon(string,bool);
	//accesores
	void add(string, Port*);
	void remove(Port*);
	bool send(string,string,bool a_isRequiredLogon=true);
	bool sendAll(string,bool a_isRequiredLogon=true);
	bool isQuit(bool);
	static Mutex& getMapMutex_r();
	static string webPortNamePrefix;
	static set<string> d_receptors;
  };//class PortManager

}//namespace ecc

#endif
