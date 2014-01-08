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
    "./dijit/HardwareReplacements",
    "../Bgws",
    "dojo/_base/declare",
    "module"
],
function(
        l_AbstractTab,
        l_dijit_HardwareReplacements,
        b_Bgws,
        d_declare,
        module
    )
{


var b_navigator_HardwareReplacements = d_declare( [ l_AbstractTab ],

/** @lends bluegene^navigator^HardwareReplacements# */
{
    _bgws : null,
    _navigator : null,

    _hardware_replacements_dij : null,


    /** @constructs */
    constructor : function( bgws, navigator )
    {
        this._bgws = bgws;
        this._navigator = navigator;
    },


    // override from AbstractTab
    _loggedIn : function( args )
    {
        if ( args.userInfo.hasHardwareRead() ) {
            // User has authority to hardware replacements so the tab should be visible.

            if ( this._hardware_replacements_dij ) {

                console.log( module.id + ": User has authority to hardware replacements tab (already displayed)." );

            } else {

                console.log( module.id + ": User has authority to hardware replacements, showing Hardware Replacements tab" );

                this._hardware_replacements_dij = new l_dijit_HardwareReplacements( { title: "Hardware Replacements" } );

                var TAB_PRIORITY = 100;
                var TAB_NAME = "hwrepl";
                this._navigator.addTab( this._hardware_replacements_dij, TAB_PRIORITY, TAB_NAME, this );

            }


            this._hardware_replacements_dij.setStore(
                    this._bgws.getHardwareReplacementsDataStore()
                );


        } else {
            // User doesn't have authority to hardware replacements so the tab should not be visible.

            if ( this._hardware_replacements_dij ) {

                console.log( module.id + ": User does not have authority to hardware replacements and displayed, hiding." );

                this._navigator.removeTab( this._hardware_replacements_dij );

                this._hardware_replacements_dij = null;

            } else {

                console.log( module.id + ": User does not have authority to hardware replacements and already hidden." );

            }

        }
    },


    // override from AbstractTab
    _refresh: function()
    {
        if ( this._hardware_replacements_dij ) {
            this._hardware_replacements_dij.notifyRefresh();
        }
    }

} );


return b_navigator_HardwareReplacements;

} );
