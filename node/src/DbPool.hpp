#ifndef ECC_DB_POOL_HPP
#define ECC_DB_POOL_HPP

#include <cc++/thread.h> 
#include <libpq-fe.h>
#include <set>
#include "LockingPtr.hpp"


namespace db {

  typedef ::PGconn* Connection_sp;	

  class DbPool {
  public:
	static DbPool& instance();
	void insert(db::Connection_sp);
	db::Connection_sp erase();
	void closeAll();
	void loadContext();
  private:
	DbPool();
	Connection_sp createConnection_sp();
	static void create();
	static void onDeadReference();
	virtual ~DbPool();
	ost::Semaphore& getSemaphore_r();
	db::Connection_sp create(db::Connection_sp);
	db::Connection_sp check(db::Connection_sp);
	static ost::Mutex& getMutex_r();
  private:
	int d_dbPoolSize;
	static int MAX_CONNECTIONS;
	static DbPool* d_dbPool_p;
	static bool d_isDestroyed;
	typedef std::set<db::Connection_sp> Set;
    volatile Set d_set;
    ost::Mutex d_mutex; 
	ost::Semaphore d_semaphore;
	string d_dbHost;
	string d_dbHostAddr;
	string d_dbPort;
	string d_dbName;
	string d_dbUser;
	string d_dbPassword;
	string d_dbConnectTimeout;
	string d_dbOptions;
	string d_dbTty;
	string d_dbRequireSsl;

  };//class DbPool

}//namespace db

#endif
