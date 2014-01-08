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

#include "Location.h"

#include <pthread.h>
#include <time.h>

#include <string>


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE configuration
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>


using bgq::util::Location;
using bgq::util::LocationError;

using std::string;


BOOST_AUTO_TEST_CASE( testTrim )
{
    Location l( " R00-M0 " );
    BOOST_CHECK_EQUAL( std::string( l ), "R00-M0" );
}

BOOST_AUTO_TEST_CASE( test_ComputeRack_00 )
{
    Location l( "R00" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), uint8_t(0) );
    BOOST_CHECK_EQUAL( l.getRackColumn(), uint8_t(0) );
}

BOOST_AUTO_TEST_CASE( test_ComputeRack_biggest )
{
    Location l( "RVV" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), unsigned(31) );
    BOOST_CHECK_EQUAL( l.getRackColumn(), unsigned(31) );
}

BOOST_AUTO_TEST_CASE( test_ComputeRack_toobig )
{
    BOOST_CHECK_THROW( Location( "RWW" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_IoRack_min )
{
    Location l( "Q00" );

    BOOST_CHECK_EQUAL( l.getType(), Location::IoRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::IoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), unsigned(0) );
    BOOST_CHECK_EQUAL( l.getRackColumn(), unsigned(0) );
}

BOOST_AUTO_TEST_CASE( test_IoRack_max )
{
    Location l( "QVV" );

    BOOST_CHECK_EQUAL( l.getType(), Location::IoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), unsigned(31) );
    BOOST_CHECK_EQUAL( l.getRackColumn(), unsigned(31) );
}

