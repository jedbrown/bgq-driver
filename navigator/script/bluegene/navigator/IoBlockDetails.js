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
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_EventsMonitorMixin,
        l_topic,
        d_array,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_IoBlockDetails = d_declare( [ l_EventsMonitorMixin ],


/** @lends bluegene^navigator^IoBlockDetails# */
{
    _bgws: null,

    _io_block_details_dij : null,

    _notify_update_machine_highlighting_fn : null,

    _machine_highlighting_data : null,


    /**
     *  ctor.
     *
     *  @class I/O block details UI.
     *  <p>
     *  This class displays the I/O block details part of the Blocks tab,
     *  and displaying the selected block details on the system view.
     *  <p>
     *  The details can be: no block selected, loading, loaded, or error.
     *  <p>
     *  It monitors the require( "bluegene/navigator/topic" ).ioBlockSelected
     *  topic.
     *  If a blockId is sent on that topic, then starts loading.
     *  Do deselect the block, send null blockId on the topic.
     *  <p>
     *  When refresh, re-loads the block details.
     *  <p>
     *
     *  @constructs
     */
    constructor: function(
            /** bluegene^Bgws */ bgws,
            /** function */ notifyUpdateMachineHighlightingFn,
            io_block_details_dij
        )
    {
        this._bgws = bgws;
        this._notify_update_machine_highlighting_fn = notifyUpdateMachineHighlightingFn;
        this._io_block_details_dij = io_block_details_dij;

        this._machine_highlighting_data = {
                loading: false,
                highlighting: {}
            };

        this._io_block_details_dij.on( "Loading", d_lang.hitch( this, this._onLoading ) );
        this._io_block_details_dij.on( "gotBlockDetails", d_lang.hitch( this, this._gotBlockDetails ) );

        l_topic.subscribe( l_topic.ioBlockSelected, d_lang.hitch( this, this._blockSelected ) );
    },


    getMachineHighlightData : function()
    {
        return this._machine_highlighting_data;
    },


    // override EventsMonitorMixin
    _loggedIn : function()
    {
        this.inherited( arguments );

        this._io_block_details_dij.setFetchBlockDetailsFn( d_lang.hitch( this, this._fetchBlockDetails ) );
    },

    // override EventsMonitorMixin
    _refresh: function()
    {
        this._io_block_details_dij.notifyRefresh();
    },


    _fetchBlockDetails : function( block_id )
    {
        return this._bgws.fetchBlockDetails( block_id );
    },


    _blockSelected : function( /**String | null*/ block_id )
    {
        this._io_block_details_dij.setBlockId( block_id );
    },


    _onLoading : function()
    {
        this._machine_highlighting_data.loading = true;
        this._notify_update_machine_highlighting_fn();
    },


    _gotBlockDetails : function( block_info )
    {
        this._machine_highlighting_data = {
                    loading: false,
                    highlighting: {}
            };

        if ( block_info ) {

            d_array.forEach( block_info.locations, d_lang.hitch( this, function( location ) {
                this._machine_highlighting_data.highlighting[location.substr( 0, 6 )] = { color: "lightblue" };
            } ) );

        }

        console.log( module.id + ": _gotBlockDetails, block_info=", block_info, "_machine_highlighting_data=", this._machine_highlighting_data );

        this._notify_update_machine_highlighting_fn();
    }


} );


return b_navigator_IoBlockDetails;

} );
