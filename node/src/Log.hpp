#ifndef ECC_LOG_HPP
#define ECC_LOG_HPP

#include <cc++/thread.h> 
#include <set>
#include "LockingPtr.hpp"
#include <fstream>
#include <cc++/numbers.h> 
#include <string>

using std::string;
using std::ofstream;

namespace ecc {

  class Log {
  public:
	static Log& instance();
	void log(const char*);
	void log0(const char*);
	void header();
	void close();
	static void loadContext();
	//  private:
	Log(string);
	static void create();
	static void onDeadReference();
	virtual ~Log();
	void killPhoenix();
	static ost::Mutex& getMutex_r();
	static string createDefaultLogFileName(string a_fileName = "ppmtx");
  private:
	static string d_fileName;
	static string d_prefix;
	string d_separator;
	static Log* d_log_p;
	volatile ofstream d_ofstream;
	static bool d_isDestroyed;
    ost::Mutex d_mutex; // para serializar el acseso a d_ofstream
	ost::Datetime d_datetime;
  };//class Log

}//namespace db

#endif
