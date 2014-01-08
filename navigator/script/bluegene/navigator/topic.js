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
    "dojo/topic"
],
function(
        d_topic
    )
{


var _base = "/bg";
var _base_diags = _base + "/diags";


/** @name bluegene^navigator^topic
 *  @namespace
 */
var b_navigator_topic =

/** @lends bluegene^navigator^topic */
{

    /** A block was created. */
    blockCreated : _base + "/blockCreated",

    /** A compute block was selected.
     *  <br>
     *  message: block ID or null for no block.
     */
    computeBlockSelected : _base + "/computeBlockSelected",

    /** Request to display the prepare service action wizard tab. */
    displayPrepareServiceAction : _base + "/displayPrepareServiceAction",

    /** The session is ending or has ended. */
    endSession : _base + "/endSession",

    /** Got the machine info.
     * <br>
     * message: Object, { machineInfo: { <i>machine info</i> } }
     */
    gotMachineInfo : _base + "/gotMachineInfo",

    /** A job from the job history was selected.
     *  <br>
     *  message: Job ID or null for no job.
     */
    historyJobSelected : _base + "/historyJobSelected",

    /** An I/O block was selected.
     *  <br>
     *  message: block ID or null for no block.
     */
    ioBlockSelected : _base + "/ioBlockSelected",

    /** Notify to set the machine highlighting.
    *
    * <br>
    * message: Object, as below, or null.
    *
    * <br>
    * All fields in the object are optional.
    *
    * <pre>
    *   {
    *     "mp-location": {
    *        color: "color", -- highlight the midplane surround in the color
    *        nodeBoards: { "nb-position": "color", ... }, -- set the color of the node board. if not present then all node boards get the color.
    *
    *        overlay: { text: "text", color: "color" }, -- put the overlay text on the midplane in the color.
    *     },
    *     ...
    *   }
    * </pre>
    */
    highlightHardware : _base + "/highlightHardware",


    /** Notify that the controller machine highlighting has changed.
     *
     *  Argument is the controller that has an update.
     */
    updateMachineHighlighting : _base + "/updateMachineHighlighting",


    /** A job was selected.
     *  <br>
     *  message: job ID or null for no job.
     */
    jobSelected : _base + "/jobSelected",


    /** A job was selected from job history.
     *  <br>
     *  message: { startTime: bluegene^TimeInterval ) }
     */
    jobHistorySelected : _base + "/jobHistorySelected",

    /** The user logged in successfully.
     * <br>
     * message: Object, { "userInfo": bluegene^UserInfo }
     */
    loggedIn : _base + "/loggedIn",

    /** Called after loggedIn.
     */
    loginComplete : _base + "/loginComplete",


    /** Something was selected in the midplanes view.<br>
     *   data is the location.
     */
    midplane : _base + "/midplane",

    /** Ras event selected
     *  <br/>
     *  Data is recordId.
     */
    ras : _base + "/ras",


    /** Display RAS events.
     *  <br/>
     *  Data is RAS filter options object.
     */
    rasEventsSelected : _base + "/rasEventsSelected",


    /** Refresh the page.
     */
    refresh : _base + "/refresh",

    /** Refresh charts */
    refreshCharts : _base + "/refreshCharts",

    /** A service action was started. */
    startServiceAction : _base + "/startServiceAction",

    /** Display the block builder. */
    displayBlockBuilder : _base + "/displayBlockBuilder",

    /** Data is { "id" : "id", "controller" : controller or null }  */
    tabControllerChange : _base + "/tabControllerChange",


    /** Diagnostics-related */
    diags : {

        /** Data is null if user doesn't have read access, false if user has read access, true if user has read and write. If the user has access then the tab exists, otherwise it's gone or going away. */
        loggedIn : _base_diags + "/loggedIn",

        /** Data is tests configuration. */
        gotTestsConfiguration : _base_diags + "/gotTestsConfiguration",

        /** Notify what to display in diagnostics details.
         *  <br>
         *  data: { location: "location" } | { runId: "runId", runStart: "timestamp" } | { blockId: "block-id", runId: "run-id", location: "location" (opt) }
         */
        display : _base_diags + "/display"

    },


    publish : d_topic.publish,
    subscribe : d_topic.subscribe

};


return b_navigator_topic;


} );
