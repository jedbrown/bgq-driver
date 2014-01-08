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

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

#include <sstream>
#include <stdexcept>
#include <vector>

using std::cout;
using std::ostringstream;
using std::string;
using std::vector;

#define THROW_LOCATION_ERROR_EXCEPTION( msg_expr ) \
    { \
        std::ostringstream tlee_oss_; \
        tlee_oss_ << msg_expr; \
        std::string tlee_str_(tlee_oss_.str()); \
        throw bgq::util::LocationError( tlee_str_ ); \
    }

namespace {

    string calcRegexStrMatchOnly( const string& str ) {
        return (string( "^" ) + str + "$");
    }

    const string ComputePart( "J([[:digit:]][[:digit:]])" );
    const string CorePart( "C([[:digit:]][[:digit:]])" );
    const string FanPart( "F([01])" );
    const string FanAssemblyPart( "H([012])" );
    const string OpticalModulePart( "O([[:digit:]][[:digit:]])" );

    const string ComputeRackBase( "R([0-9A-V])([0-9A-V])" );
    const string IoRackBase( "Q([0-9A-V])([0-9A-V])" );
    const string BulkPowerSupplyOnComputeRackBase(ComputeRackBase + "-B([0-3])");
    const string PowerModuleOnComputeRackBase(BulkPowerSupplyOnComputeRackBase + "-P([0-8])");
    const string BulkPowerSupplyInIoRackBase(IoRackBase + "-B");
    const string PowerModuleOnIoRackBase(BulkPowerSupplyInIoRackBase + "-P([0-5])");
    const string MidplaneBase(ComputeRackBase + "-M([01])");
    const string ServiceCardBase(MidplaneBase + "-S");
    const string ClockCardOnComputeBase(ComputeRackBase + "-K");
    const string ClockCardOnIoBase(IoRackBase + "-K([01])");
    const string IoBoardOnComputeBase( ComputeRackBase + "-I([CDEF])" );
    const string IoBoardOnIoBase(IoRackBase + "-I([0-9AB])");
    const string AllNodeBoardsOnMidplaneBase(MidplaneBase + "-N");
    const string NodeBoardBase(AllNodeBoardsOnMidplaneBase + "([[:digit:]][[:digit:]])");
    const string ComputeOnNodeBoardBase(NodeBoardBase + "-" + ComputePart);
    const string ComputeOnIoBoardBase(IoBoardOnIoBase + "-" + ComputePart);
    const string ComputeOnIoBoardInComputeRackBase(IoBoardOnComputeBase + "-" + ComputePart);
    const string CoreOnNodeBoardBase(ComputeOnNodeBoardBase + "-" + CorePart);
    const string CoreOnIoBoardBase(ComputeOnIoBoardBase + "-" + CorePart);
    const string DcaRegexBaseStr(NodeBoardBase + "-D([01])");
    const string PciRegexBaseStr(IoBoardOnIoBase + "-A([0-7])");
    const string LinkOnNodeBoardBase(NodeBoardBase + "-U(0[0-8])");
    const string LinkOnIoBoardInIoRackBase(IoBoardOnIoBase + "-U(0[0-5])");
    const string LinkOnIoBoardInComputeRackBase(IoBoardOnComputeBase + "-U(0[0-5])");
    const string OpticalOnNodeBoardBase(NodeBoardBase + "-" + OpticalModulePart);
    const string OpticalOnIoBoardBase(IoBoardOnIoBase + "-" + OpticalModulePart);
    const string FanAssemblyInComputeRackBase(IoBoardOnComputeBase + "-" + FanAssemblyPart);
    const string FanAssemblyInIoRackBase(IoBoardOnIoBase + "-" + FanAssemblyPart);
    const string FanInComputeRackBase(FanAssemblyInComputeRackBase + "-" + FanPart);
    const string FanInIoRackBase(FanAssemblyInIoRackBase + "-" + FanPart);
    const string LeakDetectorInComputeRackBase(ComputeRackBase + "-L");
    const string SwitchBase(string("([ABCD])_") + MidplaneBase);
    const string IoLinkOnNodeBoardBase("I_" + NodeBoardBase + "-T([[:digit:]][[:digit:]])");

