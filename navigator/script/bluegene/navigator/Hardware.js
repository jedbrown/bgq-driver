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
    "../Bgws",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_AbstractTab,
        l_topic,
        b_Bgws,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_Hardware = d_declare( [ l_AbstractTab ],

/** @lends bluegene^navigator^Hardware# */
{
    _bgws: null,

    _hardware_dij : null,


    /** @constructs */
    constructor: function(
            bgws,
            hardware_dij
        )
    {
        this._bgws = bgws;
        this._hardware_dij = hardware_dij;

        this._hardware_dij.on( "machineHighlightingChanged", d_lang.hitch( this, this._machineHighlightingChanged ) );
    },


    // override AbstractTab
    _getTabDijitName : function() { return "hardware"; },

    // override AbstractTab
    _getMachineHighlightData: function()  { return this._hardware_dij.getMachineHighlightData(); },

    // override from AbstractTab
    _loggedIn : function( args )
    {
        this._hardware_dij.setFetchDataFn( d_lang.hitch( this, this._fetchData ) );
    },

    // override AbstractTab
    _refresh: function()
    {
        this._hardware_dij.notifyRefresh();
    },

    // override, called when machine clicked.
    notifyMachineClicked : function( location )
    {
        this._hardware_dij.notifyLocationSelected( location );
    },


    _fetchData : function( location )
    {
        console.log( module.id + ": fetching ", location );

        var uri = b_Bgws.resourcePaths.machine;

        if ( location !== null ) {
            uri += "/" + location;
        }

        return this._bgws.fetch( uri );
    },


    _machineHighlightingChanged : function()
    {
        this._updateMachineHighlighting(); // AbstractTab
    }

} );


return b_navigator_Hardware;

} );
