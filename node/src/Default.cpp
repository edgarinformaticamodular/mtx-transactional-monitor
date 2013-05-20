#ifndef ECC_DEFAULT_CPP
#define ECC_DEFAULT_CPP

#include "Default.hpp"
#include <sstream>
#include "Tx.hpp"
#include "Message.hpp"
#include "QueryManager.hpp"
#include "Exception.hpp"

using ecc::Tx;
using ecc::Message;
using ecc::Default;
using ecc::QueryManager;
using ecc::Exception;

Default::Default()
{
  //notese que no defino valores ppara state y event
}

bool Default::isMatch(Tx& a_tx)
{
  return true;
}

void Default::apply(Tx& a_tx)
{
  stringstream sstream;
  sstream
	<< "Tx::evolve_default !!! este estado es inalcanzalbe !!! "
	<< endl
	<< "estado actual: -"<< a_tx.getState() << "-, evento: -"<< a_tx.getEvent() <<"-."<< endl;
  Log::instance().log( sstream.str().c_str() );
  a_tx.setEvent( Tx::UNDEFINED );
  a_tx.setState( Tx::END );
}

#endif