    const string ComputeRackRegexStr(calcRegexStrMatchOnly( ComputeRackBase ));
    const string IoRackRegexStr(calcRegexStrMatchOnly( IoRackBase ));
    const string BulkPowerSupplyOnComputeRackRegexStr(calcRegexStrMatchOnly( BulkPowerSupplyOnComputeRackBase ));
    const string PowerModuleOnComputeRackRegexStr(calcRegexStrMatchOnly( PowerModuleOnComputeRackBase ));
    const string BulkPowerSupplyInIoRackRegexStr(calcRegexStrMatchOnly( BulkPowerSupplyInIoRackBase ));
    const string PowerModuleOnIoRackRegexStr(calcRegexStrMatchOnly( PowerModuleOnIoRackBase ));
    const string MidplaneRegexStr(calcRegexStrMatchOnly( MidplaneBase ));
    const string ServiceCardRegexStr(calcRegexStrMatchOnly( ServiceCardBase ));
    const string ClockCardOnComputeRegexStr(calcRegexStrMatchOnly( ClockCardOnComputeBase ));
    const string ClockCardOnIoRegexStr(calcRegexStrMatchOnly( ClockCardOnIoBase ));
    const string IoBoardOnComputeRegexStr(calcRegexStrMatchOnly( IoBoardOnComputeBase ));
    const string IoBoardOnIoRegexStr(calcRegexStrMatchOnly( IoBoardOnIoBase ));
    const string NodeBoardRegexStr(calcRegexStrMatchOnly( NodeBoardBase ));
    const string ComputeOnNodeBoardRegexStr(calcRegexStrMatchOnly( ComputeOnNodeBoardBase ));
    const string ComputeOnIoBoardRegexStr(calcRegexStrMatchOnly( ComputeOnIoBoardBase ));
    const string ComputeOnIoBoardInComputeRackRegexStr(calcRegexStrMatchOnly( ComputeOnIoBoardInComputeRackBase ));
    const string CoreOnNodeBoardRegexStr(calcRegexStrMatchOnly( CoreOnNodeBoardBase ));
    const string CoreOnIoBoardRegexStr(calcRegexStrMatchOnly( CoreOnIoBoardBase ));
    const string DcaRegexStr(calcRegexStrMatchOnly( DcaRegexBaseStr ));
    const string PciRegexStr(calcRegexStrMatchOnly( PciRegexBaseStr ));
    const string LinkOnNodeBoardRegexStr(calcRegexStrMatchOnly( LinkOnNodeBoardBase ));
    const string LinkOnIoBoardInIoRackRegexStr(calcRegexStrMatchOnly( LinkOnIoBoardInIoRackBase ));
    const string LinkOnIoBoardInComputeRackRegexStr(calcRegexStrMatchOnly( LinkOnIoBoardInComputeRackBase ));
    const string OpticalOnNodeBoardRegexStr(calcRegexStrMatchOnly( OpticalOnNodeBoardBase ));
    const string OpticalOnIoBoardRegexStr(calcRegexStrMatchOnly( OpticalOnIoBoardBase ));
    const string FanAssemblyInComputeRackRegexStr(calcRegexStrMatchOnly( FanAssemblyInComputeRackBase ));
    const string FanAssemblyInIoRackRegexStr(calcRegexStrMatchOnly( FanAssemblyInIoRackBase ));
    const string FanInComputeRackRegexStr(calcRegexStrMatchOnly( FanInComputeRackBase ));
    const string FanInIoRackRegexStr(calcRegexStrMatchOnly( FanInIoRackBase ));
    const string LeakDetectorInComputeRackRegexStr(calcRegexStrMatchOnly( LeakDetectorInComputeRackBase ));
    const string SwitchRegexStr(calcRegexStrMatchOnly( SwitchBase ));
    const string IoLinkOnNodeBoardRegexStr(calcRegexStrMatchOnly( IoLinkOnNodeBoardBase ));
    const string AllNodeBoardsOnMidplaneRegexStr(calcRegexStrMatchOnly( AllNodeBoardsOnMidplaneBase ));

    const unsigned MaxSubstitutions(1+6); // max number of substitutions in the above regex strs (full match and 1 per '()')


    regex_t compute_rack_regex;
    regex_t io_rack_regex;
    regex_t bulk_power_supply_on_compute_rack_regex;
    regex_t power_module_on_compute_rack_regex;
    regex_t bulk_power_supply_in_io_rack_regex;
    regex_t power_module_on_io_rack_regex;
    regex_t midplane_regex;
    regex_t service_card_regex;
    regex_t clock_card_on_compute_regex;
    regex_t clock_card_on_io_regex;
    regex_t io_board_on_compute_regex;
    regex_t io_board_on_io_regex;
    regex_t nodeboard_regex;
    regex_t compute_on_nodeboard_regex;
    regex_t compute_on_io_board_regex;
    regex_t compute_on_io_board_in_compute_rack_regex;
    regex_t core_on_nodeboard_regex;
    regex_t core_on_io_board_regex;
    regex_t dca_regex;
    regex_t pci_regex;
    regex_t link_on_nodeboard_regex;
    regex_t link_on_io_board_in_io_rack_regex;
    regex_t link_on_io_board_in_compute_rack_regex;
    regex_t optical_on_nodeboard_regex;
    regex_t optical_on_io_board_regex;
    regex_t fan_assembly_in_compute_rack_regex;
    regex_t fan_assembly_in_io_rack_regex;
    regex_t fan_in_compute_rack_regex;
    regex_t fan_in_io_rack_regex;
    regex_t leak_detector_in_compute_rack_regex;
    regex_t switch_regex;
    regex_t io_link_on_node_board_regex;
    regex_t all_node_boards_on_midplane_regex;


    void compileRegex( regex_t& regex, const string& regex_str, const string& regex_name )
    {
        int regcomp_rc(regcomp( &regex, regex_str.c_str(), REG_EXTENDED ));
        if ( regcomp_rc != 0 ) {
            std::cerr << "compiling " << regex_name << " regex str failed in " << __FILE__ << "!" << std::endl;
        }
    }


