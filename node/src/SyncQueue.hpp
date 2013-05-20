#ifndef ECC_SYNCQUEUE_HPP
#define ECC_SYNCQUEUE_HPP

#include <cc++/thread.h> 
#include <queue>
#include <stdlib.h>
#include <new>
#include "Exception.hpp"
#include "Log.hpp"
#include "Message.hpp"
#include "LockingPtr.hpp"

using ecc::Exception;
using ecc::Log;
using ecc::Message;
using ecc::LockingPtr;

namespace ecc {

  template<char t_char>
  class SyncQueue {
  public:
	static SyncQueue& instance();
	void push(Message*);
	Message* pop();
	unsigned int size();
	ost::Semaphore& getSemaphore_r();
	void display();
  private:
	SyncQueue();//declarada pero no definida para que marque errror de ligado
	static void create();
	static void onDeadReference();
	virtual ~SyncQueue();
	void killPhoenix();
	static ost::Mutex& getMutex_r();
  private:
	static SyncQueue<t_char>* d_syncQueue_p;
	static bool d_isDestroyed;
    typedef std::queue<Message*> Queue;
    volatile Queue d_queue;
    ost::Mutex d_mutex; // controls access to buffer_
	ost::Semaphore d_semaphore;
  };//class SyncQueue

  typedef SyncQueue<'A'> SyncQueueAcceptor;
  typedef SyncQueue<'R'> SyncQueueReceptor;

#include "SyncQueue.cpp"

}//namespace ecc

#endif
