#ifndef ECC_TIMER_HPP
#define ECC_TIMER_HPP

#include <cc++/thread.h>

namespace ecc {

class Timer : public ost::Thread {
public:
  Timer(ost::Semaphore&, int);
  virtual void run();
  ~Timer();
private:
  ost::Semaphore& d_semaphore;
  int d_time;
};//class Timer

}//namespace ecc

#endif