BOOST_AUTO_TEST_CASE( test_IoRack_toobig )
{
    BOOST_CHECK_THROW( Location( "QWW" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_BulkPowerSupplyOnComputeRack_min )
{
    Location l( "R00-B0");

    BOOST_CHECK_EQUAL( l.getType(), Location::BulkPowerSupplyOnComputeRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getBulkPowerSupply(), 0 );
}

BOOST_AUTO_TEST_CASE( test_BulkPowerSupplyOnComputeRack_max )
{
    Location l( "R00-B3");

    BOOST_CHECK_EQUAL( l.getType(), Location::BulkPowerSupplyOnComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getBulkPowerSupply(), 3 );
}

BOOST_AUTO_TEST_CASE( test_BulkPowerSupplyOnComputeRack_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "R00-B4" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_PowerModuleOnComputeRack_min )
{
    Location l( "R00-B0-P0");

    BOOST_CHECK_EQUAL( l.getType(), Location::PowerModuleOnComputeRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getBulkPowerSupply(), 0 );
    BOOST_CHECK_EQUAL( l.getPowerModule(), 0 );
}

BOOST_AUTO_TEST_CASE( test_PowerModuleOnComputeRack_max )
{
    Location l( "R00-B0-P8");

    BOOST_CHECK_EQUAL( l.getType(), Location::PowerModuleOnComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getBulkPowerSupply(), 0 );
    BOOST_CHECK_EQUAL( l.getPowerModule(), 8 );
}

BOOST_AUTO_TEST_CASE( test_PowerModuleOnComputeRack_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "R00-B0-P9" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_BulkPowerSupplyInIoRack )
{
    Location l( "Q00-B" );

    BOOST_CHECK_EQUAL( l.getType(), Location::BulkPowerSupplyInIoRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::IoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
}

BOOST_AUTO_TEST_CASE( test_PowerModuleOnIoRack_min )
{
    Location l( "Q00-B-P0" );

    BOOST_CHECK_EQUAL( l.getType(), Location::PowerModuleOnIoRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::IoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getPowerModule(), 0 );
}

BOOST_AUTO_TEST_CASE( test_PowerModuleOnIoRack_max )
{
    Location l( "Q00-B-P5" );

    BOOST_CHECK_EQUAL( l.getType(), Location::PowerModuleOnIoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getPowerModule(), 5 );
}

BOOST_AUTO_TEST_CASE( test_PowerModuleOnIoRack_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "Q00-B-P6" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_Midplane_0 )
{
    Location l( "R00-M0" );

    BOOST_CHECK_EQUAL( l.getType(), Location::Midplane );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
}

BOOST_AUTO_TEST_CASE( test_Midplane_1 )
{
    Location l( "R99-M1" );

    BOOST_CHECK_EQUAL( l.getType(), Location::Midplane );
    BOOST_CHECK_EQUAL( l.getRackRow(), 9 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 9 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 1 );
}

BOOST_AUTO_TEST_CASE( test_Midplane_2_ex )
{
    BOOST_CHECK_THROW( Location( "R00-M2" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_ServiceCard )
{
    Location l( "R00-M0-S" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ServiceCard );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
}

BOOST_AUTO_TEST_CASE( test_ClockCardOnComputeRack )
{
    Location l( "R00-K" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ClockCardOnComputeRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
}

BOOST_AUTO_TEST_CASE( test_ClockCardOnIoRack_min )
{
    Location l( "Q00-K0" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ClockCardOnIoRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::IoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getClockCard(), 0 );
}

BOOST_AUTO_TEST_CASE( test_ClockCardOnIoRack_max )
{
    Location l( "Q00-K1" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ClockCardOnIoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getClockCard(), 1 );
}

BOOST_AUTO_TEST_CASE( test_ClockCardOnIoRack_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "Q00-K2" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_IoBoardOnComputeRack_min )
{
    Location l( "R00-IC" );

    BOOST_CHECK_EQUAL( l.getType(), Location::IoBoardOnComputeRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 12 );
}

BOOST_AUTO_TEST_CASE( test_IoBoardOnComputeRack_max )
{
    Location l( "R00-IF" );

    BOOST_CHECK_EQUAL( l.getType(), Location::IoBoardOnComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 15 );
}

BOOST_AUTO_TEST_CASE( test_IoBoardOnComputeRack_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "R00-IG" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_IoBoardOnIoRack_min )
{
    Location l( "Q00-I0" );

    BOOST_CHECK_EQUAL( l.getType(), Location::IoBoardOnIoRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::IoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0 );
}

BOOST_AUTO_TEST_CASE( test_IoBoardOnIoRack_max )
{
    Location l( "Q00-IB" );

    BOOST_CHECK_EQUAL( l.getType(), Location::IoBoardOnIoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 11 );
}

BOOST_AUTO_TEST_CASE( test_IoBoardOnIoRack_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "Q00-IC" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_NodeBoard_min )
{
    Location l( "R0A-M0-N00" );

    BOOST_CHECK_EQUAL( l.getType(), Location::NodeBoard );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 10 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
    BOOST_CHECK_EQUAL( l.getNodeBoard(), 0 );
}

BOOST_AUTO_TEST_CASE( test_NodeBoard_max )
{
    Location l( "RA0-M1-N15" );

    BOOST_CHECK_EQUAL( l.getType(), Location::NodeBoard );
    BOOST_CHECK_EQUAL( l.getRackRow(), 10 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 1 );
    BOOST_CHECK_EQUAL( l.getNodeBoard(), 15 );
}

BOOST_AUTO_TEST_CASE( test_NodeBoard_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "RA0-M0-N16" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_NodeBoard_hex_ex )
{
    BOOST_CHECK_THROW( Location( "RA0-M0-NAA" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_ComputeCardOnNodeBoard_min )
{
    Location l( "R00-M0-N00-J00" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ComputeCardOnNodeBoard );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
    BOOST_CHECK_EQUAL( l.getNodeBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getComputeCard(), 0 );
}

BOOST_AUTO_TEST_CASE( test_ComputeCardOnNodeBoard_max )
{
    Location l( "R00-M0-N00-J31" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ComputeCardOnNodeBoard );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
    BOOST_CHECK_EQUAL( l.getNodeBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getComputeCard(), 31 );
}

BOOST_AUTO_TEST_CASE( test_ComputeCardOnNodeBoard_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "R00-M0-N00-J32" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_ComputeCardOnNodeBoard_nodeboard_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "R00-M0-N16-J00" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_ComputeCardOnNodeBoard_nodeboard_max )
{
    Location l( "R00-M0-N15-J00" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ComputeCardOnNodeBoard );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
    BOOST_CHECK_EQUAL( l.getNodeBoard(), 15 );
    BOOST_CHECK_EQUAL( l.getComputeCard(), 0 );
}

BOOST_AUTO_TEST_CASE( test_ComputeCardOnIoBoard_min )
{
    Location l( "Q00-I0-J00" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ComputeCardOnIoBoard );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::IoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getComputeCard(), 0 );

    l.set( "R00-IC-J00" );
    BOOST_CHECK_EQUAL( l.getType(), Location::ComputeCardOnIoBoard );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0xc );
    BOOST_CHECK_EQUAL( l.getComputeCard(), 0 );
}

BOOST_AUTO_TEST_CASE( test_ComputeCardOnIoBoard_max )
{
    Location l( "Q00-I0-J07" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ComputeCardOnIoBoard );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getComputeCard(), 7 );

    l.set( "R00-IC-J07" );
    BOOST_CHECK_EQUAL( l.getType(), Location::ComputeCardOnIoBoard );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0xc );
    BOOST_CHECK_EQUAL( l.getComputeCard(), 7 );

}

BOOST_AUTO_TEST_CASE( test_ComputeCardOnIoBoard_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "Q00-I0-J08" ), LocationError );
    BOOST_CHECK_THROW( Location( "R00-IC-J08" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_ComputeCardOnIoBoard_io_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "Q00-IC-J00" ), LocationError );
    BOOST_CHECK_THROW( Location( "R00-IG-J00" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_ComputeCardOnIoBoard_io_max )
{
    Location l( "Q00-IB-J00" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ComputeCardOnIoBoard );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 11 );
    BOOST_CHECK_EQUAL( l.getComputeCard(), 0 );

    l.set( "R00-IF-J00" );
    
    BOOST_CHECK_EQUAL( l.getType(), Location::ComputeCardOnIoBoard );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0xf );
    BOOST_CHECK_EQUAL( l.getComputeCard(), 0 );
}

BOOST_AUTO_TEST_CASE( test_ComputeCardCoreOnNodeBoard_min )
{
    Location l( "R00-M0-N00-J00-C00" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ComputeCardCoreOnNodeBoard );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
    BOOST_CHECK_EQUAL( l.getNodeBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getComputeCard(), 0 );
    BOOST_CHECK_EQUAL( l.getCore(), 0 );
}

BOOST_AUTO_TEST_CASE( test_ComputeCardCoreOnNodeBoard_max )
{
    Location l( "R00-M0-N00-J00-C16" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ComputeCardCoreOnNodeBoard );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
    BOOST_CHECK_EQUAL( l.getNodeBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getComputeCard(), 0 );
    BOOST_CHECK_EQUAL( l.getCore(), 16 );
}

BOOST_AUTO_TEST_CASE( test_ComputeCardCoreOnNodeBoard_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "R00-M0-N00-J00-C17" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_ComputeCardCoreOnNodeBoard_nodecard_max )
{
    Location l( "R00-M0-N00-J15-C00" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ComputeCardCoreOnNodeBoard );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
    BOOST_CHECK_EQUAL( l.getNodeBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getComputeCard(), 15 );
    BOOST_CHECK_EQUAL( l.getCore(), 0 );
}

BOOST_AUTO_TEST_CASE( test_ComputeCardCoreOnIoBoard_min )
{
    Location l( "Q00-I0-J00-C00" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ComputeCardCoreOnIoBoard );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::IoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getComputeCard(), 0 );
    BOOST_CHECK_EQUAL( l.getCore(), 0 );
}

BOOST_AUTO_TEST_CASE( test_ComputeCardCoreOnIoBoard_max )
{
    Location l( "Q00-I0-J00-C16" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ComputeCardCoreOnIoBoard );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getComputeCard(), 0 );
    BOOST_CHECK_EQUAL( l.getCore(), 16 );
}

