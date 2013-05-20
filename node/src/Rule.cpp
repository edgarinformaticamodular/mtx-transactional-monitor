#ifndef ECC_RULE_CPP
#define ECC_RULE_CPP

#include "Rule.hpp"
#include <sstream>
#include "Tx.hpp"
#include "MessageFactory.hpp"
#include "QueryManager.hpp"
#include "PortManager.hpp"
#include "Exception.hpp"

using ecc::MessageFactory;
using ecc::QueryManager;
using ecc::Tx;
using ecc::Rule;
using ecc::PortManager;
using ecc::Exception;

Rule::Rule()
{
}

Rule::~Rule()
{
}

bool Rule::isMatch(Tx& a_tx)
{
  return (a_tx.getState() == d_txState) && (a_tx.getEvent() == d_event);
}

/**
 *
 */
void Rule::wt0420(Tx& a_tx)
{
  Message* message_p 
	= a_tx.messageFactory().create( 
								   a_tx.getCurrentIn()->getReceptor()
								   ,Message::H0420 
								   );

  wt042x(a_tx, message_p);
  a_tx.logAction("wt0420", message_p);
}//

/**
 *
 */
void Rule::wt0421(Tx& a_tx)
{
  Message* message_p 
	= a_tx.messageFactory().create( 
								   a_tx.getCurrentIn()->getReceptor()
								   ,Message::H0421 
								   );
  //..falta mas detalles de copia en especial de quien origino la copia
  wt042x(a_tx, message_p);
  a_tx.logAction("wt0421", message_p);
}//


/**
 *
 */
void Rule::wt042x(Tx& a_tx, Message*& a_message_p)
{
  Message* currentIn = a_tx.getCurrentIn();
  a_message_p->setReceiverName( currentIn->getReceiverName() );
  a_message_p->setTxId( currentIn->getTxId() );
  a_message_p->setSequenceId( currentIn->getSequenceId() );
  a_message_p->setAmount( currentIn->getAmount() );
  a_message_p->setTelephone( currentIn->getTelephone() );
  a_message_p->setResponseCode( currentIn->getResponseCode() );
  a_message_p->setAuthorisationCode( currentIn->getAuthorisationCode() );
  //todo:meter los original data elements
  string originalDataElements0
	= a_tx.messageFactory().getCacheOriginalDataElements();
  string date 
	= a_tx.messageFactory().getCacheDate();
  string time 
	= a_tx.messageFactory().getCacheTime();
  string originalDataElements 
	= Message::normalize(42,'0',originalDataElements0);
  a_message_p->setOriginalDataElements(originalDataElements);
  a_message_p->setSystemsTraceAuditNumber( a_tx.messageFactory().getCacheAudit() );
  a_message_p->setTransmissionDateTime( date + time );
  a_message_p->setLocalTransactionTime( time );
  a_message_p->setLocalTransactionDate( date );
  a_message_p->setSettlementDate( date );
  a_message_p->setCaptureDate( date );
  a_message_p->setAcceptor( currentIn->getAcceptor() );
  a_message_p->setReceptor( currentIn->getReceptor() );
  a_message_p->serialize();
  a_tx.addOut(a_message_p);
  PortManager::instance().send( 
							   a_message_p->getSenderName()
							   ,a_message_p->getTramaOut() 
							   );
}//

/**
 *
 */
