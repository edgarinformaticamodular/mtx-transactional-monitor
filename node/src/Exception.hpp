#ifndef ECC_EXCEPTION_HPP
#define ECC_EXCEPTION_HPP

#include <string>
#include <sstream>

using namespace std;

namespace ecc
{

  /**
   *
   */
  class Exception {
  protected:
	stringstream d_sstream;
  public:
	Exception();
	Exception(const ecc::Exception&);
	Exception(string);
	virtual ~Exception();	
	virtual string getMessage() const;
	stringstream& getStream();
	stringstream& operator << (const char*);
	stringstream& operator << (const int);
	stringstream& operator << (const string);
	stringstream& operator << ( ostream& (*) (ostream&) );
  };//class Exception

}//namespace ecc

#endif