BOOST_AUTO_TEST_CASE( test_ComputeCardCoreOnIoBoard_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "Q00-I0-J00-C17" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_ComputeCardCoreOnIoBoard_compute_max )
{
    Location l( "Q00-I0-J07-C00" );

    BOOST_CHECK_EQUAL( l.getType(), Location::ComputeCardCoreOnIoBoard );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getComputeCard(), 7 );
    BOOST_CHECK_EQUAL( l.getCore(), 0 );
}

BOOST_AUTO_TEST_CASE( test_DcaOnNodeBoard_min )
{
    Location l( "R00-M0-N00-D0" );

    BOOST_CHECK_EQUAL( l.getType(), Location::DcaOnNodeBoard );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getNodeBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getDCA(), 0 );
}

BOOST_AUTO_TEST_CASE( test_DcaOnNodeBoard_max )
{
    Location l( "R00-M0-N00-D1" );

    BOOST_CHECK_EQUAL( l.getType(), Location::DcaOnNodeBoard );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getNodeBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getDCA(), 1 );
}

BOOST_AUTO_TEST_CASE( test_DcaOnNodeBoard_toobig )
{
    BOOST_CHECK_THROW( Location( "R00-M0-N00-D2" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_PciAdapterCard_min )
{
    Location l( "Q00-I0-A0" );

    BOOST_CHECK_EQUAL( l.getType(), Location::PciAdapterCard );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::IoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getPciAdapterCard(), 0 );
}

BOOST_AUTO_TEST_CASE( test_PciAdapterCard_max )
{
    Location l( "Q00-I0-A7" );

    BOOST_CHECK_EQUAL( l.getType(), Location::PciAdapterCard );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getPciAdapterCard(), 7 );
}

