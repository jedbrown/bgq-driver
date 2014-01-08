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
#ifndef RUNJOB_SERVER_RAS_H
#define RUNJOB_SERVER_RAS_H

#include <db/include/api/cxxdb/fwd.h>
#include <db/include/api/job/types.h>

#include <ras/include/RasEventImpl.h>

#include <boost/lexical_cast.hpp>

#include <string>

#include <stdint.h>

namespace runjob {
namespace server {

/*!
 * \brief RAII wrapper for inserting RAS events into the database.
 */
class Ras
{
public:
    /*!
     * \brief types of RAS events.
     *
     * Add new RAS events by appending them in this list, do not
     * sort or re-organize them.
     */
    enum Value {
        StartingAddress = 0x0061fff,
        KillTimeout,
        CiosAuthenticationFailure,
        PrologFailure,
        EpilogFailure,
        BlockNotReady,
        HeartbeatFailure,
        AdministrativeAuthorityDenied = 0x00063001
    };

public:
    /*!
     * \brief factory
     */
    static Ras create(
            Value type  //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Ras();

    /*!
     * \brief
     */
    template <typename T>
    Ras& detail(
            const std::string& value,   //!< [in]
            const T& detail             //!< [in]
            )
    {
        _impl.setDetail( value, boost::lexical_cast<std::string>(detail) );
        return *this;
    }

    /*!
     * \brief set block ID.
     */
    Ras& block(
            const std::string& block    //!< [in]
            )
    {
        _block = block;
        return *this;
    }

    /*!
     * \brief set job ID.
     */
    Ras& job(
            const BGQDB::job::Id job  //!< [in]
            )
    {
        _job = job;
        return *this;
    }

private:
    cxxdb::QueryStatementPtr prepare(
            const cxxdb::ConnectionPtr& connection
            );

    cxxdb::ResultSetPtr insert(
            const cxxdb::ConnectionPtr& connection,
            const cxxdb::QueryStatementPtr& query
            );

    void handleControlActions(
            const cxxdb::ConnectionPtr& connection
            );

    std::string getQualifier(
            const cxxdb::ConnectionPtr& connection
            );

    Ras(
            Value type
       );

private:
    RasEventImpl _impl;
    BGQDB::job::Id _job;
    std::string _block;
};

} // server
} // runjob

#endif
