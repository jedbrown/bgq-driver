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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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


#include "DBStatics.h"

#include <db/include/api/BGQDBlib.h>

#include <db/include/api/tableapi/TxObject.h>

#include <utility/include/Log.h>

#include <boost/foreach.hpp>


LOG_DECLARE_FILE( "mmcs.server" );


namespace mmcs {
namespace server {


void
DBStatics::setLocationStatus(
         const std::vector<std::string>& locations, 
         mmcs_client::CommandReply& reply, 
         const Status status, 
         const Type type
         )
 {
    LOG_DEBUG_MSG(__FUNCTION__);
    if(locations.empty()) {
        LOG_INFO_MSG("no locations to update");
        return;
    }

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_INFO_MSG("unable to connect to database");
        reply << mmcs_client::FAIL << "Unable to connect to database." << mmcs_client::DONE;
        return;
    }

    std::ostringstream sqlstrm;
    sqlstrm << "update ";
    if(type == DBStatics::ION) {
        sqlstrm << "BGQIONODE";
    } else if(type == DBStatics::CN) {
        sqlstrm << "BGQNODE";
    } else if(type == DBStatics::IOCARD) {
        sqlstrm << "BGQIODRAWER";
    } else if(type == COMPUTECARD) {
        sqlstrm << "BGQNODECARD";
    } else if(type == SERVICECARD) {
        sqlstrm << "BGQSERVICECARD";
    }

    if(status == DBStatics::AVAILABLE) {
        sqlstrm << " set status='A' where ";
    } else if(status == DBStatics::ERROR)
        sqlstrm << " set status='E' where ";
    else if(status == DBStatics::MISSING)
        sqlstrm << " set status='M' where ";

    bool first = true;
    BOOST_FOREACH(const std::string& node, locations) {
        if(first) {
            sqlstrm << "LOCATION=\'" << node;
            first = false;
        }
        else sqlstrm << "\' OR LOCATION=\'" << node;
    }

    sqlstrm << "\'";

    LOG_DEBUG_MSG("Sending query \"" << sqlstrm.str() << "\" to database.");
    SQLRETURN sqlresult = tx.execStmt( sqlstrm.str().c_str());
    if(sqlresult != 0) {
        std::ostringstream errstrm;
        errstrm << "SQL query to set node states to " << status << " failed.";
        LOG_ERROR_MSG(errstrm.str());
        reply << mmcs_client::FAIL << errstrm.str() << mmcs_client::DONE;
        return;
    }
}

} } // namespace mmcs::server
