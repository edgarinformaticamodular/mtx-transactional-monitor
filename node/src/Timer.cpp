#ifndef ECC_TIMER_CPP
#define ECC_TIMER_CPP

#include "Timer.hpp"
#include <cc++/thread.h>

using namespace ecc;
/**
 *
 */
Timer::Timer(ost::Semaphore& a_semaphore, int a_time)
	:Thread()
	,d_semaphore(a_semaphore)
	,d_time(a_time)
{
}//Timer

/**
 *
 */
void Timer::run()
{
  Thread::sleep(d_time);
  d_semaphore.post();
}//run

/**
 *
 */
Timer::~Timer()
{
  terminate();
}//~Timer

#endif
