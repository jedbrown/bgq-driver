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

#ifndef BGWS_RESPONDER_HARDWARE_HARDWARE_HPP_
#define BGWS_RESPONDER_HARDWARE_HARDWARE_HPP_


#include "../../AbstractResponder.hpp"

#include "capena-http/http/uri/Path.hpp"

#include "chiron-json/fwd.hpp"

#include <db/include/api/cxxdb/fwd.h>

#include <boost/optional.hpp>

#include <string>


namespace bgws {
namespace responder {
namespace hardware {


class Hardware : public AbstractResponder
{
public:

    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        );


    Hardware(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _blocking_operations_thread_pool(args.blocking_operations_thread_pool)
    { /* Nothing to do */ }


    capena::http::Methods _getAllowedMethods() const;

    void _doGet();


private:

    BlockingOperationsThreadPool &_blocking_operations_thread_pool;


    void _startQuery(
            capena::server::ResponderPtr,
            const std::string& location_str
        );

    void _queryComplete(
            capena::server::ResponderPtr,
            const std::string& location_str,
            json::ValuePtr val_ptr
        );


    json::ValuePtr _getComputeRack( const std::string& location );

    json::ValuePtr _getMidplane( const std::string& location );

    json::ValuePtr _getNodeBoard( const std::string& location );

    json::ValuePtr _getIoRack( const std::string& location );

    json::ValuePtr _getIoDrawer( const std::string& location );


    void _addPowerModules( const std::string& rack_location, cxxdb::ConnectionPtr conn_ptr, json::Object& obj );
    json::Array* _addIoDrawers( const std::string& rack_location, cxxdb::ConnectionPtr conn_ptr, json::Object& obj );

};


}}} // namespace bgws::responder::hardware


#endif
