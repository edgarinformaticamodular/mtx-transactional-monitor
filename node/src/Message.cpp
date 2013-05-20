#ifndef ECC_MESSAGE_CPP
#define ECC_MESSAGE_CPP

//#include <cc++/thread.h> 
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include "Context.hpp"
#include "Log.hpp"
#include "Exception.hpp"
#include "ParseException.hpp"
#include "BadArgumentException.hpp"
#include "BadOptionalFieldException.hpp"
#include "NotFoundException.hpp"
#include "Message.hpp"
#include "Parser.hpp"
#include "BasicParser.hpp"
#include "GenericParser.hpp"
#include "GenericParserInclude.hpp"
#include "AlphanumericParser.hpp"
#include "NumericParser.hpp"
#include "ParserFunctor.hpp"

//using namespace ost;
//using namespace std;
using ecc::Parser;
using ecc::BasicParser;
using ecc::GenericParser;
using ecc::GenericParserInclude;
using ecc::AlphanumericParser;
using ecc::NumericParser;
using ecc::ParserFunctor;
using ecc::Message;
using ecc::Context;
using ecc::Log;
using ecc::Exception;
using ecc::ParseException;
using ecc::BadArgumentException;
using ecc::BadOptionalFieldException;
using ecc::NotFoundException;


const int Message::MAX_TRAMA_SIZE = 3072;
const int Message::ISO_SIZE = 5;
const int Message::MAX_BUFFER = 801;
const int Message::BASE24_HEADER_SIZE = 9;
const int Message::TYPE_SIZE = 4;
const int Message::PRIMARY_BIT_MAP_SIZE = 16;

const int Message::WAIT_TIME = 3000;

const string Message::H0200 = "0200";
const string Message::H9200 = "9200";
const string Message::H0210 = "0210";
const string Message::H9210 = "9210";
const string Message::H0420 = "0420";
const string Message::H9420 = "9420";
const string Message::H0421 = "0421";
const string Message::H9421 = "9421";
const string Message::H0430 = "0430";
const string Message::H9430 = "9430";
const string Message::H0800 = "0800";
const string Message::H9800 = "9800";
const string Message::H0810 = "0810";
const string Message::H9810 = "9810";


const char Message::B0001 = '\x1';
const char Message::B0010 = '\x2';
const char Message::B0100 = '\x4';
const char Message::B1000 = '\x8';

const int Message::P01_ID = 1;
const int Message::P01_MAX_SIZE = 16;
const int Message::P01_HEADER_SIZE = 0;

const int Message::P03_ID = 3;
const int Message::P03_MAX_SIZE = 6;
const int Message::P03_HEADER_SIZE = 0;

const int Message::P04_ID = 4;
const int Message::P04_MAX_SIZE = 12;
const int Message::P04_HEADER_SIZE = 0;

const int Message::P07_ID = 7;
const int Message::P07_MAX_SIZE = 10;
const int Message::P07_HEADER_SIZE = 0;

const int Message::P11_ID = 11;
const int Message::P11_MAX_SIZE = 6;
const int Message::P11_HEADER_SIZE = 0;

const int Message::P12_ID = 12;
const int Message::P12_MAX_SIZE = 6;
const int Message::P12_HEADER_SIZE = 0;

const int Message::P13_ID = 13;
const int Message::P13_MAX_SIZE = 4;
const int Message::P13_HEADER_SIZE = 0;

const int Message::P15_ID = 15;
const int Message::P15_MAX_SIZE = 4;
const int Message::P15_HEADER_SIZE = 0;

const int Message::P17_ID = 17;
const int Message::P17_MAX_SIZE = 4;
const int Message::P17_HEADER_SIZE = 0;

const int Message::P32_ID = 32;
const int Message::P32_MAX_SIZE = 13;//11+2
const int Message::P32_HEADER_SIZE = 2;

const int Message::P35_ID = 35;
const int Message::P35_MAX_SIZE = 39;//37+2
const int Message::P35_HEADER_SIZE = 2;

const int Message::P37_ID = 37;
const int Message::P37_MAX_SIZE = 12;
const int Message::P37_HEADER_SIZE = 0;

const int Message::P38_ID = 38;
const int Message::P38_MAX_SIZE = 6;
const int Message::P38_HEADER_SIZE = 0;

const int Message::P39_ID = 39;
const int Message::P39_MAX_SIZE = 2;
const int Message::P39_HEADER_SIZE = 0;

const int Message::P41_ID = 41;
const int Message::P41_MAX_SIZE = 16;
const int Message::P41_HEADER_SIZE = 0;

const int Message::P43_ID = 43;
const int Message::P43_MAX_SIZE = 40;
const int Message::P43_HEADER_SIZE = 0;

const int Message::P48_ID = 48;
const int Message::P48_MAX_SIZE = 47;
const int Message::P48_HEADER_SIZE = 0;

const int Message::P49_ID = 49;
const int Message::P49_MAX_SIZE = 3;
const int Message::P49_HEADER_SIZE = 0;

const int Message::P60_ID = 60;
const int Message::P60_MAX_SIZE = 15;
const int Message::P60_HEADER_SIZE = 0;

const int Message::P61_ID = 61;
const int Message::P61_MAX_SIZE = 16;
const int Message::P61_HEADER_SIZE = 0;

const int Message::S070_ID = 70;
const int Message::S070_MAX_SIZE = 3;
const int Message::S070_HEADER_SIZE = 0;

