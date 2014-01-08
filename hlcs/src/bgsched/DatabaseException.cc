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

#include <bgsched/DatabaseException.h>

#include <db/include/api/BGQDBlib.h>

#include <sstream>
#include <string>

using std::string;
using std::ostringstream;

namespace bgsched {

string
DatabaseErrors::toString(
        DatabaseErrors::Value /*v*/,
        const string& what
        )
{
    return what;
}

DatabaseErrors::Value
DatabaseErrors::fromDatabaseStatus(
        int status
        )
{
    switch (status)
    {
        case BGQDB::DB_ERROR:
            return DatabaseErrors::DatabaseError;
        case BGQDB::FAILED:
            return DatabaseErrors::OperationFailed;
        case BGQDB::INVALID_ID:
            return DatabaseErrors::InvalidKey;
        case BGQDB::NOT_FOUND:
            return DatabaseErrors::DataNotFound;
        case BGQDB::DUPLICATE:
            return DatabaseErrors::DuplicateEntry;
        case BGQDB::XML_ERROR:
            return DatabaseErrors::XmlError;
        case BGQDB::CONNECTION_ERROR:
            return DatabaseErrors::ConnectionError;
        default:
            return DatabaseErrors::UnexpectedError;
    }
}

} // namespace bgsched
