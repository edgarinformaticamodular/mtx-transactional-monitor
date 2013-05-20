#ifndef ECC_SYNCQUEUE_CPP
#define ECC_SYNCQUEUE_CPP

template<char t_char>
SyncQueue<t_char>* SyncQueue<t_char>::d_syncQueue_p = 0;

template<char t_char>
bool SyncQueue<t_char>::d_isDestroyed = false;

/**
 *
 */
template<char t_char>
SyncQueue<t_char>& SyncQueue<t_char>::instance()
{
  if(!d_syncQueue_p)
	{
	  ost::MutexLock mutexLock( getMutex_r() );
	  if(!d_syncQueue_p)
		{
		  if(d_isDestroyed)
			{
			  onDeadReference();
			}
		  else
			{
			  create();
			}
		}
	}
  return *d_syncQueue_p;
}//instance


/**
 *
 */
template<char t_char>
void SyncQueue<t_char>::create()
{
  static SyncQueue<t_char> syncQueue;
  d_syncQueue_p = &syncQueue;
}//create


/**
 *
 */
template<char t_char>
void SyncQueue<t_char>::onDeadReference()
{
  create();
  new (d_syncQueue_p) SyncQueue<t_char>();
  d_isDestroyed = false;
}//onDeadReference

/**
 *
 */
template<char t_char>
SyncQueue<t_char>::SyncQueue()
{
}//SyncQueue

/**
 *
 */
template<char t_char>
SyncQueue<t_char>::~SyncQueue()
{
  d_isDestroyed = true;
}//~SyncQueue


/**
 *
 */
template<char t_char>
void SyncQueue<t_char>::killPhoenix()
{
  d_syncQueue_p->~SyncQueue();
}//killPhoenix

/**
 *
 */
template<char t_char>
void SyncQueue<t_char>::push(Message* a_message_p)
{
  LockingPtr<Queue> queue_lp(d_queue, d_mutex);
  queue_lp->push(a_message_p);
  d_semaphore.post();
}//push

/**
 *
 */
template<char t_char>
Message* SyncQueue<t_char>::pop()
{
  LockingPtr<Queue> queue_lp(d_queue, d_mutex);
  if( queue_lp->empty() )
	return 0;
  Message* message_p = queue_lp->front();
  queue_lp->pop();
  return message_p;
}//SyncQueue

/**
 *
 */
template<char t_char>
unsigned int SyncQueue<t_char>::size() {
  return LockingPtr<Queue>(d_queue, d_mutex)->size();
}//size

/**
 *
 */
template<char t_char>
ost::Semaphore& SyncQueue<t_char>::getSemaphore_r()
{
  return d_semaphore;
}//getSemaphore_r

/**
 *
 */
template<char t_char>
void SyncQueue<t_char>::display()
{
  LockingPtr<Queue> queue_lp(d_queue, d_mutex);
  stringstream sstream;
  sstream<< "SyncQueue:"<<endl;
  for( int i = 0; i< queue_lp->size(); i++ )
	{
	  Message* message_p = queue_lp->front();
	  queue_lp->pop();
	  queue_lp->push(message_p);
	  sstream << message_p->getTramaPartition() <<endl;
	}//for
  //cout<< sstream.str()<<endl;
}//display

/**
 *
 */
template<char t_char>
ost::Mutex& SyncQueue<t_char>::getMutex_r()
{
  static ost::Mutex mutex;
  return mutex;
}//getMutex_r
#endif