const string Message::LOGON_NETWORK_MANAGEMENT_INFORMATION_CODE = "001";
const string Message::LOGOFF_NETWORK_MANAGEMENT_INFORMATION_CODE = "002";
const string Message::CHANGE_KEY_NETWORK_MANAGEMENT_INFORMATION_CODE = "161";
const string Message::NEW_KEY_NETWORK_MANAGEMENT_INFORMATION_CODE = "162";
const string Message::REPEAT_KEY_NETWORK_MANAGEMENT_INFORMATION_CODE = "163";
const string Message::VERIFY_KEY_NETWORK_MANAGEMENT_INFORMATION_CODE = "164";
const string Message::ECHO_TEST_NETWORK_MANAGEMENT_INFORMATION_CODE = "301";

const int Message::S090_ID = 90;
const int Message::S090_MAX_SIZE = 42;
const int Message::S090_HEADER_SIZE = 0;

const int Message::S100_ID = 100;
const int Message::S100_MAX_SIZE = 11;//11+2
const int Message::S100_HEADER_SIZE = 2;

const int Message::S102_ID = 102;
const int Message::S102_MAX_SIZE = 28;//28+2
const int Message::S102_HEADER_SIZE = 2;

const int Message::S103_ID = 103;
const int Message::S103_MAX_SIZE = 28;//28+2
const int Message::S103_HEADER_SIZE = 2;

const int Message::S126_ID = 126;
const int Message::S126_MAX_SIZE = 800;//incluye los 3
const int Message::S126_HEADER_SIZE = 3;

const int Message::LOGIN_SIZE = 10;
const int Message::PASSWORD_SIZE = 8;

//CODIGOS DE RESPUESTA LOCALES
const string Message::RESPONSE_CODE_INVALID_LOGIN = "09";
const string Message::RESPONSE_CODE_INVALID_PASSWORD = "10";
const string Message::RESPONSE_CODE_NO_CREDIT = "11";
const string Message::RESPONSE_CODE_LOCAL_DENIED = "12";
const string Message::RESPONSE_CODE_MONTO_INVALIDO = "13";
const string Message::RESPONSE_CODE_PP_NO_DISPONIBLE = "14";
const string Message::RESPONSE_CODE_CALL_CENTER_NO_DISPONIBLE = "08";
//CODIGOS DE RESPUESTA ISO
const string Message::RESPONSE_CODE_APPROVED = "00";
const string Message::RESPONSE_CODE_DENIED = "05";
const string Message::RESPONSE_CODE_TELEFONO_INVALIDO = "83";
const string Message::RESPONSE_CODE_TRANSACCION_INVALIDA = "84";
const string Message::RESPONSE_CODE_AUTORIZADOR_NO_DISPONIBLE_DATABASE = "89";//no hay autorizador
const string Message::RESPONSE_CODE_AUTORIZADOR_NO_DISPONIBLE_MIDDLEWARE = "91";


//status field
const string Message::STATUS_FIELD_SECURITY_MESSAGE_UNEXPECTED = "196";

/**************** CONSTRUCTORS ****************************/

/**
 *
 */
Message::Message()
  :d_state(INCONSISTENT)
  ,d_receiverName("")
  ,d_isRecover(false)
{
}//

/**
 *
 */
Message::Message( string a_receiverName )
  :d_state(INCONSISTENT)
  ,d_receiverName(a_receiverName)
  ,d_isRecover(false)
{
}//

/**
 *
 */
Message::Message(
				 string a_receiverName
				 ,string a_trama
				 )
  :d_tramaIn(a_trama)
  ,d_state(INCONSISTENT)
  ,d_receiverName(a_receiverName)
  ,d_isRecover(false)
{
}//

/** todo: que tal si aqui destructores de parsers ?
 *
 */
Message::~Message()
{
  map<int,Parser*>::iterator itr;
  for(itr = d_parsers.begin(); itr != d_parsers.end(); itr++)
	delete itr->second;
}//

/**************************** ACCESORS ***********************/

/**
 *
 */
string Message::getTxId()
{
  stringstream sstream;
  int campo = 37;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 122 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::getRetrievalReferenceNumber: \n";
	  throw e;
	}
  return itr->second->getData();
}//

/**
 *
 */
void Message::setTxId(string a_txId)
{
  stringstream sstream;
  int campo = 37;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 111 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::setRetrievalReferenceNumber: \n";
	  throw e;
	}
  itr->second->setData(a_txId);
}//

/**
 *
 */
string Message::getRetrievalReferenceNumber(void)
{
  return getTxId();
}//


/**
 *
 */
void Message::setRetrievalReferenceNumber(string a_retrievalReferenceNumber)
{
  setTxId(a_retrievalReferenceNumber);
}//

/**
 *
 */
string Message::getSequenceId(void)
{
  if( d_parsers.find(11) == d_parsers.end() )
	{
	  throw Exception("< MSG 083 >: ecc::Exception::\n\n\tNo se pudo encontrar el numero de secuencia del mensaje, porque no tiene campo 11.\nthrow in Message::getSequenceId.\n");
	}//if
  return d_parsers[11]->getData();
}//

/**
 *
 */
void Message::setSequenceId(string a_sequenceId)
{
  if( d_parsers.find(11) == d_parsers.end() )
	{
	  throw Exception("< MSG 084 >: ecc::Exception::\n\n\tNo se pudo encontrar el numero de secuencia del mensaje, porque no tiene campo 11.\nthrow in Message::setSequenceId.\n");
	}
  return d_parsers[11]->setData(a_sequenceId);
}//

/**
 *
 */
string Message::getAmount(void)
{
  if( d_parsers.find(4) == d_parsers.end() )
	{
	  throw Exception("< MSG 085 >: ecc::Exception::\n\n\tNo se pudo encontrar el Monto, porque no tiene campo 4.\nthrow in Message::getAmount.");
	}//if
  return d_parsers[4]->getData();
}//