    int initializeRegexps()
    {
        compileRegex( compute_rack_regex, ComputeRackRegexStr, "compute rack" );
        compileRegex( io_rack_regex, IoRackRegexStr, "I/O rack" );
        compileRegex( bulk_power_supply_on_compute_rack_regex, BulkPowerSupplyOnComputeRackRegexStr, "bulk power supply on compute rack" );
        compileRegex( power_module_on_compute_rack_regex, PowerModuleOnComputeRackRegexStr, "power module on compute rack" );
        compileRegex( bulk_power_supply_in_io_rack_regex, BulkPowerSupplyInIoRackRegexStr, "bulk power supply in I/O rack" );
        compileRegex( power_module_on_io_rack_regex, PowerModuleOnIoRackRegexStr, "power module on I/O rack" );
        compileRegex( midplane_regex, MidplaneRegexStr, "midplane" );
        compileRegex( service_card_regex, ServiceCardRegexStr, "service card" );
        compileRegex( clock_card_on_compute_regex, ClockCardOnComputeRegexStr, "clock card on compute rack" );
        compileRegex( clock_card_on_io_regex, ClockCardOnIoRegexStr, "clock card on I/O rack" );
        compileRegex( io_board_on_compute_regex, IoBoardOnComputeRegexStr, "I/O board on compute rack" );
        compileRegex( io_board_on_io_regex, IoBoardOnIoRegexStr, "I/O board on I/O rack" );
        compileRegex( nodeboard_regex, NodeBoardRegexStr, "nodeboard" );
        compileRegex( compute_on_nodeboard_regex, ComputeOnNodeBoardRegexStr, "compute card on nodeboard" );
        compileRegex( compute_on_io_board_regex, ComputeOnIoBoardRegexStr, "compute card on I/O board" );
        compileRegex( compute_on_io_board_in_compute_rack_regex, ComputeOnIoBoardInComputeRackRegexStr, "compute card on I/O board in compute rack" );
        compileRegex( core_on_nodeboard_regex, CoreOnNodeBoardRegexStr, "compute card core on nodeboard" );
        compileRegex( core_on_io_board_regex, CoreOnIoBoardRegexStr, "compute card core on I/O board" );
        compileRegex( dca_regex, DcaRegexStr, "DCA on nodeboard" );
        compileRegex( pci_regex, PciRegexStr, "PCI adapter card" );
        compileRegex( link_on_nodeboard_regex, LinkOnNodeBoardRegexStr, "link module on nodeboard" );
        compileRegex( link_on_io_board_in_io_rack_regex, LinkOnIoBoardInIoRackRegexStr, "link module on I/O board in I/O rack" );
        compileRegex( link_on_io_board_in_compute_rack_regex, LinkOnIoBoardInComputeRackRegexStr, "link module on I/O board in compute rack" );
        compileRegex( optical_on_nodeboard_regex, OpticalOnNodeBoardRegexStr, "optical module on nodeboard" );
        compileRegex( optical_on_io_board_regex, OpticalOnIoBoardRegexStr, "optical module on I/O board" );
        compileRegex( fan_assembly_in_compute_rack_regex, FanAssemblyInComputeRackRegexStr, "fan assembly in compute rack" );
        compileRegex( fan_assembly_in_io_rack_regex, FanAssemblyInIoRackRegexStr, "fan assembly in I/O rack" );
        compileRegex( fan_in_compute_rack_regex, FanInComputeRackRegexStr, "fan in compute rack" );
        compileRegex( fan_in_io_rack_regex, FanInIoRackRegexStr, "fan in I/O rack" );
        compileRegex( leak_detector_in_compute_rack_regex, LeakDetectorInComputeRackRegexStr, "leak detector in compute rack" );
        compileRegex( switch_regex, SwitchRegexStr, "switch" );
        compileRegex( io_link_on_node_board_regex, IoLinkOnNodeBoardRegexStr, "I/O link on node board" );
        compileRegex( all_node_boards_on_midplane_regex, AllNodeBoardsOnMidplaneRegexStr, "all node boards on midplane" );

        return 0;
    }


    const int InitRc(initializeRegexps());


    bool match( regex_t& regex, const string& str, regmatch_t matches[] )
    {
        int rc(regexec( &regex, str.c_str(), MaxSubstitutions + 1, matches, 0 /*eflags*/ ));

        if ( rc == 0 ) {
            return true;
        }

        if ( rc == REG_NOMATCH ) {
            return false;
        }

        // regexec encountered an error.

        size_t error_buf_size(regerror( rc, &regex, NULL, 0 ));
        vector<char> error_buf( error_buf_size );
        regerror( rc, &regex, &error_buf[0], error_buf_size );

        ostringstream oss;
        oss << "regexec failed with '" << &error_buf[0] << "'";
        throw std::runtime_error( oss.str() );
    }


    uint8_t parseCharToNum( int rack_num_char )
    {
        if ( rack_num_char >= '0' && rack_num_char <= '9' ) {
            return uint8_t(rack_num_char - '0');
        }

        return uint8_t(10 + (rack_num_char - 'A'));
    }

} // anonymous namespace


