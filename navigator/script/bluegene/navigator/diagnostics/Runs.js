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
    "../../Bgws",
    "dojo/_base/declare",
    "dojo/_base/lang"
],
function(
        b_navigator_topic,
        b_Bgws,
        d_declare,
        d_lang
    )
{


var b_navigator_diagnostics_Runs = d_declare( null,

/** @lends bluegene^navigator^diagnostics^Runs# */
{
    _bgws : null,
    _diagnostics : null,

    _runs_dij : null,


    /** @constructs */
    constructor : function( bgws, diagnostics )
    {
        this._bgws = bgws;
        this._diagnostics = diagnostics;

        b_navigator_topic.subscribe( b_navigator_topic.refresh, d_lang.hitch( this, this._refresh ) );
        b_navigator_topic.subscribe( b_navigator_topic.diags.loggedIn, d_lang.hitch( this, this._loggedIn ) );
    },


    _loggedIn : function( perms )
    {
        if ( perms === null ) {
            // The user doesn't have permissions and the dijit is going away.

            this._runs_dij = null;

        } else {
            // The user has permissions and the dijit is there.

            if ( ! this._runs_dij ) {
                // The diagnostics/Runs dijit was just created.

                this._runs_dij = this._diagnostics.getRunsDijit();

                this._runs_dij.on( "runSelected", d_lang.hitch( this, this._runSelected ) );

            }


            this._runs_dij.setDiagnosticsRunsStore(
                    this._bgws.getDiagnosticsRunsDataStore()
                );

        }
    },


    _refresh : function()
    {
        if ( this._runs_dij ) {
            this._runs_dij.notifyRefresh();
        }
    },


    _runSelected : function( args )
    {
        b_navigator_topic.publish( b_navigator_topic.diags.display, args );
    }

} );


return b_navigator_diagnostics_Runs;

} );
