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

// Note that all the tests are run only if the database has been populated with a 2x4x2x2 machine.

#include "api/BGQDBlib.h"
#include "api/Exception.h"
#include "api/genblock.h"
#include "api/GenBlockParams.h"

#include "api/tableapi/DBConnectionPool.h"
#include "api/tableapi/TxObject.h"

#include "api/tableapi/gensrc/bgqtableapi.h"

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE genblock
#include <boost/test/unit_test.hpp>

using namespace boost::assign; // bring 'operator+=()' into scope

using BGQDB::Connectivity;
using BGQDB::DimensionSpec;
using BGQDB::DimensionSpecs;
using BGQDB::GenBlockParams;

using std::cout;
using std::invalid_argument;
using std::logic_error;
using std::map;
using std::ostringstream;
using std::runtime_error;
using std::set;
using std::string;
using std::vector;


struct BlockInfo
{
    string istorus;
    uint64_t numcnodes;
    uint64_t sizea, sizeb, sizec, sized;

    BlockInfo(
            const string& istorus = "11111",
            uint64_t numcnodes_in = 0,
            uint64_t sizea_in = 0,
            uint64_t sizeb_in = 0,
            uint64_t sizec_in = 0,
            uint64_t sized_in = 0
        ) :
            istorus(istorus),
            numcnodes(numcnodes_in),
            sizea(sizea_in),sizeb(sizeb_in),sizec(sizec_in),sized(sized_in)
    { /* Nothing to do */ }
};


static bool checkBlockInfo( const string& block_id, const BlockInfo& info )
{
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());

    BGQDB::DBTBlock b;
    b._columns = -1;

    tx.query( &b, (string(" where blockId='") + block_id + "'").c_str() );

    tx.fetch( &b );

    BlockInfo db_block_info(
            b._istorus,
            b._numcnodes,
            b._sizea,
            b._sizeb,
            b._sizec,
            b._sized
        );

    bool ret(true);

    if ( db_block_info.istorus != info.istorus ) {
        cout << "Block istorus=" << db_block_info.istorus << " doesn't match expected " << info.istorus << "\n";
        ret = false;
    }

    if ( (info.numcnodes != 0) && (db_block_info.numcnodes != info.numcnodes) ) {
        cout << "Block numcnodes=" << db_block_info.numcnodes << " doesn't match expected " << info.numcnodes << "\n";
        ret = false;
    }

    if ( (info.sizea != 0) && (db_block_info.sizea != info.sizea) ) {
        cout << "Block sizea=" << db_block_info.sizea << " doesn't match expected " << info.sizea << "\n";
        ret = false;
    }
    if ( (info.sizeb != 0) && (db_block_info.sizeb != info.sizeb) ) {
        cout << "Block sizeb=" << db_block_info.sizeb << " doesn't match expected " << info.sizeb << "\n";
        ret = false;
    }
    if ( (info.sizec != 0) && (db_block_info.sizec != info.sizec) ) {
        cout << "Block sizec=" << db_block_info.sizec << " doesn't match expected " << info.sizec << "\n";
        ret = false;
    }
    if ( (info.sized != 0) && (db_block_info.sized != info.sized) ) {
        cout << "Block sized=" << db_block_info.sized << " doesn't match expected " << info.sized << "\n";
        ret = false;
    }

    return ret;
}


struct Coords
{
    int acoord, bcoord, ccoord, dcoord;

    Coords() {}

    Coords( int acoord, int bcoord, int ccoord, int dcoord ) :
        acoord(acoord), bcoord(bcoord), ccoord(ccoord), dcoord(dcoord)
    {}
};

typedef map<string,Coords> BpBlockMapInfo;

static bool checkBpBlockMap( const string& block_id, BpBlockMapInfo info )
{
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());

    BGQDB::DBTBpblockmap bpb;
    bpb._columns = -1;

    tx.query( &bpb, (string(" where blockId='") + block_id + "'").c_str() );

    while ( tx.fetch( &bpb ) == SQL_SUCCESS ) {
        BpBlockMapInfo::iterator i(info.find( bpb._bpid ));

        if ( i == info.end() ) {
            cout << "Unexpected bpid in bpblockmap: " << bpb._bpid << "\n";
            return false;
        }

        if ( i->second.acoord != bpb._acoord ||
             i->second.bcoord != bpb._bcoord ||
             i->second.ccoord != bpb._ccoord ||
             i->second.dcoord != bpb._dcoord ) {
            cout << "Coordinates for " << bpb._bpid << " didn't match."
                    " (" << bpb._acoord << "," << bpb._bcoord << "," << bpb._ccoord << "," << bpb._dcoord << ") !="
                    " (" << i->second.acoord << "," << i->second.bcoord << "," << i->second.ccoord << "," << i->second.dcoord << ")\n";
            return false;
        }

        info.erase( i );
    }

    if ( ! info.empty() ) {
        cout << "Didn't find " << info.begin()->first << " in bpblockmap.\n";
        return false;
    }

    return true;
}


struct SwitchSetting
{
    char include;
    char enablePorts;

    SwitchSetting() {}

    SwitchSetting( char include, char enablePorts ) : include(include), enablePorts(enablePorts) {}
};


typedef map<string,SwitchSetting> SwitchBlockMapInfo;

static void addSwitches( SwitchBlockMapInfo &sbi_in_out, const string& mp, const SwitchSetting& setting )
{
    sbi_in_out["A_" + mp] = setting;
    sbi_in_out["B_" + mp] = setting;
    sbi_in_out["C_" + mp] = setting;
    sbi_in_out["D_" + mp] = setting;
}


static bool checkSwitchBlockMap( const string& block_id, SwitchBlockMapInfo info )
{
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());

    BGQDB::DBTSwitchblockmap sb;
    sb._columns = -1;

    tx.query( &sb, (string(" where blockId='") + block_id + "'").c_str() );

    bool any_fails(false);

    while ( tx.fetch( &sb ) == SQL_SUCCESS ) {
        SwitchBlockMapInfo::iterator i(info.find( sb._switchid ));

        if ( i == info.end() ) {
            cout << "Unexpected switch in switchBlockMap: " << sb._switchid << "\n";
            any_fails = true;
            continue;
        }

        if ( i->second.include != sb._include[0] ||
             i->second.enablePorts != sb._enableports[0] ) {
            cout << "Info for " << sb._switchid << " didn't match."
                    " (" << sb._include << "," << sb._enableports << ") !="
                    " (" << i->second.include << "," << i->second.enablePorts << ")\n";
            any_fails = true;
        }

        info.erase( i );
    }


    for ( SwitchBlockMapInfo::const_iterator i(info.begin()) ; i != info.end() ; ++i ) {
        cout << "Didn't find " << i->first << " in switchblockmap.\n";
        any_fails = true;
    }

    if ( any_fails )  return false;

    return true;
}


typedef set<string> Links;


static void addLinks( Links& l_in_out, const string& mp )
{
    l_in_out.insert(mp + "A_" + mp);
    l_in_out.insert("A_" + mp + mp);
    l_in_out.insert(mp + "B_" + mp);
    l_in_out.insert("B_" + mp + mp);
    l_in_out.insert(mp + "C_" + mp);
    l_in_out.insert("C_" + mp + mp);
    l_in_out.insert(mp + "D_" + mp);
    l_in_out.insert("D_" + mp + mp);
}


static bool checkLinkBlockMap( const string& block_id, Links links )
{
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());

    BGQDB::DBTLinkblockmap lb;
    lb._columns = -1;

    tx.query( &lb, (string(" where blockId='") + block_id + "'").c_str() );

    while ( tx.fetch( &lb ) == SQL_SUCCESS ) {
        Links::iterator i(links.find( lb._linkid ));

        if ( i == links.end() ) {
            cout << "Unexpected link in linkBlockMap: " << lb._linkid << "\n";
            return false;
        }

        links.erase( i );
    }

    if ( ! links.empty() ) {
        cout << "Didn't find " << *(links.begin()) << " in linkblockmap.\n";
        return false;
    }

    return true;
}


static bool check(
        const std::string& block_id,
        const BpBlockMapInfo& bpi,
        const SwitchBlockMapInfo& sbi,
        const Links& links,
        const BlockInfo& bi = BlockInfo()
    )
{
    bool ret(true);
    ret = checkBlockInfo( block_id, bi ) && ret;
    ret = checkBpBlockMap( block_id, bpi ) && ret;
    ret = checkSwitchBlockMap( block_id, sbi ) && ret;
    ret = checkLinkBlockMap( block_id, links ) && ret;
    return ret;
}


struct BGQDBInfo {
    BGQDB::DimensionSizes machine_size;
    bool is_2422;
    bool is_4344;