namespace bgq {
namespace util {


Location::Location()
    : _type(NotValid), _rack_type(NotValid), _switch_dimension( Dimension::NONE )
{
    // Nothing to do.
}

Location::Location( const string& location, ThrowsSpec throws_spec )
{
    set( location, throws_spec );
}


void
Location::set( const string& location, ThrowsSpec throws_spec )
{
    _location = boost::algorithm::trim_copy( location );
    _type = NotValid;
    _rack_type = NotValid;

    unsigned location_size(_location.size());

    regmatch_t matches[MaxSubstitutions];
    int match_idx(1);

    if ( (location_size == ComputeRackLength) && match( compute_rack_regex, _location, matches ) ) {
        _type = ComputeRack;
        _rack_type = ComputeRack;
        _extractRackSubs( matches, &match_idx );
        return;
    }

    if ( (location_size == IoRackLength) && match( io_rack_regex, _location, matches ) ) {
        _type = IoRack;
        _rack_type = IoRack;
        _extractRackSubs( matches, &match_idx );
        return;
    }

    if ( (location_size == BulkPowerSupplyOnComputeRackLength) && match( bulk_power_supply_on_compute_rack_regex, _location, matches ) ) {
        _type = BulkPowerSupplyOnComputeRack;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );
        _extractBulkPowerSupply( matches, &match_idx );

        return;
    }

    if ( (location_size == PowerModuleOneComputeRackLength) && match( power_module_on_compute_rack_regex, _location, matches ) ) {
        _type = PowerModuleOnComputeRack;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );
        _extractBulkPowerSupply( matches, &match_idx );
        _extractPowerModule( matches, &match_idx );

        return;
    }

    if ( (location_size == BulkPowerSupplyInIoRackLength) && match( bulk_power_supply_in_io_rack_regex, _location, matches ) ) {
        _type = BulkPowerSupplyInIoRack;
        _rack_type = IoRack;

        _extractRackSubs( matches, &match_idx );

        return;
    }

    if ( (location_size == PowerModuleOnIoRackLength) && match( power_module_on_io_rack_regex, _location, matches ) ) {
        _type = PowerModuleOnIoRack;
        _rack_type = IoRack;

        _extractRackSubs( matches, &match_idx );
        _extractPowerModule( matches, &match_idx );

        return;
    }

    if ( (location_size == MidplaneLength) && match( midplane_regex, _location, matches ) ) {
        _type = Midplane;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );
        _extractMidplane( matches, &match_idx );

