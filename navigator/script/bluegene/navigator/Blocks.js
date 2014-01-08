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
    "./ComputeBlockDetails",
    "./topic",
    "../Bgws",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_AbstractTab,
        l_ComputeBlockDetails,
        l_topic,
        b_Bgws,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_Blocks = d_declare( [ l_AbstractTab ],

/** @lends bluegene^navigator^Blocks# */
{
    _bgws: null,

    _blocks_dijit : null,

    _compute_block_details : null,


    /** @constructs */
    constructor: function(
            bgws,
            blocks_dij,
            compute_block_details_dij
        )
    {
        this._bgws = bgws;
        this._blocks_dijit = blocks_dij;

        this._blocks_dijit.on( "blockSelected", function( block_id ) { l_topic.publish( l_topic.computeBlockSelected, block_id ); } );
        this._blocks_dijit.on( "createBlock", function() { l_topic.publish( l_topic.displayBlockBuilder ); } );

        this._blocks_dijit.setDeleteBlockFn( d_lang.hitch( this, this._deleteBlock ) );

        this._compute_block_details = new l_ComputeBlockDetails(
                this._bgws,
                d_lang.hitch( this, this._updateMachineHighlighting ),
                compute_block_details_dij
            );

        l_topic.subscribe( l_topic.blockCreated, d_lang.hitch( this, this._refresh ) );
    },


    // override AbstractTab
    _getTabDijitName : function() { return "blocks"; },


    // override AbstractTab
    _getMachineHighlightData: function()
    {
        return this._compute_block_details.getMachineHighlightData();
    },


    // override from AbstractTab
    _refresh: function()
    {
        this._blocks_dijit.notifyRefresh();
    },


    // override from AbstractTab
    _loggedIn : function( args )
    {
        this._blocks_dijit.setUserCanCreateBlocks( args.userInfo.hasBlockCreate() );


        this._blocks_dijit.setStore(
                this._bgws.getBlocksDataStore()
            );
    },


    _deleteBlock: function( block_id )
    {
        console.log( module.id + ": requesting delete '" + block_id + "'" );

        l_topic.publish( l_topic.computeBlockSelected, null );

        return this._bgws.deleteBlock( block_id );
    }

} );


return b_navigator_Blocks;

} );
