#ifndef ECC_TIMED_SEMAPHORE_CPP
#define ECC_TIMED_SEMAPHORE_CPP

#include "TimedSemaphore.hpp"
#include "Timer.hpp"
#include <cc++/thread.h>

using ost::Thread;
using namespace ecc;

/**
 *
 */
TimedSemaphore::TimedSemaphore()
  :Semaphore()
{
}//TimedSemaphore

/**
 *
 */
TimedSemaphore::~TimedSemaphore()
{
}//~TimedSemaphore

/**
 *
 */
void TimedSemaphore::timedWait(int a_milliseconds)
{
  /**
  //  aqui no hay problema porque no se aloja memoria en el heap
  Timer timer(*this,a_milliseconds);
  timer.start();
  wait();  
  */
  int waited = 0;
  for(;;)
	{
	  if( tryWait() )
		break;
	  if( waited > a_milliseconds )
		break;
	  Thread::sleep(50);
	  waited += 50;
	}
}//TimedWait

#endif
