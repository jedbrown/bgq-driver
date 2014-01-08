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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef BGQ_DB_DATACONV_H_
#define BGQ_DB_DATACONV_H_

#include <string>

namespace BGQDB {

void trim_right_spaces(char *input);
void trim_right_spaces(char *input, int len);

/*!
 * \brief convert hexadecimal input such as DEADBEEF into its bit data equivalent.
 *
 * As an example, the string "AB" as input will be converted to 0xAB as output
 *
 * \note source must have a length divisible by 2
 * \note destLen must be 2x source length
 * \note every character in source must be isxdigit()
 */
bool hexchar2bitdata(
        unsigned char *dest,        //!< [in] pointer to output buffer
        unsigned destLen,           //!< [in] output buffer length
        const std::string& source   //!< [in] input string
        );

} // namespace BGQDB

#endif
