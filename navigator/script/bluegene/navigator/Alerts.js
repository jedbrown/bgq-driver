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
    "./dijit/Alerts",
    "../Bgws",
    "dojo/when",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_AbstractTab,
        l_dijit_Alerts,
        b_Bgws,
        d_when,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_Alerts = d_declare( [ l_AbstractTab ],

/** @lends bluegene^navigator^Alerts# */
{
    _bgws : null,
    _navigator : null,

    _alerts_dijit : null,

    _op_promise : null,

    _op_type : null,
    _op_id : null,


    /** @constructs */
    constructor : function(
            /** bluegene^Bgws */ bgws,
            navigator
        )
    {
        this._bgws = bgws;
        this._navigator = navigator;
    },


    // override from AbstractTab
    _refresh : function()
    {
        if ( ! this._alerts_dijit )  return;

        this._alerts_dijit.notifyRefresh();
    },


    // override from AbstractTab
    _loggedIn : function( args )
    {
        if ( args.userInfo.hasHardwareRead() ) {
            // User has authority to Alerts.

            if ( this._alerts_dijit ) {

                console.log( module.id + ": User has authority to alerts tab (already displayed)." );

            } else {

                console.log( module.id + ": User has authority to alerts, showing tab" );

                this._alerts_dijit = new l_dijit_Alerts( { title: "Alerts" } );

                var TAB_PRIORITY = 20;
                var TAB_NAME = "alerts";
                this._navigator.addTab( this._alerts_dijit, TAB_PRIORITY, TAB_NAME, this );

                this._alerts_dijit.on( "close", d_lang.hitch( this, this._closeAlert ) );
                this._alerts_dijit.on( "remove", d_lang.hitch( this, this._removeAlert ) );
            }


            var can_perform_operations = args.userInfo.isAdministrator();
            this._alerts_dijit.setUserCanPerformOperations( can_perform_operations );


            this._alerts_dijit.setStore(
                    this._bgws.getAlertsDataStore()
                );


        } else {
            // User doesn't have authority to alerts.

            if ( this._alerts_dijit ) {

                console.log( module.id + ": User does not have authority to alerts and displayed, hiding." );

                this._navigator.removeTab( this._alerts_dijit );

                this._alerts_dijit = null;

            } else {

                console.log( module.id + ": User does not have authority to alerts and already hidden." );

            }

        }
    },


    _closeAlert : function( id )
    {
        if ( this._op_promise ) {
            console.log( module.id + ": Requested to close " + id + " when operation already in progress" );
            return;
        }

        console.log( module.id + ": Telling Bgws to close", id );

        this._op_promise = this._bgws.closeAlert( id );

        this._op_type = "close";
        this._op_id = id;

        this._alerts_dijit.setOperationInProgress( { opType: this._op_type, id: this._op_id } );

        d_when( this._op_promise,
                d_lang.hitch( this, this._operationSuccessful ),
                d_lang.hitch( this, this._operationFailed )
            );
    },


    _removeAlert : function( id )
    {
        if ( this._op_promise ) {
            console.log( module.id + ": Requested to remove " + id + " when operation already in progress" );
            return;
        }

        console.log( module.id + ": Telling Bgws to remove", id );

        this._op_promise = this._bgws.removeAlert( id );

        this._op_type = "remove";
        this._op_id = id;

        this._alerts_dijit.setOperationInProgress( { opType: this._op_type, id: this._op_id } );

        d_when( this._op_promise,
                d_lang.hitch( this, this._operationSuccessful ),
                d_lang.hitch( this, this._operationFailed )
            );
    },


    _operationSuccessful : function()
    {
        this._op_promise = null;
        this._alerts_dijit.setOperationInProgress( { opType: this._op_type, id: this._op_id, success: true } );
    },


    _operationFailed : function( error )
    {
        this._op_promise = null;
        this._alerts_dijit.setOperationInProgress( { opType: this._op_type, id: this._op_id, error: error.message } );
    }


} );


return b_navigator_Alerts;

} );
