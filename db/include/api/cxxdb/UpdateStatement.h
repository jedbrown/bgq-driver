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

#ifndef CXXDB_UPDATE_STATEMENT_H_
#define CXXDB_UPDATE_STATEMENT_H_

#include "AbstractStatement.h"

namespace cxxdb {

/*! \brief An update statement (INSERT, UPDATE, DELETE, etc.). */
class UpdateStatement : public AbstractStatement
{
public:

    /*! \brief Execute the Statement.
     *
     *  If affected_row_count_out is not NULL then the number of rows affected is returned.
     *  Otherwise, the number of rows affected is not calculated.
     */
    void execute(
            unsigned* affected_row_count_out = NULL
        );

private:

    UpdateStatement(
            BasicConnectionPtr connection_ptr,
            ConnectionHandle& connection_handle,
            const std::string& sql,
            const ParameterNames& parameter_names
        );


    friend class BasicConnection;
};

} // namesapce cxxdb

#endif
