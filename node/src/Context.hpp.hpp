#ifndef ECC_CONTEXT_HPP
#define ECC_CONTEXT_HPP

#define x(a) cout << a << endl;

#include "ConnectorFactory.hpp"

//#include "Server.hpp"
#include "Object.hpp"
#include "Par.hpp"
#include "Log.hpp"
#include <map>
#include <string>
#include <loki/SmartPtr.h>
#include "SyncMap.hpp"
#include "SyncQueue.hpp"
#include "Performer.hpp"

namespace ecc
{
  using namespace Loki;
  typedef SmartPtr<Object> SpObject;
  // typedef SmartPtr<Server> SpServer;

  /**
   *
   */
  class Context : public Object
  {
  private:
	static map<string,string>* d_properties_p;
	static map<string,SpObject>* d_spObjects_p;
	static map<Par,Par>* d_pars_p;
	static string d_configFileName;
	static Log* d_log_p;
	static SyncMap* d_mapTx_p;
	static SyncQueue* d_queueT_p;
	static SyncQueue* d_queueC_p;
	static Performer* d_performerT_p;
	static Performer* d_performerC_p;

  public:
	static char* CONFIGURATION_DEFAULT_p;

  public:
	static void init();
	static void finish();
	static void loadProperties(string);
	static string getProperty(string);
	static void showConfig();
	static void put(string, SpObject);
	static SpObject get(string);
	static ConnectorFactory* createConnectorFactory(
									 ConnectorFactory::ConnectorRole);
	static void loadPar(Par&);
	static void putPar(Par, Par);
	static void putPar(Par);
	//	static Par getPar(Par);
	static Par getPar(const Par&);
	static bool send(char,int,const char*,const int);
	static bool send(char,int,string);
	static Log& getLog();
	static void setLog(Log*);
	static void log(string);
	static ostream& log();
	static SyncMap& getMapTx_r();
	static SyncQueue& getQueueT_r();
	static SyncQueue& getQueueC_r();
	static Performer& getPerformerT_r();
	static Performer& getPerformerC_r();
  };//class Context
}//namespace ecc

#endif