BOOST_AUTO_TEST_CASE( test_PciAdapterCard_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "Q00-I0-A8" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_LinkModuleOnNodeBoard_min )
{
    Location l( "R00-M0-N00-U00" );

    BOOST_CHECK_EQUAL( l.getType(), Location::LinkModuleOnNodeBoard );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
    BOOST_CHECK_EQUAL( l.getNodeBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getLinkModule(), 0 );
}

BOOST_AUTO_TEST_CASE( test_LinkModuleOnNodeBoard_max )
{
    Location l( "R00-M0-N00-U08" );

    BOOST_CHECK_EQUAL( l.getType(), Location::LinkModuleOnNodeBoard );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
    BOOST_CHECK_EQUAL( l.getNodeBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getLinkModule(), 8 );
}

BOOST_AUTO_TEST_CASE( test_LinkModuleOnNodeBoard_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "R00-M0-N00-U09" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_LinkModuleOnIoBoardInIoRack_min )
{
    Location l( "Q00-I0-U00" );

    BOOST_CHECK_EQUAL( l.getType(), Location::LinkModuleOnIoBoardInIoRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::IoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getLinkModule(), 0 );
}

BOOST_AUTO_TEST_CASE( test_LinkModuleOnIoBoardInIoRack_max )
{
    Location l( "Q00-I0-U05" );

    BOOST_CHECK_EQUAL( l.getType(), Location::LinkModuleOnIoBoardInIoRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::IoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getLinkModule(), 5 );
}

BOOST_AUTO_TEST_CASE( test_LinkModuleOnIoBoardInIoRack_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "Q00-I0-U06" ), LocationError );
}


BOOST_AUTO_TEST_CASE( test_LinkModuleOnIoBoardInComputeRack_min )
{
    Location l( "R00-IC-U00" );

    BOOST_CHECK_EQUAL( l.getType(), Location::LinkModuleOnIoBoardInComputeRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0xC );
    BOOST_CHECK_EQUAL( l.getLinkModule(), 0 );
}

BOOST_AUTO_TEST_CASE( test_LinkModuleOnIoBoardInComputeRack_max )
{
    Location l( "R00-IF-U05" );

    BOOST_CHECK_EQUAL( l.getType(), Location::LinkModuleOnIoBoardInComputeRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0xF );
    BOOST_CHECK_EQUAL( l.getLinkModule(), 5 );
}

BOOST_AUTO_TEST_CASE( test_LinkModuleOnIoBoardInComputeRack_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "R00-IF--U06" ), LocationError );
}


