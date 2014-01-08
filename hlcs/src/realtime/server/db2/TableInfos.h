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

#ifndef REALTIME_SERVER_DB2_TABLE_INFOS_H_
#define REALTIME_SERVER_DB2_TABLE_INFOS_H_


#include <sqlcli.h>

#include <iosfwd>


namespace realtime {
namespace server {


class Configuration;


namespace db2 {


struct TableInfo
{
    int tablespace_id;
    int table_id;

    TableInfo()  { /* Nothing to do */  }

    TableInfo( int tablespace_id_in, int table_id_in )
        : tablespace_id( tablespace_id_in ), table_id( table_id_in )
    { /* Nothing to do */ }

    bool operator ==( const TableInfo& other ) const
    {
        return tablespace_id == other.tablespace_id && table_id == other.table_id;
    }
};

std::ostream& operator<<( std::ostream& os, const TableInfo& ti );

struct TableInfos
{
public:
    TableInfo block_table_info;
    TableInfo job_table_info;
    TableInfo midplane_table_info;
    TableInfo switch_table_info;
    TableInfo node_table_info;
    TableInfo node_card_table_info;
    TableInfo cable_table_info;
    TableInfo event_log_table_info;


    TableInfos( const Configuration& configuration );

    struct table {
        enum Value {
            BLOCK,
            JOB,
            MIDPLANE,
            NODE_CARD,
            NODE,
            SWITCH,
            CABLE,
            EVENT_LOG,
            UNKNOWN
        };
    };

    table::Value calcTable( const TableInfo& table_info ) const;

private:

    static TableInfo _s_getTableInfo(
            SQLHANDLE hdbc,
            const std::string& table_name_uc,
            const std::string& db_schema_uc
        );
};


} } } // namespace realtime::server::db2

#endif
