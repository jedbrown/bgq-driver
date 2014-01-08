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
    "./dijit/PrepareServiceAction",
    "../Bgws",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_AbstractTab,
        l_topic,
        l_dijit_PrepareServiceAction,
        b_Bgws,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_PrepareServiceAction = d_declare( [ l_AbstractTab ],

/** @lends bluegene^navigator^PrepareServiceAction# */
{
    _bgws : null,
    _navigator : null,

    _prepare_service_action_dij : null,

    _jobs_store : null,
    _machine_info : null,


    /**
     *  ctor
     *
     *  @class Prepare service action tab / wizard.
     *  @constructs
     */
    constructor: function(
            /**bluegene^Bgws*/ bgws,
            /**bluegene^navigator^Navigator*/ navigator
        )
    {
        this._bgws = bgws;
        this._navigator = navigator;

        l_topic.subscribe( l_topic.displayPrepareServiceAction, d_lang.hitch( this, this._display ) );
        l_topic.subscribe( l_topic.gotMachineInfo, d_lang.hitch( this, this._gotMachineInfo ) );
    },


    // override from AbstractTab
    _loggedIn : function( args )
    {
        this._jobs_store = this._bgws.getJobsDataStore();

        if ( this._prepare_service_action_dij ) {
            this._prepare_service_action_dij.setJobsStore( this._jobs_store );
        }
    },


    // override from AbstractTab
    _refresh: function()
    {
        if ( ! this._prepare_service_action_dij )  return;
        this._prepare_service_action_dij.notifyRefresh();
    },


    _gotMachineInfo : function( args )
    {
        this._machine_info = args;

        if ( this._prepare_service_action_dij ) {
            this._prepare_service_action_dij.setMachineInfo( args );
        }
    },


    _submitPrepareForService : function( location )
    {
        l_topic.publish( l_topic.startServiceAction, location );

        return this._bgws.submitPrepareForService( location );
    },


    _display: function()
    {
        console.log( module.id + ": Display" );

        var TAB_NAME = "prepare-service-action";

        if ( ! this._prepare_service_action_dij ) {

            // Create the dijit & put it on the tabs.
            this._prepare_service_action_dij = new l_dijit_PrepareServiceAction( { closable: true, title: "Prepare Service Action" } );


            var TAB_PRIORITY = 999;
            this._navigator.addTab( this._prepare_service_action_dij, TAB_PRIORITY, TAB_NAME, this );


            this._prepare_service_action_dij.on( "close", d_lang.hitch( this, this._closing ) );

            if ( this._jobs_store ) {
                this._prepare_service_action_dij.setJobsStore( this._jobs_store );
            }
            if ( this._machine_info ) {
                this._prepare_service_action_dij.setMachineInfo( this._machine_info );
            }

            this._prepare_service_action_dij.setSubmitPrepareForServiceFn( d_lang.hitch( this, this._submitPrepareForService ) );

        }

        this._navigator.switchTo( TAB_NAME );
    },


    _closing: function()
    {
        // Notify Navigator that no tab anymore.
        l_topic.publish( l_topic.tabControllerChange, { id: this._prepare_service_action_dij.id } );

        this._navigator.switchTo( "service-actions" /*tab_name*/);

        this._prepare_service_action_dij = null;
    }

} );


return b_navigator_PrepareServiceAction;

} );
