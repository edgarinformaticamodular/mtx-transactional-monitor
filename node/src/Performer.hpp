#ifndef ECC_PERFORMER_HPP
#define ECC_PERFORMER_HPP

#include <cc++/thread.h>
#include <cc++/numbers.h> 

#include <stdlib.h>
#include <new>
#include <iostream>
 
#include "Message.hpp"
#include "SyncQueue.hpp"
#include "QueryManager.hpp"
#include "Context.hpp"
#include "ParseException.hpp"
#include "Exception.hpp"
#include "Log.hpp"
#include "PortManager.hpp"
#include "PpMtxApp.hpp"

using ost::Thread;
using ost::Mutex;

using ecc::Message;
using ecc::Message;
using ecc::SyncQueue;
using ecc::QueryManager;
using ecc::Context;
using ecc::ParseException;
using ecc::Exception;
using ecc::Log;
using ecc::PortManager;

namespace ecc {

  template<char t_char>
  class Performer : public virtual ost::Thread {
  public:
	static Performer& instance();
	void onMessage(Message*);
	virtual void run();
	void reject(Message*,string);
	bool isQuit(bool);
  private:
	Performer(SyncQueue<t_char>&);
	static void create();
	static void onDeadReference();
	void final();
	virtual ~Performer();
	void killPhoenix();
	static ost::Mutex& getMutex_r();
  private:
	static Performer<t_char>* d_performer_p;
	static bool d_isDestroyed;
	SyncQueue<t_char>& d_syncQueue;
	bool d_isQuit;
  };//class Performer

  typedef Performer<'A'> PerformerAcceptor;
  typedef Performer<'R'> PerformerReceptor;

#include "Performer.cpp"

}//namespace db

#endif
