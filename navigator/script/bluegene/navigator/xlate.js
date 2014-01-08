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
    "dojo/_base/lang"
],
function(
        d_lang
    )
{


/** @name bluegene^navigator^xlate
 *  @namespace
 */

var b_navigator_xlate =

/** @lends bluegene^navigator^xlate */
{

    alerts: {

        locationType : {
            "A" : "Application",
            "J" : "Job",
            "C" : "Compute",
            "I" : "I/O",

            _order : [ "A", "J", "C", "I" ]
        },

        severity : {
            "F" : "Fatal",
            "E" : "Error",
            "W" : "Warning",
            "I" : "Info",

            _order : [ "F", "E", "W", "I" ]
        },

        state : {
            "1" : "Open",
            "2" : "Closed",

            _order : [ "1", "2" ]
        },

        urgency : {
            "I" : "Immediate",
            "S" : "Schedule",
            "N" : "Normal",
            "D" : "Defer",
            "O" : "Optional",

            _order : [ "I", "S", "N", "D", "O" ]
        }

    },


    blockStatus : {
        "F" : "Free",
        "A" : "Allocated",
        "B" : "Booting",
        "I" : "Initialized",
        "T" : "Terminating",

        _order : [ "F", "A", "B", "I", "T" ]
    },


    diagnostics : {

        hardwareStatus : {
            "failed" : "Failed",
            "marginal" : "Marginal",
            "success" : "Success",
            "uninitialized" : "Uninitialized",
            "unknown" : "Unknown",

            _order : [ "failed", "marginal", "success", "uninitialized", "unknown" ]
        },

        hardwareStatusCode : {
            "S" : "Success",
            "K" : "Unknown",
            "M" : "Marginal",
            "F" : "Failed",
            "U" : "Uninitialized",

            _order : [ "S", "K", "M", "F", "U" ]
        },

        runStatusCode : {
            "C" : "Completed",
            "F" : "Failed",
            "L" : "Canceled",

            _order : [ "C", "F", "L" ]
        }

    },


    hardwareReplaceType : {
        "Midplane" : "Midplane",
        "NodeCard" : "Node Board",
        "NodeBoardDCA" : "Node Board DCA",
        "Node" : "Node",
        "IONode" : "I/O Node",
        "IORack" : "I/O Rack",
        "IODrawer" : "I/O Drawer",
        "ServiceCard" : "Service Card",
        "ClockCard" : "Clock Card",
        "LinkChip" : "Link Chip",
        "IOLinkChip" : "I/O Link Chip",
        "BulkPower" : "Bulk Power"
    },

    hardwareStatus : {
        "A" : "Available",
        "M" : "Missing",
        "E" : "Error",
        "S" : "Service",
        "F" : "Software Failure",

        _order : [ "A", "M", "E", "S", "F" ]
    },


    jobStatus : {
        "D" : "Debug",
        "E" : "Error",
        "L" : "Loading",
        "N" : "Cleanup",
        "P" : "Setup",
        "R" : "Running",
        "S" : "Starting",
        "T" : "Terminated"
    },

    jobsCurrentOrder: [ "P", "L", "S", "R", "D", "N" ],
    jobsHistoryOrder: [ "T", "E" ],


    rasSeverity : {
        "I" : "Informational",
        "W" : "Warning",
        "F" : "Fatal",
        "U" : "Unknown",

        _order : [ "I", "W", "F", "U" ]
    },


    serviceActionAction : {
        "O" : "Open",
        "P" : "Prepare",
        "E" : "End",
        "C" : "Closed",

        _order : [ "O", "P", "E", "C" ]
    },

    serviceActionStatus : {
        "A" : "Active",
        "O" : "Open",
        "P" : "Prepared",
        "E" : "Error",
        "C" : "Closed",
        "F" : "Forced",

        _order : [ "A", "O", "P", "E", "C", "F" ]
    }

};


var b_navigator = d_lang.getObject( "bluegene.navigator", true );
b_navigator.xlate = b_navigator_xlate;


return b_navigator_xlate;

} );
