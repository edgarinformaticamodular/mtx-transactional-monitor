#ifndef ECC_GENERIC_PARSER_INCLUDE_CPP
#define ECC_GENERIC_PARSER_INCLUDE_CPP

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include "ParseException.hpp"

using namespace ecc;

/**
 * 
 */
template< int t_id, int t_maxFieldSize, int t_headerSize>
GenericParserInclude<t_id,t_maxFieldSize,t_headerSize>
::GenericParserInclude<t_id,t_maxFieldSize,t_headerSize>()
{
}//GenericParserInclude

/**
 * falta definirlo cual deve ser como virtual
 */
template< int t_id, int t_maxFieldSize, int t_headerSize>
GenericParserInclude<t_id,t_maxFieldSize,t_headerSize>
::~GenericParserInclude<t_id,t_maxFieldSize,t_headerSize>()
{
}//~GenericParserInclude

/**
 * parse
 */
template< int t_id, int t_maxFieldSize, int t_headerSize>
void GenericParserInclude<t_id,t_maxFieldSize,t_headerSize>
::parse(stringstream& a_stringstream)
{
	static const int ASCII_MIN = 32;//espacio en SPC
	static const int ASCII_MAX = 126;//antes del DEL
	
	/**
	 * ini especializacion que excluye la validacion de longitud leida por cabecera
	 */	  
	//	  BasicParser<t_id,t_maxFieldSize,t_headerSize>::parse(a_stringstream);
	{
	  char dataBuffer[t_maxFieldSize];//mas uno para el caracter /0
	  char* errorBuffer;
	  int dataSize;
	  //si el tamanno de la cabecera es distinto de 0
	  if( t_headerSize != 0 )
		{
		  //leo la cabecera	  
		  char headerBuffer[t_headerSize+1];//+1 para no errore compilacion cuando 0
		  a_stringstream.read(headerBuffer,t_headerSize);
		  //el stream no dava para leer el header del campo
		  if( a_stringstream.fail() )
			throw ParseException(getId_str(),string("< MSG 134 >: GenericParserInclude::\n\n\tparse:fail: stringstream::fail:Posiblemente bytes insuficientes por trama incompleta (throw in GenericParserInclude ):\n"),a_stringstream.str());
		  //lo copio al campo del objeto
		  d_header.assign(headerBuffer,t_headerSize);
		  headerBuffer[t_headerSize]='\0';//para token fin siguiente comando
		  dataSize = static_cast<int>( strtol(headerBuffer, &errorBuffer, BASE) );
		  //no se lello un numero valido de la cadena de header
		  if( errorBuffer != headerBuffer+t_headerSize )
			throw ParseException(getId_str(),"< MSG 135 >: GenericParserInclude::\n\n\tparse:invalido: caracteres no numericos en especificacion de header dinamico.(throw in GenericParserInclude):\n ",d_header);
		}
	  else 
		// si el tamanno de la cabecera es cero el tamanno de los datos es 
		//el tamanno maximo del campo
		{
		  dataSize = t_maxFieldSize;
		}
	  //la cabecera es mayor que el maximo permisible, note en este caso el header
	  //esta incluido en el maximo permicible haci que ya no le sumoel header size
	  if( t_headerSize < 0 || dataSize + t_headerSize> t_maxFieldSize )
		{
		  string aux1,aux2,aux3;
		  char buff[10];
		  sprintf(buff,"%i",dataSize);
		  aux1.assign(buff);
		  sprintf(buff,"%i",t_headerSize);
		  aux2.assign(buff);
		  sprintf(buff,"%i",t_maxFieldSize);
		  aux3.assign(buff);
		  
		  throw ParseException(getId_str(),string("< MSG 136 >: ParseException\n\n\tinvalido: especificacion dinamica de tamanno de campo:")+aux1+string(", inconsistente con el tamanno maximo permisible:")+aux3+string(". CONSIDERANDO el tamanno del header:(throw in GenericParserInclude::parse:)\n")+aux3+string(":") ,d_header);
		}
	  //el stream para los datos
	  //note que aqui le quito al data Size obtenido, el tamanno de la cabecera
	  a_stringstream.read(dataBuffer,dataSize - t_headerSize);
	  //el stream no dio para leer los datos
	  if( a_stringstream.fail() )
		throw ParseException(getId_str(),"< MSG 137 >: ParseException\n\n\t:fail: stringstream::fail:posiblemente bytes insuficientes por trama incompleta:(throw in GenericParserInclude::parse)",a_stringstream.str());
	  //esto no es relevante:dataBuffer[dataSize] = '\0';
	  //lo copio al campo del objeto
	  //aqui le copio el data size - el tamano del header porque este esta incluido 
	  d_data.assign( dataBuffer, dataSize - t_headerSize );
	}
	/*
	 * fin especializacion
	 **/
	//valido que sean caracteres validos
	string::iterator itr;
	for( itr = d_data.begin(); itr != d_data.end(); itr++ )
	  if( 
		 static_cast<int>(*itr) < ASCII_MIN 
		 || 
		 static_cast<int>(*itr) > ASCII_MAX 
		 )
		throw ParseException(getId_str(),"< MSG 138 >: ParseException:\n\n\tinvalido: caracter especial inesperado:(thro in GenericParserInclude::parse:)\n ",d_data);
}//parse

#endif
