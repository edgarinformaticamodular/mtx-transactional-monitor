#ifndef ECC_MESSAGE_HPP
#define ECC_MESSAGE_HPP

//#include <cc++/thread.h> 
//#include <loki/SmartPtr.h>
//#include <loki/Threads.h>
#include <sstream>
#include <string>
#include <map>

#include "Parser.hpp"

using namespace std;

namespace ecc
{

  /**
   *
   */
  class Message 
  {
  public:
	typedef enum {
	  INVALID, //se parseo bien pero los valores fuera del dominio esperado
	  BAD_FORMED, //no se pudo parsear devidamente, incluyendo campos no esperados
	  CONSISTENT, // los campos en el objeto corresponden a la trama uno-a-uno
	  INCONSISTENT, //los campos no corresponden a la trama o no se ha parseado
	  UNIDENTIFICABLE//ni siquiera se encontro el token cabecera ''
	} MessageState;

  protected:
	string d_receiverName;
	string d_senderName;
	bool d_isRecover;//para indicar si un 0420 mandado desde el cliente es de recover o de cliente
	char d_connectorFromId;
	int d_portFromId;
	char d_connectorToId;
	int d_portToId;
	MessageState d_state;
	string d_tramaIn;
	string d_tramaOut;
	string d_iso;
	string d_base24Header;	
	string d_type;
	string d_primaryBitMap;

	string d_queueTxId;	
	string d_login;
	string d_password;
	map<int,Parser*> d_parsers;

	static const int WAIT_TIME;

	static const int ISO_SIZE;
	static const int MAX_BUFFER;
	static const int BASE24_HEADER_SIZE;
	static const int TYPE_SIZE;
	static const int PRIMARY_BIT_MAP_SIZE;
  public:
	static const int MAX_TRAMA_SIZE;//la trama maxima de carga es de 3kb
	static const string H0200;
	static const string H9200;
	static const string H0210;
	static const string H9210;
	static const string H0420;
	static const string H9420;
	static const string H0421;
	static const string H9421;
	static const string H0430;
	static const string H9430;
	static const string H0800;
	static const string H9800;
	static const string H0810;
	static const string H9810;

	static const char B0001;
	static const char B0010;
	static const char B0100;
	static const char B1000;

	static const int P01_ID;
	static const int P01_MAX_SIZE;
	static const int P01_HEADER_SIZE;
	
	static const int P03_ID;
	static const int P03_MAX_SIZE;
	static const int P03_HEADER_SIZE;
	
	static const int P04_ID;
	static const int P04_MAX_SIZE;
	static const int P04_HEADER_SIZE;

	static const int P07_ID;
	static const int P07_MAX_SIZE;
	static const int P07_HEADER_SIZE;
	
	static const int P11_ID;
	static const int P11_MAX_SIZE;
	static const int P11_HEADER_SIZE;
	
	static const int P12_ID;
	static const int P12_MAX_SIZE;
	static const int P12_HEADER_SIZE;

	static const int P13_ID;
	static const int P13_MAX_SIZE;
	static const int P13_HEADER_SIZE;

	static const int P15_ID;
	static const int P15_MAX_SIZE;
	static const int P15_HEADER_SIZE;
	
	static const int P17_ID;
	static const int P17_MAX_SIZE;
	static const int P17_HEADER_SIZE;
	
	static const int P32_ID;
	static const int P32_MAX_SIZE;
	static const int P32_HEADER_SIZE;
	
	static const int P35_ID;
	static const int P35_MAX_SIZE;
	static const int P35_HEADER_SIZE;
	
	static const int P37_ID;
	static const int P37_MAX_SIZE;
	static const int P37_HEADER_SIZE;
	
	static const int P38_ID;
	static const int P38_MAX_SIZE;
	static const int P38_HEADER_SIZE;
  public:
	static const string P38_0420_DEFAULT;
	static const string P38_0210_DEFAULT;
  protected:
	static const int P39_ID;
	static const int P39_MAX_SIZE;
	static const int P39_HEADER_SIZE;
	
	static const int P41_ID;
	static const int P41_MAX_SIZE;
	static const int P41_HEADER_SIZE;
	
	static const int P43_ID;
	static const int P43_MAX_SIZE;
	static const int P43_HEADER_SIZE;
	
	static const int P48_ID;
	static const int P48_MAX_SIZE;
	static const int P48_HEADER_SIZE;
	
	static const int P49_ID;
	static const int P49_MAX_SIZE;
	static const int P49_HEADER_SIZE;

	static const int P60_ID;
	static const int P60_MAX_SIZE;
	static const int P60_HEADER_SIZE;
	
	static const int P61_ID;
	static const int P61_MAX_SIZE;
	static const int P61_HEADER_SIZE;
	
	static const int S070_ID;
	static const int S070_MAX_SIZE;
	static const int S070_HEADER_SIZE;
  public:
	static const string LOGON_NETWORK_MANAGEMENT_INFORMATION_CODE;
	static const string LOGOFF_NETWORK_MANAGEMENT_INFORMATION_CODE;
	static const string CHANGE_KEY_NETWORK_MANAGEMENT_INFORMATION_CODE;
	static const string NEW_KEY_NETWORK_MANAGEMENT_INFORMATION_CODE;
	static const string REPEAT_KEY_NETWORK_MANAGEMENT_INFORMATION_CODE;
	static const string VERIFY_KEY_NETWORK_MANAGEMENT_INFORMATION_CODE;
	static const string ECHO_TEST_NETWORK_MANAGEMENT_INFORMATION_CODE;
  protected:
	static const int S090_ID;
	static const int S090_MAX_SIZE;
	static const int S090_HEADER_SIZE;
	
