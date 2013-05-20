#ifndef ECC_SYNC_VECTOR_CPP
#define ECC_SYNC_VECTOR_CPP

#include "SyncVector.hpp"
#include <cc++/thread.h> 
#include "Message.hpp"
#include "LockingPtr.hpp"
#include <sstream>
#include "SyncQueue.hpp"
#include <iostream>

#define x(a) cout << a << endl;

using ecc::SyncVector;
using ecc::Message;

/**
 *
 */
bool SyncVector::pushBack(Message* a_message_p)
{
  LockingPtr<Vector> vector_lp(d_vector, d_mutex);
  vector_lp->push_back(a_message_p);
  d_timedSemaphore.post();
  return true;
}//pushBack

/**
 *
 */
Message* SyncVector::first()
{
  LockingPtr<Vector> vector_lp(d_vector, d_mutex);
  if( vector_lp->begin() == vector_lp->end() )
	return 0;
  else
	return *( vector_lp->begin() );
}//first


void SyncVector::remove(int a_int)
{
  LockingPtr<Vector> vector_lp(d_vector, d_mutex);
  if( a_int < vector_lp->size() )
	{
	  Vector::iterator itr = vector_lp->begin()+a_int;
	  vector_lp->erase( itr );
	}
}//SyncVector

/**
 *
 */
void SyncVector::purge(int a_int, SyncVector& a_messagesPurge)
{
  LockingPtr<Vector> vector_lp(d_vector, d_mutex);
  Vector::iterator inicio = vector_lp->begin() + a_int + 1;
  Vector::iterator itr;
  
  for(itr = inicio; itr!=vector_lp->end();itr++)
	{
	  a_messagesPurge.pushBack(*itr);
	}
  vector_lp->erase( inicio, vector_lp->end() );
}//purge

/**
 *
 */
void SyncVector::purge()
{
  LockingPtr<Vector> vector_lp(d_vector, d_mutex);
  Vector::iterator itr;
  for(itr = vector_lp->begin(); itr!=vector_lp->end(); itr++)
	{
	  SyncQueueAcceptor::instance().push(*itr);
	}
  vector_lp->erase( vector_lp->begin(), vector_lp->end() );
}//purge

/**
 *
 */
void SyncVector::deleteAll()
{
  LockingPtr<Vector> vector_lp(d_vector, d_mutex);
  Vector::iterator itr;
  for(itr = vector_lp->begin(); itr != vector_lp->end(); itr++)
	delete *itr;
}//deleteAll

/**
 *
 */
Message* SyncVector::last()
{
  LockingPtr<Vector> vector_lp(d_vector, d_mutex);
  if( vector_lp->begin() == vector_lp->end() )
	return 0;
  else
	return *( vector_lp->end() );
}//last

/**
 *
 */
Message* SyncVector::get(int a_int)
{
  LockingPtr<Vector> vector_lp(d_vector, d_mutex);
  if( a_int >= vector_lp->size() || a_int < 0 )
	return 0;
  Message* message_p = (*vector_lp)[a_int];
  return message_p;
}//get

/**
 *
 */
unsigned int SyncVector::size() {
    return LockingPtr<Vector>(d_vector, d_mutex)->size();
}//size

/**
 *
 */
void SyncVector::display()
{
  LockingPtr<Vector> vector_lp(d_vector, d_mutex);
  cout<< "SyncVector:"<<endl;
  Vector::const_iterator itr;
  for(itr = vector_lp->begin(); itr!=vector_lp->end(); itr++)
	{
	  cout << (*itr)->getTramaPartition() <<endl;
	}//for
}//display

/**
 *
 */
ecc::TimedSemaphore& SyncVector::getTimedSemaphore_r()
{
  return d_timedSemaphore;
}//getTimedSemaphore_r

#endif
