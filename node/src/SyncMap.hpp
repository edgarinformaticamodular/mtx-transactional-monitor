#ifndef ECC_SYNCMAP_HPP
#define ECC_SYNCMAP_HPP

#include <cc++/thread.h> 
#include <libpq-fe.h>
#include "LockingPtr.hpp"
#include <fstream>
#include <map>
#include "Tx.hpp"
#include "Message.hpp"
#include <string>

using ecc::Message;

namespace ecc {

  class SyncMap {
  public:
	static SyncMap& instance();
	void put(string,ecc::Tx_sp);
	void remove(string);
	ecc::Tx_sp get_sp(string);
	ecc::Tx_sp get_sp(string,Message*);
	unsigned int size();
	bool isExist(string);
	bool isExist(string,Message*);
	void display();
  private:
	SyncMap();
	static void create();
	static void onDeadReference();
	virtual ~SyncMap();
	void killPhoenix();
	static ost::Mutex& getMutex_r();
  private:
	static SyncMap* d_syncMap_p;
	static bool d_isDestroyed;
    typedef std::map<string,ecc::Tx_sp> Map;
    volatile Map d_map;
    ost::Mutex d_mutex; // para serializar el acseso a d_ofstream
  };//class SyncMap

}//namespace ecc

#endif
