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

#include "ssl_util.h"

#include "Log.h"

#include <openssl/err.h>
#include <openssl/ssl.h>

#include <sstream>
#include <stdexcept>


using std::string;
using std::ostringstream;
using std::runtime_error;


LOG_DECLARE_FILE( "utility" );


namespace bgq {
namespace utility {


string getSslErrorString()
{
    BIO *bio(BIO_new( BIO_s_mem() ));
    ERR_print_errors( bio );

    char *buf;
    long data_len(BIO_get_mem_data( bio, &buf ));

    string err_str( buf, static_cast<size_t>(data_len) );

    BIO_free( bio );

    return err_str;
}


void throwSslError(
        const std::string& ssl_function_name,
        const std::string& calling_function_name,
        unsigned calling_function_line
    )
{
    ostringstream msg;

    msg << ssl_function_name << " failed at " << calling_function_name << ":" << calling_function_line << "."
            " The SSL error messages follow:\n" << getSslErrorString();

    throw runtime_error( msg.str() );
}


} // namespace bgq::utility
} // namespace bgq