        return;
    }

    if ( (location_size == ServiceCardLength) && match( service_card_regex, _location, matches ) ) {
        _type = ServiceCard;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );
        _extractMidplane( matches, &match_idx );

        return;
    }

    if ( (location_size == ClockCardOnComputeRackLength) && match( clock_card_on_compute_regex, _location, matches ) ) {
        _type = ClockCardOnComputeRack;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );

        return;
    }

    if ( (location_size == ClockCardOnIoRackLength) && match( clock_card_on_io_regex, _location, matches ) ) {
        _type = ClockCardOnIoRack;
        _rack_type = IoRack;

        _extractRackSubs( matches, &match_idx );
        _extractClockCard( matches, &match_idx );

        return;
    }

    if ( (location_size == IoBoardOnComputeRackLength) && match( io_board_on_compute_regex, _location, matches ) ) {
        _type = IoBoardOnComputeRack;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );
        _extractIoBoard( matches, &match_idx );

        return;
    }

    if ( (location_size == IoBoardOnIoRackLength) && match( io_board_on_io_regex, _location, matches ) ) {
        _type = IoBoardOnIoRack;
        _rack_type = IoRack;

        _extractRackSubs( matches, &match_idx );
        _extractIoBoard( matches, &match_idx );

        return;
    }

    if ( (location_size == NodeBoardLength) && match( nodeboard_regex, _location, matches ) ) {
        _type = NodeBoard;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );
        _extractMidplane( matches, &match_idx );
        _extractNodeBoard( matches, &match_idx );

        return;
    }

    if ( (location_size == ComputeCardOnNodeBoardLength) && match( compute_on_nodeboard_regex, _location, matches ) ) {
        _type = ComputeCardOnNodeBoard;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );
        _extractMidplane( matches, &match_idx );
        _extractNodeBoard( matches, &match_idx );
        _extractComputeCard( matches, &match_idx );

        return;
    }

    if ( (location_size == ComputeCardOnIoBoardLength) && match( compute_on_io_board_regex, _location, matches ) ) {
        _type = ComputeCardOnIoBoard;
        _rack_type = IoRack;

        _extractRackSubs( matches, &match_idx );
        _extractIoBoard( matches, &match_idx );
        _extractComputeCard( matches, &match_idx );

        return;
    }

    if ( (location_size == ComputeCardOnIoBoardLength) && match( compute_on_io_board_in_compute_rack_regex, _location, matches ) ) {
        _type = ComputeCardOnIoBoard;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );
        _extractIoBoard( matches, &match_idx );
        _extractComputeCard( matches, &match_idx );

        return;
    }


    if ( (location_size == ComputeCardCoreOnNodeBoardLength) && match( core_on_nodeboard_regex, _location, matches ) ) {
        _type = ComputeCardCoreOnNodeBoard;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );
        _extractMidplane( matches, &match_idx );
        _extractNodeBoard( matches, &match_idx );
        _extractComputeCard( matches, &match_idx );
        _extractCore( matches, &match_idx );

        return;
    }

    if ( (location_size == ComputeCardCoreOnIoBoardLength) && match( core_on_io_board_regex, _location, matches ) ) {
        _type = ComputeCardCoreOnIoBoard;
        _rack_type = IoRack;

        _extractRackSubs( matches, &match_idx );
        _extractIoBoard( matches, &match_idx );
        _extractComputeCard( matches, &match_idx );
        _extractCore( matches, &match_idx );

        return;
    }

    if ( (location_size == DcaOnNodeBoardLength) && match( dca_regex, _location, matches ) ) {
        _type = DcaOnNodeBoard;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );
        _extractMidplane( matches, &match_idx );
        _extractNodeBoard( matches, &match_idx );
        _extractDCA( matches, &match_idx );

        return;
    }

    if ( (location_size == PciAdapterCardLength) && match( pci_regex, _location, matches ) ) {
        _type = PciAdapterCard;
        _rack_type = IoRack;

        _extractRackSubs( matches, &match_idx );
        _extractIoBoard( matches, &match_idx );
        _extractPciAdapter( matches, &match_idx );

        return;
    }

    if ( (location_size == LinkModuleOnNodeBoardLength) && match( link_on_nodeboard_regex, _location, matches ) ) {
        _type = LinkModuleOnNodeBoard;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );
        _extractMidplane( matches, &match_idx );
        _extractNodeBoard( matches, &match_idx );
        _extractLinkModule( matches, &match_idx );

        return;
    }

    if ( (location_size == LinkModuleOnIoBoardLength) && match( link_on_io_board_in_io_rack_regex, _location, matches ) ) {
        _type = LinkModuleOnIoBoardInIoRack;
        _rack_type = IoRack;

        _extractRackSubs( matches, &match_idx );
        _extractIoBoard( matches, &match_idx );
        _extractLinkModule( matches, &match_idx );

        return;
    }

    if ( (location_size == LinkModuleOnIoBoardLength) && match( link_on_io_board_in_compute_rack_regex, _location, matches ) ) {
        _type = LinkModuleOnIoBoardInComputeRack;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );
        _extractIoBoard( matches, &match_idx );
        _extractLinkModule( matches, &match_idx );

        return;
    }

    if ( (location_size == OpticalModuleOnNodeBoardLength) && match( optical_on_nodeboard_regex, _location, matches ) ) {
        _type = OpticalModuleOnNodeBoard;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );
        _extractMidplane( matches, &match_idx );
        _extractNodeBoard( matches, &match_idx );
        _extractOpticalModule( matches, &match_idx );

        return;
    }

    if ( (location_size == OpticalModuleOnIoBoardLength) && match( optical_on_io_board_regex, _location, matches ) ) {
        _type = OpticalModuleOnIoBoard;
        _rack_type = IoRack;

        _extractRackSubs( matches, &match_idx );
        _extractIoBoard( matches, &match_idx );
        _extractOpticalModule( matches, &match_idx );

        return;
    }

    if ( (location_size == FanAssemblyInComputeRackLength) && match( fan_assembly_in_compute_rack_regex, _location, matches ) ) {
        _type = FanAssemblyInComputeRack;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );
        _extractIoBoard( matches, &match_idx );
        _extractFanAssembly( matches, &match_idx );

        return;
    }

    if ( (location_size == FanAssemblyInIoRackLength) && match( fan_assembly_in_io_rack_regex, _location, matches ) ) {
        _type = FanAssemblyInIoRack;
        _rack_type = IoRack;

        _extractRackSubs( matches, &match_idx );
        _extractIoBoard( matches, &match_idx );
        _extractFanAssembly( matches, &match_idx );

        return;
    }

    if ( (location_size == FanInComputeRackLength) && match( fan_in_compute_rack_regex, _location, matches ) ) {
        _type = FanInComputeRack;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );
        _extractIoBoard( matches, &match_idx );
        _extractFanAssembly( matches, &match_idx );
        _extractFan( matches, &match_idx );

        return;
    }

    if ( (location_size == FanInIoRackLength) && match( fan_in_io_rack_regex, _location, matches ) ) {
        _type = FanInIoRack;
        _rack_type = IoRack;

        _extractRackSubs( matches, &match_idx );
        _extractIoBoard( matches, &match_idx );
        _extractFanAssembly( matches, &match_idx );
        _extractFan( matches, &match_idx );

        return;
    }

    if ( (location_size == LeakDetectorInComputeRackLength) && match( leak_detector_in_compute_rack_regex, _location, matches ) ) {
        _type = LeakDetectorInComputeRack;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );

        return;
    }

    if ( (location_size == SwitchLength) && match( switch_regex, _location, matches ) ) {
        _type = Switch;
        _rack_type = ComputeRack;

        string dimension_str(_location.substr( matches[match_idx++].rm_so, 1 ));

        if ( dimension_str == "A" )  _switch_dimension = Dimension::A;
        else if ( dimension_str == "B" )  _switch_dimension = Dimension::B;
        else if ( dimension_str == "C" )  _switch_dimension = Dimension::C;
        else if ( dimension_str == "D" )  _switch_dimension = Dimension::D;
        else {
            // Shouldn't be able to get here.
            THROW_LOCATION_ERROR_EXCEPTION( "location string '" << location << "' is not valid, the dimension is incorrect" );
        }

        _extractRackSubs( matches, &match_idx );
        _extractMidplane( matches, &match_idx );

        return;
    }

    if ( (location_size == IoLinkOnNodeBoardLength) && match( io_link_on_node_board_regex, _location, matches ) ) {
        _type = IoLinkOnNodeBoard;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );
        _extractMidplane( matches, &match_idx );
        _extractNodeBoard( matches, &match_idx );
        _extractPort( matches, &match_idx );

        return;
    }

    if ( (location_size == AllNodeBoardsOnMidplaneLength) && match( all_node_boards_on_midplane_regex, _location, matches ) ) {
        _type = AllNodeBoardsOnMidplane;
        _rack_type = ComputeRack;

        _extractRackSubs( matches, &match_idx );
        _extractMidplane( matches, &match_idx );

        return;
    }

    if ( throws_spec == throws_on_bad_location ) {
        THROW_LOCATION_ERROR_EXCEPTION( "location string '" << location << "' is not valid" );
    }
}


