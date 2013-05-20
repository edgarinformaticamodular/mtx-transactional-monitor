#ifndef ECC_TX_HPP
#define ECC_TX_HPP

#include <cc++/thread.h>
#include <string>
#include <vector>
#include "Message.hpp"
#include "SyncVector.hpp"
#include "QueryManager.hpp"
#include "MessageFactory.hpp"

using namespace ost;
using namespace std;

namespace ecc
{

  class Tx : public Thread
  {
  public:
	static int MAX_TRY;
	static int WAIT_TIME; //en milisegundos
	static int WAIT_TIME_FIRST;//en milisegundos
	static int MAX_VARCHAR_ESTADO;
	static int MAX_VARCHAR_TRAMA;
	static int MAX_ITERATIONS;
	int d_try;//cantidad de intentos
	bool d_isTryAgain;
	//todo: hacer que el acceso a los vectores in y out sea sincronizado
	SyncVector d_messagesIn;
	SyncVector d_messagesOut;
	SyncVector d_messagesPurge;
	QueryManager::AcceptState d_acceptState;
	int d_messageIn;
	string d_id;
	int d_iterations;
	typedef enum {//todo los estados R9??? nunca los vamos a tener, hay que quitarlos
	  INITIAL
	  ,TESTED
	  ,WT0200
	  ,WT0420
	  ,WT0421
	  ,WC0210A
	  ,WC0210R
	  ,WC0210R_WT0420
	  ,FLUSH
	  ,END
	} TxState;
	TxState d_state;
	typedef enum {
	  UNDEFINED
	  ,EXCEPTION
	  ,TIME_OUT
	  ,TIME_OUT_EXCEPTION
	  ,DONE
	  ,DONE_EXCEPTION
	  ,RC0200
	  ,RC0200_EXCEPTION
	  ,RC0420
	  ,RT0210A
	  ,RT0210A_EXCEPTION
	  ,RT0210R
	  ,RT0210R_EXCEPTION
	  ,RT0420
	  ,RT0420_EXCEPTION
	  ,RT0421
	  ,RT0430
	  ,RT9200
	  ,RC0800
	  ,RC0810
	  ,RT0800
	  ,RT0810
	  ,ITERATION_LIMIT_REACHED
	  ,TRY_LIMIT_REACHED
	} Event;
	typedef enum {
	  TRAMA_IN
	  ,TRAMA_OUT
	  ,TRAMA_PARTITION
	} TramaType;
 	Event d_event;
 	Event d_eventOld;
  public:
	Tx();
	~Tx();
	void timedWait();
	bool addIn(Message*);
	void addOut(Message*);
	void evolve();
	string getId();
	void setId(string);
	void display();
	void setState(TxState);
	void setEvent(Event);
	Event getEvent();
	QueryManager::AcceptState getAcceptState();
	void setAcceptState(QueryManager::AcceptState);
	void run();
	string getState_str(void);
	TxState getState(void);
	Message* getCurrentIn();
	Message* getFirstIn();
	static void loadContext();
	void logState();
	void logAction(string, Message*, TramaType a_tramaType = TRAMA_PARTITION);
  public:
	//nuevas
	void preprocess();
	void process();
	void postprocess();
	void setupEvent();
	void final();
	void evolve_INITIAL();
	void evolve_TESTED();
	void evolve_WT0200();
	void evolve_WT0420();
	void evolve_WT0421();
	void evolve_default();
	void evolve_WT0200_TIME_OUT();
	void evolve_WT0200_RT0210A();
	void evolve_WT0200_RT0210AEXCEPTION();
	void evolve_WT0200_RT0210R();
	void evolve_WT0200_RT0210REXCEPTION();
	void evolve_WT0200_default();
	void evolve_WT0420_TIME_OUT();
	void evolve_WT0420_TIME_OUTEXCEPTION();
	void evolve_WT0420_RT0430();
	void evolve_WT0420_default();
	void evolve_WT0421_TIME_OUT();
	void evolve_WT0421_TIME_OUTEXCEPTION();
	void evolve_WT0421_RT0430();
	void evolve_WT0421_default();
	void evolve_FLUSH();
	void evolve_INITIAL_RC0200();
	void evolve_INITIAL_RC0200EXCEPTION();
	void evolve_INITIAL_RT0420();
	void evolve_INITIAL_RC0420();
	void evolve_INITIAL_RT0421();
	void evolve_INITIAL_RT0210A();
	void evolve_INITIAL_RT0210R();
	void evolve_INITIAL_default();
	void evolve_TESTED_DONE();
	void evolve_TESTED_DONEEXCEPTION();
	void incrementTry();
	int getTry();
	bool isTryAgain(bool);
	void setCursor(int);
	int getCursor();
	MessageFactory& messageFactory(){return d_messageFactory;}
  protected:
	int d_cursor;
	static const char* txStateToString(TxState);
	static const char* eventToString(Event);
	MessageFactory d_messageFactory;
  };//class Tx

  typedef Tx* Tx_sp;

}//namespace ecc

#endif
