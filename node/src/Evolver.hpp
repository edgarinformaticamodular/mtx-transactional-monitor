#ifndef ECC_EVOLVER_HPP
#define ECC_EVOLVER_HPP

#include "Tx.hpp"
#include "Rule.hpp"
#include "Default.hpp"

namespace ecc
{
  class Evolver
  {
  public:
	static Evolver& instance();
	void evolve(Tx&);
  private:
	Evolver();
	virtual ~Evolver();
	static void create();
	static void onDeadReference();
	static ost::Mutex& getSingletonMutex_r();
	void loadContext();
  private:
	static Evolver* d_evolver_p;
	static bool d_isDestroyed;
	vector<Rule*> d_rules;
	Default d_default;
  };//class
  
}//namespace ecc

#endif
