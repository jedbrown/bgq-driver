/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2007, 2011                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#ifndef ALNTEST_H
#define ALNTEST_H

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <assert.h>

using namespace std;

static void check64 ( const std::string& msgprefix, uint64_t val1, uint64_t val2 )
{
   if( val1 != val2 )
      cout << "ERROR ";
	cout << msgprefix << ": " << val1 << " == " << val2 << std::endl;
	cout.flush();
	assert( val1 == val2 );
}

static void checkStr ( const std::string& msgprefix, const std::string& str1, const std::string& str2 )
{
   if( str1 != str2 )
      cout << "ERROR ";
	cout << msgprefix << ": " << str1 << " == " << str2 << std::endl;
	cout.flush();
	assert( str1 == str2 );
}
/*
static std::string getHexString( const std::string& bytepattern )
{
	std::ostringstream oss;
	oss << hex << setfill('0');
	for( unsigned i = 0; i<bytepattern.size(); i++ )
	{
		unsigned byte = (uint8_t)bytepattern[i];
		oss << setw(2) << byte;
	}
	return oss.str();
}*/

#ifdef __LP64__
static std::string getHexString( const uint64_t pattern[], unsigned size )
{
	std::ostringstream oss;
	oss << hex << setfill('0');
	for( unsigned i = 0; i<size; i++ )
	{
		oss << setw(16) << pattern[i];
	}
	return oss.str();
}
#endif

static std::string getHexString( const uint32_t pattern[], unsigned size )
{
	std::ostringstream oss;
	oss << hex << setfill('0');
	for( unsigned i = 0; i<size; i++ )
	{
		oss << setw(8) << pattern[i];
	}
	return oss.str();
}

static std::string getHexString( const uint8_t pattern[], unsigned size )
{
	std::ostringstream oss;
	oss << hex << setfill('0');
	for( int i = size-1; i>=0; i-- )
	{
      unsigned value = pattern[i];
		oss << setw(2) << value;
	}
	return oss.str();
}

static std::string getHexString( const uint64_t pattern, unsigned size )
{
	std::ostringstream oss;
	oss << hex << setfill('0');
	oss << setw(size) << pattern;
	return oss.str();
}

#endif
