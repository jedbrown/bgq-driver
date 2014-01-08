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


var b_navigator_environmentals_HealthCheck = d_declare( null,

/** @lends bluegene^navigator^environmentals^HealthCheck# */
{
    _bgws : null,

    _hc_dij : null,


    /** @constructs */
    constructor: function( bgws )
    {
        this._bgws = bgws;

        b_navigator_topic.subscribe( b_navigator_topic.refresh, d_lang.hitch( this, this._refresh ) );
    },


    notifyLoggedIn : function( envs_dij )
    {
        if ( envs_dij ) {

            this._hc_dij = envs_dij.hcTab;

            this._hc_dij.setStore(
                    this._bgws.getRasDataStore()
                );

        } else {

            this._hc_dij = null;

        }
    },


    _refresh: function()
    {
        if ( this._hc_dij ) {
            this._hc_dij.notifyRefresh();
        }
    }

} );


return b_navigator_environmentals_HealthCheck;

} );
