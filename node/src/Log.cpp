#ifndef ECC_LOG_CPP
#define ECC_LOG_CPP

#include "Log.hpp"
#include <stdlib.h>
#include <new>
#include "LockingPtr.hpp"
#include <cc++/thread.h> 
#include "Exception.hpp"
#include "IOException.hpp"
#include "Context.hpp"
#include <cc++/numbers.h>
#include <iostream>
using ecc::LockingPtr;
using ecc::Exception;
using ecc::Log;

Log* Log::d_log_p = 0;
bool Log::d_isDestroyed = false;
string Log::d_prefix = "ppmtx";
string Log::d_fileName = "ppmtx.log";
/**
 *
 */
Log& Log::instance()
{
  if(!d_log_p)
	{
	  ost::MutexLock mutexLock(getMutex_r());
	  if(!d_log_p)
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
  return *d_log_p;
}//instance

/**
 *
 */
void Log::loadContext()
{
  try
	{
	  d_prefix = ecc::Context::instance().getProperty_r("log_file_prefix");
	  d_fileName = createDefaultLogFileName(d_prefix);
	}catch(Exception& e){
	  d_fileName = createDefaultLogFileName();
	}
}//

/**
 *
 */
void Log::create()
{
  loadContext();
  static Log log(d_fileName);
  d_log_p = &log;
}//create

/**
 *
 */
void Log::onDeadReference()
{
  create();
  new (d_log_p) Log(d_fileName);
  //atexit(killPhoenix);
  d_isDestroyed = false;
}//onDeadReference

/**
 *
 */
Log::~Log()
{
  d_log_p = 0;
  d_isDestroyed = true;
}//~Log


/**
 *
 */
void Log::killPhoenix()
{
  d_log_p->~Log();
}//killPhoenix

/** 
 * este no hacia falta thread safe, pero d_ofstream es volatile y lo exige asi
 */
Log::Log(string a_fileName)
{
  LockingPtr<ofstream> ofstream_lp(d_ofstream, d_mutex);
  d_separator = "---------------------------------------------";
  ofstream_lp->open( a_fileName.c_str(), ios::app );
  if( ofstream_lp->fail() )
	{
	  ecc::IOException e;
	  e.getStream() << "< MSG 075 >: ecc::IOException::\n\tHubo problemas para abrir el archivo de escritura para log: -"<<a_fileName<<"-.\nthrow in Log::Log\n";
	  throw e;
	}
}//Log

/**
 *
 */
void Log::close()
{
  LockingPtr<ofstream> ofstream_lp(d_ofstream, d_mutex);
  ofstream_lp->close();
  if( ofstream_lp->fail() )
	{
	  ecc::IOException e;
	  e.getStream() << "< MSG 076 >: ecc::IOException::\n\tHubo problemas para cerrar el archivo de escritura : -"<<d_fileName<<"-.\nthrow in Log::close.\n";
	  throw e;
	}
}//close

/** todo: que el log reporte hora y fecha
 *
 */
void Log::log(const char* a_char_p)
{
  ost::Datetime datetime;
  LockingPtr<ofstream> ofstream_lp(d_ofstream, d_mutex);
  (*ofstream_lp) << d_separator << endl << "                             " << datetime.strftime(" %m-%d %H:%M:%S") <<  endl << a_char_p << endl;
}//log

/** todo: que el log reporte hora y fecha
 *
 */
void Log::log0(const char* a_char_p)
{
  LockingPtr<ofstream> ofstream_lp(d_ofstream, d_mutex);
  (*ofstream_lp) << a_char_p;
}//log

/** todo: que el log reporte hora y fecha
 * lo unico diferente es que adiciona un salto de linea
 */
void Log::header()
{
  LockingPtr<ofstream> ofstream_lp(d_ofstream, d_mutex);
  (*ofstream_lp) << d_separator << endl;
}//log

/**
 *
 */
ost::Mutex& Log::getMutex_r()
{
  static ost::Mutex mutex;
  return mutex;
}//getMutex_r

/**
 *
 */
string Log::createDefaultLogFileName(string a_prefix )
{
  ost::Datetime datetime;
  string fileName = getenv("PP_MONITOR_HOME");
  fileName += "/log/";
  fileName += a_prefix;
  fileName += datetime.strftime("_%Y_%m_%d.log");
  return fileName;
}//createDefaultLogFileName

#endif
