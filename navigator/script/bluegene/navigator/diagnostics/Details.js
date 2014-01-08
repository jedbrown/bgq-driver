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
    "dojo/_base/lang",
    "module"
],
function(
        b_navigator_topic,
        b_Bgws,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_diagnostics_Details = d_declare( null,

/** @lends bluegene^navigator^diagnostics^Details# */
{

    _bgws : null,
    _diagnostics : null,

    _details_dij : null,


    /** @constructs */
    constructor : function( bgws, diagnostics )
    {
        this._bgws = bgws;
        this._diagnostics = diagnostics;

        b_navigator_topic.subscribe( b_navigator_topic.diags.display, d_lang.hitch( this, this._notified ) );
        b_navigator_topic.subscribe( b_navigator_topic.refresh, d_lang.hitch( this, this._refresh ) );
        b_navigator_topic.subscribe( b_navigator_topic.diags.loggedIn, d_lang.hitch( this, this._loggedIn ) );
    },


    _loggedIn : function( perms )
    {
        if ( perms === null ) {
            // The user doesn't have permissions and the dijit is going away.

            this._details_dij = null;

        } else {
            // The user has permissions and the dijit is there.

            if ( ! this._details_dij ) {
                // The diagnostics/summary dijit was just created.

                this._details_dij = this._diagnostics.getDetailsDijit();
            }

            this._details_dij.setFetchDataFn( d_lang.hitch( this, this._fetchDiagsData ) );


            var testcases_store = this._bgws.createStore( { target : b_Bgws.resourcePaths.diagnostics.testcases } );

            this._details_dij.setTestcasesStore( testcases_store );
        }
    },


    _notified : function( obj )
    {
        if ( this._details_dij ) {
            console.log( module.id + ": notified", obj );

            this._details_dij.notifySelected( obj );

            this._diagnostics.switchToDetails();
        }
    },


    _refresh : function()
    {
        if ( this._details_dij ) {
            this._details_dij.notifyRefresh();
        }
    },


    _fetchDiagsData : function( req )
    {
        var req_url = ("diagnostics/" + req);

        return this._bgws.fetch( req_url );
    }

} );


return b_navigator_diagnostics_Details;

} );
