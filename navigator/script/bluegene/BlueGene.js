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

define(
[
    "module"
],
function(
        module
    )
{


/**
 * @name bluegene^BlueGene
 * @namespace Blue Gene information.
 */


var _Dimension = { A:0, B:1, C:2, D:3, E:4 };


var _rackCoordinateToLocationString = function( coord ) /**String*/
{
    return "R" + coord.row.toString( 16 ).toUpperCase() + coord.col.toString( 16 ).toUpperCase();
};


var _hardwareTypes = {
        BULK_POWER_SUPPLY: "bulkPowerSupply",
        CLOCK_CARD: "clockCard",
        COMPUTE_RACK: "computeRack",
        FAN_MODULE: "fanModule",
        IO_ADAPTER: "ioAdapter",
        IO_DRAWER: "ioDrawer",
        IO_NODE: "ioNode",
        IO_RACK: "ioRack",
        IO_LINK_CHIP: "ioLinkChip",
        LINK: "link",
        LINK_CHIP: "linkChip",
        MIDPLANE : "midplane",
        NODE_BOARD: "nodeBoard",
        NODE_BOARD_DCA: "nodeBoardDca",
        SERVICE_CARD: "serviceCard",
        SWITCH: "switch",
        CLOCK_CARD: "clockCard",

        _BULK_POWER_SUPPLY_RE: /^(R..-B.-P.)|(Q..-B-P.)$/,
        _CLOCK_CARD_RE: /^(R..-K)|(Q..-K.)$/,
        _COMPUTE_RACK_RE: /^R..$/,
        _FAN_MODULE_RE: /^(Q|R)..-I.-F$/,
        _IO_ADAPTER_RE: /^(Q|R)..-I.-A.$/,
        _IO_DRAWER_RE: /^(Q|R)..-I.$/,
        _IO_NODE_RE: /^(Q|R)..-I.-J..$/,
        _IO_RACK_RE: /^Q..$/,
        _IO_LINK_CHIP_RE: /^(Q|R)..-I.-U..$/,
        _LINK_RE: /,/, // contains a comma.
        _LINK_CHIP_RE: /^R..-M.-N..-U..$/,
        _MIDPLANE_RE: /^R..-M.$/,
        _NODE_BOARD_RE: /^R..-M.-N..$/,
        _NODE_BOARD_DCA_RE: /^R..-M.-N..-D.$/,
        _SWITCH_RE: /^._R..-M.$/,
        _SERVICE_CARD_RE: /^R..-M.-S$/,
        _CLOCK_CARD_RE: /^(R..-K)|(Q..-K.)$/
	};

_hardwareTypes.forLocation = function( location ) {
        var t = _hardwareTypes;
        return (t._BULK_POWER_SUPPLY_RE.test( location ) ? t.BULK_POWER_SUPPLY :
                t._CLOCK_CARD_RE.test( location ) ? t.CLOCK_CARD :
                t._COMPUTE_RACK_RE.test( location ) ? t.COMPUTE_RACK :
                t._FAN_MODULE_RE.test( location ) ? t.FAN_MODULE :
                t._IO_ADAPTER_RE.test( location ) ? t.IO_ADAPTER :
                t._IO_DRAWER_RE.test( location ) ? t.IO_DRAWER :
                t._IO_NODE_RE.test( location ) ? t.IO_NODE :
                t._IO_RACK_RE.test( location ) ? t.IO_RACK :
                t._IO_LINK_CHIP_RE.test( location ) ? t.IO_LINK_CHIP :
                t._LINK_RE.test( location ) ? t.LINK :
                t._LINK_CHIP_RE.test( location ) ? t.LINK_CHIP :
                t._MIDPLANE_RE.test( location ) ? t.MIDPLANE :
                t._NODE_BOARD_RE.test( location ) ? t.NODE_BOARD :
                t._NODE_BOARD_DCA_RE.test( location ) ? t.NODE_BOARD_DCA :
                t._SWITCH_RE.test( location ) ? t.SWITCH :
                t._SERVICE_CARD_RE.test( location ) ? t.SERVICE_CARD :
                t._CLOCK_CARD_RE.test( location ) ? t.HWT :
                undefined);
    };


var b_BlueGene =

/** @lends bluegene^BlueGene */
{

    Dimension : _Dimension,

    MidplaneDimensions : [
            _Dimension.A,
            _Dimension.B,
            _Dimension.C,
            _Dimension.D
        ],

    Connectivity : { Mesh :0, Torus :1 },

    nodeBoardPositions :
      [ "N00", "N01", "N02", "N03",
        "N04", "N05", "N06", "N07",
        "N08", "N09", "N10", "N11",
        "N12", "N13", "N14", "N15"
      ],

    nodesInMidplane : [ 4, 4, 4, 4, 2 ],
        // This is the number of nodes in a midplane in each dimension.

    smallBlockInfo : {
            1: {
                 nodeBoards: [ ["N00"], ["N01"], ["N02"], ["N03"],
                               ["N04"], ["N05"], ["N06"], ["N07"],
                               ["N08"], ["N09"], ["N10"], ["N11"],
                               ["N12"], ["N13"], ["N14"], ["N15"]
                             ],
                 shape: [ 2, 2, 2, 2, 2 ],
                 connectivity: [ false, false, false, true ]
               },
            2: {
                  nodeBoards: [ ["N00","N01"], ["N02","N03"],
                                ["N04","N05"], ["N06","N07"],
                                ["N08","N09"], ["N10","N11"],
                                ["N12","N13"], ["N14","N15"]
                              ],
                  shape: [ 2, 2, 4, 2, 2 ],
                  connectivity: [ false,false, true, false ]
               },
            4: {
                   nodeBoards: [ ["N00","N01","N02","N03"],
                                 ["N04","N05","N06","N07"],
                                 ["N08","N09","N10","N11"],
                                 ["N12","N13","N14","N15"]
                               ],
                   shape: [ 2, 2, 4, 4, 2 ],
                   connectivity: [ false, true, true, false ]
               },
            8: {
                   nodeBoards: [ ["N00","N01","N02","N03",
                                  "N04","N05","N06","N07"],
                                 ["N08","N09","N10","N11",
                                  "N12","N13","N14","N15"]
                               ],
                   shape: [ 4, 2, 4, 4, 2 ],
                   connectivity: [ true, true, true, false ]
               }
        },

    /** Convert a rack coordinate (row, column) to a rack location string. */
    rackCoordinateToLocationString : _rackCoordinateToLocationString,

    /** Convert a midplane coordinate to a midplane location string. */
    midplaneCoordinateToLocationString : function( /**String*/ mp_coord ) /**String*/
    {
        return _rackCoordinateToLocationString( mp_coord ) + "-M" + mp_coord.mp;
    },


    /** Block status values. */
    blockStatus : {
            FREE: "F",
            ALLOCATED: "A",
            BOOTING: "B",
            INITIALIZED: "I",
            TERMINATING: "T"
        },


    /** Job status values. */
    jobStatus : {
            DEBUG: "D",
            ERROR: "E",
            LOADING: "L",
            CLEANUP: "N",
            SETUP: "P",
            RUNNING: "R",
            STARTING: "S",
            TERMINATED: "T"
        },

    /** Hardware status values. */
    hardwareStatus : {
            AVAILABLE: "A",
            MISSING: "M",
            ERROR: "E",
            SERVICE: "S",
            SOFTWARE_FAILURE: "F"
        },

    /** Service action status values. */
    serviceActionStatus : {
            OPEN: "O",
            ACTIVE : "A",
            PREPARED: "P",
            CLOSED: "C",
            FORCED: "F",
            ERROR: "E"
        },

    DEFAULT_MICROLOADER_IMAGE : "/bgsys/drivers/ppcfloor/boot/firmware",
    DEFAULT_IO_NODE_CONFIGURATION_NAME : "IODefault",
    DEFAULT_CNK_NODE_CONFIGURATION_NAME : "CNKDefault",


    performanceMonitoring : {
            bootSteps: [
                "mmcs/allocate block",
                "mmcs/load block XML",
                "mmcs/create block",
                "mmcs/load machine XML",
                "mmcs/create target set",
                "mmcs/boot block",
                "mc_server/receive boot request",
                "mc_server/target set arbitration",
                "mc_server/copy boot request",
                "mc/enableNodes",
                "mc/initDevbus",
                "mc/startTraining",
                "mc/enableLinkChips",
                "mc/trainLinkChips",
                "mc/trainTorus",
                "mc/installFirmware",
                "mc/installPersonalities",
                "mc/startCores",
                "mc/verifyMailbox",
                "mc/monitorMailbox",
                "mc/verifyMailboxReady",
                "mc/installKernelImages",
                "mc/configureDomains",
                "mc/launchKernels",
                "mc/verifyKernelReady",
                "mmcs/verify kernel ready"
              ]
        },


    hardwareTypes : _hardwareTypes,


    tealAlertState : {
            OPEN : 1,
            CLOSED : 2
        },


    calcDiagnosticsCommand : function( post_data )
    {
        var cmd_str = "rundiags.py";
        if ( post_data.midplanes )  cmd_str += " --midplanes " + post_data.midplanes.join( "," );
        if ( post_data.io )  cmd_str += " --io " + post_data.io.join( "," );
        cmd_str += " --tests " + post_data.tests.join( "," );
        if ( post_data.stopOnError )  cmd_str += " --stoponerror";
        if ( post_data.saveAllOutput )  cmd_str += " --savealloutput";
        if ( post_data.midplanesPerProc )  cmd_str += " --midplanesperproc=" + post_data.midplanesPerProc;
        if ( post_data.midplanesPerRow )  cmd_str += " --midplanesperrow=" + post_data.midplanesPerRow;
        if ( post_data.killJobs )  cmd_str += " --killjobs";
        if ( post_data.deleteBlock )  cmd_str += " --deleteblock";
        if ( post_data.disruptMasterClock )  cmd_str += " --disruptmasterclock";
        if ( post_data.disruptIoRack )  cmd_str += " --disruptiorack";
        if ( post_data.insertRas )  cmd_str += " --insertras";

        console.log( module.id + ": calcDiagnosticsCommand data=", post_data, "str: " + cmd_str );

        return cmd_str;
    },


    diagnostics : {

        LOG_FILE_NAME : "diags.log"

    }
};


return b_BlueGene;

} );
