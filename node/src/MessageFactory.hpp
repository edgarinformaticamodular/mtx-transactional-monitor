#ifndef ECC_MESSAGE_FACTORY_HPP
#define ECC_MESSAGE_FACTORY_HPP

#include "QueryManager.hpp"
#include "Message.hpp"

namespace ecc
{
  class MessageFactory
  {
  protected:
	static map<string,string> d_tramasBase;
	string d_cacheDate;
	string d_cacheTime;
	string d_cacheAudit;
	string d_cacheOriginalDataElements;
	bool d_isCacheAuditAvailable;
  public:
	static Message* create(string a_senderName, string a_messageType);
	static void loadContext();
	MessageFactory();
	~MessageFactory();
	string getCacheDate();
	void setCacheDate(const string& a_cacheDate){d_cacheDate = a_cacheDate;}
	string getCacheTime();
	void setCacheTime(const string& a_cacheTime){d_cacheDate = a_cacheTime;}
	string& getCacheOriginalDataElements() { return d_cacheOriginalDataElements;}
	void setCacheOriginalDataElements(string a_cacheOriginalDataElements){ d_cacheOriginalDataElements = a_cacheOriginalDataElements; }
	string& getCacheAudit();
	void setCacheAudit(const string& a_cacheAudit){d_isCacheAuditAvailable = true; d_cacheAudit = a_cacheAudit;}
  };//class MessageFactory
  
}//namespace ecc

#endif
