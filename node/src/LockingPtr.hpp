#ifndef ECC_LOCKING_PTR_HPP
#define ECC_LOCKING_PTR_HPP

#include <cc++/thread.h> 

namespace ecc
{
  template <typename T>
  class LockingPtr {
  public:
	// Constructors/destructors
	LockingPtr(volatile T& a_t, ost::Mutex& a_mutex)
	  :d__t_p(const_cast<T*>(&a_t))
	  ,d_mutex_p(&a_mutex)
	{  
	  a_mutex.enterMutex();    
	}
	~LockingPtr()
	{    
	  d_mutex_p->leaveMutex();    
	}
	// Pointer behavior
	T& operator*()
	{
	  return *d__t_p; 
	}
	T* operator->()
	{
	  return d__t_p; 
	}
  private:
	T* d__t_p;
	ost::Mutex* d_mutex_p;
	LockingPtr(const LockingPtr&);
	LockingPtr& operator=(const LockingPtr&);
  };//class LockingPtr
}//namespace ecc

#endif
