#ifndef ECC_PERFORMER_CPP
#define ECC_PERFORMER_CPP
#define x(a) cout << a << endl;

template<char t_char>
Performer<t_char>* Performer<t_char>::d_performer_p = 0;

template<char t_char>
bool Performer<t_char>::d_isDestroyed = false;
/**
 *
 */
template<char t_char>
Performer<t_char>& Performer<t_char>::instance()
{
  if(!d_performer_p)
	{
	  ost::MutexLock mutexLock( getMutex_r() );
	  if(!d_performer_p)
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
  return *d_performer_p;
}//instance

/**
 *
 */
template<char t_char>
void Performer<t_char>::create()
{
  static Performer<t_char> performer;
  d_performer_p = &performer;
}//create

/**
 *
 */
template<>
void PerformerReceptor::create()
{
  static PerformerReceptor performer( SyncQueueReceptor::instance() );
  d_performer_p = &performer;
}//create

/**
 *
 */
template<>
void PerformerAcceptor::create()
{
  static PerformerAcceptor performer( SyncQueueAcceptor::instance() );
  d_performer_p = &performer;
}//create

/**
 *
 */
template<char t_char>
void Performer<t_char>::onDeadReference()
{
  create();
  new (d_performer_p) Performer<t_char>();
  //atexit(killPhoenix);
  d_isDestroyed = false;
}//onDeadReference


/**
 *
 */
template<>
void PerformerReceptor::onDeadReference()
{
  create();
  new (d_performer_p) PerformerReceptor( SyncQueueReceptor::instance() );
  //atexit(killPhoenix);
  d_isDestroyed = false;
}//onDeadReference


/**
 *
 */
template<>
void PerformerAcceptor::onDeadReference()
{
  create();
  new (d_performer_p) PerformerAcceptor( SyncQueueAcceptor::instance() );
  //atexit(killPhoenix);
  d_isDestroyed = false;
}//onDeadReference

/**
 *
 */
template<char t_char>
Performer<t_char>::~Performer()
{
  d_isDestroyed = true;
  terminate();
}//~Performer

/** 
 * 
 */
template<char t_char>
Performer<t_char>::Performer(SyncQueue<t_char>& a_syncQueue)
  :Thread(1)
  ,d_syncQueue( a_syncQueue )
{
}//Performer

/**
 *
 */
template<char t_char>
bool Performer<t_char>::isQuit(bool a_isQuit)
{
  bool isQuitOld = d_isQuit;
  d_isQuit = a_isQuit;//damos de alta lla bandera
  d_syncQueue.getSemaphore_r().post();//para que se libere el ciclo
  return isQuitOld;
}//

/** 
 * 
 */
template<char t_char>
void Performer<t_char>::final()
{
  /// this->~Performer<t_char>();//todo:va delete
}//

/**
 *
 */
template<char t_char>
void Performer<t_char>::onMessage(Message* a_message_p)
{
  try
	{
	  //acabamos de rellenar los datos de el mensaje
	  a_message_p->deserialize();
	  string receiverName = a_message_p->getReceiverName();
	  string type = a_message_p->getType();
	  //string trama = a_message_p->getTramaIn();

	  //podemos haver recibido un mensaje de transaccion o de otro tipo
	  //si es una transaccion:
	  if( 
		 type == Message::H0200
		 || type == Message::H0420
		 || type == Message::H0210
		 || type == Message::H0430
		 || type == Message::H0420
		 )
		{
		  //veo si se estaba siendo procesada y si ya estaba siendo procesada adiciono el mensaje a la transaccion correspondiente para que continue evolucionando la transaccion que ya existia, y ya acabe, solo tengo que informar que hice en el log, y esto todo.
		  string txId = a_message_p->getTxId();
		  bool isExist = SyncMap::instance().isExist(txId,a_message_p);
		  stringstream sstream;
		  sstream
			<< "< MSG 001567 >: Performer::onMessage:\n\n"
			<< "\tTx: -"
			<< txId 
			<<"-, nueva?: -"
			<< (isExist?"false":"true")
			<<"-, from: -"
			<<receiverName
			<< "-."
			<< endl
			<< a_message_p->getTramaPartition()
			<< endl;
		  Log::instance().log( sstream.str().c_str() );
		  
		  if( isExist )
			return;//ya adicione el mensaje cuando cheque si existia la transaccion

		  //si no existia entronces creo una nueva y la adiciono a el mapeo de ids de transacciones y finalmente la echo a andar.
		  Tx_sp tx_sp = Tx_sp( new Tx() );
		  tx_sp->setId( txId );
		  tx_sp->addIn( a_message_p );
		  SyncMap::instance().put(txId, tx_sp);
		  tx_sp->detach();
		  return;
		}
	  //si no es una transaccion hay varios casos
	  try
		{//si es un mensaje de logon o logoff
		  if( type == Message::H0800 )
			{
			  stringstream sstream;
			  sstream
				<< "< MSG 001568 >: Performer::onMessage:\n\n"
				<< "                    from: -"
				<< receiverName
				<< "-"
				<< endl
				<< a_message_p->getTramaPartition()
				<< endl;
			  Log::instance().log( sstream.str().c_str() );

			  string informationCodeReceived 
				= a_message_p->getNetworkManagementInformationCode();
			  string informationCodeToSend
				= Message::ECHO_TEST_NETWORK_MANAGEMENT_INFORMATION_CODE;
			  string responseCodeToSend
				= Message::RESPONSE_CODE_APPROVED;
			  
			  if( 
				 informationCodeReceived
				 == Message::ECHO_TEST_NETWORK_MANAGEMENT_INFORMATION_CODE 
				 )
				{
				  ;//no se modifican los valores con los que se inicializaron llas variables
				}
			  else if( 
					  informationCodeReceived
					  == Message::LOGON_NETWORK_MANAGEMENT_INFORMATION_CODE 
					  )
				{
				  informationCodeToSend
					= Message::LOGON_NETWORK_MANAGEMENT_INFORMATION_CODE;
				  if( PpMtxApp::instance().isReady() )
					{
					  responseCodeToSend = Message::RESPONSE_CODE_APPROVED;
					}
				  else
					{
					  responseCodeToSend = Message::RESPONSE_CODE_DENIED;
					}
				}
			  else if( 
					  informationCodeReceived
					  == Message::LOGOFF_NETWORK_MANAGEMENT_INFORMATION_CODE 
					  )
				{
				  informationCodeToSend
					= Message::LOGOFF_NETWORK_MANAGEMENT_INFORMATION_CODE;
				  responseCodeToSend = Message::RESPONSE_CODE_APPROVED;
				}
			  //note que si no fue ningun caso se toma como si fuera un echo por los valores con los que se inicializaron las variables
			  Emisor::instance().w0810(
									   a_message_p->getReceiverName()
									   ,informationCodeToSend
									   ,responseCodeToSend
									   );
			  return;
			}//si es una respuesta a un mensaje de echo no hay nada que procesar con que qquede registrado en el log es suficiente y eso yaa lo hicimos arriba
		  if( type == Message::H0810 )
			{
			  stringstream sstream;
			  sstream
				<< "< MSG 001569 >: Performer::onMessage:\n\n"
				<< "                    from: -"
				<< receiverName
				<< "-"
				<< endl
				<< a_message_p->getTramaPartition()
				<< endl;
			  Log::instance().log( sstream.str().c_str() );

			  string informationCodeReceived 
				= a_message_p->getNetworkManagementInformationCode();
			  string responseCodeReceived = a_message_p->getResponseCode();
			  if( 
				 informationCodeReceived
				 == Message::ECHO_TEST_NETWORK_MANAGEMENT_INFORMATION_CODE 
				 )
				{
				  return;//simplemente llego un eco, con verlo en la bitacora es suficiente y eso ya lo hicimos arriba
				}
			  else if( 
					  informationCodeReceived
					  == Message::LOGON_NETWORK_MANAGEMENT_INFORMATION_CODE 
					  )
				{
				  if( 
					 PpMtxApp::instance().isReady() 
					 && ( responseCodeReceived == Message::RESPONSE_CODE_APPROVED )
					 )
					{//nos dicen que si aceptan el logon
					  PortManager::instance().isLogon(
													  a_message_p->getReceiverName()
													  ,true
													  );
					  //recover los que estan pendientes para reversing en este receptor
					  PpMtxApp::instance().recoverReversing( a_message_p->getReceiverName() );
					}
				  else
					{//nos dicen que no aceptan el logon
					  PortManager::instance().isLogon(
													  a_message_p->getReceiverName()
													  ,false
													  );
					}
				  return;
				}
			  else if( 
					  informationCodeReceived
					  == Message::LOGOFF_NETWORK_MANAGEMENT_INFORMATION_CODE 
					  )
				{
				  PortManager::instance().isLogon(
												  a_message_p->getReceiverName()
												  ,false
												  );
				}
			  delete a_message_p;//ya no lo necesitamos todo:checar
			  return;
			}//si es un mensaje reyectado, esta mal formado no tiene logica procesarlo, solo informarlo en bitacora 
		  else if( 
				  type == Message::H9200 
				  || type == Message::H9420 
				  || type == Message::H9421 
				  || type == Message::H9430  
				  || type == Message::H9800 
				  || type == Message::H9810
				  )
			{
			  stringstream sstream;
			  sstream
				<< "< MSG 001570 >: Performer::onMessage:\n\n\t"
				<< "                    from: -"
				<< receiverName
				<< "-, nuestro mensaje fue reyectado! "
				<< endl
				<< a_message_p->getTramaPartition()
				<< endl;
			  Log::instance().log( sstream.str().c_str() );

			  delete a_message_p;//a este no le respondemos (porque no existe! osea fue mal formado)
			  return;
			}
		  //si ya llegamos hasta aqui es que de plano el mensaje no es de ningun tipo conociddo por nosotros parra procesar y lo rejectamos indicando que es por seguridad
		  //aqui de plano ya es un mensaje no conocido
		  //aqui el log se maneja en la funcion, esta funcion se utiliza en varias exceptiones
		  reject( 
				 a_message_p
				 ,Message::STATUS_FIELD_SECURITY_MESSAGE_UNEXPECTED  
				 );
		  delete a_message_p;//si no se fue adentro de una transaccion aqui termina

		}catch(Exception& e){//separo esta falla de las demas, porque las otras hay que rejectarlas
		  e
			<< "< MSG 005337 >: Exception: Performer<T>::onMessage:\n\n\tfalla procesando el mensaje: -"
			<< a_message_p->getTramaIn().c_str()
			<< "-. ya no se procesara mas.\n"
			<< "catch in Performer<T>::onMessage()\n"
			;
		  Log::instance().log(e.getMessage().c_str());
		}catch(...){//separo esta falla de las demas, porque las otras hay que rejectarlas
		  stringstream sstream;
		  sstream
			<< "< MSG 001097 >: ???exception: Performer<T>::onMessage:\n\n\tfalla procesando el mensaje: -"
			<< a_message_p->getTramaIn() 
			<< endl
			<< "-. ya no se procesara mas.\n"
			<< "catch in Performer<T>::onMessage()\n"
			;
		  Log::instance().log(sstream.str().c_str());
		}
	}catch(ecc::ParseException& e){//todo falta implementar el regreso del mensaje no valido
	  e
		<< "\n1. catch in Performer::onMessage.\n";
	  Log::instance().log( (e.getMessage()+"\n\n"+a_message_p->display()+"\n\n"+a_message_p->getTramaIn()).c_str() );
	  reject( a_message_p, e.getField() );
	}catch(ecc::Exception& e){
	  e
		<< "\n2. catch in Performer::onMessage.\n";
	  Log::instance().log( e.getMessage().c_str() );
	  reject(a_message_p, Message::STATUS_FIELD_SECURITY_MESSAGE_UNEXPECTED );
	}catch(std::exception& e){
	  stringstream sstream;
	  sstream
		<< e.what()
		<< endl
		<< "3. catch in Performer::onMessage. (std::exception)";
	  Log::instance().log( sstream.str().c_str() );
	  reject(a_message_p, Message::STATUS_FIELD_SECURITY_MESSAGE_UNEXPECTED );
	}catch(...){
	  Log::instance().log("???::exception::\n\tOcurrio una excepcion inesperada. Se reversa operacion./ncatch in Performer::onMessage.");
	  reject(a_message_p, Message::STATUS_FIELD_SECURITY_MESSAGE_UNEXPECTED );
	}//catch
}//onMessage


/**
 *
 */
template<char t_char>
void Performer<t_char>::run()
{
  Message* message_p;
  while(true){
	try
	  {
		d_syncQueue.getSemaphore_r().wait();
		if( d_isQuit )
		  break;//salgo
		message_p = d_syncQueue.pop();
		onMessage( message_p );
	  }catch(...){
		Log::instance().log( "< MSG 001221 >: ???::exception:\n\tcatch in Performer::run.");
	  }
  }//while
}//run

/**
 *
 */
template<char t_char>
ost::Mutex& Performer<t_char>::getMutex_r()
{
  static ost::Mutex mutex;
  return mutex;
}//getMutex_r

/**
 *
 */
template<char t_char>
void Performer<t_char>::reject(Message* a_message_p, string a_status)
{
  a_message_p
	->serializeReject( a_status );

  stringstream sstream;
  sstream
	<< "< MSG 001099 >: Performer<T>::reject\n\n\tRejectado el mensaje: -"
	<< endl
	<< a_message_p->getTramaOut()
	<< "-. \n en ecc::Performer::reject.";
  Log::instance().log( sstream.str().c_str() );

  try
	{//si es web, solo lo reporto en la base de datos, no hay nada mas que hacer
	  if( 
		 a_message_p->getReceiverName().substr(0,4) 
		 == PortManager::webPortNamePrefix 
		 )
		{		   
		  string queueTxId = a_message_p->getQueueTxId();
		  {
			QueryManager queryManager;	  
			queryManager.begin();
			queryManager.updateTx(
								  queueTxId
								  ,ecc::pp::txs::state::ABORT
								  ,ecc::pp::txs::evolve::PASIVE
								  ,"--"
								  ,"------"
								  );
			queryManager.isCommit(true);
		  }
		}
	  else
		{
		  PortManager::instance().send( 
									   a_message_p->getReceiverName()
									   ,a_message_p->getTramaOut()
									   );//que se valla por donde vino !
		}
	}catch(Exception& e){
	  stringstream sstream;
	  sstream
		<< e.getMessage()
		<< endl
		<< "catch in Performer::reject";
	  Log::instance().log( sstream.str().c_str() );
	}catch(...){
	  Log::instance().log("< MSG 001098 >: ???exception: Performer<T>::reject.\n\n\tinesperada.");
	}
}//

#endif