/**
 *
 */
void Message::setAmount(string a_amount)
{
  if( d_parsers.find(4) == d_parsers.end() )
	{
	  throw Exception("< MSG 086 >: ecc::Exception::\n\n\tNo se pudo encontrar el Monto, porque no tiene campo 4.\nthrow in Message::setAmount.\n");
	}//if
  return d_parsers[4]->setData(a_amount);
}//

/**
 *
 */
string Message::getTelephone(void)
{
  if( d_parsers.find(126) == d_parsers.end() )
	{
	  throw Exception("< MSG 087 >: ecc::Exception::\n\n\tNo se pudo encontrar el telefono, porque no tiene campo 126.\nthrow in Message::getTelephone\n");
	}//if
  return d_parsers[126]->getData().substr(26,15);
}//

/**
 *
 */
void Message::setTelephone(string a_telephone)
{
  if( d_parsers.find(126) == d_parsers.end() )
	{
	  throw Exception("< MSG 088 >: ecc::Exception::\n\n\tNo se pudo encontrar el telefono, porque no tiene campo 126.\nthrow in Message::setTelephone.\n");
	}//if
  return d_parsers[126]->setData( d_parsers[126]->getData().replace(26,15,a_telephone) );
}//


/**
 *
 */
string Message::getTransmissionDateTime(void)
{
  stringstream sstream;
  int campo = 7;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 116 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::getTransmissionDateTime:\n ";
	  throw e; 
	}
  return itr->second->getData();
}//

/** para setup iso **/
/**
 *
 */
void Message::setTransmissionDateTime(string a_transmissionDateTime)
{
  stringstream sstream;
  int campo = 7;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 105 >: ecc::Exception:\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::setTransmissionDateTime.\n";
	  throw e;
	}
  itr->second->setData(a_transmissionDateTime);
}//


/**
 *
 */
string Message::getSystemsTraceAuditNumber(void)
{
  stringstream sstream;
  int campo = 11;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 117 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::getSystemsTraceAuditNumber: \n";
	  throw e;
	}
  return itr->second->getData();
}//

/**
 *
 */
void Message::setSystemsTraceAuditNumber(string a_systemsTraceAuditNumber)
{
  stringstream sstream;
  int campo = 11;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 106 >: ecc::Exception:\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::setSystemsTraceAuditNumber\n";
	  throw e;
	}
  itr->second->setData(a_systemsTraceAuditNumber);
}//

/**
 *
 */
string Message::getLocalTransactionTime(void)
{
  stringstream sstream;
  int campo = 12;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 118 >: :exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje\nthrow in Message::getLocalTransactionTime\n";
	  throw e;
	}
  return itr->second->getData();
}//

/**
 *
 */
void Message::setLocalTransactionTime(string a_localTransactionTime)
{
  stringstream sstream;
  int campo = 12;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 107 >: ecc::Exception:\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::setLocalTransactionTime\n";
	  throw ;
	}//if
  itr->second->setData(a_localTransactionTime);
}//

/**
 *
 */
string Message::getLocalTransactionDate(void)
{
  stringstream sstream;
  int campo = 13;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 119 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::getLocalTransactionDate: \n";
	  throw e;
	}
  return itr->second->getData();
}//

/**
 *
 */
void Message::setLocalTransactionDate(string a_localTransactionDate)
{
  stringstream sstream;
  int campo = 13;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 108 >: ecc::Exception::\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::setLocalTransactionTime: \n";
	  throw e;
	}
  itr->second->setData(a_localTransactionDate);
}//

/**
 *
 */
string Message::getSettlementDate(void)
{
  stringstream sstream;
  int campo = 15;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 120 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::getSettlementDate: \n";
	  throw e;
	}
  return itr->second->getData();
}//

/**
 *
 */
void Message::setSettlementDate(string a_settlementDate)
{
  stringstream sstream;
  int campo = 15;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 109 >: ecc::Exception:\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::setSettlementDate: \n";
	  throw e;
	}
  itr->second->setData(a_settlementDate);
}//

/**
 *
 */
string Message::getCaptureDate(void)
{
  stringstream sstream;
  int campo = 17;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 121 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::getCaptureDate: \n";
	  throw e;
	}
  return itr->second->getData();
}//getCaptureDate

/**
 *
 */
void Message::setCaptureDate(string a_captureDate)
{
  stringstream sstream;
  int campo = 17;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 110 >: ecc::Exception:\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::setCaptureDate: \n";
	  throw e;
	}
  itr->second->setData(a_captureDate);
}//setCaptureDate

/**
 *
 */
string Message::getNetworkManagementInformationCode(void)
{
  stringstream sstream;
  int campo = 70;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 123 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::getRetrievalReferenceNumber: \n";
	  throw e;
	}
  return itr->second->getData();
}//

/**
 *
 */
void Message::setNetworkManagementInformationCode(string a_networkManagementInformationCode)
{
  stringstream sstream;
  int campo = 70;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 112 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\n\throw in Message::setRetrievalReferenceNumber: \n";
	  throw e;
	}
  itr->second->setData(a_networkManagementInformationCode);
}//

/**
 *
 */
string Message::getAuthorisationCode(void)
{
  stringstream sstream;
  int campo = 38;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 124 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::getAuthorisationCode: \n";
	  throw e;
	}
  return itr->second->getData();
}//

/**
 *
 */
void Message::setAuthorisationCode(string a_authorisationCode)
{
  stringstream sstream;
  int campo = 38;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 113 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::setAuthorisationCode: \n";
	  throw e;
	}
  itr->second->setData(a_authorisationCode);
}//

