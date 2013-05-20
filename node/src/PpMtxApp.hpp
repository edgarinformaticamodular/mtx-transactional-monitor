#ifndef ECC_PPMTX_APP_HPP
#define ECC_PPMTX_APP_HPP

#include "Connector.hpp"
#include <vector>
#include "DbQueueMonitor.hpp"
#include <cc++/thread.h>

using ost::Semaphore;
using ost::Thread;
using namespace std;

namespace ecc
{
  /**
   *
   */
  class PpMtxApp 
  {
  private:
	DbQueueMonitor* d_dbQueueMonitor_p;
	static PpMtxApp* d_ppMtxApp_p;
	static ost::Mutex& getMutex_r();
	bool d_isReady;
	Semaphore d_semaphore;
  public:
	void main();
	PpMtxApp();
	~PpMtxApp();
	static PpMtxApp& instance();
	static void create();
	void startup();
	void shutdown();
	void recoverInitial();
	void recoverReversing(string); 
	void whileQuit();
	void startupPortManager();
	void startupEcos();
	void startupDbQueueMonitor();
	void startupPerformers();
	bool isReady();
	bool isReady(bool);
  };//class EccApp
}//namespace ecc

#endif
