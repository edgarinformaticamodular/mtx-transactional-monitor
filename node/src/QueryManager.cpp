#ifndef ECC_QUERY_MANAGER_CPP
#define ECC_QUERY_MANAGER_CPP
//todo: revisar que el BEGIN de los store procedures con el begin usado aqui
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <libpq-fe.h>
#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <vector>
#include <exception>

using std::vector;
using std::string;
using std::stringstream;
using std::exception;

#include "QueryManager.hpp"
#include "DbPool.hpp"
#include "Exception.hpp"
#include "ToSqlSafe.hpp"
#include "Message.hpp"
#include "Log.hpp"
#include "NotFoundException.hpp"
#include "BadArgumentException.hpp"
#include "SyncMap.hpp"
#include "PortManager.hpp"
#include "MessageFactory.hpp"

using ::ecc::QueryManager;
using ::db::DbPool;
using ::ecc::Exception;
using ::ecc::ToSqlSafe;
using ::ecc::Message;
using ::ecc::Log;
using ::ecc::NotFoundException;
using ::ecc::BadArgumentException;
using ::ecc::SyncMap;
using ::ecc::PortManager;
using ::ecc::MessageFactory;

#define x(a) cout << a << endl;

int QueryManager::MAX_THREADS = 100;

/**
 *
 */
QueryManager::QueryManager()
  :d_isCommit(false)
{
  d_connection_sp = db::DbPool::instance().erase();
  if (::PQstatus(d_connection_sp) == ::CONNECTION_BAD)
	Log::instance().log("< MSG 001538 >: Exception: QueryManager::QueryManager:\n\n\tProblemas para establecer conexion con la base de datos.");
}//

/**
 *
 */
QueryManager::~QueryManager()
{
  try
	{
	  if( d_isCommit )
		commit();
	  else
		rollback();
	  db::DbPool::instance().insert(d_connection_sp);
	}catch(Exception& e){
	  e
		<< "\ncatch in QueryManager::~QueryManager"
		;
	  Log::instance().log( e.getMessage().c_str() );
	}catch(std::exception& e){
	  stringstream sstream;
	  sstream
		<< e.what()
		<< "\ncatch in QueryManager::~QueryManager"
		;
	  Log::instance().log( sstream.str().c_str() );		
	}catch(...){
	  Log::instance().log( "< MSG 005356 >: ???exception:\ncatch in QueryManager::~QueryManager" );		
	}
}//

/**
 * todo: comparar su uso con PQescapeString que es la original postgresql
 */
void QueryManager::sqlSafe(string& a_string)
{
  ToSqlSafe toSqlSafe;		  
  std::transform( 
				 a_string.begin()
				 ,a_string.end()
				 ,a_string.begin()
				 ,toSqlSafe 
				 );
}//

/**
 *
 */
void QueryManager::begin()
{
  ::PGresult*   res_p;		  
  //PQclear(res_p); no se hace puesto que no existe todavia
  ////////BEGIN
  res_p = PQexec(d_connection_sp, "BEGIN");
  if (!res_p || PQresultStatus(res_p) != PGRES_COMMAND_OK)
	{
	  Exception e;
	  e.getStream() 
		<< "< MSG 025 >: ecc::Exception BEGIN command failed."
		<< endl
		<< "SQLServer report: "
		<< PQerrorMessage(d_connection_sp)
		<< "throw in QueryManager::begin."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}
  PQclear(res_p);
}//

/**
 *
 */
void QueryManager::commit()
{
  ::PGresult*   res_p;		  
  res_p = PQexec(d_connection_sp, "COMMIT");
  if (!res_p || PQresultStatus(res_p) != PGRES_COMMAND_OK)
	{
	  Exception e;
	  e.getStream() 
		<< "< MSG 491 >: ecc::Exception BEGIN command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in begin."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}
  PQclear(res_p);
}//

/** todo: ccorregir quue no se lancen excepciones dentro de destructores !!!!!!!!!!! tataaaan
 *
 */
void QueryManager::rollback()
{
  ::PGresult*   res_p;		  
  res_p = PQexec(d_connection_sp, "ROLLBACK");
  if (!res_p || PQresultStatus(res_p) != PGRES_COMMAND_OK)
	{
	  Exception e;
	  e.getStream() 
		<< "< MSG 491 >: ecc::Exception BEGIN command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in begin."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}
  PQclear(res_p);
}//

/**
 *
 */
string QueryManager::toResponseCode(AcceptState a_acceptState)
{
  switch(a_acceptState)
	{
	case INVALID_LOGIN:
	  return Message::RESPONSE_CODE_INVALID_LOGIN;
	case INVALID_PASSWORD:
	  return Message::RESPONSE_CODE_INVALID_PASSWORD;
	case NO_CREDIT:
	  return Message::RESPONSE_CODE_NO_CREDIT;
	case ACCEPTED:
	  return Message::RESPONSE_CODE_APPROVED;
	case NO_ACCEPTED:
	  return Message::RESPONSE_CODE_LOCAL_DENIED;
	default:
	  return "RESPONSE CODE UNDEFINDED";
	}//switch
}//


