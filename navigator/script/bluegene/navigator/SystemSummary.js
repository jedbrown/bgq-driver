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


var b_navigator_SystemSummary = d_declare( [ l_AbstractTab ],

/** @lends bluegene^navigator^SystemSummary# */
{
    _bgws: null,

    _system_summary_dij : null,


    /** @constructs */
    constructor: function(
            bgws,
            system_summary_dij
        )
    {
        this._bgws = bgws;
        this._system_summary_dij = system_summary_dij;

        this._system_summary_dij.on(
                "machineHighlightDataChanged",
                d_lang.hitch( this, function() {
                    this._updateMachineHighlighting(); /*AbstractTab*/
                } )
            );

        l_topic.subscribe( l_topic.gotMachineInfo, d_lang.hitch( this, this._gotMachineInfo ) );
        l_topic.subscribe( l_topic.endSession, d_lang.hitch( this, this._sessionEnded ) );
    },


    // override AbstractTab
    _getTabDijitName : function() { return "summary"; },

    // override AbstractTab
    _getMachineHighlightData: function()  { return this._system_summary_dij.getMachineHighlightData(); },

    // override AbstractTab
    _refresh: function()
    {
        this._system_summary_dij.notifyRefresh();
    },


    // override from AbstractTab
    _loggedIn : function()
    {
        this._system_summary_dij.setFetchSummaryFn( d_lang.hitch( this, this._fetchSummary ) );

        return true;
    },


    _gotMachineInfo : function( args )
    {
        console.log( module.id + ": _gotMachineInfo. args=", args );
        this._system_summary_dij.setMachineInfo( args );
    },

    _sessionEnded : function()
    {
        this._system_summary_dij.setFetchSummaryFn( null );
    },


    _fetchSummary : function()
    {
        console.log( module.id + ": Fetching system summary data." );
        return this._bgws.fetch( b_Bgws.resourcePaths.summary.system );
    }

} );


return b_navigator_SystemSummary;

} );