void Rule::wt0200(Tx& a_tx)
{//si es un usuario valido, con password valido y con credito
  Message* currentIn_p = a_tx.getCurrentIn();
  string senderName = currentIn_p->getReceptor();
  Message* message_p 
	= a_tx.messageFactory().create( 
								   senderName
								   ,Message::H0200
								   );
  message_p->setReceiverName( currentIn_p->getReceiverName() );
  message_p->setTxId( currentIn_p->getTxId() );
  message_p->setSequenceId( currentIn_p->getSequenceId() );
  message_p->setAmount( currentIn_p->getAmount() );
  message_p->setTelephone( currentIn_p->getTelephone() );
  string date 
	= a_tx.messageFactory().getCacheDate();
  string time 
	= a_tx.messageFactory().getCacheTime();
  string audit
	= a_tx.messageFactory().getCacheAudit();
  message_p->setSystemsTraceAuditNumber( audit );
  message_p->setTransmissionDateTime( date + time );
  message_p->setLocalTransactionTime( time );
  message_p->setLocalTransactionDate( date );
  message_p->setSettlementDate( date );
  message_p->setCaptureDate( date );
  message_p->setAcceptor( currentIn_p->getAcceptor() );
  message_p->setReceptor( currentIn_p->getReceptor() );
  message_p->serialize();
  a_tx.addOut(message_p);
  message_p->setSenderName( senderName );
  PortManager::instance().send( 
							   message_p->getSenderName()
							   ,message_p->getTramaOut() 
							   );
  a_tx.logAction("wt0200",message_p);
}//

/**
 *
 */
void Rule::wc0210r(Tx& a_tx, string a_responseCode)
{
  Message* currentIn_p = a_tx.getCurrentIn();
  Message* firstIn_p = a_tx.getFirstIn();
  string senderName = firstIn_p->getReceiverName();
  Message* message_p 
	= a_tx.messageFactory().create( 
								   senderName
								   ,Message::H0210
								   );
  message_p->setTxId( currentIn_p->getTxId() );
  message_p->setSequenceId( currentIn_p->getSequenceId() );
  message_p->setAmount( currentIn_p->getAmount() );
  message_p->setTelephone( currentIn_p->getTelephone() );
  message_p->setResponseCode(a_responseCode);
  string date 
	= a_tx.messageFactory().getCacheDate();
  string time 
	= a_tx.messageFactory().getCacheTime();
  string audit
	= a_tx.messageFactory().getCacheAudit();
  message_p->setSystemsTraceAuditNumber( audit );
  /* todo: checar de acuerdo al iso si no va tambien  todo esto
  message_p->setTransmissionDateTime( date + time );
  message_p->setLocalTransactionTime( time );
  message_p->setLocalTransactionDate( date );
  message_p->setSettlementDate( date );
  message_p->setCaptureDate( date );
  */
  message_p->serialize();
  a_tx.addOut(message_p);

  PortManager::instance().send( 
							   message_p->getSenderName()
							   ,message_p->getTramaOut() 
							   );

  a_tx.logAction("wc0210r", message_p);
}//

/**
 *
 */
void Rule::wc0210a(Tx& a_tx)
{
  Message* currentIn_p = a_tx.getCurrentIn();
  Message* firstIn_p = a_tx.getFirstIn();
  string senderName = firstIn_p->getReceiverName();
  Message* message_p 
	= a_tx.messageFactory().create( senderName, Message::H0210 );

  message_p->setTxId( currentIn_p->getTxId() );
  message_p->setSequenceId( currentIn_p->getSequenceId() );
  message_p->setAmount( currentIn_p->getAmount() );
  message_p->setTelephone( currentIn_p->getTelephone() );
  message_p->setResponseCode( Message::RESPONSE_CODE_APPROVED );
  message_p->setAuthorisationCode( currentIn_p->getAuthorisationCode() );

  message_p->serialize();
  a_tx.addOut(message_p);

  PortManager::instance().send( 
							   message_p->getSenderName()
							   ,message_p->getTramaOut() 
							   );

  a_tx.logAction("wc0210a", message_p);
}//

/**
 *
 */