    BGQDBInfo() {
        bgq::utility::Properties::Ptr properties_ptr(bgq::utility::Properties::create());
        bgq::utility::initializeLogging( *properties_ptr );
        BGQDB::init( properties_ptr, 1 );

        BGQDB::STATUS db_status(BGQDB::getMachineBPSize( machine_size ));

        if ( db_status != BGQDB::OK ) {
            throw runtime_error( string("getMachineBPSize failed with ") + boost::lexical_cast<string>(db_status) );
        }

        std::cout << "Machine size is " << machine_size << "\n";

        is_2422 = (machine_size[BGQDB::Dimension::A] == 2) &&
                  (machine_size[BGQDB::Dimension::B] == 4) &&
                  (machine_size[BGQDB::Dimension::C] == 2) &&
                  (machine_size[BGQDB::Dimension::D] == 2);

        is_4344 = (machine_size[BGQDB::Dimension::A] == 4) &&
                  (machine_size[BGQDB::Dimension::B] == 3) &&
                  (machine_size[BGQDB::Dimension::C] == 4) &&
                  (machine_size[BGQDB::Dimension::D] == 4);

    }
};


static BGQDBInfo bgqdb_info;


BOOST_AUTO_TEST_CASE( test_no_id )
{
    GenBlockParams params;

    params.setMidplane( "R00-M0" );
    params.setDimensionSpecs( BGQDB::MidplaneDimensionSpecs );
    params.setOwner( "unittest" );

    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::INVALID_ARG );
}


BOOST_AUTO_TEST_CASE( test_no_midplane )
{
    GenBlockParams params;

    params.setBlockId( "TESTGB1" );
    params.setOwner( "unittest" );

    params.setDimensionSpecs( BGQDB::MidplaneDimensionSpecs );

    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::INVALID_ARG );
}


BOOST_AUTO_TEST_CASE( test_no_owner )
{
    // If don't set the owner then genBlock fails.

    GenBlockParams params;

    params.setBlockId( "TESTNOOWN" );
    params.setMidplane( "R00-M0" );

    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::INVALID_ARG );
}


BOOST_AUTO_TEST_CASE( test_R000_and_duplicate )
{
    GenBlockParams params;

    params.setBlockId( "TESTGB3" );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M0" );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );

    Links l;
    addLinks( l, "R00-M0" );

    BOOST_CHECK( check( "TESTGB3", bpi, sbi, l,
            BlockInfo( "11111", 512, 4, 4, 4, 4 )
        ) );

    // might as well also test if block already exists?
    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::DUPLICATE );

    BGQDB::deleteBlock( "TESTGB3" );
}


BOOST_AUTO_TEST_CASE( test_R001 )
{
    GenBlockParams params;

    params.setBlockId( "TESTGB5" );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M1" );

    params.setDimensionSpecs( BGQDB::MidplaneDimensionSpecs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M1"] = Coords( 0, 0, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M1", SwitchSetting( 'F', 'B' ) );

    Links l;
    addLinks( l, "R00-M1" );

    BOOST_CHECK( check( "TESTGB5", bpi, sbi, l,
            BlockInfo( "11111", 512, 4, 4, 4, 4 )
         ) );

    BGQDB::deleteBlock( "TESTGB5" );
}


