#ifndef ECC_WT0200_RT0210R_EXCEPTION_CPP
#define ECC_WT0200_RT0210R_EXCEPTION_CPP

#include "WT0200RT0210RException.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"

using ecc::Tx;
using ecc::Message;
using ecc::WT0200RT0210RException;
using ecc::QueryManager;
using ecc::Exception;

WT0200RT0210RException::WT0200RT0210RException()
{
  d_txState = Tx::WT0200;
  d_event = Tx::RT0210R_EXCEPTION;
}



void WT0200RT0210RException::apply(Tx& a_tx)
{ 
  a_tx.setState( Tx::END );
  a_tx.setEvent( Tx::EXCEPTION );

}

#endif
