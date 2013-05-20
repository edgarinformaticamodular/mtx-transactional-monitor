#ifndef ECC_WT0421_TIME_OUT_EXCEPTION_CPP
#define ECC_WT0421_TIME_OUT_EXCEPTION_CPP

#include "WT0421TimeoutException.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"

using ecc::Tx;
using ecc::Message;
using ecc::WT0421TimeoutException;
using ecc::QueryManager;
using ecc::Exception;

WT0421TimeoutException::WT0421TimeoutException()
{
  d_txState = Tx::WT0421;
  d_event = Tx::TIME_OUT_EXCEPTION;
}



void WT0421TimeoutException::apply(Tx& a_tx)
{
  a_tx.timedWait();
  a_tx.setState( Tx::WT0421 );
  a_tx.setEvent( Tx::TIME_OUT );
}

#endif
