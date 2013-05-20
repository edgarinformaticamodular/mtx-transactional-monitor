#ifndef ECC_DB_QUEUE_MONITOR_HPP
#define ECC_DB_QUEUE_MONITOR_HPP

#include <cc++/thread.h>
#include "Message.hpp"

using ecc::Message;

namespace ecc
{
  class DbQueueMonitor : public ost::Thread
  {
  public:
	static DbQueueMonitor& instance();
	virtual void run();
	void push(Message*);
	bool isReady();
	bool isReady(bool);
	bool isQuit(bool);
	void loadContext();
  private:
	DbQueueMonitor();
	static void create();
	static void onDeadReference();
	virtual ~DbQueueMonitor();
	void killPhoenix();
	static ost::Mutex& getMutex_r();
  private:
	static int INTERVAL_DEFAULT;
	static DbQueueMonitor* d_dbQueueMonitor_p;
	static bool d_isDestroyed;
	int d_interval;//intervalo de tiempo en milisegundos
	bool d_isReady;
	bool d_isQuit;
  };//class DBQueueMonitor
  
}//namespace ecc

#endif
