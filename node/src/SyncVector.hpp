#ifndef ECC_SYNC_VECTOR_HPP
#define ECC_SYNC_VECTOR_HPP

#include <cc++/thread.h> 
#include <vector>
#include "Message.hpp"
#include "TimedSemaphore.hpp"

namespace ecc
{

  class SyncVector {
  public:
	bool pushBack(Message*);
	Message* get(int);
	Message* first();
	Message* last();
	unsigned int size();
	void display();
	TimedSemaphore& getTimedSemaphore_r();
	void remove(int);
	void purge(int, SyncVector&);
	void purge();
	void deleteAll();
  private:
    typedef std::vector<Message*> Vector;
    volatile Vector d_vector;
    ost::Mutex d_mutex; // controls access to buffer_
	TimedSemaphore d_timedSemaphore;
  };
}

#endif
