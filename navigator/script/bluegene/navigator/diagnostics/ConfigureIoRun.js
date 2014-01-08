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
    "dijit/registry",
    "module"
],
function(
        b_navigator_topic,
        b_navigator_dijit_diagnostics_ConfigureIoRun,
        d_declare,
        d_lang,
        j_registry,
        module
    )
{


var b_navigator_diagnostics_ConfigureIoRun = d_declare( null,

/** @lends bluegene^navigator^diagnostics^ConfigureIoRun# */
{
    _bgws : null,
    _diagnostics : null,

    _io_drawers : null,
    _test_config : null,

    _configure_io_run_dij : null,


    /** @constructs */
    constructor : function( bgws, diagnostics )
    {
        this._bgws = bgws;
        this._diagnostics = diagnostics;

        b_navigator_topic.subscribe( b_navigator_topic.gotMachineInfo, d_lang.hitch( this, this._gotMachineInfo ) );
        b_navigator_topic.subscribe( b_navigator_topic.diags.loggedIn, d_lang.hitch( this, this._loggedIn ) );
        b_navigator_topic.subscribe( b_navigator_topic.diags.gotTestsConfiguration, d_lang.hitch( this, this._gotTestsConfiguration ) );
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

                if ( this._io_drawers ) {
                    this._configure_io_run_dij.setIoDrawers( this._io_drawers );
                }

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


    _gotMachineInfo : function( args )
    {
        this._io_drawers = args.machineInfo.ioDrawers;

        if ( this._configure_io_run_dij )  this._configure_io_run_dij.setIoDrawers( this._io_drawers );
    },


    _gotTestsConfiguration : function( config )
    {
        console.log( module.id + ": setConfig, config=", config );

        this._test_config = config;

        if ( this._configure_io_run_dij )  this._configure_io_run_dij.setConfig( this._test_config );
    },


    _submitDiagnosticsRun : function( new_diags_run_post_obj )
    {
        return this._bgws.submitDiagnosticsRun( new_diags_run_post_obj );
    }

} );


return b_navigator_diagnostics_ConfigureIoRun;

} );
