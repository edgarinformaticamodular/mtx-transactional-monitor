#ifndef ECC_EMISOR_CPP
#define ECC_EMISOR_CPP

#include <cc++/thread.h>
#include <cc++/numbers.h> 

#include <iostream>
using std::cout;

#include "Message.hpp"
#include "Emisor.hpp"
#include "Exception.hpp"
#include "Log.hpp"
#include "Context.hpp"
#include "BadArgumentException.hpp"
#include "QueryManager.hpp"
#include "PortManager.hpp"
#include "PpMtxApp.hpp"
#include "MessageFactory.hpp"

using ecc::Message;
using ecc::Emisor;
using ecc::Log;
using ecc::Context;
using ecc::Exception;
using ecc::BadArgumentException;
using ecc::QueryManager;
using ecc::PortManager;
using ecc::PpMtxApp;
using ecc::MessageFactory;

Emisor* Emisor::d_emisor_p = 0;
bool Emisor::d_isDestroyed = false;

/**
 *
 */
Emisor& Emisor::instance()
{
  if(!d_emisor_p)
	{
	  ost::MutexLock mutexLock(getSingletonMutex_r());
	  if(!d_emisor_p)
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
  return *d_emisor_p;
}//instance

/**
 *
 */
void Emisor::create()
{
  static Emisor emisor;
  d_emisor_p = &emisor;
}//create

/**
 *
 */
void Emisor::onDeadReference()
{
  create();
  new (d_emisor_p) Emisor();
  d_emisor_p->loadContext();
  d_isDestroyed = false;
}//

/**
 *
 */
Emisor::Emisor()
  :d_ecosEmissionInterval(90000)
  ,d_isEcosEmissionActive(true)
  ,d_isQuit(false)
{
  clrParent();
}//Emisor

/**
 *
 */
bool Emisor::isQuit(bool a_isQuit)
{
  bool isQuitOld = a_isQuit;
  d_isQuit = a_isQuit;
  return isQuitOld;
}//

/**
 *
 */
Emisor::~Emisor()
{
  d_emisor_p = 0;
  d_isDestroyed = true;
  terminate();
}//~Emisor

/**
 *
 */
void Emisor::final()
{
}//

/**
 *
 */
ost::Mutex& Emisor::getSingletonMutex_r()
{
  static ost::Mutex mutex;
  return mutex;
}//getMutex_r

/**
 *
 */
void Emisor::run()
{
  if( d_isEcosEmissionActive )
	{
	  for(;;)
		{
		  try
			{
			  Thread::sleep(d_ecosEmissionInterval);
			  if(d_isQuit)
				break;
			  w0800(Message::ECHO_TEST_NETWORK_MANAGEMENT_INFORMATION_CODE);
			}catch(Exception& e){
			  e
				<< endl
				<< "\ncatch in Emisor::run";
			  Log::instance().log(e.getMessage().c_str());
			}catch(...){}
		}//for
	}
}//


/**
 *
 */
void Emisor::w0800(
				   string a_senderName
				   ,string a_informationCode
				   )
{
  Message* message_p = MessageFactory::create(a_senderName, Message::H0800);
  message_p->setNetworkManagementInformationCode(a_informationCode);
  message_p->serialize();
  PortManager::instance().send( a_senderName, message_p->getTramaOut(), false );
  Log::instance().log( ("< MSG 001554 >: \n\n\t                    To: -" + a_senderName + "-\n" + message_p->getTramaPartition() ).c_str() );
}//

/**
 *
 */
void Emisor::w0800(
				   string a_informationCode
				   )
{
  Message* message_p = MessageFactory::create("WEB ", Message::H0800);
  message_p->setNetworkManagementInformationCode(a_informationCode);
  message_p->serialize();
  PortManager::instance().sendAll( message_p->getTramaOut(), false );
  Log::instance().log( ("< MSG 001555 >: \n\n\t                    To: all active senders\n\n" + message_p->getTramaPartition() ).c_str() );
}//

/**
 *
 */
void Emisor::w0810(
				   string a_senderName
				   ,string a_informationCode
				   ,string a_responseCode
				   )
{
  Message* message_p = MessageFactory::create(a_senderName, Message::H0810);
  message_p->setNetworkManagementInformationCode(a_informationCode);
  message_p->setResponseCode( a_responseCode );
  message_p->serialize();
  PortManager::instance().send( a_senderName, message_p->getTramaOut(), false );
  Log::instance().log( ("< MSG 001556 >: \n\n\t                    To: -" + a_senderName + "-\n\n" + message_p->getTramaPartition() ).c_str() );
}//

/**
 *
 */
void Emisor::loadContext()
{
  try
	{
	  string isEcosEmissionActive_string = Context::instance().getProperty_r("is_ecos_emission_active");
	  if( isEcosEmissionActive_string == "false" )
		d_isEcosEmissionActive = false;
	  else if( isEcosEmissionActive_string == "true" )
		d_isEcosEmissionActive = true;
	  else
		{
		  BadArgumentException e;
		  e.getStream() 
			<< "< MSG 494 >:  BadArgumentException:"
			<< endl
			<< "\tLa propiedad \"is_ecos_emission_active\" del archivo de configuracion, tiene un valor diferente a \"true\" o \"false\": -" << isEcosEmissionActive_string << "-. Se utilizara el default -" << d_isEcosEmissionActive << "-."
			<< endl
			<< "throw in Emisor::loadContext.\n"
			;
		  throw e;
		}	  

	  string ecosEmissionInterval_string
		= Context::instance().getProperty_r("ecos_emission_interval");
	  if( !(stringstream( ecosEmissionInterval_string ) >> d_ecosEmissionInterval) )
		{
		  BadArgumentException e;
		  e
			<< "< MSG 495 >:  BadArgumentException:\n\tValor invalido en archivo de configuracion, para el intervalo de tiempo en la variable -is_ecos_emission_interval- : -" 
			<< ecosEmissionInterval_string 
			<< "-, se utilizara valor por default: -"
			<< d_ecosEmissionInterval <<"-."
			<< "throw in Emisor::loadContext.\n"
			;
		  throw e;
		}

	}catch(Exception& e){
	  e
		<< endl
		<< "catch in Emisor::startupEcos.";
	  Log::instance().log( e.getMessage().c_str() );
	}
}//

#endif
