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
    "./topic",
    "./dijit/BlockBuilder",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_AbstractTab,
        l_topic,
        l_dijit_BlockBuilder,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_BlockBuilder = d_declare(
        [ l_AbstractTab ],

/** @lends bluegene^navigator^BlockBuilder# */
{
    _bgws: null,
    _navigator : null,

    _block_builder_dijit : null,

    _midplanes : null,


    /** @constructs */
    constructor: function(
            /** bluegene^Bgws */ bgws,
            /** bluegene^navigator^Navigator */ navigator
        )
    {
        this._bgws = bgws;
        this._navigator = navigator;

        l_topic.subscribe( l_topic.displayBlockBuilder, d_lang.hitch( this, this._display ) );
        l_topic.subscribe( l_topic.gotMachineInfo, d_lang.hitch( this, this._gotMachineInfo ) );
    },


    // override AbstractTab
    _getMachineHighlightData: function()
    {
        if ( ! this._block_builder_dijit )  {
            console.log( module.id + ": _getMachineHighlightData but no dijit?" );
            return;
        }

        var ret = {
                loading: false,
                highlighting: this._block_builder_dijit.getMachineHighlightData()
            };

        return ret;
    },


    // override AbstractTab
    notifyMachineClicked : function( location )
    {
        if ( ! this._block_builder_dijit ) {
            console.log( module.id + ": notified machine clicked but no dijit?" );
            return;
        }

        this._block_builder_dijit.notifyLocationSelected( location );
    },


    _gotMachineInfo : function( args )
    {
        this._midplanes = args.machineInfo.midplanes;

        if ( this._block_builder_dijit ) {
            this._block_builder_dijit.setMidplanes( this._midplanes );
        }
    },


    _display: function()
    {
        console.log( module.id + ": Display" );

        var TAB_NAME = "block-builder";

        if ( ! this._block_builder_dijit ) {

            // Create the dijit & put it on the tabs.
            this._block_builder_dijit = new l_dijit_BlockBuilder( { closable: true, title: "Block Builder" } );

            var TAB_PRIORITY = 999;
            this._navigator.addTab( this._block_builder_dijit, TAB_PRIORITY, TAB_NAME, this );

            this._block_builder_dijit.setCreateBlockFn( d_lang.hitch( this, this._createBlock ) );

            this._block_builder_dijit.on( "highlightChanged", d_lang.hitch( this, this._updateMachineHighlighting /*AbstractTab*/ ) );
            this._block_builder_dijit.on( "blockCreated", d_lang.hitch( this, this._onBlockCreated ) );
            this._block_builder_dijit.on( "close", d_lang.hitch( this, this._closing ) );

            if ( this._midplanes ) {
                this._block_builder_dijit.setMidplanes( this._midplanes );
            }

        }

        this._navigator.switchTo( TAB_NAME );
    },


    _createBlock: function( create_block_info )
    {
        return this._bgws.createBlock( create_block_info );
    },


    _onBlockCreated : function()
    {
        l_topic.publish( l_topic.blockCreated );
    },


    _closing : function()
    {
        console.log( module.id + ": closing." );

        // Notify Navigator that no tab anymore.
        l_topic.publish( l_topic.tabControllerChange, { id: this._block_builder_dijit.id } );

        // Switch back to Blocks.
        this._navigator.switchTo( "blocks" /*tab name*/ );

        this._block_builder_dijit = null;
    }

}

);


return b_navigator_BlockBuilder;

} );