/** todo: esta rutina no la hice con mutex, aunque deviera puesto que altera
 *
 */
QueryManager::AcceptState QueryManager::getAcceptState(
													   string a_login
													   ,string a_password
													   ,string a_amount
													   ,string a_id
													   )
{
  AcceptState acceptState = NO_ACCEPTED;
  ::PGresult*   res_p;		  

  float amount = 0;
  sqlSafe(a_login);
  sqlSafe(a_password);
  sqlSafe(a_amount);
  sqlSafe(a_id);
  if( ! (stringstream(a_amount) >> amount) )
	{
	  ecc::Exception e;
	  e.getStream() <<"< MSG 024 >: ecc::Exception:"
					<<endl
					<<"\tLa Cantidad proporcionada: -"
					<<a_amount
					<< "-. No puede transformarse a un numero entero valido."
					<<endl
					<<"throw in ecc::db::getAcceptState."
					<<endl;
	  PQclear(res_p);
	  throw e;
	}//if
  /*
   * should PQclear PGresult whenever it is no longer needed to avoid
   * memory leaks
   */
  /////////getAcceptState
	  
  stringstream query;
  query
	<< "SELECT \"sp_get_accept_state\"("
	<< "'" << a_login << "'"
	<< ",'" << a_password << "'"
	<< ", cast(" << a_amount << "/100 as numeric(8,2))" 
	<< ",'" << a_id << "'"
	<< ");";
	  
  res_p = PQexec( d_connection_sp, query.str().c_str() );
  if (!res_p || PQresultStatus(res_p) != PGRES_TUPLES_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 026 >: ecc::Exception -"<< query.str() <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in getAcceptState."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}//if
  char result_char = *PQgetvalue(res_p,0,0);
  PQclear(res_p);
	  
  int result_int = 4;//note inicializo
  switch( result_char )
	{
	case '0':
	  result_int = 0;
	  break;
	case '1':
	  result_int = 1;
	  break;
	case '2':
	  result_int = 2;
	  break;
	case '3':
	  result_int = 3;
	  break;
	default:
	  result_int = 4;
	  break;
	}//switch
  acceptState = static_cast<AcceptState>(result_int);
  //checando errores
  return acceptState;
}//


/**
 *
 */
void QueryManager::updateTx(
							string a_txId
							,int a_state
							,int a_event
							,string a_responseCode
							,string a_authorisationCode
							)
{
  PGresult*   res_p;		  

  sqlSafe(a_txId);
  sqlSafe(a_responseCode);
  sqlSafe(a_authorisationCode);
  stringstream query;
  query
	<< "SELECT \"sp_update_tx\"("
	<< "'" << a_txId << "'"
	<< "," << a_state 
	<< "," << a_event 
	<< ",'" << a_responseCode << "'"
	<< ",'" << a_authorisationCode << "'"
	<< ");";
  res_p = PQexec( d_connection_sp, query.str().c_str() );
  if (!res_p || PQresultStatus(res_p) != PGRES_TUPLES_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 035 >: ecc::Exception -"<< query.str() <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in updateTx."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}//if
  PQclear(res_p);
}//updateTx

/**
 *
 */
void QueryManager::updateTx(
							string a_txId
							,int a_state
							,int a_evolve
							)
{
  PGresult*   res_p;		  

  sqlSafe(a_txId);
  stringstream query;
  query
	<< "SELECT \"sp_update_tx\"("
	<< "'" << a_txId << "'"
	<< "," << a_state 
	<< "," << a_evolve
	<< ");";
  res_p = PQexec( d_connection_sp, query.str().c_str() );
  if (!res_p || PQresultStatus(res_p) != PGRES_TUPLES_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 037 >: ecc::Exception -"<< query.str() <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in updateTx."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}//if
  PQclear(res_p);
}//updateTx2

/**
 *
 */
void QueryManager::plusCredit(
							  string a_login
							  ,string a_amount
							  )
{
  AcceptState acceptState = NO_ACCEPTED;
  PGresult*   res_p;		  
  float amount = 0;
  sqlSafe(a_login);
  sqlSafe(a_amount);
  if( ! (stringstream(a_amount) >> amount) )//todo: mejorar la validacioon para digitos punto y espacio para evitar qque manden palabras juntocon numeros
	{
	  ecc::Exception e;
	  e.getStream() <<"< MSG 038 >: ecc::Exception:"
					<<endl
					<<"\tLa Cantidad proporcionada: -"
					<<a_amount
					<< "-. No puede transformarse a un numero entero valido."
					<<endl
					<<"throw in ecc::db::plusCredit."
					<<endl;
	  throw e;
	}//if

  stringstream query;
  query
	<< "SELECT \"sp_plus_credit\"("
	<< "'" << a_login << "'"
	<< ",cast(" << a_amount << "/100 as numeric(8,2) )"
	<< ");";

  res_p = PQexec( d_connection_sp, query.str().c_str() );
  if (!res_p || PQresultStatus(res_p) != PGRES_TUPLES_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 040 >: ecc::Exception -"<< query.str() <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in plusCredit."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}//if

  PQclear(res_p);
  /* commit the transaction */
  //res_p = PQexec(d_connection_sp, "COMMIT");
  //PQclear(res_p);	  
}//


