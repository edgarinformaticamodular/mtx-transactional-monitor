#ifndef ECC_WT0420_TIME_OUT_EXCEPTION_CPP
#define ECC_WT0420_TIME_OUT_EXCEPTION_CPP

#include "WT0420TimeoutException.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"

using ecc::Tx;
using ecc::Message;
using ecc::WT0420TimeoutException;
using ecc::QueryManager;
using ecc::Exception;

WT0420TimeoutException::WT0420TimeoutException()
{
  d_txState = Tx::WT0420;
  d_event = Tx::TIME_OUT_EXCEPTION;
}


bool WT0420TimeoutException::isMatch(Tx& a_tx)
{
  return (a_tx.getState() == Tx::WT0421 || a_tx.getState() == Tx::WT0420) && (a_tx.getEvent() == d_event);
}


void WT0420TimeoutException::apply(Tx& a_tx)
{
  a_tx.timedWait();
  a_tx.setState( Tx::WT0421 );
  a_tx.setEvent( Tx::TIME_OUT );
}

#endif