string Location::getMidplaneLocation() const
{
    if ( ! (_type == Midplane ||
            _type == ServiceCard ||
            _type == NodeBoard ||
            _type == ComputeCardOnNodeBoard ||
            _type == ComputeCardCoreOnNodeBoard ||
            _type == DcaOnNodeBoard ||
            _type == LinkModuleOnNodeBoard ||
            _type == OpticalModuleOnNodeBoard ||
            _type == Switch ||
            _type == IoLinkOnNodeBoard ||
            _type == AllNodeBoardsOnMidplane ) )
    {
        THROW_LOCATION_ERROR_EXCEPTION( "cannot get midplane from '" << _location << "' because it doesn't contain a midplane" );
    }

    if ( _type == Switch ) {
        return _location.substr( SwitchLength - MidplaneLength, MidplaneLength );
    }

    if ( _type == IoLinkOnNodeBoard ) {
        return _location.substr( string( "I-" ).size(), MidplaneLength );
    }

    return _location.substr( 0, MidplaneLength );
}


Location::Type Location::getRackType() const
{
    return _rack_type;
}


uint8_t Location::getRackRow() const
{
    return _rack_row;
}


uint8_t Location::getRackColumn() const
{
    return _rack_column;
}


uint8_t Location::getBulkPowerSupply() const
{
    if ( ! (_type == BulkPowerSupplyOnComputeRack ||
            _type == PowerModuleOnComputeRack) )
    {
        THROW_LOCATION_ERROR_EXCEPTION( "cannot get bulk power supply from '" << _location << "' because it doesn't contain a bulk power supply" );
    }
    return _bulk_power_supply;
}


uint8_t Location::getPowerModule() const
{
    if (! (_type == PowerModuleOnComputeRack ||
           _type == PowerModuleOnIoRack) )
    {
        THROW_LOCATION_ERROR_EXCEPTION( "cannot get power module from '" << _location << "' because it doesn't contain a power module" );
    }
    return _power_module;
}


uint8_t Location::getMidplane() const
{
    if ( ! (_type == Midplane ||
            _type == ServiceCard ||
            _type == NodeBoard ||
            _type == ComputeCardOnNodeBoard ||
            _type == ComputeCardCoreOnNodeBoard ||
            _type == DcaOnNodeBoard ||
            _type == LinkModuleOnNodeBoard ||
            _type == OpticalModuleOnNodeBoard ||
            _type == Switch ||
            _type == IoLinkOnNodeBoard ||
            _type == AllNodeBoardsOnMidplane ) )
    {
        THROW_LOCATION_ERROR_EXCEPTION( "cannot get midplane from '" << _location << "' because it doesn't contain a midplane" );
    }
    return _midplane;
}


uint8_t Location::getClockCard() const
{
    if ( _type != ClockCardOnIoRack )
    {
        THROW_LOCATION_ERROR_EXCEPTION( "cannot get clock card from '" << _location << "' because it doesn't contain a clock card" );
    }
    return _clock_card;
}


uint8_t Location::getIoBoard() const
{
    if ( ! (_type == IoBoardOnComputeRack ||
            _type == IoBoardOnIoRack ||
            _type == ComputeCardOnIoBoard ||
            _type == ComputeCardCoreOnIoBoard ||
            _type == PciAdapterCard ||
            _type == LinkModuleOnIoBoardInIoRack ||
            _type == LinkModuleOnIoBoardInComputeRack ||
            _type == OpticalModuleOnIoBoard ||
            _type == FanAssemblyInComputeRack ||
            _type == FanAssemblyInIoRack ||
            _type == FanInComputeRack ||
            _type == FanInIoRack) )
    {
        THROW_LOCATION_ERROR_EXCEPTION( "cannot get I/O board from '" << _location << "' because it doesn't contain an I/O board" );
    }
    return _io_board;
}


uint8_t Location::getNodeBoard() const
{
    if ( ! (_type == NodeBoard ||
            _type == ComputeCardOnNodeBoard ||
            _type == ComputeCardCoreOnNodeBoard ||
            _type == DcaOnNodeBoard ||
            _type == LinkModuleOnNodeBoard ||
            _type == OpticalModuleOnNodeBoard ||
            _type == IoLinkOnNodeBoard) )
    {
        THROW_LOCATION_ERROR_EXCEPTION( "cannot get node board from '" << _location << "' because it doesn't contain a node board" );
    }
    return _node_board;
}