BOOST_AUTO_TEST_CASE( test_OpticalModuleOnNodeBoard_min )
{
    Location l( "R00-M0-N00-O00" );

    BOOST_CHECK_EQUAL( l.getType(), Location::OpticalModuleOnNodeBoard );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
    BOOST_CHECK_EQUAL( l.getNodeBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getOpticalModule(), 0 );
}

BOOST_AUTO_TEST_CASE( test_OpticalModuleOnNodeBoard_max )
{
    Location l( "R00-M0-N00-O35" );

    BOOST_CHECK_EQUAL( l.getType(), Location::OpticalModuleOnNodeBoard );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
    BOOST_CHECK_EQUAL( l.getNodeBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getOpticalModule(), 35 );
}

BOOST_AUTO_TEST_CASE( test_OpticalModuleOnNodeBoard_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "R00-M0-N00-O36" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_OpticalModuleOnIoBoard_min )
{
    Location l( "Q00-I0-O00" );

    BOOST_CHECK_EQUAL( l.getType(), Location::OpticalModuleOnIoBoard );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::IoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getOpticalModule(), 0 );
}

BOOST_AUTO_TEST_CASE( test_OpticalModuleOnIoBoard_max )
{
    Location l( "Q00-I0-O23" );

    BOOST_CHECK_EQUAL( l.getType(), Location::OpticalModuleOnIoBoard );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getOpticalModule(), 23 );
}

BOOST_AUTO_TEST_CASE( test_OpticalModuleOnIoBoard_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "Q00-I0-O24" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_FanAssemblyInComputeRack_min )
{
    Location l( "R00-IC-H0" );

    BOOST_CHECK_EQUAL( l.getType(), Location::FanAssemblyInComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 12 );
    BOOST_CHECK_EQUAL( l.getFanAssembly(), 0 );
}

BOOST_AUTO_TEST_CASE( test_FanAssemblyInComputeRack_max )
{
    Location l( "R00-IC-H2" );

    BOOST_CHECK_EQUAL( l.getType(), Location::FanAssemblyInComputeRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 12 );
    BOOST_CHECK_EQUAL( l.getFanAssembly(), 2 );
}

BOOST_AUTO_TEST_CASE( test_FanAssemblyInComputeRack_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "R00-IC-H3" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_FanAssemblyInIoRack_min )
{
    Location l( "Q00-I0-H0" );

    BOOST_CHECK_EQUAL( l.getType(), Location::FanAssemblyInIoRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::IoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getFanAssembly(), 0 );
}

BOOST_AUTO_TEST_CASE( test_FanAssemblyInIoRack_max )
{
    Location l( "Q00-I0-H2" );

    BOOST_CHECK_EQUAL( l.getType(), Location::FanAssemblyInIoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getFanAssembly(), 2 );
}

BOOST_AUTO_TEST_CASE( test_FanAssemblyInIoRack_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "Q00-I0-H3" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_FanInComputeRack_min )
{
    Location l( "R00-IC-H0-F0" );

    BOOST_CHECK_EQUAL( l.getType(), Location::FanInComputeRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 12 );
    BOOST_CHECK_EQUAL( l.getFanAssembly(), 0 );
    BOOST_CHECK_EQUAL( l.getFan(), 0 );
}

BOOST_AUTO_TEST_CASE( test_FanInComputeRack_max )
{
    Location l( "R00-IC-H0-F1" );

    BOOST_CHECK_EQUAL( l.getType(), Location::FanInComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 12 );
    BOOST_CHECK_EQUAL( l.getFanAssembly(), 0 );
    BOOST_CHECK_EQUAL( l.getFan(), 1 );
}

BOOST_AUTO_TEST_CASE( test_FanInComputeRack_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "R00-IC-H0-F2" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_FanInIoRack_min )
{
    Location l( "Q00-I0-H0-F0" );

    BOOST_CHECK_EQUAL( l.getType(), Location::FanInIoRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::IoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getFanAssembly(), 0 );
    BOOST_CHECK_EQUAL( l.getFan(), 0 );
}

