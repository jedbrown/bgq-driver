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
    "../topic",
    "dojo/_base/declare",
    "dojo/_base/lang"
],
function(
        b_navigator_topic,
        d_declare,
        d_lang
    )
{


var b_navigator_diagnostics_Summary = d_declare( null,

/** @lends bluegene^navigator^diagnostics^Summary# */
{
    _bgws : null,
    _diagnostics : null,

    _machine_info : null,

    _summary_dij : null,


    /** @constructs */
    constructor : function( bgws, diagnostics )
    {
        this._bgws = bgws;
        this._diagnostics = diagnostics;

        b_navigator_topic.subscribe( b_navigator_topic.refresh, d_lang.hitch( this, this._refresh ) );
        b_navigator_topic.subscribe( b_navigator_topic.gotMachineInfo, d_lang.hitch( this, this._gotMachineInfo ) );
        b_navigator_topic.subscribe( b_navigator_topic.diags.loggedIn, d_lang.hitch( this, this._loggedIn ) );
    },


    _gotMachineInfo : function( args )
    {
        this._machine_info = args.machineInfo;

        if ( this._summary_dij ) {
            this._summary_dij.setMachineInfo( this._machine_info );
        }
    },


    _loggedIn : function( perms )
    {
        if ( perms === null ) {
            // The user doesn't have permissions and the dijit is going away.

            this._summary_dij = null;

        } else {
            // The user has permissions and the dijit is there.

            if ( ! this._summary_dij ) {
                // The diagnostics/summary dijit was just created.

                this._summary_dij = this._diagnostics.getSummaryDijit();

                this._summary_dij.setCancelDiagnosticsRunFn( d_lang.hitch( this, this._cancelDiagnosticsRun ) );
                this._summary_dij.on( "blockSelected", d_lang.hitch( this, this._blockSelected ) );

                if ( this._machine_info ) {
                    this._summary_dij.setMachineInfo( this._machine_info );
                }
            }

            this._summary_dij.setFetchDiagnosticsDataFn( d_lang.hitch( this, this._fetchDiagnosticsData ) );
        }
    },


    _refresh : function()
    {
        if ( this._summary_dij ) {
            this._summary_dij.notifyRefresh();
        }
    },


    _fetchDiagnosticsData : function( req )
    {
        return this._bgws.fetch( "diagnostics/" + req );
    },


    _cancelDiagnosticsRun : function( run_id )
    {
        return this._bgws.cancelDiagnosticsRun( run_id );
    },


    _blockSelected : function( obj )
    {
        b_navigator_topic.publish( b_navigator_topic.diags.display, obj );
    }

} );


return b_navigator_diagnostics_Summary;

} );
