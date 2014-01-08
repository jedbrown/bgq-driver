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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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


#ifndef REALTIME_SERVER_DB2_ABSTRACT_TABLE_H_
#define REALTIME_SERVER_DB2_ABSTRACT_TABLE_H_


#include "bgsched/Hardware.h"

#include "bgsched/realtime/AbstractDatabaseChange.h"

#include <boost/shared_ptr.hpp>


namespace realtime {
namespace server {
namespace db2 {


class AbstractTable
{
public:


    typedef boost::shared_ptr<AbstractTable> Ptr;

    struct OperationType {
        enum Values {
            INSERT,
            DELETE,
            UPDATE
        };
    };


    static bgsched::realtime::AbstractDatabaseChange::Ptr NO_CHANGE;


    /*!
     * Function to convert a fixed-size padded string from DB2 to a C-style string.
     * This is used for block IDs, etc.
     */
    static std::string charFieldToString(
            const char *padded_str,
            unsigned int padded_str_size
        );


    static bgsched::Hardware::State hardwareStateCodeToValue( char code );


    virtual bgsched::realtime::AbstractDatabaseChange::Ptr handleRecord(
            const char* user_data_buffer,
            const char* old_user_data_buffer,
            OperationType::Values operation_type
        ) =0;

    virtual ~AbstractTable()  { /* Nothing to do */ }
};


} } } // namespace realtime::server::db2

#endif
