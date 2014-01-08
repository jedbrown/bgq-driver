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


#include "AbstractTable.h"

#include "bgsched/HardwareImpl.h"


using std::string;


namespace realtime {
namespace server {
namespace db2 {


bgsched::realtime::AbstractDatabaseChange::Ptr AbstractTable::NO_CHANGE;


std::string AbstractTable::charFieldToString(
        const char *padded_str,
        unsigned int padded_str_size
    )
{
    string ret;

    // find the end of the padded string.
    const char *padded_str_end(padded_str + padded_str_size);
    while ( (padded_str_end != padded_str) && (*(padded_str_end-1) == ' ') ) {
        --padded_str_end;
    }

    // copy the significant chars
    while ( padded_str != padded_str_end ) {
        ret += *padded_str++;
    }

    return ret;
}


bgsched::Hardware::State AbstractTable::hardwareStateCodeToValue( char code )
{
    return bgsched::Hardware::Impl::convertDatabaseState( (string() + code).c_str() );
}


} } } // namespace realtime::server::db2
