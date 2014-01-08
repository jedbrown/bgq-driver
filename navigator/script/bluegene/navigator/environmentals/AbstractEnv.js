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
    "dojo/_base/declare",
    "dojo/_base/lang"
],
function(
        b_navigator_topic,
        d_declare,
        d_lang
    )
{


var b_navigator_environmentals_AbstractEnv = d_declare( null,

/** @lends bluegene^navigator^environmentals^AbstractEnv# */
{
    _bgws : null,

    _env_dij : null,


    /** @constructs */
    constructor: function(
            /** bluegene^Bgws */ bgws
        )
    {
        this._bgws = bgws;

        b_navigator_topic.subscribe( b_navigator_topic.refresh, d_lang.hitch( this, this._refresh ) );
    },


    notifyLoggedIn : function( envs_dij )
    {
        if ( envs_dij ) {

            this._env_dij = envs_dij[this._getId() + "Tab"];


            var store = this._bgws.createDataStore( {
                        target: this._getUri()
                    } );

            this._env_dij.setStore( store );

        } else {

            this._env_dij = null;

        }
    },


    // override this to provide the name.
    _getName: null,

    // override this to provide the element ID base.
    _getId: null,

    // override this to provide the URI, will be like /bg/environmentals/serviceCard.
    _getUri: null,


    _refresh: function()
    {
        if ( this._env_dij ) {
            this._env_dij.notifyRefresh();
        }
    }

} );


return b_navigator_environmentals_AbstractEnv;

} );
