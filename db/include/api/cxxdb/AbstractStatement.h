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


#ifndef CXXDB_ABSTRACT_STATEMENT_H_
#define CXXDB_ABSTRACT_STATEMENT_H_


#include "fwd.h"
#include "Parameters.h"

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/weak_ptr.hpp>

#include <string>


namespace cxxdb {


class ConnectionHandle;
class StatementHandle;


/*! \brief Base class for Statements. */
class AbstractStatement : boost::noncopyable
{
public:

    /* Prepare the SQL statement. */
    void prepare(
            const std::string& sql,
            const ParameterNames& parameter_names = ParameterNames()
        );

    /*! \brief Access the Parameters for the Statement. */
    Parameters& parameters();

    /*! \brief Cancel the statement. */
    void cancel();

    /*! \brief Test validity.
     *
     *  If is valid, may become invalid just after calling, so not terribly useful other than for testing.
     */
    bool isValid() const  { return bool(_handle_ptr); }

    virtual ~AbstractStatement();


protected:

    typedef boost::shared_ptr<StatementHandle> _HandlePtr;
    typedef boost::weak_ptr<BasicConnection> _ConnWkPtr;
    typedef boost::shared_ptr<Parameters> _ParametersPtr;


    _ConnWkPtr _conn_ptr;
    _HandlePtr _handle_ptr;
    _ParametersPtr _parameters_ptr;


    AbstractStatement(
            BasicConnectionPtr connection_ptr,
            ConnectionHandle& connection_handle,
            const std::string& sql,
            const ParameterNames& parameter_names
        );


    AbstractStatement(
            BasicConnectionPtr connection_ptr,
            ConnectionHandle& connection_handle
        );


    /*! \brief Called by BasicConnection when invalidated. */
    virtual void _invalidate();


    friend class BasicConnection;
};


} // namespace cxxdb


#endif
