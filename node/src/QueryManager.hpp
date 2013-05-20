#ifndef ECC_QUERY_MANAGER_HPP
#define ECC_QUERY_MANAGER_HPP

#include "DbPool.hpp"
#include <vector>
#include "Message.hpp"
#include "Log.hpp"

using ecc::Message;

namespace ecc {

  namespace pp {
	namespace txs {
	  namespace state {
		typedef enum
		{
		  INITIAL = 0
		  ,COMMIT = 1
		  ,ABORT = 2
		  ,REVERSING = 3
		  ,WT0200 = 4
		} StateType; 
	  }
	  namespace evolve {
		typedef enum {
		  ACTIVE = 0
		  ,PASIVE = 1
		} EvolveType;
	  }
	}
	namespace queue {
	  namespace action {
		typedef enum {
		  BEGIN = 0
		  ,ABORT = 1
		} ActionType;
	  }
	}
  }

  class QueryManager {

  public:
	typedef enum 
	{
	  INVALID_LOGIN = 0,//no hay un usuario con tal login
	  INVALID_PASSWORD,//existe el usuario pero el password no corresponde
	  NO_CREDIT,// no tiene credito disponible para realizar el abono solicitado
	  ACCEPTED, //para indicar que todo fue aceptado y que se cargo el abono correspondiente
	  NO_ACCEPTED //ocurrio una excepcion y finalmente no fue aceptado
	} AcceptState;
	static int MAX_THREADS;
  public:
	QueryManager();
	~QueryManager();
	static void sqlSafe(string&);
	static string toResponseCode(AcceptState);
	AcceptState getAcceptState(
							   string
							   ,string
							   ,string
							   ,string
							   );
	void addTx(
			   string a_txId
			   ,string a_login
			   ,string a_amount
			   ,string a_telephone
			   ,string a_audit
			   ,string a_acceptor
			   ,string a_receptor
			   ,string a_transmissionDateTime
			   );
	void updateTx(
				  string
				  ,int
				  ,int
				  ,string
				  ,string
				  );
	void updateTx(
				  string
				  ,int
				  ,int
				  ); 
	AcceptState delayReverseTx(
						string
						,int
						,int
						,string
						,string
						,string
						,string
						);
	AcceptState delayReverseTx(
						string
						,int
						,int
						,string
						,string
						);
	void plusCredit(
					string
					,string
					);  

	void loadMessages( vector<Message*>& );
	void loadForRecovery( vector<Message*>& );
	void abortUnfinishedTx(void);
	void loadForReverse( string, vector<Message*>&);

	void begin();
	void commit();
	void rollback();
	bool isCommit(bool);

	static const char* acceptStateToString(QueryManager::AcceptState);
	string createAudit();
	string createDate();
	string createTime();
	string createOriginalDataElements(string);
  private:
	::db::Connection_sp d_connection_sp;
	bool d_isCommit;
  };//QueryManager

}//namespace ecc

#endif
