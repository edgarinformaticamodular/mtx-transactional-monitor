#ifndef ECC_BASIC_PARSER_CPP
#define ECC_BASIC_PARSER_CPP

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include "ParseException.hpp"
#include "Log.hpp"
#include <iostream>

using namespace ecc;
/**
 * un parseo basico para todos los parseadores en template
 */

template<int t_id, int t_maxFieldSize, int t_headerSize>
BasicParser<t_id,t_maxFieldSize,t_headerSize>::BasicParser<t_id,t_maxFieldSize,t_headerSize>()
{
}//BasicParser

/**
 * es importante definir bien estos destructores virtuales
 */
template<int t_id, int t_maxFieldSize, int t_headerSize>
BasicParser<t_id,t_maxFieldSize,t_headerSize>
::~BasicParser<t_id,t_maxFieldSize,t_headerSize>()
{
}//~BasicParser

template<int t_id, int t_maxFieldSize, int t_headerSize>
const int BasicParser<t_id,t_maxFieldSize,t_headerSize>::BASE = 10;//esto simboliza la base 10

/**
 *
 */
template<int t_id, int t_maxFieldSize, int t_headerSize>
bool BasicParser<t_id,t_maxFieldSize,t_headerSize>
::isIncludeHeader() const
{
  return t_headerSize != 0;
}//isIncludeHeader

/**
 *
 */
template<int t_id, int t_maxFieldSize, int t_headerSize>
string BasicParser<t_id,t_maxFieldSize,t_headerSize>
::display() const
{
  stringstream sstream;
  sstream << "  BasicParser::display:info:"<<endl;
  sstream << "    t_id          :"<<t_id<<endl;
  sstream << "    t_maxFieldSize:"<<t_maxFieldSize<<endl;
  sstream << "    t_headerSize  :"<<t_headerSize<<endl;
  sstream << "    d_header      :"<<d_header<<endl;
  sstream << "    d_data        :"<<d_data<<endl;
  return sstream.str();
}//display

/**
 *
 */
template<int t_id, int t_maxFieldSize, int t_headerSize>
string BasicParser<t_id,t_maxFieldSize,t_headerSize>
::getId_str() const
{
  char buff[10];
  sprintf(buff,"%03i",t_id);//3 caracteres maximo rellenados con ceros
  string result;
  result.assign(buff,3);//si no le hago haci introduce el caracter nulo en la trama
  return result;
}//idToString

/**
 * parse
 */
template<int t_id, int t_maxFieldSize, int t_headerSize>
void BasicParser<t_id,t_maxFieldSize,t_headerSize>
::parse(stringstream& a_stringstream){
  char dataBuffer[t_maxFieldSize];//mas uno para el caracter /0
  char* errorBuffer;
  int dataSize;
  //si el tamanno de la cabecera es distinto de 0
  if( t_headerSize )
	{
	  //leo la cabecera	  
	  char headerBuffer[t_headerSize+1];//+1 para no errore compilacion cuando 0
	  a_stringstream.read(headerBuffer,t_headerSize);
	  //el stream no dava para leer el header del campo
	  if( a_stringstream.fail() )
		throw ParseException(getId_str(),string("< MSG 096 >: ParseException:\n\n\tPrimaryParser::parse:fail: stringstream::fail.\nthrow in BasicParser::parse\n"),a_stringstream.str());
	  //lo copio al campo del objeto
	  d_header.assign(headerBuffer,t_headerSize);
	  headerBuffer[t_headerSize]='\0';//para token fin siguiente comando
	  dataSize = static_cast<int>( strtol(headerBuffer, &errorBuffer, BASE) );
	  //no se lello un numero valido de la cadena de header
	  if( errorBuffer != headerBuffer+t_headerSize )
		throw ParseException(getId_str(),"< MSG 097 >: ParseException:\n\n\tinvalido: caracteres NO numericos en especificacion de header dinamico:\nthrow in PrimaryParser::parse.",d_header);
	}
  else 
	// si el tamanno de la cabecera es cero el tamanno de los datos es 
	//el tamanno maximo del campo
	{
	  dataSize = t_maxFieldSize;
	}
  //la cabecera es mayor que el maximo permisible
  if( t_headerSize < 0 || dataSize > t_maxFieldSize )
	{
	  string aux1,aux2,aux3;
	  char buff[10];
	  sprintf(buff,"%i",dataSize);
	  aux1.assign(buff);
	  sprintf(buff,"%i",t_headerSize);
	  aux2.assign(buff);
	  sprintf(buff,"%i",t_maxFieldSize);
	  aux3.assign(buff);
	  
	  throw ParseException(getId_str(),string("< MSG 098 >: ParseException:\n\n\tinvalido: especificacion dinamica de tamanno de campo:")+aux1+string(", inconsistente con el tamanno maximo permisible:")+aux3+string(". SIN CONSIDERAR el tamanno del header:")+aux3+string(":\nthrow in PrimaryParser::parse.\n") ,d_header);
	}
  //el stream para los datos
  a_stringstream.read(dataBuffer,dataSize);
  //std::cout << dataSize <<" en BasicParser< MSG 001514 >: "<<ii<< std::endl;
  //el stream no dio para leer los datos
  if( a_stringstream.fail() )
	throw ParseException(getId_str(),"< MSG 099 >: ParseException:\n\n\tposiblemente bytes insuficientes por trama incompleta:\nthrow in PrimaryParser::parse:fail: stringstream::fail.\n",a_stringstream.str());
  //  dataBuffer[dataSize] = '\0';//todo:esto es necesario ?
  //lo copio al campo del objeto
  d_data.assign( dataBuffer, dataSize );
}//parse

#endif