/**
 *
 */
string Message::getResponseCode(void)
{
  stringstream sstream;
  int campo = 39;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 125 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::getResponseCode: \n";
	  throw e;
	}
  return itr->second->getData();
}//

/**
 *
 */
void Message::setResponseCode(string a_responseCode)
{
  stringstream sstream;
  if( 
	 a_responseCode == "89" || 
	 a_responseCode == "91" ||
	 a_responseCode == "84" || 
	 a_responseCode == "83" || 
	 a_responseCode == "13" || 
	 a_responseCode == "12" || 
	 a_responseCode == "11" || 
	 a_responseCode == "10" || 
	 a_responseCode == "09" || 
	 a_responseCode == "00" 
	 )
	{
	  static int CAMPO_N = 39;
	  map<int,Parser*>::iterator itr = d_parsers.find(CAMPO_N);
	  if( itr == d_parsers.end() )
		{
		  BadOptionalFieldException e;
		  e.getStream() << "< MSG 114 >: ecc::BadOptionalFieldException::\n\n\t No se ha dado de contenedor para alojar valor para el campo:-"<<CAMPO_N<<"-.\nthrow in Message::setResponseCode\n";
		  throw e;
		}
	  itr->second->setData(a_responseCode);
	}else{
	  Exception e;
	  e.getStream() << "< MSG 115 >: ecc::Exception\n\n\tCodigo de respuesta invalido: -"<<a_responseCode <<"-.\nthrow in Message::setResponseCode: \n";
	  throw e;
	}//else
}//

/**
 *
 */
string Message::getOriginalDataElements(void)
{
  stringstream sstream;
  int campo = 90;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 297 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::getOriginalDataElements: \n";
	  throw e;
	}
  return itr->second->getData();
}//

/**
 *
 */
void Message::setOriginalDataElements(string a_originalDataElements)
{
  stringstream sstream;
  int campo = 90;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 298 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::setOriginalDataElements: \n";
	  throw e;
	}
  itr->second->setData(a_originalDataElements);
}//

/**
 *
 */
string Message::getAcceptor(void)
{
  stringstream sstream;
  int campo = 41;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 356 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::getAcceptor: \n";
	  throw e;
	}
  return itr->second->getData();
}//

/**
 *
 */
void Message::setAcceptor(string a_acceptor)
{
  stringstream sstream;
  int campo = 41;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 355 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::setAcceptor \n";
	  throw e;
	}
  itr->second->setData(a_acceptor);
}//

/**
 *
 */
string Message::getReceptor(void)
{
  stringstream sstream;
  int campo = 126;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 359 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::getReceptor: \n";
	  throw e;
	}
  string content = itr->second->getData();
  if( content.size() < 29)//por aquello del header
	{
	  Exception e;
	  e.getStream() << "< MSG 360 >: ecc::Exception\n\n\tEl campo -"<<campo<<"-, Es mas corto de lo esperado, por lo que no se puede obtener la informacion del password: -"<<content.size()<<"-:-"<<content<<"-.\nthrow in Message::getReceptor: \n";
	  throw e;
	}
  return content.substr(22,4);
}//getPassword

/**
 *
 */
void Message::setReceptor(string a_receptor)
{
  stringstream sstream;
  int campo = 126;
  map<int,Parser*>::iterator itr = d_parsers.find(campo);
  if( itr == d_parsers.end() )
	{
	  Exception e;
	  e.getStream() << "< MSG 357 >: ecc::Exception\n\n\tNo se ha encontrado el campo -"<<campo<<"-, en este mensaje. Las causas pueden ser que no lo trae, o que no se ha deserializado el mensaje.\nthrow in Message::setReceptor: \n";
	  throw e;
	}
  string content = itr->second->getData();
  if( content.size() < 29)
	{
	  Exception e;
	  e.getStream() << "< MSG 358 >: ecc::Exception\n\n\tEl campo -"<<campo<<"-, Es mas corto de lo esperado, por lo que no se puede establecer la informacion relacionada con el password. La longitud actual del campo es: -" << content.size()<<"-.\nthrow in Message::setReceptor: \n";
	  throw ;
	}
  content.replace(22,4,a_receptor);
  itr->second->setData(content);
}//


/************************ SIMPLE ACCESORS ******************************/

/**
 *
 */
string Message::getType(void) const
{
  return d_type;
}//

/** todo: checar la evaluacion de or vs. ||
 *
 */
void Message::setType(string a_Type)//todo: aqui se puede optimizar, pero notese quue no es redundante con validacion de deserialize mas sin embargo se puede usar el mismo codigo para no repetir
{
  if( a_Type == "0200" || a_Type == "0210" || a_Type == "9200" || a_Type == "9210" || a_Type == "0420" || a_Type == "9420" ||a_Type == "0421" ||a_Type == "9421" || a_Type == "0430" || a_Type == "9430" || a_Type == "0800" || a_Type == "9800" || a_Type == "0810" || a_Type == "9810")
	d_type = a_Type;
  else
	{
	  Exception e;
	  e.getStream() << "< MSG 104 >: ecc::Exception::\n\n\tIntentando asignar un tipo ISO no valido. El tipo de mensaje iso propuesto es: -" << a_Type << "-.\nthrow in Message::setType.\n";
	  throw e;
	}//else
}//

/**
 *
 */
void Message::setQueueTxId(string a_queueTxId)
{
  d_queueTxId = a_queueTxId;
}//

/**
 *
 */
string Message::getQueueTxId()
{
  return d_queueTxId;
}//

/**
 *
 */
