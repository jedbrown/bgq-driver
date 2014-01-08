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
    "./RasDetails",
    "./topic",
    "./dijit/Ras",
    "../Bgws",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_AbstractTab,
        l_RasDetails,
        l_topic,
        l_dijit_Ras,
        b_Bgws,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_Ras = d_declare( [ l_AbstractTab ],

/** @lends bluegene^navigator^Ras# */
{

    _bgws : null,
    _navigator : null,

    _ras_details : null,

    _ras_dij : null,
    _ras_results_dij : null,


    /** @constructs */
    constructor: function( bgws, navigator )
    {
        this._bgws = bgws;
        this._navigator = navigator;

        this._ras_details = new l_RasDetails( this._bgws, this );

        l_topic.subscribe( l_topic.rasEventsSelected, d_lang.hitch( this, this._selected ) );
    },


    getDetailsDijit : function()
    {
        return this._ras_dij.getDetailsDijit();
    },


    // override AbstractTab
    _refresh: function()
    {
        if ( this._ras_results_dij ) {
            this._ras_results_dij.notifyRefresh();
        }
    },

    // override from AbstractTab
    _loggedIn : function( args )
    {
        if ( args.userInfo.hasHardwareRead() ) {
            // User has authority to RAS.

            if ( this._ras_dij ) {

                console.log( module.id + ": User has authority to RAS tab (already displayed)." );

            } else {

                console.log( module.id + ": User has authority to RAS, showing tab" );


                this._ras_dij = new l_dijit_Ras( { title: "RAS" } );

                var TAB_PRIORITY = 70;
                var TAB_NAME = "ras";
                this._navigator.addTab( this._ras_dij, TAB_PRIORITY, TAB_NAME, this );

                this._ras_results_dij = this._ras_dij.getResultsDijit();

                this._ras_results_dij.on( "eventSelected", d_lang.hitch( this, this._eventSelected ) );

            }


            this._ras_results_dij.setRasStore(
                    this._bgws.getRasDataStore()
                );

            var HAS_AUTHORITY = true;
            this._ras_details.notifyLoggedIn( HAS_AUTHORITY );

        } else {
            // User does not have authority to RAS.

            if ( this._ras_dij ) {

                console.log( module.id + ": User does not have authority to RAS and displayed, hiding." );

                this._navigator.removeTab( this._ras_dij );

                this._ras_dij = null;
                this._ras_results_dij = null;

            } else {

                console.log( module.id + ": User does not have authority to RAS and already hidden." );

            }


            var HAS_AUTHORITY = false;
            this._ras_details.notifyLoggedIn( HAS_AUTHORITY );
        }
    },


    _eventSelected : function( rec_id )
    {
        l_topic.publish( l_topic.ras, rec_id );
    },


    _selected : function( args )
    {
        if ( ! this._ras_results_dij )  return;

        this._ras_results_dij.setFilter( args );
        this._navigator.switchTo( "ras" );
    }


} );


return b_navigator_Ras;

} );