BOOST_AUTO_TEST_CASE( test_1112 )
{
    if ( bgqdb_info.machine_size[BGQDB::Dimension::D] != 2 )  { cout << "skipping test because D is not 2.\n"; return; }

    GenBlockParams params;

    params.setBlockId( "TESTGB6" );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M0" );

    DimensionSpecs dim_specs = { {
            DimensionSpec( 1 ),
            DimensionSpec( 1 ),
            DimensionSpec( 1 ),
            DimensionSpec( 2 )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R00-M1"] = Coords( 0, 0, 0, 1 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["D_R00-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R00-M1", SwitchSetting( 'F', 'B' ) );
    sbi["D_R00-M1"] = SwitchSetting( 'T', 'B' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R00-M1" );
    l.insert( "D_R00-M0D_R00-M1" );
    l.insert( "D_R00-M1D_R00-M0" );

    BOOST_CHECK( check( "TESTGB6", bpi, sbi, l,
            BlockInfo( "11111", 512 * 2, 4, 4, 4, 8 )
          ) );

    BGQDB::deleteBlock( "TESTGB6" );
}


BOOST_AUTO_TEST_CASE( test_1121 )
{
    if ( bgqdb_info.machine_size[BGQDB::Dimension::C] != 2 )  { cout << "skipping test because C is not 2.\n"; return; }

    GenBlockParams params;

    params.setBlockId( "TESTGB7" );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M0" );

    DimensionSpecs dim_specs = { {
            DimensionSpec( 1 ),
            DimensionSpec( 1 ),
            DimensionSpec( 2 ),
            DimensionSpec( 1 )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R01-M0"] = Coords( 0, 0, 1, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["C_R00-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R01-M0", SwitchSetting( 'F', 'B' ) );
    sbi["C_R01-M0"] = SwitchSetting( 'T', 'B' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R01-M0" );
    l.insert( "C_R00-M0C_R01-M0" );
    l.insert( "C_R01-M0C_R00-M0" );

    BOOST_CHECK( check( "TESTGB7", bpi, sbi, l,
            BlockInfo( "11111", 512*2, 4, 4, 4*2, 4 )
          ) );

    BGQDB::deleteBlock( "TESTGB7" );
}


BOOST_AUTO_TEST_CASE( test_1411 )
{
    if ( bgqdb_info.machine_size[BGQDB::Dimension::B] != 4 )  { cout << "skipping test because B is not 4.\n"; return; }

    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    GenBlockParams params;

    params.setBlockId( "TESTGB8" );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M0" );

    DimensionSpecs dim_specs = { {
            DimensionSpec( 1 ),
            DimensionSpec( 4 ),
            DimensionSpec( 1 ),
            DimensionSpec( 1 )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R20-M0"] = Coords( 0, 1, 0, 0 );
    bpi["R30-M0"] = Coords( 0, 2, 0, 0 );
    bpi["R10-M0"] = Coords( 0, 3, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R00-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R10-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R10-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R30-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R30-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R20-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R20-M0"] = SwitchSetting( 'T', 'B' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R10-M0" );
    addLinks( l, "R30-M0" );
    addLinks( l, "R20-M0" );
    l.insert( "B_R00-M0B_R20-M0" );
    l.insert( "B_R20-M0B_R30-M0" );
    l.insert( "B_R30-M0B_R10-M0" );
    l.insert( "B_R10-M0B_R00-M0" );

    BOOST_CHECK( check( "TESTGB8", bpi, sbi, l,
            BlockInfo( "11111", 512*4, 4, 4*4, 4, 4 )
           ) );

    BGQDB::deleteBlock( "TESTGB8" );
}


BOOST_AUTO_TEST_CASE( test_2111 )
{
    if ( bgqdb_info.machine_size[BGQDB::Dimension::A] != 2 )  { cout << "skipping test because A is not 2.\n"; return; }

    GenBlockParams params;

    params.setBlockId( "TESTGB9" );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M0" );

    DimensionSpecs dim_specs = { {
            DimensionSpec( 2 ),
            DimensionSpec( 1 ),
            DimensionSpec( 1 ),
            DimensionSpec( 1 )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R02-M0"] = Coords( 1, 0, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["A_R00-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R02-M0", SwitchSetting( 'F', 'B' ) );
    sbi["A_R02-M0"] = SwitchSetting( 'T', 'B' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R02-M0" );
    l.insert( "A_R00-M0A_R02-M0" );
    l.insert( "A_R02-M0A_R00-M0" );

    BOOST_CHECK( check( "TESTGB9", bpi, sbi, l,
            BlockInfo( "11111", 512*2, 4*2, 4, 4, 4 )
           ) );

    BGQDB::deleteBlock( "TESTGB9" );
}


BOOST_AUTO_TEST_CASE( test_2422 )
{
    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    GenBlockParams params;

    params.setBlockId( "TESTGB10" );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M0" );

    params.setDimensionSpecs( BGQDB::FullDimensionSpecs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R00-M1"] = Coords( 0, 0, 0, 1 );
    bpi["R01-M0"] = Coords( 0, 0, 1, 0 );
    bpi["R01-M1"] = Coords( 0, 0, 1, 1 );
    bpi["R02-M0"] = Coords( 1, 0, 0, 0 );
    bpi["R02-M1"] = Coords( 1, 0, 0, 1 );
    bpi["R03-M0"] = Coords( 1, 0, 1, 0 );
    bpi["R03-M1"] = Coords( 1, 0, 1, 1 );

    bpi["R20-M0"] = Coords( 0, 1, 0, 0 );
    bpi["R20-M1"] = Coords( 0, 1, 0, 1 );
    bpi["R21-M0"] = Coords( 0, 1, 1, 0 );
    bpi["R21-M1"] = Coords( 0, 1, 1, 1 );
    bpi["R22-M0"] = Coords( 1, 1, 0, 0 );
    bpi["R22-M1"] = Coords( 1, 1, 0, 1 );
    bpi["R23-M0"] = Coords( 1, 1, 1, 0 );
    bpi["R23-M1"] = Coords( 1, 1, 1, 1 );

    bpi["R30-M0"] = Coords( 0, 2, 0, 0 );
    bpi["R30-M1"] = Coords( 0, 2, 0, 1 );
    bpi["R31-M0"] = Coords( 0, 2, 1, 0 );
    bpi["R31-M1"] = Coords( 0, 2, 1, 1 );
    bpi["R32-M0"] = Coords( 1, 2, 0, 0 );
    bpi["R32-M1"] = Coords( 1, 2, 0, 1 );
    bpi["R33-M0"] = Coords( 1, 2, 1, 0 );
    bpi["R33-M1"] = Coords( 1, 2, 1, 1 );

    bpi["R10-M0"] = Coords( 0, 3, 0, 0 );
    bpi["R10-M1"] = Coords( 0, 3, 0, 1 );
    bpi["R11-M0"] = Coords( 0, 3, 1, 0 );
    bpi["R11-M1"] = Coords( 0, 3, 1, 1 );
    bpi["R12-M0"] = Coords( 1, 3, 0, 0 );
    bpi["R12-M1"] = Coords( 1, 3, 0, 1 );
    bpi["R13-M0"] = Coords( 1, 3, 1, 0 );
    bpi["R13-M1"] = Coords( 1, 3, 1, 1 );

    BOOST_CHECK( checkBpBlockMap( "TESTGB10", bpi ) );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R00-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R01-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R01-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R02-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R02-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R03-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R03-M1", SwitchSetting( 'T', 'B' ) );

    addSwitches( sbi, "R10-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R10-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R11-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R11-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R12-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R12-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R13-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R13-M1", SwitchSetting( 'T', 'B' ) );

    addSwitches( sbi, "R20-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R20-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R21-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R21-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R22-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R22-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R23-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R23-M1", SwitchSetting( 'T', 'B' ) );

    addSwitches( sbi, "R30-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R30-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R31-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R31-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R32-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R32-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R33-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R33-M1", SwitchSetting( 'T', 'B' ) );

    BOOST_CHECK( checkSwitchBlockMap( "TESTGB10", sbi ) );

    BGQDB::deleteBlock( "TESTGB10" );
}


BOOST_AUTO_TEST_CASE( test_passthru_at_end )
{
    if ( bgqdb_info.machine_size[BGQDB::Dimension::B] != 4 )  { cout << "skipping test because B is not 4.\n"; return; }

    GenBlockParams params;

    params.setBlockId( "TESTGB11" );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M0" );

    DimensionSpecs dim_specs = { {
            DimensionSpec( 1 ),
            DimensionSpec( ":1110" ),
            DimensionSpec( 1 ),
            DimensionSpec( 1 )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R20-M0"] = Coords( 0, 1, 0, 0 );
    bpi["R30-M0"] = Coords( 0, 2, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R00-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R20-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R20-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R30-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R30-M0"] = SwitchSetting( 'T', 'B' );
    sbi["B_R10-M0"] = SwitchSetting( 'F', 'B' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R20-M0" );
    addLinks( l, "R30-M0" );
    l.insert( "B_R00-M0B_R20-M0" );
    l.insert( "B_R20-M0B_R30-M0" );
    l.insert( "B_R30-M0B_R10-M0" );
    l.insert( "B_R10-M0B_R00-M0" );

    BOOST_CHECK( check( "TESTGB11", bpi, sbi, l,
            BlockInfo( "11111", 512*3, 4, 4*3, 4, 4 )
           ) );

    BGQDB::deleteBlock( "TESTGB11" );
}


BOOST_AUTO_TEST_CASE( test_passthru_2nd )
{
    if ( bgqdb_info.machine_size[BGQDB::Dimension::B] != 4 )  { cout << "skipping test because B is not 4.\n"; return; }

    GenBlockParams params;

    params.setBlockId( "TESTGB12" );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M0" );

    DimensionSpecs dim_specs = { {
            DimensionSpec( 1 ),
            DimensionSpec( ":1011" ),
            DimensionSpec( 1 ),
            DimensionSpec( 1 )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R30-M0"] = Coords( 0, 1, 0, 0 );
    bpi["R10-M0"] = Coords( 0, 2, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R00-M0"] = SwitchSetting( 'T', 'B' );
    sbi["B_R20-M0"] = SwitchSetting( 'F', 'B' );
    addSwitches( sbi, "R30-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R30-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R10-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R10-M0"] = SwitchSetting( 'T', 'B' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R30-M0" );
    addLinks( l, "R10-M0" );
    l.insert( "B_R00-M0B_R20-M0" );
    l.insert( "B_R20-M0B_R30-M0" );
    l.insert( "B_R30-M0B_R10-M0" );
    l.insert( "B_R10-M0B_R00-M0" );

    BOOST_CHECK( check( "TESTGB12", bpi, sbi, l,
            BlockInfo( "11111", 512*3, 4, 4*3, 4, 4 )
           ) );

    BGQDB::deleteBlock( "TESTGB12" );
}


BOOST_AUTO_TEST_CASE( test_passthru_2_passthroughs )
{
    if ( bgqdb_info.machine_size[BGQDB::Dimension::B] != 4 )  { cout << "skipping test because B is not 4.\n"; return; }

    GenBlockParams params;

    params.setBlockId( "TESTGB12" );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M0" );

    DimensionSpecs dim_specs = { {
            DimensionSpec( 1 ),
            DimensionSpec( ":1010" ),
            DimensionSpec( 1 ),
            DimensionSpec( 1 )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R30-M0"] = Coords( 0, 1, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R00-M0"] = SwitchSetting( 'T', 'B' );
    sbi["B_R10-M0"] = SwitchSetting( 'F', 'B' );
    addSwitches( sbi, "R30-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R30-M0"] = SwitchSetting( 'T', 'B' );
    sbi["B_R20-M0"] = SwitchSetting( 'F', 'B' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R30-M0" );
    l.insert( "B_R00-M0B_R20-M0" );
    l.insert( "B_R20-M0B_R30-M0" );
    l.insert( "B_R30-M0B_R10-M0" );
    l.insert( "B_R10-M0B_R00-M0" );

    BOOST_CHECK( check( "TESTGB12", bpi, sbi, l,
            BlockInfo( "11111", 512*2, 4, 4*2, 4, 4 )
            ) );

    BGQDB::deleteBlock( "TESTGB12" );
}


BOOST_AUTO_TEST_CASE( test_passthru_2 )
{
    if ( bgqdb_info.machine_size[BGQDB::Dimension::B] != 4 )  { cout << "skipping test because B is not 4.\n"; return; }

    GenBlockParams params;

    params.setBlockId( "TESTGB13" );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M0" );

    DimensionSpecs dim_specs = { {
            DimensionSpec( 1 ),
            DimensionSpec( ":1001" ),
            DimensionSpec( 1 ),
            DimensionSpec( 1 )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R10-M0"] = Coords( 0, 1, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R00-M0"] = SwitchSetting( 'T', 'B' );
    sbi["B_R20-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R30-M0"] = SwitchSetting( 'F', 'B' );
    addSwitches( sbi, "R10-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R10-M0"] = SwitchSetting( 'T', 'B' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R10-M0" );
    l.insert( "B_R00-M0B_R20-M0" );
    l.insert( "B_R20-M0B_R30-M0" );
    l.insert( "B_R30-M0B_R10-M0" );
    l.insert( "B_R10-M0B_R00-M0" );

    BOOST_CHECK( check( "TESTGB13", bpi, sbi, l,
            BlockInfo( "11111", 512*2, 4, 4*2, 4, 4 )
            ) );

    BGQDB::deleteBlock( "TESTGB13" );
}


BOOST_AUTO_TEST_CASE( test_2222_passthru_1010 )
{
    if ( bgqdb_info.machine_size[BGQDB::Dimension::B] != 4 )  { cout << "skipping test because B is not 4.\n"; return; }

    GenBlockParams params;

    params.setBlockId( "TESTGB14" );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M0" );

    DimensionSpecs dim_specs = { {
            DimensionSpec( 2 ),
            DimensionSpec( ":1010" ),
            DimensionSpec( 2 ),
            DimensionSpec( 2 )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R00-M1"] = Coords( 0, 0, 0, 1 );
    bpi["R01-M0"] = Coords( 0, 0, 1, 0 );
    bpi["R01-M1"] = Coords( 0, 0, 1, 1 );
    bpi["R02-M0"] = Coords( 1, 0, 0, 0 );
    bpi["R02-M1"] = Coords( 1, 0, 0, 1 );
    bpi["R03-M0"] = Coords( 1, 0, 1, 0 );
    bpi["R03-M1"] = Coords( 1, 0, 1, 1 );
    bpi["R30-M0"] = Coords( 0, 1, 0, 0 );
    bpi["R30-M1"] = Coords( 0, 1, 0, 1 );
    bpi["R31-M0"] = Coords( 0, 1, 1, 0 );
    bpi["R31-M1"] = Coords( 0, 1, 1, 1 );
    bpi["R32-M0"] = Coords( 1, 1, 0, 0 );
    bpi["R32-M1"] = Coords( 1, 1, 0, 1 );
    bpi["R33-M0"] = Coords( 1, 1, 1, 0 );
    bpi["R33-M1"] = Coords( 1, 1, 1, 1 );

    BOOST_CHECK( checkBpBlockMap( "TESTGB14", bpi ) );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R00-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R01-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R01-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R02-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R02-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R03-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R03-M1", SwitchSetting( 'T', 'B' ) );

    sbi["B_R10-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R10-M1"] = SwitchSetting( 'F', 'B' );
    sbi["B_R11-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R11-M1"] = SwitchSetting( 'F', 'B' );
    sbi["B_R12-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R12-M1"] = SwitchSetting( 'F', 'B' );
    sbi["B_R13-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R13-M1"] = SwitchSetting( 'F', 'B' );

    sbi["B_R20-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R20-M1"] = SwitchSetting( 'F', 'B' );
    sbi["B_R21-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R21-M1"] = SwitchSetting( 'F', 'B' );
    sbi["B_R22-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R22-M1"] = SwitchSetting( 'F', 'B' );
    sbi["B_R23-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R23-M1"] = SwitchSetting( 'F', 'B' );

    addSwitches( sbi, "R30-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R30-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R31-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R31-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R32-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R32-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R33-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R33-M1", SwitchSetting( 'T', 'B' ) );

    BOOST_CHECK( checkSwitchBlockMap( "TESTGB14", sbi ) );

    BGQDB::deleteBlock( "TESTGB14" );
}


BOOST_AUTO_TEST_CASE( test_torus_invalid_no_passthrough )
{
    if ( bgqdb_info.machine_size[BGQDB::Dimension::B] <= 2 )  { cout << "skipping test because B <= 2.\n"; return; }

    GenBlockParams params;

    params.setBlockId( "TESTGB15" );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M0" );

    DimensionSpecs dim_specs = { {
            DimensionSpec( 2 ),
            DimensionSpec( 2 ),
            DimensionSpec( 2 ),
            DimensionSpec( 2 )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::INVALID_ARG );

    BGQDB::deleteBlock( "TESTGB15" ); // just in case!
}


BOOST_AUTO_TEST_CASE( test_torus_invalid_passthrough )
{
    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    GenBlockParams params;

    params.setBlockId( "TESTGB16" );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M0" );

    DimensionSpecs dim_specs = { {
            DimensionSpec( 2 ),
            DimensionSpec( ":101" ),
            DimensionSpec( 2 ),
            DimensionSpec( 2 )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::INVALID_ARG );

    BGQDB::deleteBlock( "TESTGB16" ); // just in case!
}


BOOST_AUTO_TEST_CASE( test_mesh )
{

    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    GenBlockParams params;

    params.setBlockId( "TESTGB17" );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M0" );

    DimensionSpecs dim_specs = { {
            DimensionSpec( 1 ),
            DimensionSpec( "M:11" ),
            DimensionSpec( 1 ),
            DimensionSpec( 1 )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R20-M0"] = Coords( 0, 1, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R00-M0"] = SwitchSetting( 'T', 'O' );
    addSwitches( sbi, "R20-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R20-M0"] = SwitchSetting( 'T', 'I' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R20-M0" );
    l.insert( "B_R00-M0B_R20-M0" );

    BOOST_CHECK( check( "TESTGB17", bpi, sbi, l,
            BlockInfo( "10111", 512*2, 4, 4*2, 4, 4 )
        ) );

    BGQDB::deleteBlock( "TESTGB17" );
}

BOOST_AUTO_TEST_CASE( test_mesh_passthrough )
{
    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    GenBlockParams params;

    params.setBlockId( "TESTGB17" );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M0" );

    DimensionSpecs dim_specs = { {
            DimensionSpec( 1 ),
            DimensionSpec( "M:101" ),
            DimensionSpec( 1 ),
            DimensionSpec( 1 )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R30-M0"] = Coords( 0, 1, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R00-M0"] = SwitchSetting( 'T', 'O' );
    sbi["B_R20-M0"] = SwitchSetting( 'F', 'B' );
    addSwitches( sbi, "R30-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R30-M0"] = SwitchSetting( 'T', 'I' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R30-M0" );
    l.insert( "B_R00-M0B_R20-M0" );
    l.insert( "B_R20-M0B_R30-M0" );

    BOOST_CHECK( check( "TESTGB17", bpi, sbi, l,
            BlockInfo( "10111", 512*2, 4, 4*2, 4, 4 )
        ) );

    BGQDB::deleteBlock( "TESTGB17" );
}


BOOST_AUTO_TEST_CASE( test_small_and_duplicate )
{
    GenBlockParams params;

    params.setBlockId( "TESTGB18" );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M0" );
    params.setNodeBoardAndCount( "N00", 1 );

    BGQDB::genBlockEx( params );

    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::DUPLICATE );

    BOOST_CHECK( checkBlockInfo( "TESTGB18", BlockInfo( "00001", 32, 2, 2, 2, 2 ) ) );

    BGQDB::deleteBlock( "TESTGB18" );
}


BOOST_AUTO_TEST_CASE( test_small_by_node_boards )
{
    GenBlockParams params;

    GenBlockParams::NodeBoardLocations node_board_locations;
    node_board_locations.push_back( "R00-M1-N04" );
    node_board_locations.push_back( "R00-M1-N05" );
    node_board_locations.push_back( "R00-M1-N06" );
    node_board_locations.push_back( "R00-M1-N07" );

    params.setBlockId( "TESTGBSBNBL" );
    params.setOwner( "unittest" );
    params.setNodeBoardLocations( node_board_locations );

    BGQDB::genBlockEx( params );

    BOOST_CHECK( checkBlockInfo( "TESTGBSBNBL", BlockInfo( "01101", 32*4, 2, 2, 4, 4 ) ) );

    BGQDB::deleteBlock( "TESTGBSBNBL" );
}


BOOST_AUTO_TEST_CASE( test_midplane_not_found_err )
{
    const string block_id( "TESTGBNOMP" );
    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );
    params.setMidplane( "RFF-M0" );

    params.setDimensionSpecs( BGQDB::MidplaneDimensionSpecs );

    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::NOT_FOUND );

    BGQDB::deleteBlock( block_id ); // just in case!
}


BOOST_AUTO_TEST_CASE( test_passthru_first )
{
    // Can pass through the first. Create a block like "genBPblock block1 R00-M0 1 2 1 1  1 0110 1 1"

    if ( bgqdb_info.machine_size[BGQDB::Dimension::B] != 4 )  { cout << "skipping test because B != 4.\n"; return; }

    const string block_id( "TESTGBPT1" );
    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );
    params.setMidplane( "R00-M0" );

    DimensionSpecs dim_specs = { {
            DimensionSpec( 1, "1" ),
            DimensionSpec( 2, "0110" ),
            DimensionSpec( 1, "1" ),
            DimensionSpec( 1, "1" )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R20-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R30-M0"] = Coords( 0, 1, 0, 0 );

    SwitchBlockMapInfo sbi;
    sbi["B_R00-M0"] = SwitchSetting( 'F', 'B' );
    addSwitches( sbi, "R20-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R20-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R30-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R30-M0"] = SwitchSetting( 'T', 'B' );
    sbi["B_R10-M0"] = SwitchSetting( 'F', 'B' );

    Links l;
    addLinks( l, "R20-M0" );
    addLinks( l, "R30-M0" );
    l.insert( "B_R00-M0B_R20-M0" );
    l.insert( "B_R20-M0B_R30-M0" );
    l.insert( "B_R30-M0B_R10-M0" );
    l.insert( "B_R10-M0B_R00-M0" );

    BOOST_CHECK( check( block_id, bpi, sbi, l, BlockInfo( "11111", 512*2, 4, 4*2, 4, 4 ) ) );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_R000 )
{
    const string block_id( "TEST_MP1" );
    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )
        );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );

    Links l;
    addLinks( l, "R00-M0" );

    BOOST_CHECK( check( block_id, bpi, sbi, l,
            BlockInfo( "11111", 512, 4, 4, 4, 4 )
        ) );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_1112 )
{
    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TESTGBMP1112" );

    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )( "R00-M1" )
        );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R00-M1"] = Coords( 0, 0, 0, 1 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["D_R00-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R00-M1", SwitchSetting( 'F', 'B' ) );
    sbi["D_R00-M1"] = SwitchSetting( 'T', 'B' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R00-M1" );
    l.insert( "D_R00-M0D_R00-M1" );
    l.insert( "D_R00-M1D_R00-M0" );

    BOOST_CHECK( check( block_id, bpi, sbi, l,
            BlockInfo( "11111", 512*2, 4, 4, 4, 4*2 )
         ) );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_1112_start1 )
{
    // If first midplane is R00-M1 rather than R00-M0, still works with diff coords.

    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TESTGBMP1112" );

    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M1" )( "R00-M0" )
        );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 1 );
    bpi["R00-M1"] = Coords( 0, 0, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["D_R00-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R00-M1", SwitchSetting( 'F', 'B' ) );
    sbi["D_R00-M1"] = SwitchSetting( 'T', 'B' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R00-M1" );
    l.insert( "D_R00-M0D_R00-M1" );
    l.insert( "D_R00-M1D_R00-M0" );

    BOOST_CHECK( check( block_id, bpi, sbi, l,
            BlockInfo( "11111", 512*2, 4, 4, 4, 4*2 )
          ) );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_1411 )
{
    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_name( "TESTGBMP1411" );

    GenBlockParams params;

    params.setBlockId( block_name );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )( "R10-M0" )( "R20-M0" )( "R30-M0" )
        );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R20-M0"] = Coords( 0, 1, 0, 0 );
    bpi["R30-M0"] = Coords( 0, 2, 0, 0 );
    bpi["R10-M0"] = Coords( 0, 3, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R00-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R10-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R10-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R30-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R30-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R20-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R20-M0"] = SwitchSetting( 'T', 'B' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R10-M0" );
    addLinks( l, "R30-M0" );
    addLinks( l, "R20-M0" );
    l.insert( "B_R00-M0B_R20-M0" );
    l.insert( "B_R20-M0B_R30-M0" );
    l.insert( "B_R30-M0B_R10-M0" );
    l.insert( "B_R10-M0B_R00-M0" );

    BOOST_CHECK( check( block_name, bpi, sbi, l,
            BlockInfo( "11111", 512*4, 4, 4*4, 4, 4 )
          ) );

    BGQDB::deleteBlock( block_name );
}


BOOST_AUTO_TEST_CASE( test_midplanes_2111 )
{
    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TESTGBMP9" );

    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )( "R02-M0" )
        );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R02-M0"] = Coords( 1, 0, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["A_R00-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R02-M0", SwitchSetting( 'F', 'B' ) );
    sbi["A_R02-M0"] = SwitchSetting( 'T', 'B' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R02-M0" );
    l.insert( "A_R00-M0A_R02-M0" );
    l.insert( "A_R02-M0A_R00-M0" );

    BOOST_CHECK( check( block_id, bpi, sbi, l,
            BlockInfo( "11111", 512*2, 4*2, 4, 4, 4 )
           ) );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_2422 )
{
    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    string block_id( "TESTGBMP10" );

    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )( "R00-M1" )( "R01-M0" )( "R01-M1" )( "R02-M0" )( "R02-M1" )( "R03-M0" )( "R03-M1" )
                   ( "R10-M0" )( "R10-M1" )( "R11-M0" )( "R11-M1" )( "R12-M0" )( "R12-M1" )( "R13-M0" )( "R13-M1" )
                   ( "R20-M0" )( "R20-M1" )( "R21-M0" )( "R21-M1" )( "R22-M0" )( "R22-M1" )( "R23-M0" )( "R23-M1" )
                   ( "R30-M0" )( "R30-M1" )( "R31-M0" )( "R31-M1" )( "R32-M0" )( "R32-M1" )( "R33-M0" )( "R33-M1" )
        );

    params.setDimensionSpecs( BGQDB::FullDimensionSpecs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R00-M1"] = Coords( 0, 0, 0, 1 );
    bpi["R01-M0"] = Coords( 0, 0, 1, 0 );
    bpi["R01-M1"] = Coords( 0, 0, 1, 1 );
    bpi["R02-M0"] = Coords( 1, 0, 0, 0 );
    bpi["R02-M1"] = Coords( 1, 0, 0, 1 );
    bpi["R03-M0"] = Coords( 1, 0, 1, 0 );
    bpi["R03-M1"] = Coords( 1, 0, 1, 1 );

    bpi["R20-M0"] = Coords( 0, 1, 0, 0 );
    bpi["R20-M1"] = Coords( 0, 1, 0, 1 );
    bpi["R21-M0"] = Coords( 0, 1, 1, 0 );
    bpi["R21-M1"] = Coords( 0, 1, 1, 1 );
    bpi["R22-M0"] = Coords( 1, 1, 0, 0 );
    bpi["R22-M1"] = Coords( 1, 1, 0, 1 );
    bpi["R23-M0"] = Coords( 1, 1, 1, 0 );
    bpi["R23-M1"] = Coords( 1, 1, 1, 1 );

    bpi["R30-M0"] = Coords( 0, 2, 0, 0 );
    bpi["R30-M1"] = Coords( 0, 2, 0, 1 );
    bpi["R31-M0"] = Coords( 0, 2, 1, 0 );
    bpi["R31-M1"] = Coords( 0, 2, 1, 1 );
    bpi["R32-M0"] = Coords( 1, 2, 0, 0 );
    bpi["R32-M1"] = Coords( 1, 2, 0, 1 );
    bpi["R33-M0"] = Coords( 1, 2, 1, 0 );
    bpi["R33-M1"] = Coords( 1, 2, 1, 1 );

    bpi["R10-M0"] = Coords( 0, 3, 0, 0 );
    bpi["R10-M1"] = Coords( 0, 3, 0, 1 );
    bpi["R11-M0"] = Coords( 0, 3, 1, 0 );
    bpi["R11-M1"] = Coords( 0, 3, 1, 1 );
    bpi["R12-M0"] = Coords( 1, 3, 0, 0 );
    bpi["R12-M1"] = Coords( 1, 3, 0, 1 );
    bpi["R13-M0"] = Coords( 1, 3, 1, 0 );
    bpi["R13-M1"] = Coords( 1, 3, 1, 1 );

    BOOST_CHECK( checkBpBlockMap( block_id, bpi ) );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R00-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R01-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R01-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R02-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R02-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R03-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R03-M1", SwitchSetting( 'T', 'B' ) );

    addSwitches( sbi, "R10-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R10-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R11-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R11-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R12-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R12-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R13-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R13-M1", SwitchSetting( 'T', 'B' ) );

    addSwitches( sbi, "R20-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R20-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R21-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R21-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R22-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R22-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R23-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R23-M1", SwitchSetting( 'T', 'B' ) );

    addSwitches( sbi, "R30-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R30-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R31-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R31-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R32-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R32-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R33-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R33-M1", SwitchSetting( 'T', 'B' ) );

    BOOST_CHECK( checkSwitchBlockMap( block_id, sbi ) );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_passthru_at_end )
{
    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TESTGBMPPT1" );

    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )( "R20-M0" )( "R30-M0" ),
            list_of( "R10-M0" )
        );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R20-M0"] = Coords( 0, 1, 0, 0 );
    bpi["R30-M0"] = Coords( 0, 2, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R00-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R20-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R20-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R30-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R30-M0"] = SwitchSetting( 'T', 'B' );
    sbi["B_R10-M0"] = SwitchSetting( 'F', 'B' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R20-M0" );
    addLinks( l, "R30-M0" );
    l.insert( "B_R00-M0B_R20-M0" );
    l.insert( "B_R20-M0B_R30-M0" );
    l.insert( "B_R30-M0B_R10-M0" );
    l.insert( "B_R10-M0B_R00-M0" );

    BOOST_CHECK( check( block_id, bpi, sbi, l,
            BlockInfo( "11111", 512*3, 4, 4*3, 4, 4 )
           ) );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_passthru_2_passthroughs )
{
    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TESTGBMPPT2" );

    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )( "R30-M0" ),
            list_of( "R10-M0" )( "R20-M0" )
        );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R30-M0"] = Coords( 0, 1, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R00-M0"] = SwitchSetting( 'T', 'B' );
    sbi["B_R10-M0"] = SwitchSetting( 'F', 'B' );
    addSwitches( sbi, "R30-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R30-M0"] = SwitchSetting( 'T', 'B' );
    sbi["B_R20-M0"] = SwitchSetting( 'F', 'B' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R30-M0" );
    l.insert( "B_R00-M0B_R20-M0" );
    l.insert( "B_R20-M0B_R30-M0" );
    l.insert( "B_R30-M0B_R10-M0" );
    l.insert( "B_R10-M0B_R00-M0" );

    BOOST_CHECK( check( block_id, bpi, sbi, l,
            BlockInfo( "11111", 512*2, 4, 4*2, 4, 4 )
           ) );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_passthru_2 )
{
    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TESTGBMPPT3" );

    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )( "R10-M0" ),
            list_of( "R20-M0" )( "R30-M0" )
        );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R10-M0"] = Coords( 0, 1, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R00-M0"] = SwitchSetting( 'T', 'B' );
    sbi["B_R20-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R30-M0"] = SwitchSetting( 'F', 'B' );
    addSwitches( sbi, "R10-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R10-M0"] = SwitchSetting( 'T', 'B' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R10-M0" );
    l.insert( "B_R00-M0B_R20-M0" );
    l.insert( "B_R20-M0B_R30-M0" );
    l.insert( "B_R30-M0B_R10-M0" );
    l.insert( "B_R10-M0B_R00-M0" );

    BOOST_CHECK( check( block_id, bpi, sbi, l,
            BlockInfo( "11111", 512*2, 4, 4*2, 4, 4 )
           ) );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_2222_passthru_1010 )
{
    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TESTGBMPPT4");

    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of
                ( "R00-M0" )( "R00-M1" )( "R01-M0" )( "R01-M1" )( "R02-M0" )( "R02-M1" )( "R03-M0" )( "R03-M1" )
                ( "R30-M0" )( "R30-M1" )( "R31-M0" )( "R31-M1" )( "R32-M0" )( "R32-M1" )( "R33-M0" )( "R33-M1" ),
            list_of
                ( "R10-M0" )( "R10-M1" )( "R11-M0" )( "R11-M1" )( "R12-M0" )( "R12-M1" )( "R13-M0" )( "R13-M1" )
                ( "R20-M0" )( "R20-M1" )( "R21-M0" )( "R21-M1" )( "R22-M0" )( "R22-M1" )( "R23-M0" )( "R23-M1" )
        );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R00-M1"] = Coords( 0, 0, 0, 1 );
    bpi["R01-M0"] = Coords( 0, 0, 1, 0 );
    bpi["R01-M1"] = Coords( 0, 0, 1, 1 );
    bpi["R02-M0"] = Coords( 1, 0, 0, 0 );
    bpi["R02-M1"] = Coords( 1, 0, 0, 1 );
    bpi["R03-M0"] = Coords( 1, 0, 1, 0 );
    bpi["R03-M1"] = Coords( 1, 0, 1, 1 );
    bpi["R30-M0"] = Coords( 0, 1, 0, 0 );
    bpi["R30-M1"] = Coords( 0, 1, 0, 1 );
    bpi["R31-M0"] = Coords( 0, 1, 1, 0 );
    bpi["R31-M1"] = Coords( 0, 1, 1, 1 );
    bpi["R32-M0"] = Coords( 1, 1, 0, 0 );
    bpi["R32-M1"] = Coords( 1, 1, 0, 1 );
    bpi["R33-M0"] = Coords( 1, 1, 1, 0 );
    bpi["R33-M1"] = Coords( 1, 1, 1, 1 );

    BOOST_CHECK( checkBpBlockMap( block_id, bpi ) );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R00-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R01-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R01-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R02-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R02-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R03-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R03-M1", SwitchSetting( 'T', 'B' ) );

    sbi["B_R10-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R10-M1"] = SwitchSetting( 'F', 'B' );
    sbi["B_R11-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R11-M1"] = SwitchSetting( 'F', 'B' );
    sbi["B_R12-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R12-M1"] = SwitchSetting( 'F', 'B' );
    sbi["B_R13-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R13-M1"] = SwitchSetting( 'F', 'B' );

    sbi["B_R20-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R20-M1"] = SwitchSetting( 'F', 'B' );
    sbi["B_R21-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R21-M1"] = SwitchSetting( 'F', 'B' );
    sbi["B_R22-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R22-M1"] = SwitchSetting( 'F', 'B' );
    sbi["B_R23-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R23-M1"] = SwitchSetting( 'F', 'B' );

    addSwitches( sbi, "R30-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R30-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R31-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R31-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R32-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R32-M1", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R33-M0", SwitchSetting( 'T', 'B' ) );
    addSwitches( sbi, "R33-M1", SwitchSetting( 'T', 'B' ) );

    BOOST_CHECK( checkSwitchBlockMap( block_id, sbi ) );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_mesh_b_2 )
{
    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TESTGBMPMB2" );

    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of
                ( "R00-M0" )( "R20-M0" )
        );

    DimensionSpecs dim_specs = { {
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Mesh ),
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Torus )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R20-M0"] = Coords( 0, 1, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R00-M0"] = SwitchSetting( 'T', 'O' );
    addSwitches( sbi, "R20-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R20-M0"] = SwitchSetting( 'T', 'I' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R20-M0" );
    l.insert( "B_R00-M0B_R20-M0" );

    BOOST_CHECK( check( block_id, bpi, sbi, l,
            BlockInfo( "10111", 512*2, 4, 4*2, 4, 4 )
        ) );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_mesh_b_3 )
{
    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TESTGBMPMB3" );

    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )( "R20-M0" )( "R30-M0" )
        );

    DimensionSpecs dim_specs = { {
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Mesh ),
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Torus )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R20-M0"] = Coords( 0, 1, 0, 0 );
    bpi["R30-M0"] = Coords( 0, 2, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R00-M0"] = SwitchSetting( 'T', 'O' );
    addSwitches( sbi, "R20-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R20-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R30-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R30-M0"] = SwitchSetting( 'T', 'I' );

    Links l;
    addLinks( l, "R00-M0" );
    l.insert( "B_R00-M0B_R20-M0" );
    addLinks( l, "R20-M0" );
    l.insert( "B_R20-M0B_R30-M0" );
    addLinks( l, "R30-M0" );

    BOOST_CHECK( check( block_id, bpi, sbi, l,
            BlockInfo( "10111", 512*3, 4, 4*3, 4, 4 )
        ) );

    BGQDB::deleteBlock( block_id );
}

BOOST_AUTO_TEST_CASE( test_midplanes_mesh_b_3_order )
{
    // Can specify the midplanes (besides the first) in a different order.

    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TESTGBMPMB3" );

    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )( "R30-M0" )( "R20-M0" )
        );

    DimensionSpecs dim_specs = { {
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Mesh ),
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Torus )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R20-M0"] = Coords( 0, 1, 0, 0 );
    bpi["R30-M0"] = Coords( 0, 2, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R00-M0"] = SwitchSetting( 'T', 'O' );
    addSwitches( sbi, "R20-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R20-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R30-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R30-M0"] = SwitchSetting( 'T', 'I' );

    Links l;
    addLinks( l, "R00-M0" );
    l.insert( "B_R00-M0B_R20-M0" );
    addLinks( l, "R20-M0" );
    l.insert( "B_R20-M0B_R30-M0" );
    addLinks( l, "R30-M0" );

    BOOST_CHECK( check( block_id, bpi, sbi, l,
            BlockInfo( "10111", 512*3, 4, 4*3, 4, 4 )
        ) );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_mesh_b_3_wraps )
{
    // Test that can wrap around the edge of the machine and still calc the size correctly.

    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TESTGBMPMB3" );

    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R30-M0" )( "R10-M0" )( "R00-M0" )
        );

    DimensionSpecs dim_specs = { {
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Mesh ),
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Torus )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R30-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R10-M0"] = Coords( 0, 1, 0, 0 );
    bpi["R00-M0"] = Coords( 0, 2, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R30-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R30-M0"] = SwitchSetting( 'T', 'O' );
    addSwitches( sbi, "R10-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R10-M0"] = SwitchSetting( 'T', 'B' );
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R00-M0"] = SwitchSetting( 'T', 'I' );

    Links l;
    addLinks( l, "R30-M0" );
    l.insert( "B_R30-M0B_R10-M0" );
    addLinks( l, "R10-M0" );
    l.insert( "B_R10-M0B_R00-M0" );
    addLinks( l, "R00-M0" );

    BOOST_CHECK( check( block_id, bpi, sbi, l,
            BlockInfo( "10111", 512*3, 4, 4*3, 4, 4 )
        ) );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_mesh_a )
{
    // Try a mesh in another dimension... too bad don't have a bigger machine!

    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TESTGBMPMA" );
    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )( "R02-M0" )
        );

    DimensionSpecs dim_specs = { {
            DimensionSpec( Connectivity::Mesh ),
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Torus )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R02-M0"] = Coords( 1, 0, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["A_R00-M0"] = SwitchSetting( 'T', 'O' );
    addSwitches( sbi, "R02-M0", SwitchSetting( 'F', 'B' ) );
    sbi["A_R02-M0"] = SwitchSetting( 'T', 'I' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R02-M0" );
    l.insert( "A_R00-M0A_R02-M0" );

    BOOST_CHECK( check( block_id, bpi, sbi, l,
            BlockInfo( "01111", 512*2, 4*2, 4, 4, 4 )
        ) );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_mesh_all_2222 )
{

    // Try a mesh in another dimension... too bad don't have a bigger machine!

    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TESTGBMPMALL" );
    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of
                ( "R00-M0" )( "R00-M1" )( "R20-M0" )( "R20-M1" )
                ( "R01-M0" )( "R01-M1" )( "R21-M0" )( "R21-M1" )
                ( "R02-M0" )( "R02-M1" )( "R22-M0" )( "R22-M1" )
                ( "R03-M0" )( "R03-M1" )( "R23-M0" )( "R23-M1" )
        );

    DimensionSpecs dim_specs = { {
            DimensionSpec( Connectivity::Mesh ),
            DimensionSpec( Connectivity::Mesh ),
            DimensionSpec( Connectivity::Mesh ),
            DimensionSpec( Connectivity::Mesh )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R00-M1"] = Coords( 0, 0, 0, 1 );
    bpi["R02-M0"] = Coords( 1, 0, 0, 0 );
    bpi["R02-M1"] = Coords( 1, 0, 0, 1 );
    bpi["R20-M0"] = Coords( 0, 1, 0, 0 );
    bpi["R20-M1"] = Coords( 0, 1, 0, 1 );
    bpi["R22-M0"] = Coords( 1, 1, 0, 0 );
    bpi["R22-M1"] = Coords( 1, 1, 0, 1 );

    bpi["R01-M0"] = Coords( 0, 0, 1, 0 );
    bpi["R01-M1"] = Coords( 0, 0, 1, 1 );
    bpi["R03-M0"] = Coords( 1, 0, 1, 0 );
    bpi["R03-M1"] = Coords( 1, 0, 1, 1 );
    bpi["R21-M0"] = Coords( 0, 1, 1, 0 );
    bpi["R21-M1"] = Coords( 0, 1, 1, 1 );
    bpi["R23-M0"] = Coords( 1, 1, 1, 0 );
    bpi["R23-M1"] = Coords( 1, 1, 1, 1 );


    BOOST_CHECK( checkBpBlockMap( block_id, bpi ) );


    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'T', 'O' ) );

    addSwitches( sbi, "R00-M1", SwitchSetting( 'T', 'O' ) );
    sbi["D_R00-M1"] = SwitchSetting( 'T', 'I' );

    addSwitches( sbi, "R02-M0", SwitchSetting( 'T', 'O' ) );
    sbi["A_R02-M0"] = SwitchSetting( 'T', 'I' );

    addSwitches( sbi, "R02-M1", SwitchSetting( 'T', 'O' ) );
    sbi["A_R02-M1"] = SwitchSetting( 'T', 'I' );
    sbi["D_R02-M1"] = SwitchSetting( 'T', 'I' );

    addSwitches( sbi, "R20-M0", SwitchSetting( 'T', 'O' ) );
    sbi["B_R20-M0"] = SwitchSetting( 'T', 'I' );

    addSwitches( sbi, "R20-M1", SwitchSetting( 'T', 'O' ) );
    sbi["B_R20-M1"] = SwitchSetting( 'T', 'I' );
    sbi["D_R20-M1"] = SwitchSetting( 'T', 'I' );

    addSwitches( sbi, "R22-M0", SwitchSetting( 'T', 'O' ) );
    sbi["A_R22-M0"] = SwitchSetting( 'T', 'I' );
    sbi["B_R22-M0"] = SwitchSetting( 'T', 'I' );

    addSwitches( sbi, "R22-M1", SwitchSetting( 'T', 'I' ) );
    sbi["C_R22-M1"] = SwitchSetting( 'T', 'O' );


    addSwitches( sbi, "R01-M0", SwitchSetting( 'T', 'O' ) );
    sbi["C_R01-M0"] = SwitchSetting( 'T', 'I' );

    addSwitches( sbi, "R01-M1", SwitchSetting( 'T', 'O' ) );
    sbi["C_R01-M1"] = SwitchSetting( 'T', 'I' );
    sbi["D_R01-M1"] = SwitchSetting( 'T', 'I' );

    addSwitches( sbi, "R03-M0", SwitchSetting( 'T', 'O' ) );
    sbi["C_R03-M0"] = SwitchSetting( 'T', 'I' );
    sbi["A_R03-M0"] = SwitchSetting( 'T', 'I' );

    addSwitches( sbi, "R03-M1", SwitchSetting( 'T', 'O' ) );
    sbi["A_R03-M1"] = SwitchSetting( 'T', 'I' );
    sbi["C_R03-M1"] = SwitchSetting( 'T', 'I' );
    sbi["D_R03-M1"] = SwitchSetting( 'T', 'I' );

    addSwitches( sbi, "R21-M0", SwitchSetting( 'T', 'O' ) );
    sbi["B_R21-M0"] = SwitchSetting( 'T', 'I' );
    sbi["C_R21-M0"] = SwitchSetting( 'T', 'I' );

    addSwitches( sbi, "R21-M1", SwitchSetting( 'T', 'O' ) );
    sbi["B_R21-M1"] = SwitchSetting( 'T', 'I' );
    sbi["C_R21-M1"] = SwitchSetting( 'T', 'I' );
    sbi["D_R21-M1"] = SwitchSetting( 'T', 'I' );

    addSwitches( sbi, "R23-M0", SwitchSetting( 'T', 'O' ) );
    sbi["A_R23-M0"] = SwitchSetting( 'T', 'I' );
    sbi["B_R23-M0"] = SwitchSetting( 'T', 'I' );
    sbi["C_R23-M0"] = SwitchSetting( 'T', 'I' );

    addSwitches( sbi, "R23-M1", SwitchSetting( 'T', 'I' ) );

    BOOST_CHECK( checkSwitchBlockMap( block_id, sbi ) );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_mesh_passthrough )
{
    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TEST_GB_MP_M_PT" );
    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )( "R30-M0" ),
            list_of( "R20-M0" )
        );

    DimensionSpecs dim_specs = { {
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Mesh ),
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Torus )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R00-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R30-M0"] = Coords( 0, 1, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R00-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R00-M0"] = SwitchSetting( 'T', 'O' );
    sbi["B_R20-M0"] = SwitchSetting( 'F', 'B' );
    addSwitches( sbi, "R30-M0", SwitchSetting( 'F', 'B' ) );
    sbi["B_R30-M0"] = SwitchSetting( 'T', 'I' );

    Links l;
    addLinks( l, "R00-M0" );
    addLinks( l, "R30-M0" );
    l.insert( "B_R00-M0B_R20-M0" );
    l.insert( "B_R20-M0B_R30-M0" );

    BOOST_CHECK( check( block_id, bpi, sbi, l,
            BlockInfo( "10111", 512*2, 4, 4*2, 4, 4 )
        ) );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_missing_midplane_torus )
{
    // If missing a midplane / passthrough in a Torus then fails with INVALID_ARG.

    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TEST_BG_MP_MISSING_MP_T" );

    GenBlockParams params;
    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )( "R10-M0" )( "R20-M0" )
        );

    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::INVALID_ARG );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_missing_midplane_mesh )
{
    // If missing a midplane / passthrough in a Mesh then fails with INVALID_ARG.

    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TEST_BG_MP_MISSING_MP_M" );

    GenBlockParams params;
    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )( "R30-M0" )
        );

    DimensionSpecs dim_specs = { {
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Mesh ),
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Torus )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::INVALID_ARG );

    BGQDB::deleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_midplanes_extra_passthrough )
{
    // If there's a passthrough that's not needed then should fail.
    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TEST_GB_MP_M_XPT" );
    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )( "R30-M0" ),
            list_of( "R10-M0" )( "R20-M0" )
        );

    DimensionSpecs dim_specs = { {
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Mesh ),
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Torus )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::INVALID_ARG );

    BGQDB::deleteBlock( block_id ); // shouldn't exist.
}


BOOST_AUTO_TEST_CASE( test_midplanes_mesh_one_ex )
{
    // If set connectivity is mesh when only one midplane in the dim then fails.

    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TEST_GB_MP_M_1" );
    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )
        );

    DimensionSpecs dim_specs = { {
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Mesh ),
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Torus )
        } };

    params.setDimensionSpecs( dim_specs );

    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::INVALID_ARG );

    BGQDB::deleteBlock( block_id ); // shouldn't exist.
}


