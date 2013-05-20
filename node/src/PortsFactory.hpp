#ifndef ECC_PORTS_FACTORY_HPP
#define ECC_PORTS_FACTORY_HPP

#include <string>
#include <vector>

using std::string;
using std::vector;

/**
 *
 */
namespace ecc
{
  class Port;

  class PortsFactory
  {
  protected:
	typedef struct 
	{
	  string connectionName;
	  string hostName;
	  string networkRol;
	  string bussinesRol;
	  int portNumber;
	} ContextItem;
	vector< ContextItem > d_context;//contiene parametros para dar de alta los ports definidos en el archivo de configuracion
	void loadContext();
  public:
	PortsFactory();
	~PortsFactory();
	vector<Port*> create();
	void createAutoclients();
  };//class PortFactory
}//namespace ecc

#endif