string Message::getTramaIn() const
{
  return d_tramaIn;
}//getTrama

/**
 *
 */
void Message::setTramaIn(string a_tramaIn)
{
  d_tramaIn = a_tramaIn;
}//setTrama

/**
 *
 */
string Message::getTramaOut() const
{
  return d_tramaOut;
}//getTrama

/**
 *
 */
void Message::setTramaOut(string a_tramaOut)
{
  d_tramaOut = a_tramaOut;
}//setTrama


/**
 *
 */
Message::MessageState Message::getState() const
{
  return d_state;
}//

/**
 *
 */
void Message::setState(MessageState a_state)
{
  d_state = a_state;
}//

/**
 *
 */
string Message::getLogin()
{
  if( d_login.size() == 0 )//si no lo trae esta en el mensaje iso
	{
	  if( d_parsers.find(126) == d_parsers.end() )
		{
		  throw Exception("< MSG 001571 >: ecc::Exception::\n\n\tNo se pudo encontrar el login, porque no tiene campo 126.\nthrow in Message::getLogin\n");
		}//if
	  return d_parsers[126]->getData().substr(106,10);
	}
  else
	return d_login;
}//

/**
 *
 */
void Message::setLogin(string a_login)
{
  d_login = a_login;
}//

/**
 *
 */
string Message::getPassword()
{
  if( d_password.size() == 0 )//si no lo trae esta en el mensaje iso
	{
	  if( d_parsers.find(126) == d_parsers.end() )
		{
		  throw Exception("< MSG 001572 >: ecc::Exception::\n\n\tNo se pudo encontrar el login, porque no tiene campo 126.\nthrow in Message::getPassword\n");
		}//if
	  return d_parsers[126]->getData().substr(116,8);//todo:cachar exepcion de substring para indicar que esta mal formado el campo aqui y en login
	}
  return d_password;
}//

/**
 *
 */
void Message::setPassword(string a_password)
{
  d_password = a_password;
}//

/**
 *
 */
string Message::getReceiverName() const
{
  return d_receiverName;
}//

/**
 *
 */
void Message::setReceiverName(string a_receiverName)
{
  d_receiverName = a_receiverName;
}//

/**
 *
 */
string Message::getSenderName() const
{
  return d_senderName;
}//

/**
 *
 */
void Message::setSenderName(string a_senderName)
{
  d_senderName = a_senderName;
}//

/******************** FORMATERS *****************************/

/**
 *
 */
string Message::display()
{
  stringstream sstream;
  sstream << "Message::display:info:ini."<< endl;
  sstream << "  d_iso          :"<< d_iso << endl;
  sstream << "  d_state        :"<< d_state << endl;
  sstream << "  d_base24Header :"<< d_base24Header << endl;
  sstream << "  d_type         :"<< d_type << endl;
  sstream << "  d_primaryBitMap:"<< d_primaryBitMap << endl;
  map<int,Parser*>::iterator itr;
  for(itr = d_parsers.begin(); itr != d_parsers.end(); itr++ )
	{
	  sstream << itr->second->display() << endl;
	}//for
  sstream << "Message::display:info:fin."<<endl;
  return sstream.str();
}//

/**
 *
 */
string Message::getTramaPartition()
{
  stringstream sstream;
  //el header
  sstream 
	<< d_iso << "|"
	<< d_state << "|"
	<< d_base24Header << "|"
	<< d_type << "|"
	<< d_primaryBitMap << "|";

  //el body
  map<int,Parser*>::iterator itr;
  for(itr = d_parsers.begin(); itr != d_parsers.end(); itr++ )
	{
	  sstream << itr->second->getData() << "|";
	}//for

  return sstream.str();
}//

/*************************** SERIALIZATION, DESERIALIZATION *******************/

/**
 *
 */
void Message::simpleParse(
						stringstream& a_sstream
						,char* a_buffer
						,int a_size
						,string& a_target
						,const char* a_message
						)
{
  a_sstream.read(a_buffer,a_size);
  if( a_sstream.fail() )
	{
	  string message = a_message;
	  throw ParseException("197",message + "\nthrow in Message::SimpleParse.\n" );
	}//if
  else
	a_target.assign(a_buffer,a_size);
}//

/**
 * desecha los caracteres entre el EOT y el token ISO de inicio de mensaje
 * entre estos puede ir el caracter de inicio de transmision y la cabecera del mensaje
 * base 24 que no utilizamos
 */
void Message::findTokenISO(stringstream& a_sstream)
{
  char buff[3] = {'0','0','0'};
  while( ! a_sstream.fail() )
	{
	  buff[0] = buff[1];
	  buff[1] = buff[2];
	  buff[2] = a_sstream.get();
	  if( buff[0] == 'I' )
		if( buff[1] == 'S' )
		  if( buff[2] == 'O')
			break;
	}//while
  if( a_sstream.fail() )
	{
	  setState(UNIDENTIFICABLE);
	  throw ecc::Exception("< MSG 093 >: ParseException: stringstream::fail::\n\n\tNo se pudo encontrar el token 'ISO' de cabecera en el mensaje.\nthrow in Parser::findTokenISO.\n");
	}//if
}//

/**
 * aqui puede haver una validacion, si d_iso tiene sus ultimos tres caracteres = ISO
 */
