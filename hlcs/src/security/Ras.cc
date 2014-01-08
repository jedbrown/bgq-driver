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
#include "Ras.h"

#include <db/include/api/tableapi/gensrc/DBTEventlog.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>

#include <boost/assign/list_of.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>

LOG_DECLARE_FILE( "security" );

namespace hlcs {
namespace security {

Ras
Ras::create(
        Value type
        )
{
    Ras result( type );
    return result;
}

Ras::Ras(
        Value type
        ) :
    _impl( type )
{

}

Ras&
Ras::object(
        const Object& object
        )
{
    if ( object.type() == Object::Block ) {
        _impl.setDetail( RasEvent::BLOCKID, object.name() );
    } else if ( object.type() == Object::Job ) {
        _impl.setDetail( RasEvent::JOBID, object.name() );
    }
    _impl.setDetail( "OBJECT", boost::lexical_cast<std::string>(object.type()) );
    _impl.setDetail( "ID", object.name() );

    return *this;
}

Ras& 
Ras::action(
        Action::Type action
        )
{
    _impl.setDetail( "ACTION", boost::lexical_cast<std::string>( action ) );
    
    return *this;
}

Ras& 
Ras::user(
        const bgq::utility::UserId& user    //!< [in]
        )
{
    _impl.setDetail( "USER", boost::lexical_cast<std::string>( user.getUser() ) );
    
    return *this;
}

Ras::~Ras()
{
    try {
        LOG_TRACE_MSG( "inserting event" );
        RasEventHandlerChain::handle( _impl );
        this->insert();
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    } catch ( ... ) {
        // can't throw from a dtor
        LOG_WARN_MSG( "caught some other exception" );
    }
}

void
Ras::insert()
{
    const cxxdb::ConnectionPtr db(
            BGQDB::DBConnectionPool::Instance().getConnection()
        );
    if ( !db ) {
        BOOST_THROW_EXCEPTION(
                std::runtime_error( "could not get database connection" )
                );
    }

    const cxxdb::UpdateStatementPtr statement(
            db->prepareUpdate(
                std::string("INSERT into ") + BGQDB::DBTEventlog().getTableName() + " " +
                "(" +
                BGQDB::DBTEventlog::MSG_ID_COL + "," +
                BGQDB::DBTEventlog::CATEGORY_COL + "," +
                BGQDB::DBTEventlog::COMPONENT_COL + "," +
                BGQDB::DBTEventlog::SEVERITY_COL + "," +
                BGQDB::DBTEventlog::MESSAGE_COL + "," +
                BGQDB::DBTEventlog::LOCATION_COL + "," +
                BGQDB::DBTEventlog::BLOCK_COL + "," +
                BGQDB::DBTEventlog::JOBID_COL + 
                ") " +
                "VALUES (?,?,?,?,?,?,?,?)",
                boost::assign::list_of
                ( BGQDB::DBTEventlog::MSG_ID_COL )
                ( BGQDB::DBTEventlog::CATEGORY_COL )
                ( BGQDB::DBTEventlog::COMPONENT_COL )
                ( BGQDB::DBTEventlog::SEVERITY_COL )
                ( BGQDB::DBTEventlog::MESSAGE_COL )
                ( BGQDB::DBTEventlog::LOCATION_COL )
                ( BGQDB::DBTEventlog::BLOCK_COL )
                ( BGQDB::DBTEventlog::JOBID_COL )
                )
            );

    if ( !statement ) {
        BOOST_THROW_EXCEPTION(
                std::runtime_error( "could not prepare statement" )
                );
    }
    statement->parameters()[ BGQDB::DBTEventlog::MSG_ID_COL ].set( _impl.getDetail(RasEvent::MSG_ID) );
    statement->parameters()[ BGQDB::DBTEventlog::CATEGORY_COL ].set( _impl.getDetail(RasEvent::CATEGORY) );
    statement->parameters()[ BGQDB::DBTEventlog::COMPONENT_COL ].set( _impl.getDetail(RasEvent::COMPONENT) );
    statement->parameters()[ BGQDB::DBTEventlog::SEVERITY_COL ].set( _impl.getDetail(RasEvent::SEVERITY) );
    statement->parameters()[ BGQDB::DBTEventlog::MESSAGE_COL ].set( _impl.getDetail(RasEvent::MESSAGE) );
    statement->parameters()[ BGQDB::DBTEventlog::LOCATION_COL ].setNull();
    if ( _impl.getDetail(RasEvent::BLOCKID).empty() ) {
        statement->parameters()[ BGQDB::DBTEventlog::BLOCK_COL ].setNull();
    } else {
        statement->parameters()[ BGQDB::DBTEventlog::BLOCK_COL ].set( _impl.getDetail(RasEvent::BLOCKID) );
    }
    if ( _impl.getDetail(RasEvent::JOBID).empty() ) {
        statement->parameters()[ BGQDB::DBTEventlog::JOBID_COL ].setNull();
    } else {
        statement->parameters()[ BGQDB::DBTEventlog::JOBID_COL ].cast( boost::lexical_cast<BGQDB::job::Id>(_impl.getDetail(RasEvent::JOBID)) );
    }

    statement->execute();
}

} // security
} // hlcs

