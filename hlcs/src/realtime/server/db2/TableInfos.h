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


#include "AbstractTable.h"

#include <sqlcli.h>

#include <iosfwd>
#include <map>


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
    { return tablespace_id == other.tablespace_id && table_id == other.table_id; }

    bool operator<( const TableInfo& other ) const
    { return tablespace_id < other.tablespace_id || (tablespace_id == other.tablespace_id && table_id < other.table_id); }

};


std::ostream& operator<<( std::ostream& os, const TableInfo& ti );


struct TableInfos
{
public:

    TableInfos( const Configuration& configuration );

    AbstractTable::Ptr calcTable( const TableInfo& table_info ) const;


private:

    typedef std::map<TableInfo,AbstractTable::Ptr> _TableMap;


    static TableInfo _s_getTableInfo(
            SQLHANDLE hdbc,
            const std::string& table_name_uc,
            const std::string& db_schema_uc
        );


    _TableMap _table_map;

};


} } } // namespace realtime::server::db2

#endif
