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

#include "blue_gene.hpp"

#include <bgq_util/include/Location.h>

#include <boost/throw_exception.hpp>

#include <stdexcept>

#include <unistd.h>


using std::string;


namespace bgws {
namespace blue_gene {


const std::vector<std::string> ALL_NB_POS = {
        "N00", "N01", "N02", "N03",
        "N04", "N05", "N06", "N07",
        "N08", "N09", "N10", "N11",
        "N12", "N13", "N14", "N15"
    };


namespace diagnostics {

const std::string HARDWARE_BLOCK_PREFIX( "_DIAGS_" );

namespace run_status_db {

const std::string RUNNING("running");
const std::string COMPLETED("completed");
const std::string FAILED("failed");
const std::string CANCELED("canceled");

}

} // namespace bgws::blue_gene::diagnostics


namespace performance_monitoring {

const std::string& getFunctionBoot() {
    static const std::string s("boot");
    return s;
}

} // namespace bgws::blue_gene::performance_monitoring

namespace service_actions {


void setCwdBaremetalBin()
{
   if ( chdir( "/bgsys/drivers/ppcfloor/baremetal/bin" ) ) {
       BOOST_THROW_EXCEPTION( std::runtime_error( "Failed to set CWD to /bgsys/drivers/ppcfloor/baremetal/bin when starting service action." ) );
   }
}


namespace action {

const std::string CLOSE("close");
const std::string END("end");
const std::string PREPARE("prepare");

} // namespace bgws::blue_gene::service_actions::action


namespace db_actions {

const std::string CLOSED("CLOSED");
const std::string END("END");
const std::string OPEN("OPEN");
const std::string PREPARE("PREPARE");

} // namespace bgws::blue_gene::service_actions::db_actions


namespace db_statuses {

const std::string ACTIVE("A");
const std::string ERROR("E");
const std::string OPEN("O");
const std::string CLOSED("C");
const std::string FORCED("F");
const std::string PREPARED("P");

} // namespace bgws::blue_gene::service_actions::db_statuses


namespace hardware_type_name {

const std::string RACK("ServiceRack");
const std::string MIDPLANE("ServiceMidplane");
const std::string NODE_BOARD("ServiceNodeBoard");
const std::string NODE_DCA("ServiceNodeDCA");
const std::string IO_DRAWER("ServiceIoDrawer");
const std::string BULK_POWER_MODULE("ServiceBulkPowerModule");
const std::string CLOCK_CARD("ServiceClockCard");
    // Check PrepareServiceAction.cc if add a new type name.


const std::string& fromLocation( const bgq::util::Location& location )
{
    bgq::util::Location::Type loc_type(location.getType());

    if ( loc_type == bgq::util::Location::ComputeRack || loc_type ==  bgq::util::Location::IoRack ) {
        return RACK;
    }
    if ( loc_type == bgq::util::Location::Midplane ) {
        return MIDPLANE;
    }
    if ( loc_type == bgq::util::Location::NodeBoard || loc_type == bgq::util::Location::AllNodeBoardsOnMidplane ) {
        return NODE_BOARD;
    }
    if ( loc_type == bgq::util::Location::DcaOnNodeBoard ) {
        return NODE_DCA;
    }
    if ( loc_type == bgq::util::Location::IoBoardOnComputeRack || loc_type == bgq::util::Location::IoBoardOnIoRack ) {
        return IO_DRAWER;
    }
    if ( loc_type == bgq::util::Location::PowerModuleOnComputeRack || loc_type == bgq::util::Location::PowerModuleOnIoRack ) {
        return BULK_POWER_MODULE;
    }
    if ( loc_type == bgq::util::Location::ClockCardOnComputeRack ) {
        return CLOCK_CARD;
    }

    BOOST_THROW_EXCEPTION( InvalidLocationError( string(location) ) );

    static const std::string NOT_A_TYPE("notAtype");
    return NOT_A_TYPE;
}


InvalidLocationError::InvalidLocationError( const std::string& location )
    : std::invalid_argument( string() + "invalid location '" + location + "'" )
{
    // Nothing to do.
}


} // namespace bgws::blue_gene::service_actions::hardware_type_name


} // namespace service_actions

} } // namespace bgws::blue_gene
