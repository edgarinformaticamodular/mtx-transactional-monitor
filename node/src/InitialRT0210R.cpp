#ifndef ECC_INITIAL_RT0210R_CPP
#define ECC_INITIAL_RT0210R_CPP

#include "InitialRT0210R.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"

using ecc::Tx;
using ecc::Message;
using ecc::InitialRT0210R;
using ecc::QueryManager;
using ecc::Exception;

InitialRT0210R::InitialRT0210R()
{
  d_txState = Tx::INITIAL;
  d_event = Tx::RT0210R;
}

void InitialRT0210R::apply(Tx& a_tx)
{
  a_tx.timedWait();//parra quitar el primer counter del semaforo
  a_tx.setState(Tx::END);
  a_tx.setEvent(Tx::DONE);
}

#endif