BOOST_AUTO_TEST_CASE( test_midplanes_invalid_midplane_location_ex )
{
    // If supply a midplane that doesn't exist then fails with NOT_FOUND.

    const string block_id( "TEST_GB_MP_LOC_EX" );
    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "RZZ-M0" )
        );

    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::NOT_FOUND );

    BGQDB::deleteBlock( block_id ); // shouldn't exist.
}


BOOST_AUTO_TEST_CASE( test_midplanes_invalid_pt_location_ex )
{
    // If supply a passthrough midplane that doesn't exist then fails with NOT_FOUND.

    const string block_id( "TEST_GB_MP_LOC_EX" );
    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )( "R00-M1" ),
            list_of( "RZZ-M0" )
        );

    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::NOT_FOUND );

    BGQDB::deleteBlock( block_id ); // shouldn't exist.
}


BOOST_AUTO_TEST_CASE( test_midplanes_passthrough_when_need_midplane )
{
    // If there's a passthrough when need to include then should fail.

    if ( ! bgqdb_info.is_2422 )  { cout << "skipping test because db not populate with 2422.\n"; return; }

    const string block_id( "TEST_GB_MP_M_XPT2" );
    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R00-M0" )( "R02-M0" )( "R00-M1" ),
            list_of( "R02-M1" )
        );

    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::INVALID_ARG );

    BGQDB::deleteBlock( block_id ); // shouldn't exist.
}


