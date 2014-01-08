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

#include "BGQDBlib.h"
#include "dataconv.h"
#include "extract_db.h"
#include "utility.h"

#include "tableapi/dbdataconv.h"
#include "tableapi/DBConnectionPool.h"
#include "tableapi/gensrc/bgqtableapi.h"

#include <utility/include/Log.h>

#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>

#include <bitset>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

using std::ostream;
using std::string;
using std::endl;
using std::vector;

LOG_DECLARE_FILE( "database" );

static extract_db_status retrieve_table(BGQDB::TxObject& tx, BGQDB::DBObj& tableObject, const std::string& whereclause = std::string());
static uint32_t next_line(BGQDB::TxObject& tx, BGQDB::DBObj& tableObject);

const char psetq[16][3] =
  {"Q1","Q1","Q1","Q1","Q2","Q2","Q2","Q2","Q3","Q3","Q3","Q3","Q4","Q4","Q4","Q4"};

const char ncstr[16][4] =
  {"N00","N01","N02","N03","N04","N05","N06","N07","N08","N09","N10","N11","N12","N13","N14","N15"};


extract_db_status
extract_compact_machine(
        std::ostream &os,
        std::vector<std::string>* invalid_memory_locations
)
{
    BGQDB::DBVNodecard nodecard;
    BGQDB::DBTMachine machine;
    BGQDB::DBTEthgateway gw;
    BGQDB::DBTIodrawer iodrawer;
    BGQDB::DBVIonetconfig ionetcfg;
    BGQDB::DBTIcon iconchip;
    BGQDB::DBVMidplane midplane;
    BGQDB::DBVLink link;
    BGQDB::DBVToruslink tlink;
    BGQDB::DBVIolink iolink;
    BGQDB::DBVCniolink cniolink;
    BGQDB::DBTMidplanesubnet ms;
    BGQDB::DBTCable cable;
    BGQDB::DBVNode  node;
    BGQDB::ColumnsBitmap colBitmap;
    extract_db_status result;
    int mem, maxmem, memmod;
    string sqlstr;
    SQLLEN ind[3];
    char location[65];
    SQLRETURN sqlrc, status, queryresult;
    SQLHANDLE hstmt;
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if ( ! tx.getConnection() ) {
        LOG_ERROR_MSG(__FUNCTION__ << " No connection to database");
        return DB_COMM_ERR;
    }
    BGQDB::TxObject tx2(BGQDB::DBConnectionPool::Instance());
    if ( ! tx2.getConnection() ) {
        LOG_ERROR_MSG(__FUNCTION__ << " No connection to database");
        return DB_COMM_ERR;
    }

    string condition = " where serialNumber='BGQ'";
    if ((result = retrieve_table(tx, machine, condition)) != DB_OK) {
        if (result == DB_NO_DATA) {
            LOG_ERROR_MSG(__FUNCTION__ << " Machine serial number not found");
            return DB_NO_DATA;
        } else {
            // Error message is logged in retrieve_table
            return DB_ERROR;
        }
    }

    // generate description tag for machine
    os << "<BGQMidplaneMachine " ;
    os << " clockHz='" << machine._clockhz << "'";
    os << " bgsysIpv4Address='" << machine._bgsysipv4address << "'";
    if (machine._ind[machine.BGSYSIPV6ADDRESS] != SQL_NULL_DATA) {
        os << " bgsysIpv6Address='"  << machine._bgsysipv6address << "'";
    }
    os << " bgsysRemotePath='" << machine._bgsysremotepath << "'";

    if (machine._ind[machine.FILESYSTEMTYPE] != SQL_NULL_DATA) {
        os << " fileSystemType='"  << machine._filesystemtype << "'";
    }
    if (machine._ind[machine.BGSYSMOUNTOPTIONS] != SQL_NULL_DATA) {
        os << " bgsysMountOptions='"  << machine._bgsysmountoptions << "'";
    }


    if (machine._ind[machine.SNIPV4ADDRESS] != SQL_NULL_DATA) {
        os << " serviceNodeIpv4Address='"  << machine._snipv4address << "'";
    }
    if (machine._ind[machine.SNIPV6ADDRESS] != SQL_NULL_DATA) {
        os << " serviceNodeIpv6Address='"  << machine._snipv6address << "'";
    }

    if (machine._ind[machine.DISTROIPV4ADDRESS] != SQL_NULL_DATA) {
        os << " distroIpv4Address='"  << machine._distroipv4address << "'";
    }
    if (machine._ind[machine.DISTROIPV6ADDRESS] != SQL_NULL_DATA) {
        os << " distroIpv6Address='"  << machine._distroipv6address << "'";
    }
    if (machine._ind[machine.DISTROREMOTEPATH] != SQL_NULL_DATA) {
        os << " distroRemotePath='"  << machine._distroremotepath << "'";
    }
    if (machine._ind[machine.DISTROMOUNTOPTIONS] != SQL_NULL_DATA) {
        os << " distroMountOptions='"  << machine._distromountoptions << "'";
    }


    sqlstr =  "select min(memorysize) , min(memorymodulesize) from  bgqcomputenode n where status <> 'M' ";
    queryresult = tx.execQuery(sqlstr.c_str(), &hstmt);
    if (queryresult != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << queryresult << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    if ((sqlrc = SQLBindCol(hstmt, 1, SQL_INTEGER, &mem, 4, &ind[0])) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    if ((sqlrc = SQLBindCol(hstmt, 2, SQL_INTEGER, &memmod, 4, &ind[1])) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = SQLFetch(hstmt);
    if ((sqlrc != SQL_SUCCESS) || (ind[0] == SQL_NULL_DATA)) {
        mem = 4096;    // default is 4 GB
    }
    os << " computeNodeMemory='" << mem << "'";

    if ((sqlrc != SQL_SUCCESS) || (ind[1] == SQL_NULL_DATA)) {
        if ( mem == 4096 ) {
            memmod = 2;
        } else {
            memmod = 1;
        }
    }
    os << " computeDDRModuleSize='" << memmod << "'";

    SQLCloseCursor(hstmt);

    sqlstr = "select min(memorysize) , min(memorymodulesize) from  bgqionode n  where status <> 'M' ";
    queryresult = tx.execQuery(sqlstr.c_str(), &hstmt);
    if (queryresult != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << queryresult << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    if ((sqlrc = SQLBindCol(hstmt, 1, SQL_INTEGER, &mem, 4, &ind[0])) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    if ((sqlrc = SQLBindCol(hstmt, 2, SQL_INTEGER, &memmod, 4, &ind[1])) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = SQLFetch(hstmt);
    if ((sqlrc != SQL_SUCCESS) || (ind[0] == SQL_NULL_DATA)) {
        mem = 4096;    // default is 4 GB
    }
    os << "  ioNodeMemory='" << mem << "'";

    if ((sqlrc != SQL_SUCCESS) || (ind[1] == SQL_NULL_DATA)) {
        if ( mem == 4096 ) {
            memmod = 2;
        } else {
            memmod = 1;
        }
    }
    os << " ioDDRModuleSize='" << memmod << "'";

    os << " mtu='" << machine._mtu << "'";


    SQLCloseCursor(hstmt);

    condition = " where  machineSerialNumber='" +  string(machine._serialnumber) + string("' ");

    if ((result = retrieve_table(tx, gw, condition)) == DB_OK) {
        BGQDB::trim_right_spaces(gw._ipaddress);
        BGQDB::trim_right_spaces(gw._broadcast);
        BGQDB::trim_right_spaces(gw._mask);
        os << " gateway='" << gw._ipaddress << "'";
        os << " broadcast='" << gw._broadcast << "'";
        os << " ipv4Netmask='" << gw._mask << "'";
        os << " ipv6Netmask='" << gw._ipv6mask << "'";
    }

    os << ">" << endl;

    condition = " where  machineSerialNumber='" +  string(machine._serialnumber) + string("'");

    // there must be at least one midplane in each machine
    if ((result = retrieve_table(tx, midplane, condition)) == DB_OK) {
        // for each midplane in the machine
        do {
            os << " <BGQMidplane";
            SET_ALL_COLUMN(ms);
            sprintf(ms._posinmachine,"%s",midplane._location);
            if ((status = tx2.queryByKey(&ms))== SQL_SUCCESS) {
                if ((status = tx2.fetch(&ms))== SQL_SUCCESS) {
                    os << " nfsIpAddress='" << ms._nfsipaddress << "' gateway='" << ms._ipaddress << "' broadcast='" << ms._broadcast << "'" ;
                }
                tx2.close(&ms);
            }
            sqlstr =  "select min(memorysize),max(memorysize), min(memorymodulesize) from  bgqnode n  ";
            sqlstr += " where n.midplanepos = '" + string(midplane._location);
            sqlstr += "'  and status <> 'M'  ";
            queryresult = tx2.execQuery(sqlstr.c_str(), &hstmt);
            if (queryresult != SQL_SUCCESS) {
                LOG_ERROR_MSG( "Database query failed with error: " << queryresult << " at " << __FUNCTION__ << ':' << __LINE__ );
                return DB_ERROR;
            }

            if ((sqlrc = SQLBindCol(hstmt, 1, SQL_INTEGER, &mem, 4, &ind[0])) != SQL_SUCCESS) {
                LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
                return DB_ERROR;
            }
            if ((sqlrc = SQLBindCol(hstmt, 2, SQL_INTEGER, &maxmem, 4, &ind[1])) != SQL_SUCCESS) {
                LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
                return DB_ERROR;
            }
            if ((sqlrc = SQLBindCol(hstmt, 3, SQL_INTEGER, &memmod, 4, &ind[2])) != SQL_SUCCESS) {
                LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
                return DB_ERROR;
            }

            sqlrc = SQLFetch(hstmt);
            if ((sqlrc != SQL_SUCCESS) || (ind[0] == SQL_NULL_DATA) || (ind[1] == SQL_NULL_DATA)) {
                // nothing to do
            } else if (maxmem != mem) {
                // validate the memory and that its uniform across the midplane

                bool checkmem = true;
                try {
                    const bgq::utility::Properties::ConstPtr prop(BGQDB::DBConnectionPool::Instance().getProperties());
                    if (!prop->getValue("database", "bypass_memory_check").empty()) {
                        checkmem = false;
                    }
                } catch ( const std::exception& e ) {
                    LOG_DEBUG_MSG( e.what() );
                }

                if (checkmem) {
                    char memstr[4];
                    sprintf(memstr,"%i", mem);
                    SQLCloseCursor(hstmt);
                    sqlstr =  "select  n.location from  bgqnode n  ";
                    sqlstr += " where memorysize = " + string(memstr) + " and midplanepos = '" + string(midplane._location);
                    sqlstr += "'  and status <> 'M' ";
                    queryresult = tx2.execQuery(sqlstr.c_str(), &hstmt);
                    if (queryresult != SQL_SUCCESS) {
                        LOG_ERROR_MSG( "Database query failed with error: " << queryresult << " at " << __FUNCTION__ << ':' << __LINE__ );
                        return DB_ERROR;
                    }

                    if ((sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR, location, 65, &ind[0])) != SQL_SUCCESS) {
                        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
                        return DB_ERROR;
                    }

                    LOG_ERROR_MSG(__FUNCTION__ << " Non-uniform memory for nodes on " << string(midplane._location));
                    sqlrc = SQLFetch(hstmt);
                    for (; sqlrc == SQL_SUCCESS; ) {
                        LOG_WARN_MSG(__FUNCTION__ << " Node " << string(location) << " has different memory than other nodes on the midplane");
                        // Mark node card in error
                        sqlstr = "update tbgqnodecard set status = 'E' where serialnumber = (select a.serialnumber from tbgqnodecard a, bgqnode b where b.midplanepos = a.midplanepos and b.nodecardpos = a.position and b.location = '" + string(location);
                        sqlstr += "' )";

                        BGQDB::TxObject tx3(BGQDB::DBConnectionPool::Instance());
                        if ( !tx3.getConnection() ) {
                            LOG_ERROR_MSG( "could not get database connection" );
                            return DB_ERROR;
                        }
                        SQLRETURN rc = tx3.execStmt(sqlstr.c_str());
                        if (rc != SQL_SUCCESS) {
                            LOG_ERROR_MSG( "Database update failed with error: " << rc << " at " << __FUNCTION__ << ':' << __LINE__ );
                            return DB_ERROR;
                        }

                        if (invalid_memory_locations) {
                            invalid_memory_locations->push_back( location );
                        }

                        sqlrc = SQLFetch(hstmt);
                    }
                    LOG_ERROR_MSG(__FUNCTION__ << " A service action must be performed to replace the node(s)");
                } else {  //  memory was non-uniform but the bypass flag was on
                    os << " computeNodeMemory='" << mem << "'";
                    os << " computeDDRModuleSize='" << memmod << "'";
                }

            } else {   // memory was uniform
                os << " computeNodeMemory='" << mem << "'";
                os << " computeDDRModuleSize='" << memmod << "'";
            }

            SQLCloseCursor(hstmt);

            os << " posInMachine='" << midplane._location << "'>" << endl;

            condition = " where  midplanepos= '" + string(midplane._location) + string("'");

            if ((result = retrieve_table(tx2, nodecard, condition)) == DB_OK) {
                do {
                    os << "  <BGQNodeCard";
                    os << " location='" <<  nodecard._location << "' ";
                    os << " posInMidplane='" <<  nodecard._position << "' ";
                    os << "/>" << endl;
                } while(next_line(tx2, nodecard));
            }

            os << " </BGQMidplane>" << endl;
        } while(next_line(tx, midplane));
    }


    // get the IO drawer information
    condition = " where  status <> 'M'  ";
    if ((result = retrieve_table(tx, iodrawer, condition)) == DB_OK) {
        colBitmap.set();
        ionetcfg._columns = colBitmap.to_ulong();

        do {
            os << " <BGQIODrawer";
            os << " posInMachine='" << iodrawer._location << "'>" << endl;

            condition  =  " where  iopos = '" + string(iodrawer._location) + string("' and interface <> ''  order by location, interface");

            // We'll keep track of when location and interface change, to consolidate the output
            char node_prev[sizeof(ionetcfg._location)];
            char intf_prev[sizeof(ionetcfg._interface)];

            tx2.query(&ionetcfg, condition.c_str());
            status = tx2.fetch(&ionetcfg);
            if ( status != SQL_SUCCESS ) {
                tx2.close(&ionetcfg);
                os << " </BGQIODrawer>" << endl;
                continue;
            }

            strcpy(node_prev,ionetcfg._location);
            strcpy(intf_prev,ionetcfg._interface);

            os << "<BGQIONode location='" << ionetcfg._location << "' > " << endl;
            os << "<BGQNetConfig interface='"  << ionetcfg._interface << "' ";

            for (;status==SQL_SUCCESS;) {
                BGQDB::trim_right_spaces(ionetcfg._itemvalue);

                if (strcmp(node_prev,ionetcfg._location)!=0) {
                    os << " /> " << endl;
                    os << "</BGQIONode> " << endl;
                    os << "<BGQIONode location='" << ionetcfg._location << "' > " << endl;
                    os << "<BGQNetConfig interface='"  << ionetcfg._interface << "' ";
                    strcpy(intf_prev,ionetcfg._interface);
                }

                if (strcmp(intf_prev,ionetcfg._interface)!=0) {
                    os << " /> " << endl;
                    os << "<BGQNetConfig interface='"  << ionetcfg._interface << "' ";
                }

                os <<  ionetcfg._itemname << "='" << ionetcfg._itemvalue << "' ";
                strcpy(node_prev,ionetcfg._location);
                strcpy(intf_prev,ionetcfg._interface);
                status = tx2.fetch(&ionetcfg);
            }
            os << " /> " << endl;
            os << "</BGQIONode> " << endl;
            tx2.close(&ionetcfg);

            os << " </BGQIODrawer>" << endl;
        } while(next_line(tx, iodrawer));
    }


    // handle the compute torus cables
    condition = " order by  dim, sourcemid, destinationmid";
    memset(tlink._dim, 0, sizeof(tlink._dim));
    memset(tlink._destinationmid, 0, sizeof(tlink._destinationmid));
    memset(tlink._sourcemid, 0, sizeof(tlink._sourcemid));

    if ((result = retrieve_table(tx, tlink, condition)) == DB_OK) {
        do {
            BGQDB::trim_right_spaces((char *)tlink._dim, sizeof(tlink._dim));
            BGQDB::trim_right_spaces((char *)tlink._sourcemid, sizeof(tlink._sourcemid));
            BGQDB::trim_right_spaces((char *)tlink._destinationmid, sizeof(tlink._destinationmid));
            os << " <BGQCable axis='" << tlink._dim << "' sourceMidplane='" << tlink._sourcemid << "' destinationMidplane='" << tlink._destinationmid << "' ";
            if (tlink._badwires == 0)
                os << " />" << endl;
            else {
                os << " >" << endl;
                condition = " where substr(fromlocation,1,6) = '" + string(tlink._sourcemid) +
                    string("' and substr(tolocation,1,6) = '") + string(tlink._destinationmid) +
                    string("' and badwiremask <> 0 ");

                if ((result = retrieve_table(tx2, cable, condition)) == DB_OK) {
                    do {
                        os << "  <BGQBadWireMask  nodeBoard='" << string(&cable._fromlocation[7]).substr(0,3) << "' fromPort='" << &cable._fromlocation[11] << "' toPort='" << &cable._tolocation[11] << "' mask='" << cable._badwiremask << "'  />" << endl;
                    } while(next_line(tx2, cable));
                }
                os << " </BGQCable>" << endl;
            }
        } while(next_line(tx, tlink));
    }


    // handle the IO torus cables
    const std::vector<std::string> dims = boost::assign::list_of("A")("B")("C")("D");
    BOOST_FOREACH( const std::string& dim, dims ) {
        condition = " where source like '" + dim + "_%' and destination like '" + dim + "_%' ";
        if ((result = retrieve_table(tx, iolink, condition)) == DB_OK) {
            do {
                BGQDB::trim_right_spaces((char *)iolink._source, sizeof(iolink._source));
                BGQDB::trim_right_spaces((char *)iolink._destination, sizeof(iolink._destination));
                os << "  <BGQIOCable axis='" << dim << "' sourceIODrawer='" << &iolink._source[2] << "' destinationIODrawer='" << &iolink._destination[2] << "'>" << endl;

                condition = " where substr(fromlocation,1,6) = '" + string(iolink._source).substr(2) +
                    string("' and substr(tolocation,1,6) = '") + string(iolink._destination).substr(2) +
                    string("' and badwiremask <> 0 ");

                if ((result = retrieve_table(tx2, cable, condition)) == DB_OK) {
                    do {
                        os << "  <BGQBadWireMask  nodeBoard='' fromPort='";
                        os << &cable._fromlocation[7];
                        os << "' toPort='";
                        os << &cable._tolocation[7];
                        os << "' mask='" << cable._badwiremask;
                        os << "'  />" << endl;
                    } while(next_line(tx2, cable));
                }

                os << " </BGQIOCable>";
            } while(next_line(tx, iolink));
        }
    }

    // handle the compute to IO links
    condition = " order by 1,2";
    if ((result = retrieve_table(tx, cniolink, condition)) == DB_OK) {
        do {
            os << " <BGQIOLink computeNode='" << cniolink._source << "' ioNode='" << cniolink._ion << "' cnConnector='" << cniolink._cnconnector << "' ioConnector='" << &cniolink._destination[7] << "' ";
            if (!strcmp(cniolink._status,"A") && !strcmp(cniolink._ionstatus,"A")) {
                os << "enabled='true' ";
            } else {
                os << "enabled='false' ";
            }
            if (cniolink._badwires == 0)
                os << " />" << endl;
            else {
                os << " >" << endl;
                os << "  <BGQBadWireMask  fromPort='" << cniolink._cnconnector << "' toPort='" <<  &cniolink._destination[7] << "' mask='" << cniolink._badwires << "'  />" << endl;
                os << " </BGQIOLink>" << endl;
            }
        } while(next_line(tx, cniolink));
    }


    os << "</BGQMidplaneMachine>" << endl;
    os << endl;

    return DB_OK;
}

extract_db_status
extract_compact_block(
        std::ostream& os,
        const std::string& block,
        bool diags
)
{
    BGQDB::DBTBlock dbblock;
    BGQDB::DBTBpblockmap bp;
    BGQDB::DBTIoblockmap io;
    BGQDB::DBTSwitchblockmap swb;
    BGQDB::DBTSmallblock sb;
    BGQDB::DBTCable cable;
    BGQDB::ColumnsBitmap colBitmap;
    extract_db_status result;

    colBitmap.set(cable.TOLOCATION);
    cable._columns = colBitmap.to_ulong();

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if ( ! tx.getConnection() ) {
        LOG_ERROR_MSG(__FUNCTION__ << " No connection to database");
        return DB_COMM_ERR;
    }
    BGQDB::TxObject tx2(BGQDB::DBConnectionPool::Instance());
    if ( ! tx2.getConnection() ) {
        LOG_ERROR_MSG(__FUNCTION__ << " No connection to database");
        return DB_COMM_ERR;
    }

    char lastbp[17];
    string sqlstr;
    string condition("");
    string portCondition("");

    if ( !block.empty() ) {
        condition = " where blockId='" + block + string("'");
    } else {
        LOG_ERROR_MSG(__FUNCTION__ << " Block name not provided");
        return DB_NO_DATA;
    }

    if ((result = retrieve_table(tx, dbblock, condition)) != DB_OK) {
        if (result == DB_NO_DATA) {
            LOG_ERROR_MSG(__FUNCTION__ << " Block name " << block << " not found in database");
            return DB_NO_DATA;
        } else {
            // Error message is logged in retrieve_table
            return DB_ERROR;
        }
    }

    // zero posinmachine memory
    memset( sb._posinmachine, 0, sizeof( sb._posinmachine ) );

    if (dbblock._numionodes != 0) {             //   IO block
        condition = " where blockId='" + block + string("'");
        condition += " order by location";

        if (retrieve_table(tx, io, condition) == DB_OK) {
            BGQDB::trim_right_spaces(sb._posinmachine);
            os << "<BGQBlock name='" << dbblock._blockid << "' id='" << dbblock._creationid << "' " ;
            os << " numionodes='" << dbblock._numionodes << "'";

            os << " boot_options='"  << dbblock._bootoptions << "' ";

            if (dbblock._ind[dbblock.SECURITYKEY] != SQL_NULL_DATA) {
                os << " securityKey='" <<  bitDataToChar(dbblock._securitykey, sizeof(dbblock._securitykey))  << "'";
            }
            os << ">" << endl;

            do {
                BGQDB::trim_right_spaces(io._location);
                os << "  <BGQIONodes location='" << io._location << "'  />" << endl;

            } while(next_line(tx, sb));


            try {
                if ((dbblock._numionodes == 8) && (strstr(dbblock._bootoptions,"train_io_torus"))) {
                    os << " <BGQSwitch axis='A' include='F' enablePorts='B'/> " << endl;
                    os << " <BGQSwitch axis='B' include='F' enablePorts='B'/> " << endl;
                    os << " <BGQSwitch axis='C' include='F' enablePorts='B'/> " << endl;
                } else if (strstr(dbblock._bootoptions,"train_io_torus") && dbblock._numionodes > 8 ) {
                    if (strstr(dbblock._bootoptions,"io_torus_A"))
                        os << " <BGQSwitch axis='A' include='T' enablePorts='B'/> " << endl;
                    else
                        os << " <BGQSwitch axis='A' include='F' enablePorts='B'/> " << endl;

                    if (strstr(dbblock._bootoptions,"io_torus_B"))
                        os << " <BGQSwitch axis='B' include='T' enablePorts='B'/> " << endl;
                    else
                        os << " <BGQSwitch axis='B' include='F' enablePorts='B'/> " << endl;

                    if (strstr(dbblock._bootoptions,"io_torus_C"))
                        os << " <BGQSwitch axis='C' include='T' enablePorts='B'/> " << endl;
                    else
                        os << " <BGQSwitch axis='C' include='F' enablePorts='B'/> " << endl;
                }
            } catch ( const std::exception& e ) {
                LOG_WARN_MSG( e.what() );
            }
        }
    } else {                                  // Compute block, either large or small
        condition += " ORDER by acoord,bcoord,ccoord,dcoord";

        if (diags) {
            BGQDB::trim_right_spaces(dbblock._bootoptions);
            if (strlen(dbblock._bootoptions) == 0)
                strcpy(dbblock._bootoptions, "no_io_links");
            else
                strcat(dbblock._bootoptions, ",no_io_links");
        }

        if (retrieve_table(tx, bp, condition) == DB_OK) {     //  Compute block, large

            os << "<BGQBlock name='" << dbblock._blockid << "' id='" << dbblock._creationid << "' " ;

            os << " boot_options='"  << dbblock._bootoptions << "' ";

            if (dbblock._ind[dbblock.SECURITYKEY] != SQL_NULL_DATA) {
                os << " securityKey='" <<  bitDataToChar(dbblock._securitykey, sizeof(dbblock._securitykey))  << "'";
            }
            os << ">" << endl;

            do {
                BGQDB::trim_right_spaces(bp._bpid);
                os << "<BGQMidplane midplane='" << bp._bpid << "' aPos='" << bp._acoord
                << "' bPos='" << bp._bcoord
                << "' cPos='" << bp._ccoord
                << "' dPos='" << bp._dcoord   << "'>" << endl;

                os << "  <BGQComputeNodes board='" << ncstr[0] << "' />" << endl;
                os << "  <BGQComputeNodes board='" << ncstr[1] << "' />" << endl;
                os << "  <BGQComputeNodes board='" << ncstr[2] << "' />" << endl;
                os << "  <BGQComputeNodes board='" << ncstr[3] << "' />" << endl;
                os << "  <BGQComputeNodes board='" << ncstr[4] << "' />" << endl;
                os << "  <BGQComputeNodes board='" << ncstr[5] << "' />" << endl;
                os << "  <BGQComputeNodes board='" << ncstr[6] << "' />" << endl;
                os << "  <BGQComputeNodes board='" << ncstr[7] << "' />" << endl;
                os << "  <BGQComputeNodes board='" << ncstr[8] << "' />" << endl;
                os << "  <BGQComputeNodes board='" << ncstr[9] << "' />" << endl;
                os << "  <BGQComputeNodes board='" << ncstr[10] << "' />" << endl;
                os << "  <BGQComputeNodes board='" << ncstr[11] << "' />" << endl;
                os << "  <BGQComputeNodes board='" << ncstr[12] << "' />" << endl;
                os << "  <BGQComputeNodes board='" << ncstr[13] << "' />" << endl;
                os << "  <BGQComputeNodes board='" << ncstr[14] << "' />" << endl;
                os << "  <BGQComputeNodes board='" << ncstr[15] << "' />" << endl;

                condition = " where blockId='" + block + string("'");
                condition += " and switchid like '%";
                condition += bp._bpid;
                condition += "%' ORDER BY switchid";

                if (retrieve_table(tx2, swb, condition) == DB_OK) {
                    do {
                        BGQDB::trim_right_spaces(swb._switchid);

                        os << " <BGQSwitch axis='" << swb._switchid[0] << "' include='" << swb._include << "' enablePorts='" << swb._enableports << "'/>" << endl;

                    } while(next_line(tx2, swb));
                }

                os << "</BGQMidplane>" << endl;
            } while(next_line(tx, bp));

            condition = " where blockId='" + block + string("'");
            condition += " and substr(switchid,3) not in (select bpid from ";
            condition += bp.getTableName();
            condition += " where blockId='" + block + string("') order by substr(switchid,3)");

            portCondition = "where substr(fromlocation, 1,6) in (select bpid from bgqbpblockmap where blockid = '"
                +block + string("' )  and substr(tolocation,5,1) = 'I' ");

            strcpy(lastbp,"none");

            if (retrieve_table(tx2, swb, condition) == DB_OK) {
                do {
                    BGQDB::trim_right_spaces(swb._switchid);

                    if (strcmp(lastbp,&swb._switchid[2])!=0) {
                        if (strcmp(lastbp,"none")!=0) {
                            os << "</BGQMidplane>" << endl;
                        }

                        os << "<BGQMidplane midplane='" << &swb._switchid[2] << "' aPos='0' bPos='0' cPos='0' dPos='0'>" << endl;
                    }

                    os << " <BGQSwitch axis='" << swb._switchid[0] << "' include='" << swb._include << "' enablePorts='" << swb._enableports << "'/>" << endl;

                    strcpy(lastbp,&swb._switchid[2]);
                } while(next_line(tx2, swb));
                os << "</BGQMidplane>" << endl;
            }

        } else {                                    // Compute block, small
            condition = " where blockId='" + block + string("'");
            condition += " order by nodecardpos";

            portCondition = " where substr(fromlocation, 1, 10) in (select posinmachine || '-' || nodecardpos from bgqsmallblock where blockid = '"
                + block + string("' )  and substr(tolocation,5,1) = 'I' ");


            if (retrieve_table(tx, sb, condition) == DB_OK) {
                BGQDB::trim_right_spaces(sb._posinmachine);
                os << "<BGQBlock name='" << dbblock._blockid << "' id='" << dbblock._creationid << "' " ;

                os << " boot_options='"  << dbblock._bootoptions << "' ";

                if (dbblock._ind[dbblock.SECURITYKEY] != SQL_NULL_DATA) {
                    os << " securityKey='" <<  bitDataToChar(dbblock._securitykey, sizeof(dbblock._securitykey))  << "'";
                }

                os << ">" << endl;

                os << "<BGQMidplane midplane='" << sb._posinmachine << "'>" << endl;

                do {
                    BGQDB::trim_right_spaces(sb._nodecardpos);
                    os << "  <BGQComputeNodes board='" << sb._nodecardpos << "'  />" << endl;
                } while(next_line(tx, sb));

                int cNodes = dbblock._numcnodes;

                try {
                    bgq::utility::Properties::ConstPtr prop(BGQDB::DBConnectionPool::Instance().getProperties());
                    if ( !prop->getValue("database","forceTorus").empty() && cNodes == 32 ) {
                        os << " <BGQSwitch axis='A' include='F' enablePorts='B'/> " << endl;
                        os << " <BGQSwitch axis='B' include='F' enablePorts='B'/> " << endl;
                        os << " <BGQSwitch axis='C' include='F' enablePorts='B'/> " << endl;
                        os << " <BGQSwitch axis='D' include='F' enablePorts='B'/> " << endl;
                    }
                } catch ( const std::exception& e ) {
                    LOG_DEBUG_MSG( e.what() );
                }

                if (cNodes >= 64)
                    os << " <BGQSwitch axis='C' include='F' enablePorts='B'/> " << endl;
                if (cNodes >= 128)
                    os << " <BGQSwitch axis='D' include='F' enablePorts='B'/> " << endl;
                if (cNodes >= 256)
                    os << " <BGQSwitch axis='A' include='F' enablePorts='B'/> " << endl;

                os << "</BGQMidplane>" << endl;
            }
        }

        // add the IO ports to the block, since these will be used for arbitration
        tx2.query(&cable, portCondition.c_str());
        SQLRETURN rc = tx2.fetch(&cable);
        for (;rc==SQL_SUCCESS;) {
            os << "<BGQIOPort port='" << cable._tolocation << "' />" << endl;
            rc = tx2.fetch(&cable);
        }
        tx2.close(&cable);
    }

    os << "</BGQBlock>" << endl;
    os.flush();
    return DB_OK;
}

extract_db_status
extract_db_bplist(
        std::ostream &os
)
{
    BGQDB::DBVBpblockstatus bp;
    extract_db_status result;
    int mem, maxmem;
    string where, sqlstr;
    SQLLEN ind[3];
    SQLRETURN sqlrc, queryresult;
    SQLHANDLE hstmt;
    BGQDB::ColumnsBitmap colBitmap;
    bool uniformMem = false;

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if ( ! tx.getConnection() ) {
        LOG_ERROR_MSG(__FUNCTION__ << " No connection to database");
        return DB_COMM_ERR;
    }
    BGQDB::TxObject tx2(BGQDB::DBConnectionPool::Instance());
    if ( ! tx2.getConnection() ) {
        LOG_ERROR_MSG(__FUNCTION__ << " No connection to database");
        return DB_COMM_ERR;
    }

    string condition = " where machineserialNumber='BGQ'";
    if ((result = retrieve_table(tx, bp, condition)) != DB_OK) {
        if (result == DB_NO_DATA) {
            LOG_ERROR_MSG(__FUNCTION__ << " Database problem, no midplanes");
            return DB_NO_DATA;
        } else {
            // Error message is logged in retrieve_table
            return DB_ERROR;
        }
    }

    mem = 4096;   // Default is 4gb
    sqlstr =  "select  min(memorysize), max(memorysize) from bgqnode n ";
    sqlstr += " where  status = 'A'  ";
    queryresult = tx2.execQuery(sqlstr.c_str(), &hstmt);
    if (queryresult != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << queryresult << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    if ((sqlrc = SQLBindCol(hstmt, 1, SQL_INTEGER, &mem, 4, &ind[0])) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }
    if ((sqlrc = SQLBindCol(hstmt, 2, SQL_INTEGER, &maxmem, 4, &ind[1])) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = SQLFetch(hstmt);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    if (mem == maxmem) {
        uniformMem = true;
    }

    SQLCloseCursor(hstmt);

    os << "<BGQBPlist>" << endl;
    do {
        BGQDB::trim_right_spaces(bp._bpid);
        BGQDB::trim_right_spaces(bp._status);

        if (!uniformMem) {
            sqlstr =  "select  max(memorysize) from bgqnode n ";
            sqlstr += " where midplanepos = '" + string(bp._bpid);
            sqlstr += "' ";
            queryresult = tx2.execQuery(sqlstr.c_str(), &hstmt);
            if (queryresult != SQL_SUCCESS) {
                LOG_ERROR_MSG( "Database query failed with error: " << queryresult << " at " << __FUNCTION__ << ':' << __LINE__ );
                return DB_ERROR;
            }

            if ((sqlrc = SQLBindCol(hstmt, 1, SQL_INTEGER, &mem, 4, &ind[0])) != SQL_SUCCESS) {
                LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
                return DB_ERROR;
            }

            sqlrc = SQLFetch(hstmt);
            if (sqlrc != SQL_SUCCESS) {
                LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
                return DB_ERROR;
            }
            SQLCloseCursor(hstmt);
        }

        os << " <BGQBP"
           << " BPId='" <<  bp._bpid << "'"
           << " status='" << bp._status << "'"
           << " statusSeqID='" << bp._seqid << "'"
           << " computeNodeMemory='" << mem << "'"
           << " row='" << bp._row << "'"
           << " column='" << bp._column << "'"
           << " midplane='" << bp._midplane << "'"
           << " torusA='" << bp._torusa << "'"
           << " torusB='" << bp._torusb << "'"
           << " torusC='" << bp._torusc << "'"
           << " torusD='" << bp._torusd << "'" ;

        if (strncmp(bp._blockstatus,"F",1) == 0 || strncmp(bp._blockstatus," ",1) == 0) {
            where = " where midplanepos = '";
            where += bp._bpid;
            where += "'  ";
            int subdivNCs = tx2.count("BGQSmallBlockStatus",where.c_str());
            if (subdivNCs == 0)
                os << " inUse='N'";
            else
                os << " inUse='S'";
            os << " blockId='' ";
        } else {
            os << " inUse='Y' blockId='" << bp._blockid << "' ";
        }

        os << "/>" << endl;

    } while(next_line(tx, bp));
    os << "</BGQBPlist>" << endl;

    return DB_OK;
}

extract_db_status
extract_db_nodecards(
        std::ostream &os,
        const std::string& midplane
)
{
    int nodecount;

    SQLLEN ind[6];
    SQLRETURN sqlrc, queryresult;
    SQLHANDLE hstmt;

    char block[33];
    char posm[4];
    string sqlstr;
    char ncstatus[2];

    long long int seqID;

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if ( ! tx.getConnection() ) {
        LOG_ERROR_MSG(__FUNCTION__ << " No connection to database");
        return DB_COMM_ERR;
    }

    os << "<BGQNodeCards BPId='" << midplane << "' >" << endl;

    sqlstr = "select b.position,b.seqid,b.status,count(*), max(blockid)  from tbgqnodecard b left outer join bgqnode c on  c.midplanepos = b.midplanepos and c.nodecardpos = b.position  and c.status ='A' left outer join bgqsmallblockstatus d on b.position =d.nodecardpos and d.midplanepos = b.midplanepos  where  b.midplanepos  = '"
        + midplane + "'  group by b.position,b.seqid,b.status  order by 1";

    queryresult = tx.execQuery(sqlstr.c_str(), &hstmt);
    if (queryresult != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << queryresult << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    if ((sqlrc = SQLBindCol(hstmt, 1, SQL_CHAR, posm, 4, &ind[0])) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }
    if ((sqlrc = SQLBindCol(hstmt, 2, SQL_C_SBIGINT, &seqID, 8, &ind[1])) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }
    if ((sqlrc = SQLBindCol(hstmt, 3, SQL_CHAR, ncstatus, 2, &ind[2])) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }
    if ((sqlrc = SQLBindCol(hstmt, 4, SQL_INTEGER, &nodecount, 4, &ind[3])) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }
    if ((sqlrc = SQLBindCol(hstmt, 5, SQL_CHAR, block, 33, &ind[4])) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = SQLFetch(hstmt);
    bool isMetaState = false;

    for (unsigned psetnum = 0; psetnum < 16; psetnum++) {

        if (((sqlrc == SQL_SUCCESS)||(sqlrc == SQL_SUCCESS_WITH_INFO))  && (strcmp(posm,ncstr[psetnum]) == 0)) {

            isMetaState = false;
            if (nodecount < 32) {
                // If the node board status is not "E" in the database and we are setting it to "E" to
                // reflect unavailable compute nodes than set the meta-state indicator to true
                if (strcmp(ncstatus,"E") != 0) {
                    isMetaState = true;
                }
                strcpy(ncstatus,"E");
            }

            os << " <BGQNodeCard  NodeCardId='" << string(ncstr[psetnum])
               << "' status='" << ncstatus
               << "' statusSeqID='" << seqID
               << "' quarter='" << string(psetq[psetnum])
               << "' nodeCount='" << nodecount  << "' ";

            if (isMetaState) {
                os << " isMetaState='Y'";
            } else {
                os << " isMetaState='N'";
            }

            if (ind[4] == SQL_NULL_DATA) {
                os << " inUse='N' blockId=''  />" << endl;
            } else {
                BGQDB::trim_right_spaces(block);
                os << " inUse='Y' blockId='" << block << "' />" << endl;
            }

            sqlrc = SQLFetch(hstmt);

        } else {
            LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }
    }

    os << "</BGQNodeCards>" << endl;

    SQLCloseCursor(hstmt);
    return DB_OK;
}

extract_db_status
extract_db_bpwires(
        std::ostream &os
)
{

    BGQDB::DBVLinkblockstatus bp;
    extract_db_status result;
    BGQDB::STATUS result2;
    string condition;
    char linkid_prev[sizeof(bp._linkid)];

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if ( ! tx.getConnection() ) {
        LOG_ERROR_MSG(__FUNCTION__ << " No connection to database");
        return DB_COMM_ERR;
    }

    condition = " order by linkid, linkstatus  desc ";

    memset(bp._linkid,' ',sizeof(bp._linkid));
    memset(linkid_prev,'p',sizeof(bp._linkid));

    if ((result = retrieve_table(tx, bp, condition)) != DB_OK) {
        if (result == DB_NO_DATA) {
            uint32_t a, b, c, d, total_midplanes;
            // Special check to see if this is a single midplane system (1/2 rack) which would have no torus cables
            if ((result2 = BGQDB::getMachineBPSize(a, b, c, d)) == BGQDB::OK) {
                total_midplanes = a*b*c*d;
                // If we only have 1 midplane than return the XML without any torus cables
                if (total_midplanes == 1) {
                    os << "<BGQWirelist>" << endl;
                    os << "</BGQWirelist>" << endl;
                    return DB_OK;
                // Configs with more than 1 midplane should have torus cables
                } else {
                    LOG_ERROR_MSG(__FUNCTION__ << " Database problem, no midplane torus cables");
                    return DB_NO_DATA;
                }

            // Problem with getMachineBPSize() call so return the no midplane torus cables error
            } else {
                LOG_ERROR_MSG(__FUNCTION__ << " Database problem, no midplane torus cables");
                return DB_NO_DATA;
            }
        } else {
            // Error message is logged in retrieve_table
            return DB_ERROR;
        }
    }

    os << "<BGQWirelist>" << endl;
    do {
        BGQDB::trim_right_spaces((char *)bp._linkid, sizeof(bp._linkid));
        BGQDB::trim_right_spaces((char *)bp._source, sizeof(bp._source));
        BGQDB::trim_right_spaces((char *)bp._destination, sizeof(bp._destination));
        BGQDB::trim_right_spaces(bp._blockid);

        if (strcmp(bp._linkid,linkid_prev) != 0) {
            os << " <BGQWire";
            os << " wireId='" << bp._linkid << "'"
            << " status='" << bp._linkstatus << "'"
            << " source='" << bp._source << "'"
            << " destination='" << bp._destination << "'"
            << " blockId='" << bp._blockid << "'"
            << " blockStatus='" << bp._blockstatus << "'"
            << " blockStatusSeqID='" << bp._blockseqid << "'";
            if (bp._ind[BGQDB::DBVLinkblockstatus::LINKSEQID] != SQL_NULL_DATA) {
                os << " statusSeqID='" << bp._linkseqid << "'";
            }
            os << "/>" << endl;
        }

        strcpy(linkid_prev,bp._linkid);
        memset(bp._linkid,' ',sizeof(bp._linkid));

    } while(next_line(tx, bp));
    os << "</BGQWirelist>" << endl;

    return DB_OK;
}


extract_db_status
extract_db_iowires(
        std::ostream& os,
        const std::string& midplane
)
{
    BGQDB::DBVCniolink link;
    extract_db_status result;
    string condition;

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if ( ! tx.getConnection() ) {
        LOG_ERROR_MSG(__FUNCTION__ << " No connection to database");
        return DB_COMM_ERR;
    }
/* This version of the code only returned I/O links for I/O nodes with available I/O nodes
    if ( midplane != "*ALL") {
        condition = " where source like '" + midplane + string("%'  and ionstatus ='A'  order by source");
    } else {
        condition = " where ionstatus = 'A'   order by source";
    }
*/
    // Return I/O links for all I/O nodes including those not in (A)vailable state
    if ( midplane != "*ALL") {
        condition = " where source like '" + midplane + string("%'  order by source");
    } else {
        condition = " order by source";
    }

    if ((result = retrieve_table(tx, link, condition)) != DB_OK) {
        if (result == DB_NO_DATA) {
            // When retrieving single midplane and no I/O links are returned it can be bad midplane name or DB problem
            if ( midplane != "*ALL") {
                LOG_ERROR_MSG(__FUNCTION__ << " No I/O links found for midplane " << midplane);
            } else {
                // When retrieving all midplanes and no I/O links are returned it must be a DB problem
                LOG_ERROR_MSG(__FUNCTION__ << " No I/O links found in the database");
            }
            return DB_NO_DATA;
        } else {
            // Error message is logged in retrieve_table
            return DB_ERROR;
        }
    }

    os << "<BGQIOWirelist>" << endl;
    do {
        os << " <BGQIOWire";
        os << " source='" << link._source << "'"
        << " destination='" << link._ion << "'"
        << " status='" << link._status << "'"
        << " statusSeqID='" << link._seqid << "'"
        << " IONstatus='" << link._ionstatus << "'";
        os << "/>" << endl;

    } while(next_line(tx, link));
    os << "</BGQIOWirelist>" << endl;

    return DB_OK;
}

extract_db_status
extract_db_iolinks(
        std::ostream& os,
        const std::string& block
)
{
    BGQDB::DBVCniolink link;
    extract_db_status result;
    string condition;

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if ( ! tx.getConnection() ) {
        LOG_ERROR_MSG(__FUNCTION__ << " No connection to database");
        return DB_COMM_ERR;
    }

    // condition = " where ionstatus = 'A' and (substr(source,1,6) in (select bpid from bgqbpblockmap where blockid = '"
    condition = " where (substr(source,1,6) in (select bpid from bgqbpblockmap where blockid = '"
        + block
        + string("' )  or substr(source,1,10) in (select  posinmachine || '-' || nodecardpos from bgqsmallblock where blockid = '")
        + block
        + string("') )");

    if ((result = retrieve_table(tx, link, condition)) != DB_OK) {
        if (result == DB_NO_DATA) {
            LOG_ERROR_MSG(__FUNCTION__ << " No I/O links found for block " << block);
            return DB_NO_DATA;
        } else {
            // Error message is logged in retrieve_table
            return DB_ERROR;
        }
    }

    os << "<BGQIOWirelist blockId='" << block << "' >" << endl;
    do {
        os << " <BGQIOWire";
        os << " source='" << link._source << "'"
        << " destination='" << link._ion << "'"
        << " status='" << link._status << "'"
        << " statusSeqID='" << link._seqid << "'"
        << " IONstatus='" << link._ionstatus << "'";
        os << "/>" << endl;

    } while(next_line(tx, link));
    os << "</BGQIOWirelist>" << endl;

    return DB_OK;
}

extract_db_status
extract_db_switches(
        std::ostream &os
)
{
    BGQDB::DBTSwitch sw;
    BGQDB::DBVSwitchblockinuse sb;
    extract_db_status result;

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if ( ! tx.getConnection() ) {
        LOG_ERROR_MSG(__FUNCTION__ << " No connection to database");
        return DB_COMM_ERR;
    }
    BGQDB::TxObject tx2(BGQDB::DBConnectionPool::Instance());
    if ( ! tx2.getConnection() ) {
        LOG_ERROR_MSG(__FUNCTION__ << " No connection to database");
        return DB_COMM_ERR;
    }

    string condition = " where machineserialNumber='BGQ' order by switchid";
    if ((result = retrieve_table(tx, sw, condition)) != DB_OK) {
        if (result == DB_NO_DATA) {
            LOG_ERROR_MSG(__FUNCTION__ << " Database problem, no switches");
            return DB_NO_DATA;
        } else {
            // Error message is logged in retrieve_table
            return DB_ERROR;
        }
    }

    condition = " where blockstatus <> ' ' order by switchid";
    retrieve_table(tx2, sb, condition);

    BGQDB::trim_right_spaces(sb._switchid);

    os << "<BGQSwitchlist>" << endl;
    do {
        BGQDB::trim_right_spaces(sw._switchid);
        os << " <BGQSwitch";
        os << " switchId='" << sw._switchid << "'"
        << " bpId='" <<  &sw._switchid[2] << "'"
        << " dimension='" << sw._dimension << "'"
        << " status='" << sw._status << "'"
        << " statusSeqID='" << sw._seqid << "'"
        << ">" << endl;

        for(; ((result==DB_OK) && (strcmp(sb._switchid,sw._switchid)==0)) ;)
        {
            BGQDB::trim_right_spaces(sb._include);
            BGQDB::trim_right_spaces(sb._enableports);
            BGQDB::trim_right_spaces(sb._blockid);
            BGQDB::trim_right_spaces(sb._blockstatus);
            BGQDB::trim_right_spaces(sb._switchid);

            os << "   <BGQSwitchSetting"
               << " switchId='" << sb._switchid << "'"
               << " include='" << sb._include << "'"
               << " enablePorts='" << sb._enableports << "'"
               << " blockId='" << sb._blockid << "'"
               << " blockStatus='" << sb._blockstatus << "'"
               << " blockStatusSeqID='" << sb._blockseqid << "'"
               << " passThru='" << sb._passthru << "'"
               << "/>" << endl;

            SQLRETURN sqlrc = tx2.fetch(&sb);
            if (sqlrc != SQL_SUCCESS) {
                if (sqlrc == SQL_NO_DATA_FOUND) {
                    result = DB_NO_DATA;
                } else {
                    result = DB_ERROR;
                }
            }
            BGQDB::trim_right_spaces(sb._switchid);
        }

        os << " </BGQSwitch>" << endl;

    } while(next_line(tx, sw));
    os << "</BGQSwitchlist>" << endl;

    return DB_OK;
}

extract_db_status
retrieve_table(
        BGQDB::TxObject& tx,
        BGQDB::DBObj& tableObject,
        const std::string& whereclause
)
{
    SQLRETURN sqlrc;
    SET_ALL_COLUMN(tableObject);
    sqlrc = tx.query(&tableObject, whereclause.c_str());
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }
    sqlrc = tx.fetch(&tableObject);
    if (sqlrc == SQL_SUCCESS) {
        return DB_OK;
    }
    if (sqlrc == SQL_NO_DATA_FOUND) {
        return DB_NO_DATA;
    } else {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }
}

uint32_t
next_line(
        BGQDB::TxObject& tx,
        BGQDB::DBObj& tableObject
)
{
    SQLRETURN rc = tx.fetch(&tableObject);
    if (rc == SQL_SUCCESS || rc== SQL_SUCCESS_WITH_INFO) {
        return 1;
    }

    return 0;
}