void Message::deserialize()
{
  try
	{//este tray es para captar las exepciones al tratar de buscar el ISO
	  char buffer[MAX_BUFFER];
	  stringstream sstream(d_tramaIn);
	  string dummy;
	  try
		{
		  //verificacion de la longtud total
		  //		  char startOfTransmission;
		  //sstream >> startOfTransmission;
		  char digitBase256[2];
		  sstream >> digitBase256[1];
		  sstream >> digitBase256[0];
		  string numberBase256;
		  numberBase256.assign(digitBase256,2);
		  int actual = toBase10(numberBase256);
		  ofstream xxx("receiveTrama");//todo:quitar esto para eficiencia
		  xxx << sstream.str();
		  xxx.close();
		  //int real = sstream.str().size()-1;//para incluir el ini trans 
		  int real = sstream.str().size()-2;// 
		  if( actual != real )
			{
			  int d1 = static_cast<int>(digitBase256[1]);
			  int d0 = static_cast<int>(digitBase256[0]);
			  BadArgumentException e;
			  e
				<< "< MSG 281 >: BadArgumentException:\n\n\tTrama con header pre ISO: ascii: -"
				<< ( (d1<0)?d1+256:d1 )
				<< ","
				<< ( (d0<0)?d0+256:d0 )
				<< "-, marcando longitud: -"
				<< actual
				<< "-, diferente a la real: -"
				<< real
				<< "-.\nthrow in Message::deserialize.";
			  throw e;
			}
		  //busco la i
		  findTokenISO( sstream );
		  simpleParse( sstream, buffer, BASE24_HEADER_SIZE, d_base24Header,
					   "< MSG 091 >: ecc::ParseException: \n\n\tLellendo header despues del token ISO. Posiblemente no hay suficientes bytes disponibles, por trama incompleta.\tthrow in Message::deserialize.\n");
		  simpleParse( sstream, buffer, TYPE_SIZE, d_type,
					   "< MSG 092 >: ecc::ParseException: \n\n\tLellendo tipo de mensaje, posiblmente no hay suficientes bytes disponibles por trama incompleta.\nthrow in Message::deserialize.\n");
		  //valido que es un tipo de mensaje valido
		  bool isType = 
			d_type == "0200" 
			|| d_type == "9200" 
			|| d_type == "0210" 
			|| d_type == "0420" 
			|| d_type == "9420" 
			|| d_type == "0421" 
			|| d_type == "9421" 
			|| d_type == "0430" 
			|| d_type == "0800" 
			|| d_type == "9800" 
			|| d_type == "0810"
			;
		  if( ! isType )
			{
			  throw ParseException("197",string("< MSG 001527 >: ecc::ParseException::\n\n\tTipo de mensaje no esperado: -")+d_type+"-.\n throw in Message::parse.\n");//todo: checar el codigo de error iso si realmente es 197
			}//if
		}catch(ParseException& e){
		  setState(UNIDENTIFICABLE);
		  throw e;
		}//catch
	  try
		{
		  simpleParse( sstream, buffer, PRIMARY_BIT_MAP_SIZE, d_primaryBitMap, "< MSG 094 >: ecc::Exception::\n\n\t sstream::fail: Lellendo primary bit map. Posiblemente bytes insuficientes, por trama incompleta.\nthrow in Message::deserialize.\n");
		  setupPrimaryParsers(sstream);
		  setupSecondaryParsers(sstream);
		}catch(ParseException& pe){
		  setState(BAD_FORMED);
		  throw;
		  //return;//este return es importante para que mantenga el estado asignado //todo: checar esta sentencia por lo que dice el comentario
		}//catch
	  setState(CONSISTENT);
	}catch(ParseException& e){
	  throw;
	}catch(Exception& e){
	  throw;
	}//catch
}//

/**
 * esta deve de utilizar una trama de salida algo asi como d_tramaOut
 */
void Message::serialize()
{
  stringstream sstream;
  //  sstream.put('\x01');//todo:este caracter deve realmente ir, hay alguna justificacion?
  //  sstream.put('\x02');//begin of transmision
  sstream << "ISO" << d_base24Header << d_type << d_primaryBitMap;
  map<int,Parser*>::iterator itr;
  for(itr = d_parsers.begin(); itr != d_parsers.end(); itr++ )
	{
	  if( itr->second->isIncludeHeader() )
		sstream << itr->second->getHeader();
	  // cout << "contenido:"<<itr->second->getData()<<":tamanno:"<<itr->second->getData().size()<<endl;
	  sstream << itr->second->getData();
	  //  cout << "Esto va:"<<sstream.str()<<":"<<endl;
	}//for
  //  sstream.put('\x03');//end of transmision
  //  cout << "De este tamanno quedo la trama:"<< 

  //notese que el + 1 corresponde al caracter de fin de transmision
  string trama0 = sstream.str();
  string vacia;
  d_tramaOut = vacia;
  //  d_tramaOut += '\x02';
  d_tramaOut += toBase256( trama0.size() + 1 ) +  trama0 + '\x03';
}//

/**
 *
 */
void Message::setupPrimaryParsers(stringstream& a_stringstream)
{
  string primaryBitMap = d_primaryBitMap;
  transform( primaryBitMap.begin(), 
			 primaryBitMap.end(), 
			 primaryBitMap.begin(),
			 &Message::toHex );
  //creamos los parsers a utilizar
  string::iterator itr;
  int id = 0;
  for(itr = primaryBitMap.begin(); itr != primaryBitMap.end(); itr++)
	{
	  id++;
	  if( B1000 & *itr ){
		addParser(id);
	  }
	  id++;
	  if( B0100 & *itr ){
		addParser(id);
	  }
	  id++;
	  if( B0010 & *itr ){
		addParser(id);
	  }
	  id++;
	  if( B0001 & *itr ){
		addParser(id);
	  }
	}//for
  
  //parseamos propiamente
  for_each( d_parsers.begin(),
			d_parsers.upper_bound(64),//este es el limite como si fuera end()!
			ParserFunctor(a_stringstream) );
}//

