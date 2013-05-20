#ifndef ECC_MESSAGE_FACTORY_CPP
#define ECC_MESSAGE_FACTORY_CPP

#include "Message.hpp"
#include "QueryManager.hpp"
#include "MessageFactory.hpp"
#include "Context.hpp"
#include "NotFoundException.hpp"
#include "BadArgumentException.hpp"
#include <iostream>
#include "PortManager.hpp"

using ecc::MessageFactory;
using ecc::Message;
using ecc::QueryManager;
using ecc::Context;
using ecc::NotFoundException;
using ecc::BadArgumentException;
using ecc::PortManager;

map<string,string> MessageFactory::d_tramasBase;

/**
 *
 */
MessageFactory::MessageFactory()
{
}//

MessageFactory::~MessageFactory()
{
}//


/**
 *
 */
void MessageFactory::loadContext()
{
  //obtengo todos las connecciones que seran activadas en la ejecucion actual
  string activeConnections_string 
	= Context::instance().getProperty_r("acceptors")
	+ ","
	+ Context::instance().getProperty_r("receptors")
	;
  const string delimiters(",");
  vector<string> activeConnections;
  string::size_type f_begin, f_end;
  f_begin = activeConnections_string.find_first_not_of(delimiters);
  while( f_begin != string::npos )
	{
	  f_end = activeConnections_string.find_first_of(delimiters,f_begin);
	  if( f_end == string::npos )
		  f_end = activeConnections_string.length();
	  activeConnections.push_back( activeConnections_string.substr(f_begin,f_end-f_begin) );
	  f_begin = activeConnections_string.find_first_not_of(delimiters, f_end);
	}

  activeConnections.push_back(PortManager::webPortNamePrefix);// esto es para la trama generica
  //todos los tipos de mensajes
  vector<string> messageTypes;
  messageTypes.push_back(Message::H0200);
  messageTypes.push_back(Message::H0210);
  messageTypes.push_back(Message::H0420);
  messageTypes.push_back(Message::H0421);
  messageTypes.push_back(Message::H0430);
  messageTypes.push_back(Message::H0800);
  messageTypes.push_back(Message::H0810);

  //ahora creo un mapeo de nombre_de_conexion_mas_tipo_de_mensaje -> trama
  for(
	  vector<string>::iterator f_iterator = activeConnections.begin();
	  f_iterator != activeConnections.end();
	  f_iterator++
	  )
	{
	  string connectionName = *f_iterator;
	  for(
		  vector<string>::iterator f_iterator2 = messageTypes.begin();
		  f_iterator2 != messageTypes.end();
		  f_iterator2++
		  )
		{
		  string messageType = *f_iterator2;
		  d_tramasBase[ connectionName + messageType ]
			= Context::instance().getProperty_r( 
												connectionName 
												+ "_" 
												+ messageType
												);
		}//for
	}//for
}//

/**
 *
 */
string MessageFactory::getCacheDate() {  
  ost::Datetime datetime; 
  return datetime.strftime("%m%d");
}//

/**
 *
 */
string MessageFactory::getCacheTime() {
  ost::Datetime datetime; 
  return datetime.strftime("%H%M%S");
}//

/**
 *
 */
string& MessageFactory::getCacheAudit()
{
  if( d_isCacheAuditAvailable )
	{
	  d_isCacheAuditAvailable = false;
	  return d_cacheAudit;
	}
  else
	{
	  NotFoundException e;
	  e
		<< "< MSG 005107 >: NotFoundException::\n\n\tNo se encontra cachado un valor para el audit, el actual ya ha sido usado, recarge uno nuevo consultando de nuevo la base de datos.\nthrow in MessageFactory::getCacheAudit.\n";
	  throw e;
	}//if
}//

/**
 * 
 */
Message* MessageFactory::create(
								string a_senderName
								,string a_messageType
								)
{
  string efectiveSenderName = a_senderName;
  if( a_senderName.substr(0,4) == PortManager::webPortNamePrefix )
	efectiveSenderName = PortManager::webPortNamePrefix;
  Message* message_p( new Message() );

  map<string,string>::iterator f_iterator 
	= d_tramasBase.find( efectiveSenderName + a_messageType );

  if( ( f_iterator == d_tramasBase.end() ) || ( f_iterator->second == "" ))
	{
	  NotFoundException e;
	  e
		<< "< MSG 101 >: NotFoundException::\n\n\tNo se encontro asignado valor a la propiedad -"
		<< a_senderName + "_" + a_messageType
		<< "-.\nthrow in MessageFactory::create.\n";
	  throw e;
	}//if

  string& tramaBase = f_iterator->second;

  if( tramaBase.size() > Message::MAX_TRAMA_SIZE )
	{
	  BadArgumentException e;
	  e
		<< "< MSG 103 >: BadArgumentException:\n\n\tEl tamanno de la trama asignada a traves de la propiedad -"
		<< a_senderName + "_" + a_messageType
		<< "- es -"
		<< tramaBase.size()
		<< "-, el cual, excede el tamanno maximo que es -"
		<< Message::MAX_TRAMA_SIZE
		<< "-.\nthrow in MessageFactory::setupMessage.\n";
	  throw e;
	}//if
  string tramaIn;
  tramaIn += Message::toBase256( tramaBase.size() + 1 ) + tramaBase + '\x03';
  message_p->setTramaIn( tramaIn );
  message_p->deserialize();

  message_p->setSenderName( a_senderName );

  ost::Datetime datetime;
  string date = datetime.strftime("%m%d");
  string time = datetime.strftime("%H%M%S");

  message_p->setTransmissionDateTime( date + time );

  return message_p;
}//

#endif
