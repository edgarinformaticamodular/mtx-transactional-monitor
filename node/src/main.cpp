#include <iostream>
#include <exception>
#include <typeinfo>
#include "Log.hpp"
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cc++/thread.h>
#include "Asserter.hpp"
#include "PpMtxApp.hpp"
#include "Exception.hpp"
#include "IOException.hpp"
#include "Context.hpp"

#define x(a) cout << a << endl;

using ecc::PpMtxApp;
using ecc::Exception;
using ecc::IOException;
using ecc::Log;
using ecc::Context;

using std::cout;
using std::string;

using ost::Semaphore;

Semaphore g_semaphore;

int exitCode = 0;
/**
 *
 */
void makePidFile()
{
  pid_t oldPid;
  pid_t pid = getpid();
  string base = getenv("PP_MONITOR_HOME");
  string path = base + "/dat/ppmtx.pid";
  //obtengo el actual pidof
  ifstream f_ifstream(path.c_str());
  if( f_ifstream >> oldPid )
	if( oldPid != pid )
	  {
		cout << "ppmtx::warning !!! Cheque que no se esta corriendo otro ppmtx simultaneamente..." << endl;
		cout << "con numero de proceso "<< oldPid << endl;
		cout << "o borre el fichero "<< path << endl;
		cout << "No se pudo arrancar el servidor."<< endl;
		f_ifstream.close();
		::exit(1);
	  }

  ofstream f_ofstream(path.c_str());
  f_ofstream << pid;
  f_ofstream.close();
}//

/**
 *
 */
void sigusr1(int a_signal)
{
  PpMtxApp::instance().isReady(false);
  Log::instance().log("main: info:\n\trecibida sennal SIGUSR1, Terminado proceso principal controlado...");
  g_semaphore.post();
  exitCode = a_signal;
}//

/**
 *
 */
void sigsegv(int a_signal)
{
  Log::instance().log("main: info: < MSG 005230 >: KILL SIGNAL (9) recibida.\nDando lugar a secuencia de shutdown.");
  ::exit(a_signal);
}//

/**
 *
 */
int main(int argc, char* argv[])
{
  signal(SIGUSR1,sigusr1);
  signal(SIGSEGV,sigsegv);
  try
	{
	  makePidFile();
	  Log::instance().log( "   *****************************************************************" );
	  Log::instance().log("main: info:\n\tIniciando proceso principal...");
	  PpMtxApp::instance().main();
	  //  g_semaphore.wait();//espera a desbloquearse esto lo maneja en el main.cpp mediante el SIGUSR1 handler
	  //PpMtxApp::instance().shutdown();

	  Log::instance().log("main: info:\n\tCompletada secuencia de shutdown.");
	  //if( Context::instance().getProperty_r("mode") == "interactive" )
	  //PpMtxApp::instance().whileQuit();
	  //	  cout << 6 << endl;
	  return exitCode;
	}catch(ecc::Exception& e){
	  Log::instance().log( e.getMessage().c_str() );
	  cout << e.getMessage() << endl << "< MSG 543 >: catch in main" << endl;
	}catch(std::exception& e){
	  string message;
	 message += "main: std::exception: ";
	 message += typeid(e).name();
	 message += e.what();
	 Log::instance().log( message.c_str() );
	 cout << e.what() << endl << "< MSG 546 >: catch in main" << endl; 
	}catch(...){
	  Log::instance().log( "main: excepcion no conocida" );
	  cout << "< MSG 547 >: catch in main" << endl;
	}//catch
}//main
