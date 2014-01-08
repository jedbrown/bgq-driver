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
    "./AbstractTab",
    "./IoBlockDetails",
    "./topic",
    "../Bgws",
    "dojo/_base/declare",
    "dojo/_base/lang"
],
function(
        l_AbstractTab,
        l_IoBlockDetails,
        l_topic,
        b_Bgws,
        d_declare,
        d_lang
    )
{


var b_navigator_IoBlocks = d_declare( [ l_AbstractTab ],

/** @lends bluegene^navigator^IoBlocks# */
{
    _bgws: null,

    _io_blocks_dij : null,

    _io_block_details : null,



    /** @constructs */
    constructor: function(
            bgws,
            io_blocks_tab_dij,
            io_block_details_dij
        )
    {
        this._bgws = bgws;
        this._io_blocks_dij = io_blocks_tab_dij;

        this._io_block_details = new l_IoBlockDetails(
                this._bgws,
                d_lang.hitch( this, this._updateMachineHighlighting ), // from AbstractTab
                io_block_details_dij
            );

        this._io_blocks_dij.on( "blockSelected", d_lang.hitch( this, this._blockSelected ) );
    },


    // override AbstractTab
    _getTabDijitName : function() { return "io-blocks"; },

    // override AbstractTab
    _refresh: function()
    {
        this._io_blocks_dij.notifyRefresh();
    },

    // override AbstractTab
    _loggedIn : function( args )
    {
        this._io_blocks_dij.setStore(
                this._bgws.getBlocksDataStore()
            );
    },

    // override AbstractTab
    _getMachineHighlightData: function()
    {
        return this._io_block_details.getMachineHighlightData();
    },


    _blockSelected : function( block_id )
    {
        l_topic.publish( l_topic.ioBlockSelected, block_id );
    }

} );


return b_navigator_IoBlocks;

} );