void Rule::wt0430(Tx& a_tx)
{
  Message* currentIn_p = a_tx.getCurrentIn();
  string senderName = currentIn_p->getReceiverName();
  Message* message_p 
	= a_tx.messageFactory().create( senderName, Message::H0430 );

  message_p->setTxId( currentIn_p->getTxId() );
  message_p->setSequenceId( currentIn_p->getSequenceId() );
  message_p->setAmount( currentIn_p->getAmount() );
  message_p->setTelephone( currentIn_p->getTelephone() );


  string date 
	= a_tx.messageFactory().getCacheDate();
  string time 
	= a_tx.messageFactory().getCacheTime();
  string audit
	= a_tx.messageFactory().getCacheAudit();
  message_p->setSystemsTraceAuditNumber( audit );
  message_p->setTransmissionDateTime( date + time );

  message_p->serialize();
  a_tx.addOut(message_p);

  PortManager::instance().send( 
							   message_p->getSenderName()
							   ,message_p->getTramaOut() 
							   );
  a_tx.logAction("wt0430", message_p);
}//

/**
 *
 */
void Rule::reject(Tx& a_tx)
{
  Message* message_p = a_tx.getCurrentIn();
  message_p
	->serializeReject( Message::STATUS_FIELD_SECURITY_MESSAGE_UNEXPECTED );
  //ya no checo si sale bien o mal, puesto que solo es un reject, si
  //me mandaran algo importante lo van a volver a intentar con un time out
  //  string sender 
  //	= Evolver::instance().sender( message_p );
  message_p->setSenderName(message_p->getReceiverName());
  PortManager::instance().send( 
							   message_p->getSenderName()
							   ,message_p->getTramaOut() 
							   );
  a_tx.logAction("reject", message_p, Tx::TRAMA_OUT);
}//

/**
 *
 */
void Rule::cach(
				MessageFactory& a_messageFactory
				,QueryManager& a_queryManager
				)
{
  a_messageFactory
	.setCacheAudit(
				   a_queryManager
				   .createAudit()
				   );
  a_messageFactory
	.setCacheTime(
				  a_queryManager
				  .createTime()
				  );
  a_messageFactory
	.setCacheDate(
				  a_queryManager
				  .createDate()
				  );
}//


/**
 *
 */
void Rule::setupEvent(Tx& a_tx)
{
  Message* currentIn_sp = a_tx.getCurrentIn();
  string type = currentIn_sp->getType();
  a_tx.setEvent( Tx::UNDEFINED );//por si no se ejecuta ninguno  

  if(
	 PortManager::isReceptor( 
							 currentIn_sp->getReceiverName() 
							 )
	 )
	{
	  if( type == Message::H0210 )
		{
		  if( 
			 currentIn_sp->getResponseCode() 
			 == Message::RESPONSE_CODE_APPROVED 
			 )
			{
			  a_tx.setEvent( Tx::RT0210A );
			  return;
			}
		  else
			{
			  a_tx.setEvent( Tx::RT0210R );
			  return;
			}
		}
	  else if( type == Message::H0420 )
		{
		  a_tx.setEvent( Tx::RT0420 );
		  return;
		}
	  else if( type == Message::H0421 )
		{
		  a_tx.setEvent( Tx::RT0421 );
		  return;
		}
	  else if( type == Message::H0430 )
		{
		  a_tx.setEvent( Tx::RT0430 );
		  return;
		}
	  else if( type == Message::H9200 )
		{
		  a_tx.setEvent( Tx::RT9200 );
		  return;
		}
	}
  else //entonces es acceptor
	{
	  if( type == Message::H0200 )
		{
		  a_tx.setEvent( Tx::RC0200 );
		  return;
		}
	  else if( type == Message::H0420 )
		{
		  a_tx.setEvent( Tx::RC0420 );
		  return;
		}
	}
  //si continua es que no es un tipo de mensaje conocido o inesperado
  {
	ecc::Exception e;
	e
	  << "< MSG 005186 >: ecc::Exception:\n\n\tEl mensaje:\n"
	  << currentIn_sp->getTramaPartition()
	  << endl
	  << "no se esperaba de la conexion:-"
	  << currentIn_sp->getReceiverName()
	  << "-. Porque no corresponde el receptor\n"
	  << "throw in Tx::setupEvent.\n"
	  ;
	throw e;
  }
}//setupEvent

#endif
