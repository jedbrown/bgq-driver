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
    "../dijit/diagnostics/ConfigureIoRun",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        b_navigator_topic,
        b_navigator_dijit_diagnostics_ConfigureIoRun,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_diagnostics_ConfigureIoRun = d_declare( null,

/** @lends bluegene^navigator^diagnostics^ConfigureIoRun# */
{
    _bgws : null,
    _diagnostics : null,

    _test_config : null,

    _configure_io_run_dij : null,


    /** @constructs */
    constructor : function( bgws, diagnostics )
    {
        this._bgws = bgws;
        this._diagnostics = diagnostics;

        b_navigator_topic.subscribe( b_navigator_topic.diags.loggedIn, d_lang.hitch( this, this._loggedIn ) );
        b_navigator_topic.subscribe( b_navigator_topic.diags.gotTestsConfiguration, d_lang.hitch( this, this._gotTestsConfiguration ) );
    },


    getDijit : function()  { return  this._configure_io_run_dij; },


    getMachineHighlightData : function()
    {
        if ( this._configure_io_run_dij ) {
            return this._configure_io_run_dij.getMachineHighlightData();
        }
        return null;
    },


    notifyMachineClicked : function( loc )
    {
        if ( ! this._configure_io_run_dij )  return;
        this._configure_io_run_dij.notifyMachineClicked( loc );
    },


    _loggedIn : function( perms )
    {
        console.log( module.id + ": logged in, perms=", perms );

        if ( perms ) {
            // The user has authority to configure an I/O run, the tab should be displayed.

            if ( this._configure_io_run_dij ) {

                console.log( module.id + ": User has authority to configure I/O run, already displayed" );

            } else {

                console.log( module.id + ": User has authority to configure I/O run, displaying the tab." );

                this._configure_io_run_dij = new b_navigator_dijit_diagnostics_ConfigureIoRun( { title: "Configure I/O Run" } );

                this._diagnostics.addTab( this._configure_io_run_dij );

                this._configure_io_run_dij.setSubmitDiagnosticsRunFn( d_lang.hitch( this, this._submitDiagnosticsRun ) );
                this._configure_io_run_dij.on( "machineHighlightingChanged", d_lang.hitch( this, this._machineHighlightingChanged ) );

                if ( this._test_config ) {
                    this._configure_io_run_dij.setConfig( this._test_config );
                }
            }

        } else {
            // The user doesn't have authority to configure an I/O run.

            if ( perms === null ) {

                // The dijit doesn't exist or is going to be destroyed automatically.
                this._configure_io_run_dij = null;

            } else {

                if ( this._configure_io_run_dij ) {

                    console.log( module.id + ": User doesn't have authority to configure I/O run, hiding the tab." );

                    this._diagnostics.removeTab( this._configure_io_run_dij );

                    this._configure_io_run_dij = null;

                } else {

                    console.log( module.id + ": User doesn't have authority to configure I/O run, tab is already hidden." );

                }

            }
        }
    },


    _gotTestsConfiguration : function( config )
    {
        console.log( module.id + ": setConfig, config=", config );

        this._test_config = config;

        if ( this._configure_io_run_dij )  this._configure_io_run_dij.setConfig( this._test_config );
    },


    _machineHighlightingChanged : function()
    {
        this._diagnostics.updateMachineHighlighting();
    },


    _submitDiagnosticsRun : function( new_diags_run_post_obj )
    {
        return this._bgws.submitDiagnosticsRun( new_diags_run_post_obj );
    }

} );


return b_navigator_diagnostics_ConfigureIoRun;

} );
