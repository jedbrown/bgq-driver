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
    "./dijit/Environmentals",
    "./environmentals/BulkPower",
    "./environmentals/Coolant",
    "./environmentals/Fan",
    "./environmentals/HealthCheck",
    "./environmentals/IoCard",
    "./environmentals/LinkChip",
    "./environmentals/Node",
    "./environmentals/NodeCard",
    "./environmentals/Optical",
    "./environmentals/ServiceCard",
    "./environmentals/ServiceCardTemp",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_AbstractTab,
        l_dijit_Environmentals,
        l_environmentals_BulkPower,
        l_environmentals_Coolant,
        l_environmentals_Fan,
        l_environmentals_HealthCheck,
        l_environmentals_IoCard,
        l_environmentals_LinkChip,
        l_environmentals_Node,
        l_environmentals_NodeCard,
        l_environmentals_Optical,
        l_environmentals_ServiceCard,
        l_environmentals_ServiceCardTemp,
        d_array,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_Environmentals = d_declare( [ l_AbstractTab ],

/** @lends bluegene^navigator^Environmentals# */
{
    _navigator : null,

    _controllers : null, // [] controllers (AbstractEnv and HealthCheck.

    _envs_dij : null,

    _measurement_system : null,


    /** @constructs */
    constructor: function( bgws, navigator )
    {
        this._navigator = navigator;

        this._controllers = [
                 new l_environmentals_HealthCheck( bgws ),
                 new l_environmentals_BulkPower( bgws ),
                 new l_environmentals_Coolant( bgws ),
                 new l_environmentals_Fan( bgws ),
                 new l_environmentals_IoCard( bgws ),
                 new l_environmentals_LinkChip( bgws ),
                 new l_environmentals_Node( bgws ),
                 new l_environmentals_NodeCard( bgws ),
                 new l_environmentals_Optical( bgws ),
                 new l_environmentals_ServiceCard( bgws ),
                 new l_environmentals_ServiceCardTemp( bgws )
             ];
    },


    setMeasurementSystem : function( new_system )
    {
        this._measurement_system = new_system;

        if ( this._envs_dij ) {
            this._envs_dij.coolantTab.set( "measurementSystem", this._measurement_system );
        }
    },


    // override from AbstractTab
    _loggedIn : function( args )
    {
        if ( args.userInfo.hasHardwareRead() ) {
            // User has authority to envs.

            if ( this._envs_dij ) {

                console.log( module.id + ": User has authority to environmentals tab (already displayed)." );

            } else {

                console.log( module.id + ": User has authority to environmentals, showing tab" );

                this._envs_dij = new l_dijit_Environmentals( { title: "Environmentals" } );

                var TAB_PRIORITY = 80;
                var TAB_NAME = "env";
                this._navigator.addTab( this._envs_dij, TAB_PRIORITY, TAB_NAME, this );


                if ( this._measurement_system ) {
                    this._envs_dij.coolantTab.set( "measurementSystem", this._measurement_system );
                }
            }

            d_array.forEach(
                    this._controllers,
                    d_lang.hitch( this, function( c ) {
                        c.notifyLoggedIn( this._envs_dij );
                    } )
                );

        } else {
            // User doesn't have authority to envs.

            d_array.forEach(
                    this._controllers,
                    d_lang.hitch( this, function( c ) {
                        c.notifyLoggedIn( null );
                    } )
                );

            if ( this._envs_dij ) {

                console.log( module.id + ": User does not have authority to environmentals and displayed, hiding." );

                this._navigator.removeTab( this._envs_dij );

                this._envs_dij = null;

            } else {

                console.log( module.id + ": User does not have authority to environmentals and already hidden." );

            }
        }

    }

} );


return b_navigator_Environmentals;

} );
