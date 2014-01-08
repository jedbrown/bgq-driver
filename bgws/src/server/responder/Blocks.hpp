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

#ifndef BGWS_RESPONDER_BLOCKS_HPP_
#define BGWS_RESPONDER_BLOCKS_HPP_


#include "../AbstractResponder.hpp"

#include "capena-http/http/uri/Path.hpp"

#include <db/include/api/GenBlockParams.h>

#include <db/include/api/cxxdb/fwd.h>

#include <db/include/api/filtering/BlockFilter.h>
#include <db/include/api/filtering/BlockSort.h>

#include <hlcs/include/security/Enforcer.h>


namespace bgws {
namespace responder {


class Blocks : public AbstractResponder
{
public:


    static const capena::http::uri::Path &RESOURCE_PATH;
    static const capena::http::uri::Path RESOURCE_PATH_EMPTY_CHILD;


    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        );


    Blocks(
            CtorArgs& args
        ) :
            AbstractResponder( args ),
            _blocking_operations_thread_pool(args.blocking_operations_thread_pool)
    { /* Nothing to do */ }


    capena::http::Methods _getAllowedMethods() const;

    // override
    void _doGet();

    // override
    void _doPost( json::ConstValuePtr val_ptr );


private:

    BlockingOperationsThreadPool &_blocking_operations_thread_pool;


    void _doQuery(
            capena::server::ResponderPtr,
            const BGQDB::filtering::BlockFilter& block_filter,
            const BGQDB::filtering::BlockSort& block_sort
        );

    void _queryComplete(
            capena::server::ResponderPtr,
            cxxdb::ConnectionPtr,
            cxxdb::ResultSetPtr rs_ptr
        );


    void _checkCreateBlockAuthority();

    BGQDB::GenBlockParams _parseCreateBlock(
            json::ConstValuePtr val_ptr
        );

    void _createBlock(
            const BGQDB::GenBlockParams& params
        );

};


} } // namespace bgws::responder

#endif
