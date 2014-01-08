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

#ifndef BGWS_RESPONDER_LOGGING_HPP_
#define BGWS_RESPONDER_LOGGING_HPP_


#include "../AbstractResponder.hpp"

#include <log4cxx/logger.h>

#include <map>
#include <string>


namespace bgws {
namespace responder {


class Logging : public AbstractResponder
{
public:


    static const capena::http::uri::Path &RESOURCE_PATH;


    static bool matchesUrl(
            const capena::http::uri::Path& request_path
        );


    Logging(
            CtorArgs& args
        ) :
            AbstractResponder( args )
    { /* Nothing to do */ }

    capena::http::Methods _getAllowedMethods() const;

    // override
    void _doGet();

    // override
   void _doPut( json::ConstValuePtr val_ptr );


private:

    typedef std::map<std::string,std::string> _LoggingInfo;


    void _formatLoggingConfiguration(
            const log4cxx::LoggerList& loggers
        );

    _LoggingInfo _parseLoggingUpdate(
            json::ConstValuePtr val_ptr
        );

};


} } // namespace bgws::responder

#endif
