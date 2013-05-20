#ifndef ECC_SYNCMAP_CPP
#define ECC_SYNCMAP_CPP

#include <stdlib.h>
#include <new>
#include <iostream>

#include <cc++/thread.h> 

#include "SyncMap.hpp"
#include "Exception.hpp"
#include "SyncMap.hpp"
#include "Tx.hpp"
#include "LockingPtr.hpp"
#include "Context.hpp"
#include "Log.hpp"

using std::cout;

using ::ecc::LockingPtr;
using ::ecc::Exception;
using ::ecc::SyncMap;
using ::ecc::Message;

SyncMap* SyncMap::d_syncMap_p = 0;
bool SyncMap::d_isDestroyed = false;
/**
 *
 */
SyncMap& SyncMap::instance()
{
  if(!d_syncMap_p)
	{
	  ost::MutexLock mutexLock(getMutex_r());
	  if(!d_syncMap_p)
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
  return *d_syncMap_p;
}//instance

/**
 *
 */
void SyncMap::create()
{
  static SyncMap syncMap;
  d_syncMap_p = &syncMap;
}//create

/**
 *
 */
void SyncMap::onDeadReference()
{
  create();
  new (d_syncMap_p) SyncMap;
  //atexit(killPhoenix);
  d_isDestroyed = false;
}//onDeadReference

/**
 *
 */
SyncMap::SyncMap()
{
}//SyncMap


/**
 *
 */
SyncMap::~SyncMap()
{
  d_syncMap_p = 0;
  d_isDestroyed = true;
}//~SyncMap


/**
 *
 */
void SyncMap::killPhoenix()
{
  d_syncMap_p->~SyncMap();
}//killPhoenix


void SyncMap::put(string a_int, ecc::Tx_sp a_tx_sp)
{
  LockingPtr<Map> map_lp(d_map, d_mutex);
  (*map_lp)[a_int] = a_tx_sp;
}//SyncMap

void SyncMap::remove(string a_int)
{
  LockingPtr<Map> map_lp(d_map, d_mutex);
  Map::iterator itr = map_lp->find(a_int);
  if( itr != map_lp->end() )
	{
	  map_lp->erase( itr );
	}
}//SyncMap


ecc::Tx_sp SyncMap::get_sp(string a_int)
{
  LockingPtr<Map> map_lp(d_map, d_mutex);
  Map::iterator itr = map_lp->find(a_int);
  if( itr == map_lp->end() )
	return 0;
  else
	return itr->second;
}//SyncMap


unsigned int SyncMap::size() {
    return LockingPtr<Map>(d_map, d_mutex)->size();
}

bool SyncMap::isExist(string a_int)
{
  LockingPtr<Map> map_lp(d_map, d_mutex);
  Map::iterator itr = map_lp->find(a_int);
  if( itr == map_lp->end() )
	return true;
  else
	return false;
}//isExist

bool SyncMap::isExist(string a_int,Message* a_message_p)
{
  LockingPtr<Map> map_lp(d_map, d_mutex);
  Map::iterator itr = map_lp->find(a_int);
  if( itr == map_lp->end() )
	return false;
  bool aux = itr->second->addIn(a_message_p);
  return aux;
}//isExist

/**
 *
 */
void SyncMap::display()
{
  LockingPtr<Map> map_lp(d_map, d_mutex);
  //cout << "Map::display: transacciones en lista" << endl;
  map<string,ecc::Tx_sp>::const_iterator itr;
  for(itr = map_lp->begin(); itr!=map_lp->end(); itr++)
	{
	  itr->second->display();
	}//for
  
  stringstream sstream;
  //cout << endl << "Hay: -" << map_lp->size() << "- Tx pendientes." << endl;
}//display

/**
 *
 */
ost::Mutex& SyncMap::getMutex_r()
{
  static ost::Mutex mutex;
  return mutex;
}//getMutex_r

#endif