	static const int S100_ID;
	static const int S100_MAX_SIZE;
	static const int S100_HEADER_SIZE;
	
	static const int S102_ID;
	static const int S102_MAX_SIZE;
	static const int S102_HEADER_SIZE;
	
	static const int S103_ID;
	static const int S103_MAX_SIZE;
	static const int S103_HEADER_SIZE;
	
	static const int S126_ID;
	static const int S126_MAX_SIZE;
	static const int S126_HEADER_SIZE;

	static const int LOGIN_SIZE;
	static const int PASSWORD_SIZE;

  public:
	//CODIGOS DE RESPUESTA LOCALES
	static const string RESPONSE_CODE_LOCAL_DENIED;
	static const string RESPONSE_CODE_INVALID_LOGIN;
	static const string RESPONSE_CODE_INVALID_PASSWORD;
	static const string RESPONSE_CODE_NO_CREDIT;
	static const string RESPONSE_CODE_CALL_CENTER_NO_DISPONIBLE;
	static const string RESPONSE_CODE_PP_NO_DISPONIBLE;
	//CODIGOS DE RESPUESTA ISO
	static const string RESPONSE_CODE_APPROVED;
	static const string RESPONSE_CODE_DENIED;
	static const string RESPONSE_CODE_TELEFONO_INVALIDO;
	static const string RESPONSE_CODE_TRANSACCION_INVALIDA;
	static const string RESPONSE_CODE_AUTORIZADOR_NO_DISPONIBLE_DATABASE;
	static const string RESPONSE_CODE_AUTORIZADOR_NO_DISPONIBLE_MIDDLEWARE;
	static const string RESPONSE_CODE_MONTO_INVALIDO;
	//status field
	static const string STATUS_FIELD_SECURITY_MESSAGE_UNEXPECTED;
  public:
	Message();
	Message(string);
	Message(string,string);
	//crear el objeto y deserializarlo al mismo tiempo
	//para no tener que causar excepciones en la creacion de un objeto
	//mejor utilizar un metodo factory o una funcion deserializadora despues
	~Message();
	string getTxId();
	void setTxId(string);
	string getRetrievalReferenceNumber(void);
	void setRetrievalReferenceNumber(string);
	string getSequenceId(void);
	void setSequenceId(string);
	string getAmount(void);
	void setAmount(string);
	string getTelephone(void);
	void setTelephone(string);
	string getTransmissionDateTime(void);
	void setTransmissionDateTime(string);
	string getSystemsTraceAuditNumber(void);
	void setSystemsTraceAuditNumber(string);
	string getLocalTransactionTime(void);
	void setLocalTransactionTime(string);
	string getLocalTransactionDate(void);
	void setLocalTransactionDate(string);
	string getSettlementDate(void);
	void setSettlementDate(string);
	string getCaptureDate(void);
	void setCaptureDate(string);
	string getNetworkManagementInformationCode(void);
	void setNetworkManagementInformationCode(string);
	string getAuthorisationCode(void);
	void setAuthorisationCode(string);
	string getResponseCode(void);
	void setResponseCode(string);
	string getOriginalDataElements(void);
	void setOriginalDataElements(string);
	string getAcceptor(void);
	void setAcceptor(string);
	string getReceptor(void);
	void setReceptor(string);

	string getType(void) const;
	void setType(string);
	string getQueueTxId();
	void setQueueTxId(string);
	string getTramaIn() const;
	void setTramaIn(string);
	string getTramaOut() const;
	void setTramaOut(string);
	MessageState getState() const;	
	void setState(MessageState);
	string getPassword();
	void setPassword(string);
	void setReceiverName(string);
	string getReceiverName() const;
	void setSenderName(string);
	string getSenderName() const;
	string getLogin();
	void setLogin(string);

	string display();
	string getTramaPartition();

	static void simpleParse(
					 stringstream& a_sstream
					 ,char* a_buffer
					 ,int a_size
					 ,string& a_target
					 ,const char* a_message
					 );
	void findTokenISO(stringstream&);
	void deserialize();
	void serialize();
	void setupPrimaryParsers(stringstream&);//por referencia para cuando se llame
	void setupSecondaryParsers(stringstream&);//a el secondary este recorrido el cursor
	void addParser(int a_id);
	void serializeReject(string);

	static char toHex(char);
	static string toBase256(int);
	static int toBase10(string);
	static string normalize(int,char,string);

	bool isRecover();
	bool isRecover(bool);

  };//class Message

  /*
	typedef Loki::SmartPtr 
	< ecc::Message
	//  ,Loki::RefCountedMT< ecc::Message*, Loki::ObjectLevelLockable>
	,Loki::RefCounted
	,Loki::DisallowConversion
	,Loki::AssertCheck
	,Loki::DefaultSPStorage 
	> Message_sp;
  */
  //typedef Message* Message_sp;
}//namespace ecc

#endif