/**
 *
 */
void Message::setupSecondaryParsers(stringstream& a_stringstream)
{
  map<int,Parser*>::iterator secondaryBitMap_itr = d_parsers.find(P01_ID);
  //si existe el mapa de bits secundario
  if( secondaryBitMap_itr == d_parsers.end() )
	{
	  return;
	}
  string secondaryBitMap = secondaryBitMap_itr->second->getData();
  transform( secondaryBitMap.begin(), 
			 secondaryBitMap.end(), 
			 secondaryBitMap.begin(),
			 toHex );
  
  //creamos los parsers a utilizar
  string::iterator itr;
  int id = 64;//este es el cero para los secundarios
  for(itr = secondaryBitMap.begin(); itr != secondaryBitMap.end(); itr++)
	{
	  id++;
	  if( B1000 & *itr ){
		addParser(id);
	  }
	  id++;
	  if( B0100 & *itr ){
		addParser(id);
	  }
	  id++;
	  if( B0010 & *itr ){
		addParser(id);
	  }
	  id++;
	  if( B0001 & *itr ){
		addParser(id);
	  }
	}//for
  
  //parseamos propiamente
  for_each( d_parsers.upper_bound(64),
			d_parsers.end(),
			ParserFunctor(a_stringstream) );
}//

/**
 *
 */
void Message::addParser(int a_id)
{
  switch(a_id)
	{
	case P01_ID:
	  d_parsers[P01_ID] = 
		new AlphanumericParser<P01_ID,P01_MAX_SIZE,P01_HEADER_SIZE>();
	  break;
	case P03_ID:
	  d_parsers[P03_ID] = 
		new NumericParser<P03_ID,P03_MAX_SIZE,P03_HEADER_SIZE>();
	  break;
	case P04_ID:
	  d_parsers[P04_ID] =
		new NumericParser<P04_ID,P04_MAX_SIZE,P04_HEADER_SIZE>();
	  break;
	case P07_ID:
	  d_parsers[P07_ID] = 
		new NumericParser<P07_ID,P07_MAX_SIZE,P07_HEADER_SIZE>();
	  break;
	case P11_ID:
	  d_parsers[P11_ID] = 
		new NumericParser<P11_ID,P11_MAX_SIZE,P11_HEADER_SIZE>();
	  break;
	case P12_ID:
	  d_parsers[P12_ID] = 
		new NumericParser<P12_ID,P12_MAX_SIZE,P12_HEADER_SIZE>();
	  break;
	case P13_ID:
	  d_parsers[P13_ID] = 
		new NumericParser<P13_ID,P13_MAX_SIZE,P13_HEADER_SIZE>();
	  break;
	case P15_ID:
	  d_parsers[P15_ID] = 
		new NumericParser<P15_ID,P15_MAX_SIZE,P15_HEADER_SIZE>();
	  break;
	case P17_ID:
	  d_parsers[P17_ID] = 
		new NumericParser<P17_ID,P17_MAX_SIZE,P17_HEADER_SIZE>();
	  break;
	case P32_ID:
	  d_parsers[P32_ID] = 
		new NumericParser<P32_ID,P32_MAX_SIZE,P32_HEADER_SIZE>();
	  break;
	case P35_ID:
	  d_parsers[P35_ID] = 
		new GenericParser<P35_ID,P35_MAX_SIZE,P35_HEADER_SIZE>();
	  break;
	case P37_ID:
	  d_parsers[P37_ID] = 
		new AlphanumericParser<P37_ID,P37_MAX_SIZE,P37_HEADER_SIZE>();
	  break;
	case P38_ID:
	  d_parsers[P38_ID] = 
		new AlphanumericParser<P38_ID,P38_MAX_SIZE,P38_HEADER_SIZE>();
	  break;
	case P39_ID:
	  d_parsers[P39_ID] = 
		new AlphanumericParser<P39_ID,P39_MAX_SIZE,P39_HEADER_SIZE>();
	  break;
	case P41_ID:
	  d_parsers[P41_ID] = 
		new GenericParser<P41_ID,P41_MAX_SIZE,P41_HEADER_SIZE>();
	  break;
	case P43_ID:
	  d_parsers[P43_ID] = 
		new GenericParser<P43_ID,P43_MAX_SIZE,P43_HEADER_SIZE>();
	  break;
	case P48_ID:
	  d_parsers[P48_ID] = 
		new GenericParser<P48_ID,P48_MAX_SIZE,P48_HEADER_SIZE>();
	  break;
	case P49_ID:
	  d_parsers[P49_ID] = 
		new NumericParser<P49_ID,P49_MAX_SIZE,P49_HEADER_SIZE>();
	  break;
	case P60_ID:
	  d_parsers[P60_ID] = 
		new GenericParser<P60_ID,P60_MAX_SIZE,P60_HEADER_SIZE>();
	  break;
	case P61_ID:
	  d_parsers[P61_ID] = 
		new GenericParserInclude<P61_ID,P61_MAX_SIZE,P61_HEADER_SIZE>();
	  break;
	case S070_ID:
	  d_parsers[S070_ID] = 
		new NumericParser<S070_ID,S070_MAX_SIZE,S070_HEADER_SIZE>();
	  break;
	case S090_ID:
	  d_parsers[S090_ID] = 
		new NumericParser<S090_ID,S090_MAX_SIZE,S090_HEADER_SIZE>();
	  break;
	case S100_ID:
	  d_parsers[S100_ID] = 
		new NumericParser<S100_ID,S100_MAX_SIZE,S100_HEADER_SIZE>();
	  break;
	case S102_ID:
	  d_parsers[S102_ID] = 
		new GenericParser<S102_ID,S102_MAX_SIZE,S102_HEADER_SIZE>();
	  break;
	case S103_ID:
	  d_parsers[S103_ID] = 
		new GenericParser<S103_ID,S103_MAX_SIZE,S103_HEADER_SIZE>();
	  break;
	case S126_ID:
	  d_parsers[S126_ID] = 
		new GenericParserInclude<S126_ID,S126_MAX_SIZE,S126_HEADER_SIZE>();
	  break;
	default:
	  {
		stringstream sstream;
		sstream << "Message::addParser: Warning::"<<endl<<"No existe clase de Parser disponible. Seguramente se esta mandando campos no previstos o no implementados aun! en este caso: "<<a_id<<" es el campo solicidado. Se prosigue con ejecucion normal, esto es solo aceptable para la version de pruebas.";
		Log::instance().log( sstream.str().c_str() );
		break;
	  }
	}//switch
}//