BOOST_AUTO_TEST_CASE( test_midplanes_passthrough_4344 )
{
    if ( ! bgqdb_info.is_4344 )  { cout << "skipping test because db not populate with 4344.\n"; return; }

    const string block_id( "TEST_4344_1" );

    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    /*
0003(R01-M0)
0103(R11-M0)
1003(R09-M0)
1103(R19-M0)

and these as passthrough

0203(R21-M0)
1203(R29-M0)
2003(R39-M0)
2103(R49-M0)
3003(R31-M0)
3103(R41-M0)

     */

    params.setMidplanes(
            list_of( "R01-M0" )( "R11-M0" )( "R09-M0" )( "R19-M0" ),
            list_of
              ( "R21-M0" )( "R29-M0" )( "R39-M0" )
              ( "R49-M0" )( "R31-M0" )( "R41-M0" )
        );

    BGQDB::genBlockEx( params );

    BpBlockMapInfo bpi;
    bpi["R01-M0"] = Coords( 0, 0, 0, 0 );
    bpi["R09-M0"] = Coords( 1, 0, 0, 0 );
    bpi["R11-M0"] = Coords( 0, 1, 0, 0 );
    bpi["R19-M0"] = Coords( 1, 1, 0, 0 );

    SwitchBlockMapInfo sbi;
    addSwitches( sbi, "R01-M0", SwitchSetting( 'F', 'B' ) );
    sbi["A_R01-M0"] = SwitchSetting( 'T', 'B' );
    sbi["B_R01-M0"] = SwitchSetting( 'T', 'B' );

    addSwitches( sbi, "R09-M0", SwitchSetting( 'F', 'B' ) );
    sbi["A_R09-M0"] = SwitchSetting( 'T', 'B' );
    sbi["B_R09-M0"] = SwitchSetting( 'T', 'B' );

    addSwitches( sbi, "R11-M0", SwitchSetting( 'F', 'B' ) );
    sbi["A_R11-M0"] = SwitchSetting( 'T', 'B' );
    sbi["B_R11-M0"] = SwitchSetting( 'T', 'B' );

    addSwitches( sbi, "R19-M0", SwitchSetting( 'F', 'B' ) );
    sbi["A_R19-M0"] = SwitchSetting( 'T', 'B' );
    sbi["B_R19-M0"] = SwitchSetting( 'T', 'B' );

    sbi["A_R39-M0"] = SwitchSetting( 'F', 'B' );
    sbi["A_R31-M0"] = SwitchSetting( 'F', 'B' );

    sbi["A_R49-M0"] = SwitchSetting( 'F', 'B' );
    sbi["A_R41-M0"] = SwitchSetting( 'F', 'B' );

    sbi["B_R21-M0"] = SwitchSetting( 'F', 'B' );
    sbi["B_R29-M0"] = SwitchSetting( 'F', 'B' );

    Links l;
    addLinks( l, "R01-M0" );
    l.insert( "A_R01-M0A_R09-M0" );
    l.insert( "B_R01-M0B_R11-M0" );

    addLinks( l, "R09-M0" );
    l.insert( "A_R09-M0A_R39-M0" );
    l.insert( "B_R09-M0B_R19-M0" );

    addLinks( l, "R11-M0" );
    l.insert( "A_R11-M0A_R19-M0" );
    l.insert( "B_R11-M0B_R21-M0" );

    addLinks( l, "R19-M0" );
    l.insert( "A_R19-M0A_R49-M0" );
    l.insert( "B_R19-M0B_R29-M0" );

    l.insert( "A_R39-M0A_R31-M0" );
    l.insert( "A_R31-M0A_R01-M0" );

    l.insert( "A_R49-M0A_R41-M0" );
    l.insert( "A_R41-M0A_R11-M0" );

    l.insert( "B_R21-M0B_R01-M0" );
    l.insert( "B_R29-M0B_R09-M0" );

    BOOST_CHECK( check( block_id, bpi, sbi, l,
            BlockInfo( "11111", 2048, 8, 8, 4, 4 )
        ) );

    BGQDB::deleteBlock( block_id );
}

