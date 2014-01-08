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

#include "pc_util.h"

#include "Log.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>


LOG_DECLARE_FILE( "utility" );


using std::runtime_error;
using std::string;


namespace bgq {
namespace utility {
namespace pc_util {


boost::shared_ptr<std::string> extractPeerCn(
        portConfig::Socket& ssl_stream
    )
{
    const boost::shared_ptr<X509> cert(
            SSL_get_peer_certificate( ssl_stream.impl()->ssl ),
            boost::bind( &X509_free, _1)
            );

    if ( ! cert ) {
        return boost::shared_ptr<std::string>();
    }

    X509_NAME *subject_name(X509_get_subject_name( cert.get() ));

    if ( ! subject_name ) {
        BOOST_THROW_EXCEPTION( runtime_error( "failed to get peer subject name" ) );
    }

    char peer_cn[256];
    int rc = X509_NAME_get_text_by_NID( subject_name, NID_commonName, peer_cn, sizeof ( peer_cn ) );

    if ( rc == -1 ) {
        BOOST_THROW_EXCEPTION( runtime_error( "failed to get peer CN from certificate" ) );
    }

    return boost::shared_ptr<std::string>( new string(peer_cn) );
}


}}} // namespace bgq::utility::pc_util
