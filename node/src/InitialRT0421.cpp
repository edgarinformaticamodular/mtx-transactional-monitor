#ifndef ECC_INITIAL_RT0421_CPP
#define ECC_INITIAL_RT0421_CPP

#include "InitialRT0421.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"

using ecc::Tx;
using ecc::Message;
using ecc::InitialRT0421;
using ecc::QueryManager;
using ecc::Exception;

InitialRT0421::InitialRT0421()
{
  d_txState = Tx::INITIAL;
  d_event = Tx::RT0421;
}

void InitialRT0421::apply(Tx& a_tx)
{
  a_tx.timedWait();//parra quitar el primer counter del semaforo
  d_txState = Tx::INITIAL;
  d_event = Tx::RT0420;
}

#endif
