#ifndef ECC_CONTEXT_CPP
#define ECC_CONTEXT_CPP

#include <cc++/thread.h> 

#include <new>
#include <map>

#include <stdlib.h>

#include "Context.hpp"
#include "Exception.hpp"
#include "NotFoundException.hpp"
#include "IOException.hpp"

using ost::Thread;

using std::map;

using ecc::Context;
using ecc::Exception;
using ecc::NotFoundException;
using ecc::IOException;

Context* Context::d_context_p = 0;
bool Context::d_isDestroyed = false;
const char* Context::DEFAULT_CONFIG_FILE = "ppmtx.config";

/**
 *
 */
Context& Context::instance()
{
  if(!d_context_p)
	{
	  ost::MutexLock mutexLock( getMutex_r() );
	  if(!d_context_p)
		{
		  if(d_isDestroyed)
			{
			  onDeadReference();
			}
		  else
			{
			  create();
			}
		}
	}
  return *d_context_p;
}//

/**
 *
 */
void Context::create()
{
  static Context context;
  d_context_p = &context;
}//

/**
 *
 */
void Context::onDeadReference()
{
  create();
  new (d_context_p) Context;
  d_isDestroyed = false;
}//

/**
 *
 */
Context::~Context()
{
  d_context_p = 0;
  d_isDestroyed = true;
}//

/** 
 * este no hacia falta thread safe, pero d_ofstream es volatile y lo exige asi
 */
Context::Context()
{
  string configFileName = ::getenv("PPMTX_CONFIG_FILE");
  if( configFileName == "" )
	{
	  configFileName = DEFAULT_CONFIG_FILE;
	}
  ifstream f_ifstream;
  f_ifstream.open(configFileName.c_str());
  if( f_ifstream.fail() )
	{
	  ecc::IOException e;
	  e 
		<< "< MSG 063 >: ecc::IOException::\n\n\tHubo problemas para abrir el archivo de configuracion: -"
		<< configFileName
		<< "-.\nthrow in Context::Context\n"
		;
	  throw e;
	}

  static string lineNull = "";
  string line;
  while(! f_ifstream.eof() )
	{
	  getline( f_ifstream, line );
	  if( f_ifstream.bad() )
		{
		  ecc::IOException e;
		  e
			<< "< MSG 064 >: ecc::IOException::\n\n\tProblemas en la lectura del archivo de configuración: -"
			<< configFileName
			<< "-.\nthrow in Context::Context."
			<< endl
			;
		  throw e;
		}//if
	  string::size_type position = line.find_first_of('=',0);
	  if( 
		 line != lineNull // la linea no es vacia
		 && position != string::npos //si existe el signo igual
		 && position != line.size()-1 //el signo igual no esta al final
		 && position != 0 //el signo igual no esta al inicio
		 ){
		string identifyer = line.substr(0,position);
		string value = line.substr(position+1,line.size()+1-(position+1));
		d_properties[identifyer] = value;
	  }//if
	}//while
}//Context

/**
 *
 */
string& Context::getProperty_r(string a_identifyer)
{
  Map::iterator itr = d_properties.find(a_identifyer);
  if( itr == d_properties.end() )
	{
	  ecc::NotFoundException e;
	  e 
		<< "< MSG 065 >: Context::getProperty: NotFoundException::\n\n\tNo se encontro el elemento de nombre -"
		<< a_identifyer 
		<< "-.\n"
		<< "throw in Context::getProperty_r"
		;
	  throw e; 
	}else
	  return itr->second;
}//getProperty

/**
 *
 */
ost::Mutex& Context::getMutex_r()
{
  static ost::Mutex mutex; // para evitar race conditions in constructor
  return mutex;
}//getMutexDoubleChecked

#endif
