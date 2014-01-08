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
    "./EventsMonitorMixin",
    "./topic",
    "../Bgws",
    "dojo/_base/declare",
    "dojo/_base/lang"
],
function(
        l_EventsMonitorMixin,
        l_topic,
        b_Bgws,
        d_declare,
        d_lang
    )
{


var bluegene_navigator_ComputeBlockDetails = d_declare( [ l_EventsMonitorMixin ],


/** @lends bluegene^navigator^ComputeBlockDetails# */
{

    _bgws : null,
    _update_machine_highlighting_fn : null,

    _compute_block_details_dij : null,


    /**
     *  ctor.
     *
     *  @class Compute block details UI.
     *  <p>
     *  This class displays the compute block details part of the Blocks tab,
     *  and displaying the selected block details on the system view.
     *  <p>
     *  The details can be: no block selected, loading, loaded, or error.
     *  <p>
     *  It monitors the require( "bluegene/navigator/topic" ).computeBlockSelected
     *  topic.
     *  If a blockId is sent on that topic, then starts loading.
     *  Do deselect the block, send null blockId on the topic.
     *  <p>
     *  When refresh, re-loads the block details.
     *  <p>
     *
     *  @constructs
     *
     */
    constructor: function(
            /** bluegene^Bgws */ bgws,
            update_machine_highlighting_fn,
            compute_block_details_dij
        )
    {
        this._bgws = bgws;
        this._update_machine_highlighting_fn = update_machine_highlighting_fn;

        l_topic.subscribe( l_topic.computeBlockSelected, d_lang.hitch( this, this._blockSelected ) );


        this._compute_block_details_dij = compute_block_details_dij;
        this._compute_block_details_dij.setBlockFetchFn( d_lang.hitch( this, this._fetchBlockDetails ) );

        this._compute_block_details_dij.setJobsStore(
                this._bgws.getJobsDataStore()
            );

        this._compute_block_details_dij.on( "machineHighlightingChanged", d_lang.hitch( this, this._machineHighlightingChanged ) );
    },


    getMachineHighlightData: function()
    {
        return this._compute_block_details_dij.getMachineHighlighting();
    },


    // override EventsMonitorMixin.
    _refresh: function()
    {
        this._compute_block_details_dij.notifyRefresh();
    },


    _fetchBlockDetails : function( block_id )
    {
        return this._bgws.fetchBlockDetails( block_id );
    },


    _blockSelected: function( /**String | null*/ block_id )
    {
        this._compute_block_details_dij.setBlockId( block_id );
    },


    _machineHighlightingChanged : function()
    {
        this._update_machine_highlighting_fn();
    }

} );


return bluegene_navigator_ComputeBlockDetails;

} );