BOOST_AUTO_TEST_CASE( test_midplanes_passthrough_4344_extra_include_ex )
{
    if ( ! bgqdb_info.is_4344 )  { cout << "skipping test because db not populate with 4344.\n"; return; }

    const string block_id( "TEST_4344_2" );

    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R01-M0" )( "R11-M0" )( "R09-M0" )( "R19-M0" )( "R59-M0" ), // R59-M0 shouldn't be there.
            list_of
              ( "R21-M0" )( "R29-M0" )( "R39-M0" )
              ( "R49-M0" )( "R31-M0" )( "R41-M0" )
        );

    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        std::cout << "e.what=" << e.what() << "\n";
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::INVALID_ARG );

    BGQDB::deleteBlock( block_id ); // shouldn't exist.
}

BOOST_AUTO_TEST_CASE( test_midplanes_passthrough_4344_diff_pt_ex )
{
    // have diff passthroughs in different directions...

    if ( ! bgqdb_info.is_4344 )  { cout << "skipping test because db not populate with 4344.\n"; return; }

    const string block_id( "TEST_4344_3" );

    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R01-M0" )( "R11-M0" )( "R09-M0" )( "R29-M0" ),
            list_of
              ( "R21-M0" )( "R39-M0" )( "R19-M0" )
              ( "R49-M0" )( "R31-M0" )( "R41-M0" )
        );

    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        std::cout << "e.what=" << e.what() << "\n";
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::INVALID_ARG );

    BGQDB::deleteBlock( block_id ); // shouldn't exist.
}

BOOST_AUTO_TEST_CASE( test_midplanes_passthrough_4344_extra_pt_ex )
{
    if ( ! bgqdb_info.is_4344 )  { cout << "skipping test because db not populate with 4344.\n"; return; }

    const string block_id( "TEST_4344_4" );

    GenBlockParams params;

    params.setBlockId( block_id );
    params.setOwner( "unittest" );

    params.setMidplanes(
            list_of( "R01-M0" )( "R11-M0" )( "R09-M0" )( "R19-M0" ),
            list_of
              ( "R21-M0" )( "R29-M0" )( "R39-M0" )
              ( "R49-M0" )( "R31-M0" )( "R41-M0" )
              ( "R59-M0" ) // not valid.
        );

    BGQDB::STATUS db_status(BGQDB::OK);
    try {
        BGQDB::genBlockEx( params );
    } catch( BGQDB::Exception& e ) {
        std::cout << "e.what=" << e.what() << "\n";
        db_status = e.getStatus();
    }
    BOOST_CHECK_EQUAL( db_status, BGQDB::INVALID_ARG );

    BGQDB::deleteBlock( block_id ); // shouldn't exist.
}
