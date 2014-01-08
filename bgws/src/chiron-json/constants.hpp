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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#ifndef JSON_CONSTANTS_HPP_
#define JSON_CONSTANTS_HPP_


/*! \file
 *
 * \brief JSON constants.
 *
 */


#include <string>


namespace json {


/*! \brief Begin JSON object character. */
const char beginObject('{');

/*! \brief End JSON object character. */
const char endObject('}');

/*! \brief Begin JSON array character. */
const char beginArray('[');

/*! \brief End JSON array character. */
const char endArray(']');

/*! \brief JSON name-value separator character. */
const char nameSeparator(':');

/*! \brief JSON value separator character. */
const char valueSeparator(',');

/*! \brief JSON quote marks for strings. */
const char quotationMark('"');

/*! \brief JSON escape character in strings. */
const char reverseSolidus('\\');

/*! \brief JSON Boolean true string. */
const std::string trueString( "true" );

/*! \brief JSON Boolean false string. */
const std::string falseString( "false" );

/*! \brief JSON Boolean null string. */
const std::string nullString( "null" );


} // namespace json


#endif