BOOST_AUTO_TEST_CASE( test_FanInIoRack_max )
{
    Location l( "Q00-I0-H0-F1" );

    BOOST_CHECK_EQUAL( l.getType(), Location::FanInIoRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getIoBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getFanAssembly(), 0 );
    BOOST_CHECK_EQUAL( l.getFan(), 1 );
}

BOOST_AUTO_TEST_CASE( test_FanInIoRack_toobig_ex )
{
    BOOST_CHECK_THROW( Location( "Q00-I0-H0-F2" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_LeakDetectorInComputeRack )
{
    Location l( "R00-L" );

    BOOST_CHECK_EQUAL( l.getType(), Location::LeakDetectorInComputeRack );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
}

BOOST_AUTO_TEST_CASE( test_Switch_A )
{
    Location l( "A_R00-M0" );

    BOOST_CHECK_EQUAL( l.getType(), Location::Switch );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
    BOOST_CHECK_EQUAL( l.getSwitchDimension(), bgq::util::Location::Dimension::A );
}

BOOST_AUTO_TEST_CASE( test_Switch_D )
{
    Location l( "D_R00-M0" );

    BOOST_CHECK_EQUAL( l.getType(), Location::Switch );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
    BOOST_CHECK_EQUAL( l.getSwitchDimension(), bgq::util::Location::Dimension::D );
}

BOOST_AUTO_TEST_CASE( test_Switch_E_ex )
{
    BOOST_CHECK_THROW( Location( "E_R00-M0" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_IoLinkOnNodeBoard_min ) {
    Location l( "I_R00-M0-N00-T00" );

    BOOST_CHECK_EQUAL( l.getType(), Location::IoLinkOnNodeBoard );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
    BOOST_CHECK_EQUAL( l.getNodeBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getPort(), 0 );
}

BOOST_AUTO_TEST_CASE( test_IoLinkOnNodeBoard_max ) {
    Location l( "I_R00-M0-N00-T11" );

    BOOST_CHECK_EQUAL( l.getType(), Location::IoLinkOnNodeBoard );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
    BOOST_CHECK_EQUAL( l.getNodeBoard(), 0 );
    BOOST_CHECK_EQUAL( l.getPort(), 11 );
}

BOOST_AUTO_TEST_CASE( test_IoLinkOnNodeBoard_toobig_ex ) {
    BOOST_CHECK_THROW( Location( "I_R00-M0-N00-T12" ), LocationError );
}

BOOST_AUTO_TEST_CASE( test_AllNodeBoardsOnMidplane ) {
    Location l( "R00-M0-N" );

    BOOST_CHECK_EQUAL( l.getType(), Location::AllNodeBoardsOnMidplane );
    BOOST_CHECK_EQUAL( l.getRackType(), Location::ComputeRack );
    BOOST_CHECK_EQUAL( l.getRackRow(), 0 );
    BOOST_CHECK_EQUAL( l.getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( l.getMidplane(), 0 );
}

BOOST_AUTO_TEST_CASE( test_getMidplaneLocation_valid )
{
    BOOST_CHECK_EQUAL( Location( "R00-M0" ).getMidplaneLocation(), "R00-M0" );
    BOOST_CHECK_EQUAL( Location( "R00-M0-S" ).getMidplaneLocation(), "R00-M0" );
    BOOST_CHECK_EQUAL( Location( "R00-M0-N00" ).getMidplaneLocation(), "R00-M0" );
    BOOST_CHECK_EQUAL( Location( "R00-M0-N00-J00" ).getMidplaneLocation(), "R00-M0" );
    BOOST_CHECK_EQUAL( Location( "R00-M0-N00-J00-C00" ).getMidplaneLocation(), "R00-M0" );
    BOOST_CHECK_EQUAL( Location( "R00-M0-N00-D0" ).getMidplaneLocation(), "R00-M0" );
    BOOST_CHECK_EQUAL( Location( "R00-M0-N00-U00" ).getMidplaneLocation(), "R00-M0" );
    BOOST_CHECK_EQUAL( Location( "R00-M0-N00-O00" ).getMidplaneLocation(), "R00-M0" );
    BOOST_CHECK_EQUAL( Location( "A_R00-M0" ).getMidplaneLocation(), "R00-M0" );
    BOOST_CHECK_EQUAL( Location( "I_R00-M0-N00-T00" ).getMidplaneLocation(), "R00-M0" );
    BOOST_CHECK_EQUAL( Location( "R00-M0-N" ).getMidplaneLocation(), "R00-M0" );
}

BOOST_AUTO_TEST_CASE( test_getMidplaneLocation_not_valid_ex )
{
    BOOST_CHECK_THROW( Location( "R00" ).getMidplaneLocation(), LocationError );
    BOOST_CHECK_THROW( Location( "Q00" ).getMidplaneLocation(), LocationError );
    BOOST_CHECK_THROW( Location( "R00-B0" ).getMidplaneLocation(), LocationError );
    BOOST_CHECK_THROW( Location( "R00-B0-P0" ).getMidplaneLocation(), LocationError );
    BOOST_CHECK_THROW( Location( "Q00-B" ).getMidplaneLocation(), LocationError );
    BOOST_CHECK_THROW( Location( "Q00-B-P0" ).getMidplaneLocation(), LocationError );
    BOOST_CHECK_THROW( Location( "R00-K" ).getMidplaneLocation(), LocationError );
    BOOST_CHECK_THROW( Location( "Q00-K" ).getMidplaneLocation(), LocationError );
    BOOST_CHECK_THROW( Location( "R00-IC" ).getMidplaneLocation(), LocationError );
    BOOST_CHECK_THROW( Location( "Q00-I0" ).getMidplaneLocation(), LocationError );
    BOOST_CHECK_THROW( Location( "Q00-I0-F0" ).getMidplaneLocation(), LocationError );
    BOOST_CHECK_THROW( Location( "Q00-I0-J00" ).getMidplaneLocation(), LocationError );
    BOOST_CHECK_THROW( Location( "Q00-I0-J00-C00" ).getMidplaneLocation(), LocationError );
    BOOST_CHECK_THROW( Location( "Q00-I0-A0" ).getMidplaneLocation(), LocationError );
    BOOST_CHECK_THROW( Location( "Q00-I0-U00" ).getMidplaneLocation(), LocationError );
    BOOST_CHECK_THROW( Location( "Q00-I0-O00" ).getMidplaneLocation(), LocationError );
    BOOST_CHECK_THROW( Location( "R00-IC-H0" ).getMidplaneLocation(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_compute_rack_ex )
{
    Location l( "R00" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_io_rack_ex )
{
    Location l( "Q00" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_BulkPowerSupplyOnComputeRack_ex )
{
    Location l( "R00-B0" );

    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_PowerModuleOnComputeRack_ex )
{
    Location l( "R00-B0-P0" );

    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_BulkPowerSupplyInIoRack_ex )
{
    Location l( "Q00-B" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_PowerModuleOnIoRack_ex )
{
    Location l( "Q00-B-P0" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_Midplane_ex )
{
    Location l( "R00-M0" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_ServiceCard_ex )
{
    Location l( "R00-M0-S" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_ClockCardOnComputeRack_ex )
{
    Location l( "R00-K" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_ClockCardOnIoRack_ex )
{
    Location l( "Q00-K0" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_IoBoardOnComputeRack_ex )
{
    Location l( "R00-IC" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_IoBoardOnIoRack_ex )
{
    Location l( "Q00-I0" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_NodeBoard_ex )
{
    Location l( "R00-M0-N00" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_ComputeCardOnNodeBoard_ex )
{
    Location l( "R00-M0-N00-J00" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_ComputeCardOnIoBoard_ex )
{
    Location l( "Q00-I0-J00" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_ComputeCardCoreOnNodeBoard_ex )
{
    Location l( "R00-M0-N00-J00-C00" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_ComputeCardCoreOnIoBoard_ex )
{
    Location l( "Q00-I0-J00-C00" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_DcaOnNodeBoard_ex )
{
    Location l( "R00-M0-N00-D0" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_PciAdapterCard_ex )
{
    Location l( "Q00-I0-A0" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_LinkModuleOnNodeBoard_ex )
{
    Location l( "R00-M0-N00-U00" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_LinkModuleOnIoBoard_ex )
{
    Location l( "Q00-I0-U00" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_OpticalModuleOnNodeBoard_ex )
{
    Location l( "R00-M0-N00-O00" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_OpticalModuleOnIoBoard_ex )
{
    Location l( "Q00-I0-O00" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_FanAssemblyInComputeRack_ex )
{
    Location l( "R00-IC-H0" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_FanAssemblyInIoRack_ex )
{
    Location l( "Q00-I0-H0" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_FanInComputeRack_ex )
{
    Location l( "R00-IC-H0-F0" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_FanInIoRack_ex )
{
    Location l( "Q00-I0-H0-F0" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getMidplane(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_Switch_ex )
{

    Location l( "A_R00-M0" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_IoLinkOnNodeBoard_ex )
{
    Location l( "I_R00-M0-N00-T00" );

    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_getters_AllNodeBoardsOnMidplane_ex )
{
    Location l( "R00-M0-N" );

    BOOST_CHECK_THROW( l.getNodeBoard(), LocationError );
    BOOST_CHECK_THROW( l.getBulkPowerSupply(), LocationError );
    BOOST_CHECK_THROW( l.getPowerModule(), LocationError );
    BOOST_CHECK_THROW( l.getClockCard(), LocationError );
    BOOST_CHECK_THROW( l.getIoBoard(), LocationError );
    BOOST_CHECK_THROW( l.getFan(), LocationError );
    BOOST_CHECK_THROW( l.getComputeCard(), LocationError );
    BOOST_CHECK_THROW( l.getCore(), LocationError );
    BOOST_CHECK_THROW( l.getDCA(), LocationError );
    BOOST_CHECK_THROW( l.getPciAdapterCard(), LocationError );
    BOOST_CHECK_THROW( l.getLinkModule(), LocationError );
    BOOST_CHECK_THROW( l.getOpticalModule(), LocationError );
    BOOST_CHECK_THROW( l.getFanAssembly(), LocationError );
    BOOST_CHECK_THROW( l.getSwitchDimension(), LocationError );
    BOOST_CHECK_THROW( l.getPort(), LocationError );
}

BOOST_AUTO_TEST_CASE( test_nothrow_bad_location )
{
    // If create bad location with nothrow then type is NotValid.
    BOOST_CHECK_EQUAL( Location( "", Location::nothrow ).getType(), Location::NotValid );
}

BOOST_AUTO_TEST_CASE( test_nothrow_good_location )
{
    // If create good location with nothrow then type is not NotValid.
    BOOST_CHECK_EQUAL( Location( "R00-M0", Location::nothrow ).getType(), Location::Midplane );
}


void *modify_location_for_5_sec( void* location_voidp )
{
    Location *location_p(static_cast<Location*> ( location_voidp ));

    time_t end_time;
    time( &end_time );
    end_time += 5;

    int i(1);

    string location_str( "R01-M0" );

    while ( true ) {
        time_t cur_time;
        time( &cur_time );

        if ( cur_time > end_time ) {
            break;
        }

        ++i;
        if ( i == 10 ) i = 1;

        location_str[2] = ('0' + i);
        location_p->set( location_str );

        if ( location_p->getRackColumn() != i ) {
            return NULL;
        }
    }

    location_p->set( "R00-M0" );

    return NULL;
}


BOOST_AUTO_TEST_CASE( test_thread_safety )
{
    // Should be able to modify different Locations

    Location ls[7];
    pthread_t threads[7];

    for ( int i(0) ; i < 7 ; ++i ) {
        pthread_create( &(threads[i]), NULL, modify_location_for_5_sec, &(ls[i]) );
    }

    for ( int i(0) ; i < 7 ; ++i ) {
        pthread_join( threads[i], NULL );
    }

    BOOST_CHECK_EQUAL( ls[0].getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( ls[1].getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( ls[2].getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( ls[3].getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( ls[4].getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( ls[5].getRackColumn(), 0 );
    BOOST_CHECK_EQUAL( ls[6].getRackColumn(), 0 );
}