/**
 *
 */
void Message::serializeReject(string a_code)//el codifo de falla
{
  d_tramaOut = d_tramaIn;
  string::size_type iso = d_tramaIn.find("ISO",0);
  d_tramaOut[iso + 12] = '9'; //indico en el tipo de mensaje que es rejectado
  int TAMANNO = 3;//tamanno de la cadena de codigo de falla
  //indico el codigo de falla, lo hago asi porque es mas robusto
  if( a_code.size() != TAMANNO )
	{
	  BadArgumentException e;
	  e.getStream() <<"< MSG 102 >: ecc::BadArgumentException:\n\n\tEl codigo de falla -"<<a_code<<"-, no tiene el tamanno admisible. Su tamanno es : -"<<a_code.size()<<"- y deve de ser -"<<TAMANNO<<"-.\nthrow in Message::serializeReject.\n";
	  throw e;
	}//if 
  d_tramaOut[8] = a_code[0];
  d_tramaOut[9] = a_code[1];
  d_tramaOut[10] = a_code[2];
}//

/******************** HELPERS ******************************/


/**
 * transforma a exadecimal explicito lo que era exadecimal implicito
 */
char Message::toHex(char a_char)
{
  switch(a_char)
	{
	case '0':
	  return '\x0';
	case '1':
	  return '\x1';
	case '2':
	  return '\x2';
	case '3':
	  return '\x3';
	case '4':
	  return '\x4';
	case '5':
	  return '\x5';
	case '6':
	  return '\x6';
	case '7':
	  return '\x7';
	case '8':
	  return '\x8';
	case '9':
	  return '\x9';
	case 'A':
	  return '\xA';
	case 'a':
	  return '\xA';
	case 'B':
	  return '\xB';
	case 'b':
	  return '\xB';
	case 'C':
	  return '\xC';
	case 'c':
	  return '\xC';
	case 'D':
	  return '\xD';
	case 'd':
	  return '\xD';
	case 'E':
	  return '\xE';
	case 'e':
	  return '\xE';
	case 'F':
	  return '\xF';
	case 'f':
	  return '\xF';
	default:
	  {
		BadArgumentException e;
		e
		  << "< MSG 280 >: BadArgumentException:\n\n\tNo es un digito base 16: -"
		  << a_char
		  << "-\nthrow in Message::toHex(char)";
	  }
	  break;
	}//switch
}//toHex

/**
 *
 */
string Message::toBase256(int a_decimal)
{
  int digitBase10[2];
  char digitBase256[2];
  string final;

  digitBase10[0] = a_decimal%256;
  digitBase10[1] = (a_decimal - digitBase10[0])/256;

  for(int i=0;i<2;i++)
	{
	  if( digitBase10[i] > 255 || digitBase10[i] < 0 )
		{
		  BadArgumentException e;
		  e
			<< "< MSG 282 >: BadArgumentException:\n\n\tEs un numero decimal muy grande: -"
			<< a_decimal
			<< "- para ser representado con base 256"
			<< "\nthrow in Message::toBaseHex";
		}
	}
  digitBase256[1] = static_cast<char>(digitBase10[0]);//notese que el primer caracter es el correspondiente al digito cero
  digitBase256[0] = static_cast<char>(digitBase10[1]);//notese que el primer caracter es el correspondiente al digito cero
  final.assign( digitBase256, 2);
  return final;
}//

/**
 *
 */
int Message::toBase10(string a_string)
{
  if( a_string.size() != 2 )
	{
	  BadArgumentException e;
	  e
		<< "< MSG 276 >: BadArgumentException:\n\n\tNo es una pareja de digitos base 256: -"
		<< a_string 
		<< "-\nthrow in Message::toBase10";
	}
  int d1 = static_cast<int>(a_string[1]);
  int d0 = static_cast<int>(a_string[0]);
  d1 = (d1<0)?d1+256:d1;
  d0 = (d0<0)?d0+256:d0;
  return d1*256 + d0;
}//

/**
 *
 */
string Message::normalize(
						  int a_total
						  ,char a_char
						  ,string a_string
						  )
{
  int size = a_string.size();
  string prefix;
  if( size < a_total )
	{
	  for( int i=size;i<a_total;i++)
		prefix += a_char;
	  return prefix + a_string;
	}
  else if (size == a_total )
	return a_string;
  else if (size > a_total )
	return a_string.substr(size-a_total,a_total);
}//normalize

/**
 *
 */
bool Message::isRecover()
{
  return d_isRecover;
}//

/**
 *
 */
bool Message::isRecover(bool a_isRecover)
{
  bool old = a_isRecover;
  d_isRecover = a_isRecover;
  return old;
}//

#endif