uint8_t Location::getFan() const
{
    if ( ! (_type == FanInComputeRack ||
            _type == FanInIoRack) )
    {
        THROW_LOCATION_ERROR_EXCEPTION( "cannot get fan from '" << _location << "' because it doesn't contain a fan" );
    }
    return _fan;
}


uint8_t Location::getComputeCard() const
{
    if ( ! (_type == ComputeCardOnNodeBoard ||
            _type == ComputeCardOnIoBoard ||
            _type == ComputeCardCoreOnNodeBoard ||
            _type == ComputeCardCoreOnIoBoard) )
    {
        THROW_LOCATION_ERROR_EXCEPTION( "cannot get compute card from '" << _location << "' because it doesn't contain a compute card" );
    }
    return _compute_card;
}


uint8_t Location::getCore() const
{
    if ( ! (_type == ComputeCardCoreOnNodeBoard ||
            _type == ComputeCardCoreOnIoBoard) )
    {
        THROW_LOCATION_ERROR_EXCEPTION( "cannot get compute core from '" << _location << "' because it doesn't contain a compute core" );
    }
    return _core;
}


uint8_t Location::getDCA() const
{
    if ( _type != DcaOnNodeBoard )
    {
        THROW_LOCATION_ERROR_EXCEPTION( "cannot get DCA from '" << _location << "' because it doesn't contain a DCA" );
    }
    return _dca;
}


uint8_t Location::getPciAdapterCard() const
{
    if ( _type != PciAdapterCard )
    {
        THROW_LOCATION_ERROR_EXCEPTION( "cannot get PCI adapter card from '" << _location << "' because it doesn't contain a PCI adapter card" );
    }
    return _pci_adapter_card;
}


uint8_t Location::getLinkModule() const
{
    if ( ! (_type == LinkModuleOnNodeBoard ||
            _type == LinkModuleOnIoBoardInIoRack ||
            _type == LinkModuleOnIoBoardInComputeRack) )
    {
        THROW_LOCATION_ERROR_EXCEPTION( "cannot get link module from '" << _location << "' because it doesn't contain a link module" );
    }
    return _link_module;
}


uint8_t Location::getOpticalModule() const
{
    if ( ! (_type == OpticalModuleOnNodeBoard ||
            _type == OpticalModuleOnIoBoard) )
    {
        THROW_LOCATION_ERROR_EXCEPTION( "cannot get optical module from '" << _location << "' because it doesn't contain an optical module" );
    }
    return _optical_module;
}


uint8_t Location::getFanAssembly() const
{
    if ( ! (_type == FanAssemblyInComputeRack ||
            _type == FanAssemblyInIoRack ||
            _type == FanInComputeRack ||
            _type == FanInIoRack) )
    {
        THROW_LOCATION_ERROR_EXCEPTION( "cannot get fan assembly from '" << _location << "' because it doesn't contain a fan module" );
    }
    return _fan_assembly;
}


Location::Dimension::Value Location::getSwitchDimension() const
{
    if ( _type != Switch )
    {
        THROW_LOCATION_ERROR_EXCEPTION( "cannot get switch dimension from '" << _location << "' because it doesn't contain a switch dimension" );
    }
    return _switch_dimension;
}


uint8_t Location::getPort() const
{
    if ( _type != IoLinkOnNodeBoard ) {
        THROW_LOCATION_ERROR_EXCEPTION( "cannot get port from '" << _location << "' because it doesn't contain a port" );
    }
    return _port;
}


void Location::_extractRackType( regmatch_t matches[], int* match_idx_in_out )
{
    char rack_type_char(_location[matches[(*match_idx_in_out)++].rm_so]);
    _rack_type = (rack_type_char == 'R' ? ComputeRack : IoRack);
}


void Location::_extractRackSubs( regmatch_t matches[], int* match_idx_in_out )
{
    int rack_row_num_char(_location[matches[(*match_idx_in_out)++].rm_so]);
    _rack_row = parseCharToNum( rack_row_num_char );

    int rack_col_num_char(_location[matches[(*match_idx_in_out)++].rm_so]);
    _rack_column = parseCharToNum( rack_col_num_char );
}


void Location::_extractBulkPowerSupply( regmatch_t matches[], int* match_idx_in_out )
{
    string bps_pos_str(_location.substr( matches[(*match_idx_in_out)++].rm_so, 1 ));
    _bulk_power_supply = boost::lexical_cast<unsigned>( bps_pos_str );
}


void Location::_extractPowerModule( regmatch_t matches[], int* match_idx_in_out )
{
    string power_module_pos_str(_location.substr( matches[(*match_idx_in_out)++].rm_so, 1 ));
    _power_module = boost::lexical_cast<unsigned>( power_module_pos_str );
}


void Location::_extractMidplane( regmatch_t matches[], int* match_idx_in_out )
{
    string midplane_str(_location.substr( matches[(*match_idx_in_out)++].rm_so, 1 ));
    _midplane = boost::lexical_cast<unsigned>( midplane_str );
}


