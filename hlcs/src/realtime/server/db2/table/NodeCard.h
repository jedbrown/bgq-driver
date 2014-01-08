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


#ifndef REALTIME_SERVER_DB2_TABLE_NODE_CARD_H_
#define REALTIME_SERVER_DB2_TABLE_NODE_CARD_H_


#include "../AbstractTable.h"


namespace realtime {
namespace server {
namespace db2 {
namespace table {


class NodeCard : public AbstractTable
{
public:

    static const std::string& getTableNameUc()  { static std::string table_name_uc("TBGQNODECARD"); return table_name_uc; };

    static Ptr create()  { return Ptr( new NodeCard() ); }


    bgsched::realtime::AbstractDatabaseChange::Ptr handleRecord(
            const char* user_data_buffer,
            const char* old_user_data_buffer,
            OperationType::Values operation_type
        );


private:

    NodeCard() { /* Nothing to do */ }

};


} } } } // namespace realtime::server::db2::table

#endif
