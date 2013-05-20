#ifndef ECC_TIMED_SEMAPHORE_HPP
#define ECC_TIMED_SEMAPHORE_HPP

#include <cc++/thread.h>

namespace ecc {

  class TimedSemaphore : public ost::Semaphore {

  public:
	TimedSemaphore();
	~TimedSemaphore();
	void timedWait(int);
  };//class TimedSemaphore

}//namespace ecc

#endif
