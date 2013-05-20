#ifndef ECC_EXCEPTION_CPP
#define ECC_EXCEPTION_CPP

#include "Exception.hpp"
#include <sstream>

using namespace ecc;

Exception::Exception()
{
}

Exception::Exception(const ecc::Exception& a_exception)
{
  d_sstream << a_exception.getMessage();
}

Exception::Exception(string a_message)
{
  d_sstream << a_message;
}

Exception::~Exception()
{}

string Exception::getMessage() const
{
  return d_sstream.str();
}

stringstream& Exception::getStream()
{
  d_sstream << endl;
  return d_sstream;
}

stringstream& Exception::operator << (const char* a_char_p)
{
  d_sstream << a_char_p;
  return d_sstream;
}

stringstream& Exception::operator << (const int a_int)
{
  d_sstream << a_int;
  return d_sstream;
}

stringstream& Exception::operator << (const string a_string)
{
  d_sstream << a_string;
  return d_sstream;
}
stringstream& Exception::operator << ( ostream& (*a_fun) (ostream&) )
{
  (*a_fun)( d_sstream );
  return d_sstream;
}

#endif
