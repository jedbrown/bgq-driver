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

/*!
 * Various inline functions for converting data types to and from strings.
 */

#ifndef _DATAUTIL_H_
#define _DATAUTIL_H_

#include <sstream>
#include <vector>
#include <string>
#include <stdint.h>
#include <limits>
#include <iomanip>
#include "Time.h"

/*!
 * Convert a vector of uint8 to a string of bytes.
 */
inline std::string dataVectorToString( std::vector<uint8_t> data )
{
       std::string returnString;
       returnString.assign((char*)&data[0], data.size());
       return returnString;
       
       /*
		stringstream targetReply;
		std::vector<uint8_t>::const_iterator byte;
		for( byte = data.begin(); byte != data.end(); byte++ )
		{
			targetReply << *byte;
		}
	    return targetReply.str();
	    */
}

/*!
 * Convert a string to a vector of uint8.
 */
inline std::vector<uint8_t> stringToDataVector( const std::string& instring )
{
	std::vector<uint8_t> data;
	data.resize(instring.size());
    memcpy(&data[0], instring.c_str(), instring.size());        // one copy of the data.
	
	/*
	for( unsigned i = 0; i < instring.size(); i++ )
	{
		data.push_back( instring[i] );
	}
	*/
	return data;
}

/*!
 * Convert a double to a string.
 */
inline std::string stringifydub( double adub )
{
  	std::ostringstream ss;
  	// std::numeric_limits<double>::digits10 is the highest precision
  	ss << std::fixed << std::noshowpoint << std::setprecision(3) << std::setw(1) << adub;
  	//std::cerr << "adub = " << adub << "; precision = " << precision << "; str = " << ss.str() << std::endl;
  	return ss.str();
}

/*!
 * \brief Return a string representation of an integer.
 * The above routine for double will work, but gives you a decimal point with zeros after it.
 */
inline std::string stringify( long long anint )
{
  	std::ostringstream ss;
  	ss << std::fixed << anint;
  	return ss.str();
}
  
/*!
 * \brief Return a string with newlines and nulls and other garbage removed.
 */
inline std::string stringify( const std::string& instring, std::string* errors = NULL )
{
    std::string outstring = "";

    const char *begin  = instring.c_str();
    const char *end  = begin + instring.size();

    for (const char *cp = begin; cp < end; ++cp)
    {
        if( *cp < ' ' || *cp > '~')// matches write_text in XML.cc
        {
            char hexchar[5];
            snprintf(hexchar, sizeof(hexchar), "[%02X]", (unsigned char)*cp);
            if( errors != NULL ) 
            {
                std::stringstream ss;
                ss << " non-printable character at byte " << cp - begin << " value = 0x" << hexchar; 
                errors->append( ss.str() ); 
            }
            outstring.append(hexchar);
        }

        else // OK character
            outstring.push_back(*cp);		 
    }
    return outstring;
}
  
/*!
 * Convert a timeval to a string in the format used by DB2.
 */
inline std::string stringify( timeval atime )
{
  	Time mytime(atime);
  	//return mytime.asLongString(true,true);// C99 sortable with microseconds
  	return mytime.asDB2timestamp();
}
  
inline std::vector<std::string> getArgv(const std::string& buf, const std::string& delims) {
  std::vector<std::string> result;
  
  std::string::size_type begIdx = buf.find_first_not_of(delims);
  while (begIdx != std::string::npos) {
    std::string::size_type endIdx = buf.find_first_of(delims, begIdx);
    result.push_back(std::string(buf, begIdx, endIdx-begIdx));
    begIdx = buf.find_first_not_of(delims, endIdx);
  }
  return result;
}



#endif //_DATAUTIL_H_
