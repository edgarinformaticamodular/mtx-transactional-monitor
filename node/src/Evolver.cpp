#ifndef ECC_EVOLVER_CPP
#define ECC_EVOLVER_CPP

#include <cc++/thread.h>
#include <cc++/numbers.h> 

#include <iostream>

#include "Message.hpp"
#include "Evolver.hpp"
#include "Exception.hpp"
#include "Log.hpp"
#include "Context.hpp"
#include "BadArgumentException.hpp"
#include "QueryManager.hpp"
#include "PpMtxApp.hpp"
#include "InitialRC0200.hpp"
#include "InitialRC0200Exception.hpp"
#include "InitialRC0420.hpp"
#include "InitialRT0420.hpp"
#include "InitialRT0421.hpp"
#include "InitialRT0210A.hpp"
#include "InitialRT0210R.hpp"
#include "TestedDone.hpp"
#include "TestedDoneException.hpp"
#include "WT0200Timeout.hpp"
#include "WT0200RT0210A.hpp"
#include "WT0200RT0210AException.hpp"
#include "WT0200RT0210R.hpp"
#include "WT0200RT0210RException.hpp"
#include "WT0420Timeout.hpp"
#include "WT0421Timeout.hpp"
#include "WT0420TimeoutException.hpp"
#include "WT0420RT0430.hpp"

using ecc::InitialRC0200;
using ecc::InitialRC0200Exception;
using ecc::InitialRC0420;
using ecc::InitialRT0420;
using ecc::InitialRT0421;
using ecc::InitialRT0210A;
using ecc::InitialRT0210R;
using ecc::TestedDone;
using ecc::TestedDoneException;
using ecc::WT0200Timeout;
using ecc::WT0200RT0210A;
using ecc::WT0200RT0210AException;
using ecc::WT0200RT0210R;
using ecc::WT0200RT0210RException;
using ecc::WT0420Timeout;
using ecc::WT0421Timeout;
using ecc::WT0420TimeoutException;
using ecc::WT0420RT0430;


using ecc::Message;
using ecc::Evolver;
using ecc::Log;
using ecc::Context;
using ecc::Exception;
using ecc::BadArgumentException;
using ecc::QueryManager;

Evolver* Evolver::d_evolver_p = 0;
bool Evolver::d_isDestroyed = false;

/**
 *
 */
Evolver& Evolver::instance()
{
  if(!d_evolver_p)
	{
	  ost::MutexLock mutexLock(getSingletonMutex_r());
	  if(!d_evolver_p)
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
  return *d_evolver_p;
}//instance

/**
 *
 */
void Evolver::create()
{
  static Evolver evolver;
  d_evolver_p = &evolver;
}//create

/**
 *
 */
void Evolver::onDeadReference()
{
  create();
  new (d_evolver_p) Evolver();
  d_evolver_p->loadContext();
  d_isDestroyed = false;
}//

/**
 *
 */
void Evolver::loadContext()
{
}//

/**
 *
 */
Evolver::Evolver()
{
  loadContext();
  d_rules.push_back( new InitialRC0200() );
  d_rules.push_back( new InitialRC0200Exception() );
  d_rules.push_back( new InitialRC0420() );
  d_rules.push_back( new InitialRT0420() );
  d_rules.push_back( new InitialRT0421() );
  d_rules.push_back( new InitialRT0210A() );
  d_rules.push_back( new InitialRT0210R() );
  d_rules.push_back( new TestedDone() );
  d_rules.push_back( new TestedDoneException() );
  d_rules.push_back( new WT0200Timeout() );
  d_rules.push_back( new WT0200RT0210A() );
  d_rules.push_back( new WT0200RT0210AException() );
  d_rules.push_back( new WT0200RT0210R() );
  d_rules.push_back( new WT0200RT0210RException() );
  d_rules.push_back( new WT0420Timeout() );
  d_rules.push_back( new WT0420TimeoutException() );
  d_rules.push_back( new WT0421Timeout() );
  d_rules.push_back( new WT0420RT0430() );
}//Evolver

/**
 *
 */
Evolver::~Evolver()
{
  d_evolver_p = 0;
  d_isDestroyed = true;
  vector<Rule*>::iterator itr;
  for( itr = d_rules.begin(); itr != d_rules.end(); itr++ )
	{
	  Rule* rule_p = *itr;
	  delete rule_p;
	}
}//~Evolver

/**
 *
 */
ost::Mutex& Evolver::getSingletonMutex_r()
{
  static ost::Mutex mutex;
  return mutex;
}//getMutex_r

/**
 *
 */
void Evolver::evolve(Tx& a_tx)
{
  vector<Rule*>::iterator itr;
  for( itr = d_rules.begin(); itr != d_rules.end(); itr++ )
	{	
	  Rule* rule_p = *itr;
	  if( rule_p->isMatch(a_tx) )
		{
		  rule_p->apply(a_tx);
		  return;
		}
	}
  //si llego hasta aqui es que ninguna se ha aplicado
  d_default.apply(a_tx);
}//

#endif
