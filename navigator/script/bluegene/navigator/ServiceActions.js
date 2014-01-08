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
    "./dijit/ServiceActions",
    "./topic",
    "../Bgws",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_AbstractTab,
        l_dijit_ServiceActions,
        l_topic,
        b_Bgws,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_ServiceActions = d_declare( [ l_AbstractTab ],

/** @lends bluegene^navigator^ServiceActions */
{
    _bgws : null,
    _navigator : null,

    _service_actions_dij : null,


    /** @constructs */
    constructor : function( bgws, navigator )
    {
        this._bgws = bgws;
        this._navigator = navigator;

        l_topic.subscribe( l_topic.startServiceAction, d_lang.hitch( this, this._refresh ) );
    },


    // override from AbstractTab
    _refresh: function()
    {
        if ( this._service_actions_dij ) {
            this._service_actions_dij.notifyRefresh();
        }
    },


    // override from AbstractTab
    _loggedIn : function( args )
    {
        if ( args.userInfo.hasHardwareRead() ) {
            // User has authority to Service Actions

            if ( this._service_actions_dij ) {

                console.log( module.id + ": User has authority to service actions tab (already displayed)." );

            } else {

                console.log( module.id + ": User has authority to alerts, showing service actions tab" );

                this._service_actions_dij = new l_dijit_ServiceActions( { title: "Service Actions" } );

                var TAB_PRIORITY = 120;
                var TAB_NAME = "service-actions";
                this._navigator.addTab( this._service_actions_dij, TAB_PRIORITY, TAB_NAME, this );

                this._service_actions_dij.setEndServiceActionFn( d_lang.hitch( this, this._endServiceAction ) );
                this._service_actions_dij.setCloseServiceActionFn( d_lang.hitch( this, this._closeServiceAction ) );
                this._service_actions_dij.on( "prepareServiceAction", function() { l_topic.publish( l_topic.displayPrepareServiceAction ); } );
            }


            this._service_actions_dij.setServiceActionsStore(
                    this._bgws.getServiceActionsStore()
                );


            if ( args.userInfo.isAdministrator() ) {

                this._service_actions_dij.setUserAllowedOperations( true );

            } else {

                console.log( module.id + ": user isn't administrator so hiding the service action action buttons." );

                this._service_actions_dij.setUserAllowedOperations( false );

            }

        } else {
            // User doesn't have authority to alerts.

            if ( this._service_actions_dij ) {

                console.log( module.id + ": User does not have authority to service actions and displayed, hiding." );

                this._navigator.removeTab( this._service_actions_dij );

                this._service_actions_dij = null;

            } else {

                console.log( module.id + ": User does not have authority to service actions and already hidden." );
            }

        }
    },


    _endServiceAction : function( id )
    {
        return this._bgws.submitEndServiceAction( id );
    },


    _closeServiceAction : function( id )
    {
        return this._bgws.submitCloseServiceAction( id );
    }

} );


return b_navigator_ServiceActions;

} );