/** 
 * 
 */
void QueryManager::addTx(
						 string a_txId
						 ,string a_login
						 ,string a_amount
						 ,string a_telephone
						 ,string a_audit
						 ,string a_acceptor
						 ,string a_receptor
						 ,string a_transmissionDateTime
						 )
{
  PGresult*   res_p;		  
  sqlSafe(a_txId);
  sqlSafe(a_login);
  sqlSafe(a_amount);
  sqlSafe(a_telephone);
  sqlSafe(a_acceptor);
  sqlSafe(a_receptor);
  sqlSafe(a_transmissionDateTime);
  
  stringstream query;
  query
	<< "SELECT \"sp_add_tx\"("
	<< "'" << a_txId << "'"
	<< ",'" << a_login << "'"
	<< ", cast(" << a_amount << "/100 as numeric(8,2))" 
	<< ",'" << a_telephone << "'"
	<< ",'" << a_audit << "'"
	<< ",'" << a_acceptor << "'"
	<< ",'" << a_receptor << "'"
	<< ",'" << a_transmissionDateTime << "'"
	<< ");";

  float amount;
  if( ! (stringstream(a_amount) >> amount) )
	{
	  BadArgumentException e;
	  e
		<<"< MSG 005147 >:  ecc::BadArgumentException:"
					<<endl
					<<"\tLa Cantidad proporcionada: -"
					<<a_amount
					<< "-. No puede transformarse a un numero entero valido."
					<<endl
					<<"throw in ecc::QueryManager::addTx."
					<<endl;
	  PQclear(res_p);
	  throw e;
	}//if

  res_p = PQexec( d_connection_sp, query.str().c_str() );
  if (!res_p || PQresultStatus(res_p) != PGRES_TUPLES_OK)
	{
	  ecc::Exception e;
	  e 
		<< "< MSG 005148 >: ecc::Exception -"<< query.str() <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in ecc::QueryManager::addTx."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}//if
  PQclear(res_p);
}//

/** 
 * este es para comit con reverso
 */
QueryManager::AcceptState QueryManager::delayReverseTx(
								  string a_txId
								  ,int a_state
								  ,int a_evolve
								  ,string a_authorisationCode
								  ,string a_login
								  ,string a_password
								  ,string a_telephone
								  )
{
  PGresult*   res_p;		  
  sqlSafe(a_txId);
  sqlSafe(a_authorisationCode);
  sqlSafe(a_login);
  sqlSafe(a_password);
  sqlSafe(a_telephone);

  stringstream query;
  query
	<< "SELECT \"sp_delay_reverse_tx\"("
	<< "'" << a_txId << "'"
	<< "," << a_state
	<< "," << a_evolve
	<< ",'" << a_authorisationCode << "'"
	<< ",'" << a_login << "'"
	<< ",'" << a_password << "'"
	<< ",'" << a_telephone << "'"
	<< ");";
  //  cout << query.str() << endl;
  res_p = PQexec( d_connection_sp, query.str().c_str() );
  if (!res_p || PQresultStatus(res_p) != PGRES_TUPLES_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 044 >: ecc::Exception -"<< query.str() <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in delayReverseTx."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}//if
  //PQclear(res_p);//esta la cambie de lugar
  char result_char = *PQgetvalue(res_p,0,0);
  PQclear(res_p);
	  
  int result_int = 4;//note inicializo
  switch( result_char )
	{
	case '0':
	  result_int = 0;
	  break;
	case '1':
	  result_int = 1;
	  break;
	case '2':
	  result_int = 2;
	  break;
	case '3':
	  result_int = 3;
	  break;
	default:
	  result_int = 4;
	  break;
	}//switch
  AcceptState acceptState = static_cast<AcceptState>(result_int);
  //checando errores
  return acceptState;
}//

/** 
 * este es para comit con reverso
 */
QueryManager::AcceptState QueryManager::delayReverseTx(
								  string a_txId
								  ,int a_state
								  ,int a_evolve
								  ,string a_authorisationCode
								  ,string a_telephone
								  )
{
  PGresult*   res_p;		  
  sqlSafe(a_txId);
  sqlSafe(a_authorisationCode);
  sqlSafe(a_telephone);

  stringstream query;
  query
	<< "SELECT \"sp_delay_reverse_tx\"("
	<< "'" << a_txId << "'"
	<< "," << a_state
	<< "," << a_evolve
	<< ",'" << a_authorisationCode << "'"
	<< ",'" << a_telephone << "'"
	<< ");";
  //cout << query.str() << endl;
  res_p = PQexec( d_connection_sp, query.str().c_str() );
  if (!res_p || PQresultStatus(res_p) != PGRES_TUPLES_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 523 >: ecc::Exception -"<< query.str() <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in delayReverseTx."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}//if
  char result_char = *PQgetvalue(res_p,0,0);
  PQclear(res_p);
	  
  int result_int = 4;//note inicializo
  switch( result_char )
	{
	case '0':
	  result_int = 0;
	  break;
	case '1':
	  result_int = 1;
	  break;
	case '2':
	  result_int = 2;
	  break;
	case '3':
	  result_int = 3;
	  break;
	default:
	  result_int = 4;
	  break;
	}//switch
  AcceptState acceptState = static_cast<AcceptState>(result_int);
  //checando errores
  return acceptState;
}//

/**
 *
 */
void QueryManager::loadMessages( vector<Message*>& a_vector)
{
  PGresult*   res_p;		  
  string query;
  query =
	"DECLARE mycursor CURSOR FOR SELECT \
                   tx_id\
                   ,action\
                   ,login\
                   ,password\
                   ,cast(amount*100 as int8) as amount_int\
                   ,telephone\
                   ,audit\
                   ,authorisation_code\
                   ,acceptor\
                   ,receptor\
                   ,transmission_date_time\
       FROM \"t_queue\"";
  res_p = PQexec( d_connection_sp, query.c_str() );
  if (!res_p || PQresultStatus(res_p) != PGRES_COMMAND_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 046 >: ecc::Exception -"<< query <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in loadMessages."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}//if
  ///////////////////////////////
  PQclear(res_p);

  query = "FETCH ALL in mycursor";
  res_p = PQexec( d_connection_sp, query.c_str() ); 
  if (!res_p || PQresultStatus(res_p) != PGRES_TUPLES_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 047 >: ecc::Exception -"<< query <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in loadMessages."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}

  /////////////////////////////
  int tuplesN = PQntuples(res_p);
  string txId;
  string action;
  string login;
  string password;
  string amountInt;
  string telephone;
  string audit;
  string authorisationCode;
  string acceptor;
  string receptor;
  string transmissionDateTime;
	  
  vector<string> txIds;
  vector<string> overruns;
  //	  cout << "Numero de tuplas encontradas : "<<tuplesN<<endl;
  //se rellena
  for(int tupleId = 0; tupleId < tuplesN; tupleId++)
	{
	  txId.assign( 
				  PQgetvalue(res_p,tupleId,0), 
				  PQgetlength(res_p,tupleId,0) 
				  );
	  action.assign( 
					PQgetvalue(res_p,tupleId,1), 
					PQgetlength(res_p,tupleId,1) 
					);
	  login.assign( 
				   PQgetvalue(res_p,tupleId,2), 
				   PQgetlength(res_p,tupleId,2) 
				   );
	  password.assign( 
					  PQgetvalue(res_p,tupleId,3), 
					  PQgetlength(res_p,tupleId,3) 
					  );
	  amountInt.assign( 
					PQgetvalue(res_p,tupleId,4), 
					PQgetlength(res_p,tupleId,4) 
					);
	  telephone.assign( 
					   PQgetvalue(res_p,tupleId,5), 
					   PQgetlength(res_p,tupleId,5) 
					   );
	  audit.assign( 
					   PQgetvalue(res_p,tupleId,6), 
					   PQgetlength(res_p,tupleId,6) 
					   );
	  authorisationCode.assign( 
					   PQgetvalue(res_p,tupleId,7), 
					   PQgetlength(res_p,tupleId,7) 
					   );
	  acceptor.assign( 
					   PQgetvalue(res_p,tupleId,8), 
					   PQgetlength(res_p,tupleId,8) 
					   );
	  receptor.assign( 
					   PQgetvalue(res_p,tupleId,9), 
					   PQgetlength(res_p,tupleId,9) 
					   );
	  transmissionDateTime.assign( 
					   PQgetvalue(res_p,tupleId,10), 
					   PQgetlength(res_p,tupleId,10) 
					   );
	  //lo guardo para despues borrarlo
	  txIds.push_back(txId);

	  //checo si no esta saturado el servidor
	  int smsize = SyncMap::instance().size();
	  if( smsize+tupleId+1 > MAX_THREADS )//es mas uno porque el tupleId empieza en 0
		{
		  stringstream sstream;
		  sstream 
			<< "< MSG 005149 >: WARNING! ecc::QueryManager::loadMessages: WARNING!\\n\n\tOVERRUN, The max threads has been reached:-"
			<< MAX_THREADS
			<< "-, and there are :-" 
			<< smsize+tupleId+1 
			<< "- pending messages for input!. The current petition will be ignored and -anti overrun- response code mark. Este problema es debido a que se esta solicitando muchas transacciones al mismo tiempo, lo cual puede ser por una gran cantidad de ventas o por un ataque de seguridad de alguien ageno que esta buscando atinarle a un password o simplemente intentar hacernos llegar a un punto extremo enviando un gran numero de transacciones."
			<< endl;
		  Log::instance().log( sstream.str().c_str() );
		  overruns.push_back(txId);		  
		  continue;
		}
	  int action_int;
	  if( !(stringstream(action) >> action_int) )
		{
		  BadArgumentException e;
		  e
			<< "< MSG 307 >: BadArgumentException:\n\n\t"
			<< "No se pudo transformar a entero el id de action: -"
			<< action
			<< "-. \nthrow in QueryManager::loadMessages.";
		  throw e;
		}

	  //este try es porsi faalla algo een laa deserialiizacion del mensaaje se aborte
	  Message* message_p;
	  try
		{
		  switch( action_int )
			{
			case ecc::pp::queue::action::ABORT:
			  {
				message_p = MessageFactory::create(
												   PortManager::webPortNamePrefix
												   ,Message::H0420
												   );
				message_p->setAuthorisationCode(authorisationCode);
			  }
			break;
			default:// ecc::pp::queue::action::BEGIN:
			  {
				message_p = MessageFactory::create(
												   PortManager::webPortNamePrefix
												   ,Message::H0200
												   );
			  }
			break;
			}
		  //			break;
		  //	default:
		  {
			stringstream sstream;
			sstream
			  //<< "\tLa accion solicitada, action : -"
			  //<< action_int
			  //<< "-,\nno corresponde a ninguna accion valida.\n"
			  << "ecc::QueryManager::loadMessages:\n"
			  << "\tTx: -" << txId << "-,"<< endl
			  << "\t\taction:-" << action << "-" << endl
			  << "\t\tlogin:-" << login << "-"<< endl
			  << "\t\tpassword:-" << password << "-"<< endl
			  << "\t\tamountInt:-" << amountInt << "-"<< endl
			  << "\t\ttelephone:-" << telephone << "-"<< endl
			  << "\t\taudit:-" << audit << "-"<< endl
			  << "\t\tauthorisation_code:-" << authorisationCode << "-" << endl
			  << "\t\tacceptor:-" << acceptor << "-"<< endl
			  << "\t\treceptor:-" << receptor << "-" << endl
			  << "\t\ttransmissionDateTime:" << "-"<< transmissionDateTime << "-" << endl;
			Log::instance().log(sstream.str().c_str() );
			//	continue;
		  }
		  //	}//switch
		  //creo mensajes
		  message_p->setQueueTxId( txId );
		  message_p->setLogin(login);
		  message_p->setPassword(password);
		  
		  message_p->setTxId( txId );
		  message_p->setTelephone(telephone);
		  message_p->setAmount( Message::normalize(12,'0',amountInt) );
		  string a,b,c;
		  b = createDate();
		  c = createTime();
		  message_p->setSystemsTraceAuditNumber( audit );
		  message_p->setLocalTransactionDate( b );
		  message_p->setLocalTransactionTime( c );
		  message_p->setTransmissionDateTime( transmissionDateTime );
		  
		  message_p->setAcceptor(acceptor);
		  message_p->setReceptor(receptor);
		  
		  message_p->serialize();
		  message_p->setTramaIn( message_p->getTramaOut() );//para emular el bug
		  message_p->setReceiverName( acceptor );
		  a_vector.push_back( message_p );  
		}catch(ecc::Exception& e){
		  e
			<< "\n< MSG 005150 >: Warning! ecc::QueryManager::loadMessages\n\n\tse aborta solicitud al intentar llenar el mensaje de "
			<< "-" << message_p->getLogin() << "-, from"
			<< "-" << message_p->getReceiverName() << "-"
			<< "\ncatch in QueryManager::loadMessage.";
		  Log::instance().log( e.getMessage().c_str() );
		  continue;//y se continua ccon las demas peticiones
		}
	}//for
  /////////////////////////////////
  PQclear(res_p);
  query = "CLOSE mycursor";
  res_p = PQexec(d_connection_sp, query.c_str() );
  if (!res_p || PQresultStatus(res_p) != PGRES_COMMAND_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 048 >: ecc::Exception -"<< query <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in loadMessages."
			<< endl;
	  PQclear(res_p);
		  throw e;
	}//if
  PQclear(res_p);

  //////////////////////////////
  vector<string>::iterator itr;
  for(itr = txIds.begin(); itr != txIds.end(); itr++ )
	{
	  query = "DELETE FROM \"t_queue\" WHERE \"tx_id\" = '";
	  query += *itr;
	  query += "'";
	  res_p = PQexec( d_connection_sp, query.c_str() );
	  if (!res_p || PQresultStatus(res_p) != PGRES_COMMAND_OK)
		{
		  ecc::Exception e;
		  e.getStream() 
			<< "< MSG 048 >: ecc::Exception -"<< query <<"- command failed."
			<< endl
			<< "Server report: -"
			<< PQerrorMessage(d_connection_sp)
			<< "-."
			<< "throw in loadMessages."
			<< endl;
		  PQclear(res_p);
		  throw e;
		}//if
	  PQclear(res_p);
	}//for
  ///////////////////////////////////////
  for(itr = overruns.begin(); itr != overruns.end(); itr++ )
	{
	  query = "UPDATE t_txs SET response_code = '15', state = 2, evolve = 1 WHERE id = '";
	  query += *itr;
	  query += "'";
	  res_p = PQexec( d_connection_sp, query.c_str() );
	  if (!res_p || PQresultStatus(res_p) != PGRES_COMMAND_OK)
		{
		  ecc::Exception e;
		  e.getStream() 
			<< "< MSG 488 >: ecc::Exception -"<< query <<"- command failed."
			<< endl
			<< "Server report: -"
			<< PQerrorMessage(d_connection_sp)
			<< "-."
			<< "throw in loadMessages."
			<< endl;
		  PQclear(res_p);
		  throw e;
		}//if
	  PQclear(res_p);
	}//for
  ///////////////////////////////////////
}//

/**
 *
 */
void QueryManager::abortUnfinishedTx(void)
{
  PGresult*   res_p;		  
  stringstream query;
  query
	<< "SELECT \"sp_abort_unfinished_tx\"();";
  res_p = PQexec( d_connection_sp, query.str().c_str() );
  if (!res_p || PQresultStatus(res_p) != PGRES_TUPLES_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 050 >: ecc::Exception -"<< query.str() <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in abortUnfinishedTx."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}//if
  PQclear(res_p);
}//abortUnstartedTx

/**
 *
 */
const char* QueryManager::acceptStateToString(
											  QueryManager::AcceptState a_acceptState)
{
  switch(a_acceptState)
	{
	case INVALID_LOGIN:
	  return "INVALID_LOGIN";
	case INVALID_PASSWORD:
	  return "INVALID_PASSWORD";
	case NO_CREDIT:
	  return "NO_CREDIT";
	case ACCEPTED:
	  return "ACCEPTED";
	case NO_ACCEPTED:
	  return "NO_ACCEPTED";
	default:
	  return "AcceptState???";
	}
}//acceptStateToString


/**
 *
 */
void QueryManager::loadForReverse( 
								  string a_receptor
								  ,vector<Message*>& a_vector
								  )
{
  PGresult*   res_p;		  
  string query;


  query =
	"DECLARE mycursor CURSOR FOR SELECT ";
  query +="      t_txs.id as id";
  query +="      ,t_txs.state as state";
  query +="      ,t_txs.evolve as evolve";
  query +="      ,t_txs.response_code as response_code";
  query +="      ,t_txs.authorisation_code as authorisation_code";
  query +="      ,t_txs.login as login";
  query +="      ,cast(t_txs.amount*100 as int8) as amount_int ";
  query +="      ,t_txs.telephone as telephone";
  query +="      ,t_txs.audit as audit";
  query +="      ,t_txs.date as date";
  query +="      ,t_txs.time as time";
  query +="      ,t_txs.acceptor as acceptor";
  query +="      ,t_txs.receptor as receptor";
  query +="      ,t_txs.transmission_date_time as transmission_date_time";
  query +="      ,c_logins.password as password";
  query +=" FROM t_txs, c_logins WHERE t_txs.state = 3  AND t_txs.evolve = 0 AND t_txs.created > current_date AND t_txs.receptor = '";
  query +=a_receptor;
  query +="' AND c_logins.login = t_txs.login";
  res_p = PQexec( d_connection_sp, query.c_str() );
  if (!res_p || PQresultStatus(res_p) != PGRES_COMMAND_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 052 >: ecc::Exception -"<< query <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in loadForReverse."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}//if
  ///////////////////////////////
  PQclear(res_p);
  query = "FETCH ALL in mycursor";
  res_p = PQexec( d_connection_sp, query.c_str() ); 
  if (!res_p || PQresultStatus(res_p) != PGRES_TUPLES_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 053 >: ecc::Exception -"<< query <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in loadForReverse."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}
  /////////////////////////////
  int tuplesN = PQntuples(res_p);
  string id;
  string state;
  string evolve;
  string responseCode;
  string authorisationCode;
  string login;
  string amountInt;
  string telephone;
  string audit;
  string date;
  string time;
  string acceptor;
  string receptor;
  string transmissionDateTime;
  string password;
  vector<string> txIds;
  //	  cout << "Numero de tuplas encontradas : "<<tuplesN<<endl;
  //se rellena
  for(int tupleId = 0; tupleId < tuplesN; tupleId++)
	{
	  id.assign( 
				PQgetvalue(res_p,tupleId,0), 
				PQgetlength(res_p,tupleId,0) 
				);
	  state.assign( 
				   PQgetvalue(res_p,tupleId,1), 
				   PQgetlength(res_p,tupleId,1) 
				   );
	  evolve.assign( 
				   PQgetvalue(res_p,tupleId,2), 
				   PQgetlength(res_p,tupleId,2) 
				   );
	  responseCode.assign( 
						  PQgetvalue(res_p,tupleId,3), 
						  PQgetlength(res_p,tupleId,3) 
						  );
	  authorisationCode.assign( 
							   PQgetvalue(res_p,tupleId,4), 
							   PQgetlength(res_p,tupleId,4) 
							   );
	  login.assign( 
				  PQgetvalue(res_p,tupleId,5), 
				  PQgetlength(res_p,tupleId,5) 
				  );
	  amountInt.assign( 
					PQgetvalue(res_p,tupleId,6), 
					PQgetlength(res_p,tupleId,6) 
					);
	  telephone.assign( 
					   PQgetvalue(res_p,tupleId,7), 
					   PQgetlength(res_p,tupleId,7) 
					   );
	  audit.assign( 
				   PQgetvalue(res_p,tupleId,8), 
				   PQgetlength(res_p,tupleId,8) 
				   );
	  date.assign( 
				   PQgetvalue(res_p,tupleId,9), 
				   PQgetlength(res_p,tupleId,9) 
				   );
	  time.assign( 
				   PQgetvalue(res_p,tupleId,10), 
				   PQgetlength(res_p,tupleId,10) 
				   );
	  acceptor.assign( 
				   PQgetvalue(res_p,tupleId,11), 
				   PQgetlength(res_p,tupleId,11) 
				   );
	  receptor.assign( 
				   PQgetvalue(res_p,tupleId,12), 
				   PQgetlength(res_p,tupleId,12) 
				   );
	  transmissionDateTime.assign( 
				   PQgetvalue(res_p,tupleId,13), 
				   PQgetlength(res_p,tupleId,13) 
				   );

	  password.assign( 
				   PQgetvalue(res_p,tupleId,14), 
				   PQgetlength(res_p,tupleId,14) 
				   );

	  //lo guardo para despues borrarlo
	  txIds.push_back(id);
	  //creo mensajes
	  {
		stringstream sstream;
		sstream
		  << "< MSG 005335 >: QueryManager::loadForReverse\n\n\tCargando para reversar:\nTx: -"
		  << id 
		  << "state:" << state
		  << "|evolve:" << evolve 
		  << "|responseCode:" << responseCode 
		  << "|autorisationCode:" << authorisationCode 
		  << "|login:" << login 
		  << "|amount:" << amountInt 
		  << "|telephone:" << telephone
		  << "|audit:" << audit
		  << "|date:" << date
		  << "|time:" << time
		  << "|acceptor:" << acceptor
		  << "|receptor:" << receptor
		  << "|transmissionDateTime:" << transmissionDateTime
		  << "|password:(no mostrado)"
		  << endl;
		Log::instance().log( sstream.str().c_str() );
	  
	  }
	  try
		{
		  Message* message_p = MessageFactory::create(
													  PortManager::webPortNamePrefix
													  ,Message::H0420
													  );
		  message_p->setTxId( id );
		  message_p->setResponseCode(responseCode);
		  message_p->setAuthorisationCode(authorisationCode);
		  message_p->setAmount( Message::normalize(12,'0',amountInt) );
		  message_p->setTelephone(Message::normalize(15,'0',telephone) );
		  
		  string a, b, c,d;
		  {
			QueryManager queryManager;
			a = queryManager.createAudit();
			b = queryManager.createDate();
			c = queryManager.createTime();
			queryManager.begin();
			d = queryManager.createOriginalDataElements(id);
			queryManager.isCommit(true);
		  }
		  d = Message::normalize(42,'0',d);
		  message_p->setOriginalDataElements(d);
		  message_p->setSystemsTraceAuditNumber( a );
		  message_p->setTransmissionDateTime( b+c );
		  message_p->setLocalTransactionTime( c );
		  message_p->setLocalTransactionDate( b );
		  message_p->setSettlementDate( b );
		  message_p->setCaptureDate( b );
		  message_p->setAcceptor( acceptor );
		  message_p->setReceptor( receptor );
		  message_p->setReceiverName( PortManager::webPortNamePrefix );//recordar que el constructor especifica el sender, mas aqui es importante poner el receiver
		  message_p->serialize();
		  message_p->setTramaIn( message_p->getTramaOut() );
		  
		  //importantisimmo !!!
		  message_p->isRecover(true);
		  message_p->setLogin(login);
		  message_p->setPassword(password);
		  a_vector.push_back( message_p );
		}catch(Exception& e){
		  e
			<< "Exception:\n\t"
			<< e.getMessage()
			<< "- .\nthrow loadForReverse\ncatch loadForReverse\n";
		  throw e;
		}
	}//for
  /////////////////////////////////
  PQclear(res_p);
  query = "CLOSE mycursor";
  res_p = PQexec(d_connection_sp, query.c_str() );
  if (!res_p || PQresultStatus(res_p) != PGRES_COMMAND_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 548 >: ecc::Exception -"<< query <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in loadMessages."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}//if
  PQclear(res_p);
}//

/**
 *
 */
bool QueryManager::isCommit(bool a_isCommit)
{
  bool isCommit = d_isCommit;
  d_isCommit = a_isCommit;
  return isCommit;

}//isCommit

/**
 *
 */
string QueryManager::createAudit()
{
  PGresult*   res_p;		  
  stringstream query;

  query
	<< "SELECT \"sp_create_audit\"()";

  res_p = PQexec( d_connection_sp, query.str().c_str() );
  if (!res_p || PQresultStatus(res_p) != PGRES_TUPLES_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 026 >: ecc::Exception -"<< query.str() <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in createAudit."
		<< endl;
	  throw e;
	}//if
	  
  string result;
  result.assign( 
			   PQgetvalue(res_p,0,0), 
			   PQgetlength(res_p,0,0) 
			   );

  PQclear(res_p);
  return result;
}//getAudit

/**
 *
 */
string QueryManager::createTime()
{
  PGresult*   res_p;		  
  stringstream query;

  query 
	<< "SELECT \"sp_create_time\"()";

  res_p = PQexec( d_connection_sp, query.str().c_str() );
  if (!res_p || PQresultStatus(res_p) != PGRES_TUPLES_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 026 >: ecc::Exception -"<< query.str() <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in createTime."
		<< endl;
	  throw e;
	}//if
	  
  string result;
  result.assign( 
			   PQgetvalue(res_p,0,0), 
			   PQgetlength(res_p,0,0) 
			   );

  PQclear(res_p);

  return result;
}//createTime

/**
 *
 */
string QueryManager::createDate()
{
  PGresult*   res_p;		  
  stringstream query;

  query 
	<< "SELECT \"sp_create_date\"()";

  res_p = PQexec( d_connection_sp, query.str().c_str() );
  if (!res_p || PQresultStatus(res_p) != PGRES_TUPLES_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 026 >: ecc::Exception -"<< query.str() <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in createDate."
		<< endl;
	  throw e;
	}//if
	  
  string result;
  result.assign( 
			   PQgetvalue(res_p,0,0), 
			   PQgetlength(res_p,0,0) 
			   );

  PQclear(res_p);

  return result;
}//createDate

/**
 *
 */
string QueryManager::createOriginalDataElements(string a_id)
{
  PGresult*   res_p;		  
  stringstream query;

  sqlSafe(a_id);
  query
	<< "DECLARE mycursor CURSOR FOR SELECT id, audit, date, time, authorisation_code FROM t_txs WHERE id='" << a_id << "'";
  res_p = PQexec( d_connection_sp, query.str().c_str() );
  if (!res_p || PQresultStatus(res_p) != PGRES_COMMAND_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 443 >: ecc::Exception -"<< query <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in createOriginalDataElements."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}//if
  ///////////////////////////////
  PQclear(res_p);
  string query2 = "FETCH ALL in mycursor";
  res_p = PQexec( d_connection_sp, query2.c_str() ); 
  if (!res_p || PQresultStatus(res_p) != PGRES_TUPLES_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 444 >:  ecc::Exception -"<< query <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in loadMessages."
		<< endl;
	  PQclear(res_p);
	  throw e;
	}

  /////////////////////////////
  int tuplesN = PQntuples(res_p);
  string messageType = "0200";
  string id;
  string audit;
  string date;
  string time;
  string authorisationCode;
	  
  //	  cout << "Numero de tuplas encontradas : "<<tuplesN<<endl;
  //se rellena
  if( tuplesN == 0 )
	{
	  NotFoundException e;
	  e 
		<< "< MSG 299 >: NotFoundException:\n\n\tNo se encontro registro asociado a la transaccion: -"
		<< a_id
		<< "-. Imposible obtener informacion original para reverso."
		<< endl 
		<< "throw in ecc::QueryManager::createOriginalDataElements"
		<< endl
		;
	  throw e;
	}

  id.assign( 
			PQgetvalue(res_p,0,0), 
			PQgetlength(res_p,0,0) 
			);
  audit.assign( 
			   PQgetvalue(res_p,0,1), 
			   PQgetlength(res_p,0,1) 
			   );
  date.assign( 
			  PQgetvalue(res_p,0,2), 
			  PQgetlength(res_p,0,2) 
			  );
  time.assign( 
			  PQgetvalue(res_p,0,3), 
			  PQgetlength(res_p,0,3) 
			  );
  authorisationCode.assign( 
						   PQgetvalue(res_p,0,4), 
						   PQgetlength(res_p,0,4) 
						   );
	  //creo la trama correspondiente
  /////////////////////////////////
  PQclear(res_p);
  string query3 = "CLOSE mycursor";
  res_p = PQexec(d_connection_sp, query3.c_str() );
  if (!res_p || PQresultStatus(res_p) != PGRES_COMMAND_OK)
	{
	  ecc::Exception e;
	  e.getStream() 
		<< "< MSG 493 >: : ecc::Exception -"<< query <<"- command failed."
		<< endl
		<< "Server report: -"
		<< PQerrorMessage(d_connection_sp)
		<< "-."
		<< "throw in loadMessages."
			<< endl;
	  PQclear(res_p);
		  throw e;
	}//if
  PQclear(res_p);
  //nuevo  commit();//todo: checar esto
  return messageType + Message::normalize(12,'0',id) + date + time + "00" + date + "0000000000";
}//

#endif