void Location::_extractClockCard( regmatch_t matches[], int* match_idx_in_out )
{
    string clock_card_str(_location.substr( matches[(*match_idx_in_out)++].rm_so, 1 ));
    _clock_card = boost::lexical_cast<unsigned>( clock_card_str );
}


void Location::_extractIoBoard( regmatch_t matches[], int* match_idx_in_out )
{
    string io_board_str(_location.substr( matches[(*match_idx_in_out)++].rm_so, 1 ));
    _io_board = parseCharToNum( io_board_str[0] );
}


void Location::_extractNodeBoard( regmatch_t matches[], int* match_idx_in_out )
{
    string nodeboard_pos_str(_location.substr( matches[(*match_idx_in_out)++].rm_so, 2 ));
    _node_board = boost::lexical_cast<unsigned>( nodeboard_pos_str );

    if ( _node_board >= NodeBoardsOnMidplane ) {
        THROW_LOCATION_ERROR_EXCEPTION( "location string '" << _location << "' is not valid, the node board position is too large" );
    }
}


void Location::_extractFan( regmatch_t matches[], int* match_idx_in_out )
{
    string fan_pos_str(_location.substr( matches[(*match_idx_in_out)++].rm_so, 1 ));
    _fan = boost::lexical_cast<unsigned>( fan_pos_str );
}


void Location::_extractComputeCard( regmatch_t matches[], int* match_idx_in_out )
{
    string compute_card_pos_str(_location.substr( matches[(*match_idx_in_out)++].rm_so, 2 ));
    _compute_card = boost::lexical_cast<unsigned>( compute_card_pos_str );

    uint8_t max_compute_card(0);

    if ( (_type == ComputeCardOnNodeBoard) ||
         (_type == ComputeCardCoreOnNodeBoard) ) {
        max_compute_card = ComputeCardsOnNodeBoard;
    } else if ( (_type == ComputeCardOnIoBoard) ||
                (_type == ComputeCardCoreOnIoBoard)
            ) {
        max_compute_card = ComputeCardsOnIoBoard;
    }

    if ( _compute_card >= max_compute_card ) {
        THROW_LOCATION_ERROR_EXCEPTION( "location string '" << _location << "' is not valid, the compute card position is too large" );
    }
}


void Location::_extractCore( regmatch_t matches[], int* match_idx_in_out )
{
    string core_pos_str(_location.substr( matches[(*match_idx_in_out)++].rm_so, 2 ));
    _core = boost::lexical_cast<unsigned>( core_pos_str );

    if ( _core >= ComputeCardCoresOnBoard ) {
        THROW_LOCATION_ERROR_EXCEPTION( "location string '" << _location << "' is not valid, the compute card core position is too large" );
    }
}


void Location::_extractDCA( regmatch_t matches[], int* match_idx_in_out )
{
    string dca_pos_str(_location.substr( matches[(*match_idx_in_out)++].rm_so, 1 ));
    _dca = boost::lexical_cast<unsigned>( dca_pos_str );
}


void Location::_extractPciAdapter( regmatch_t matches[], int* match_idx_in_out )
{
    string pci_pos_str(_location.substr( matches[(*match_idx_in_out)++].rm_so, 1 ));
    _pci_adapter_card = boost::lexical_cast<unsigned>( pci_pos_str );
}


void Location::_extractLinkModule( regmatch_t matches[], int* match_idx_in_out )
{
    string link_module_pos_str(_location.substr( matches[(*match_idx_in_out)++].rm_so, 2 ));
    _link_module = boost::lexical_cast<unsigned>( link_module_pos_str );
}


void Location::_extractOpticalModule( regmatch_t matches[], int* match_idx_in_out )
{
    string optical_module_pos_str(_location.substr( matches[(*match_idx_in_out)++].rm_so, 2 ));
    _optical_module = boost::lexical_cast<unsigned>( optical_module_pos_str );

    uint8_t max_optical_module_pos(0);

    if ( _type == OpticalModuleOnNodeBoard ) {
        max_optical_module_pos = OpticalModulesOnNodeBoard;
    } else if ( _type == OpticalModuleOnIoBoard ) {
        max_optical_module_pos = OpticalModulesOnIoBoard;
    }

    if ( _optical_module >= max_optical_module_pos ) {
        THROW_LOCATION_ERROR_EXCEPTION( "location string '" << _location << "' is not valid, the optical module position is too large" );
    }
}


void Location::_extractFanAssembly( regmatch_t matches[], int* match_idx_in_out )
{
    string fan_assembly_str(_location.substr( matches[(*match_idx_in_out)++].rm_so, 1 ));
    _fan_assembly = boost::lexical_cast<unsigned>( fan_assembly_str );
}


void Location::_extractPort( regmatch_t matches[], int* match_idx_in_out )
{
    string port_str(_location.substr( matches[(*match_idx_in_out)++].rm_so, 2 ));
    _port = boost::lexical_cast<unsigned>( port_str );

    if ( _port > 11 ) {
        THROW_LOCATION_ERROR_EXCEPTION( "location string '" << _location << "' is not valid, the port position is too large" );
    }
}


LocationError::LocationError( const string& what )
    : std::invalid_argument( what )
{
    // Nothing to do.
}


} // namespace bgq::util
} // namespace bgq
