#ifndef ECC_CONTEXT_HPP
#define ECC_CONTEXT_HPP

#include <cc++/thread.h> 
#include <libpq-fe.h>
#include <fstream>
#include <map>
#include "Connector.hpp"

using std::map;
using std::string;

using ost::Mutex;

namespace ecc {

  class Context {
  public:
	static Context& instance();
	string& getProperty_r(string);
  private:
	Context();
	static void create();
	static void onDeadReference();
	virtual ~Context();
	static ost::Mutex& getMutex_r();
  private:
	static const char* DEFAULT_CONFIG_FILE;
	static Context* d_context_p;
	static bool d_isDestroyed;
	typedef map<string,string> Map;
	Map d_properties;
  };//class Context

}//

#endif
