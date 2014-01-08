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

#ifndef BGWS_RESPONDER_BLOCK_HPP_
#define BGWS_RESPONDER_BLOCK_HPP_


#include "../AbstractResponder.hpp"

#include "capena-http/http/uri/Path.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/cxxdb/fwd.h>

#include <hlcs/include/security/Enforcer.h>

#include <string>


namespace bgws {
namespace responder {


class Block : public AbstractResponder
{
public:

    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        );

    static capena::http::uri::Path calcPath(
            const capena::http::uri::Path& path_base,
            const std::string& block_id
        );


    Block(
            CtorArgs& args
        ) :
            AbstractResponder( args )
    { /* Nothing to do */ }


    capena::http::Methods _getAllowedMethods() const;

    void _doGet();
    void _doDelete();


private:


    std::string _calcBlockId( bool* is_valid_out ) const;

    void _getBlockDetails(
            const std::string& block_id
        );

    json::ValuePtr _queryBlock( const std::string& block_id );

    void _addSmallBlockInfo(
            const std::string& block_id,
            json::ObjectValuePtr obj_val_ptr,
            cxxdb::ConnectionPtr conn_ptr
        );

    void _addLargeBlockInfo(
            const std::string& block_id,
            json::ObjectValuePtr obj_val_ptr,
            cxxdb::ConnectionPtr conn_ptr
        );

    void _addJobCount(
            const std::string& block_id,
            json::ObjectValuePtr obj_val_ptr,
            cxxdb::ConnectionPtr conn_ptr
        );

    void _addIoBlockInfo(
            const std::string& block_id,
            json::ObjectValuePtr obj_val_ptr,
            cxxdb::ConnectionPtr conn_ptr
        );


    void _deleteBlock(
            const std::string& block_id
        );

};


}} // namespace bgws::responder

#endif
