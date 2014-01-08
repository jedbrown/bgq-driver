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
    "./Bgws",
    "dojo/when",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_Bgws,
        d_when,
        d_declare,
        d_lang,
        module
    )
{


var b_System = d_declare( null,

/** @lends bluegene^System# */
{
    _bgws: null,
    _fetched_callback: function() {},
    _machine_info: null,
    _midplanes: null,


    /** @constructs */
    constructor: function(
            bgws,
            fetched_callback
        )
    {
        this._bgws = bgws;
        this._fetched_callback = fetched_callback;
    },

    fetch: function()
    {
        d_when(
                this._bgws.fetch( l_Bgws.resourcePaths.summary.machine ),
                d_lang.hitch( this, this._gotMachineInfo )
            );
    },

    getMachineInfo: function()
    {
        return this._machine_info;
    },

    getMidplanes: function()
    {
        // summary:
        //    returns the midplanes on the machine, an array like [ "R00-M0", "R00-M1", ... ]

        return this._midplanes;
    },


    _gotMachineInfo: function( machine_info )
    {
        console.log( module.id + ": got machine info = ", machine_info );
        this._machine_info = machine_info;

        // Create _midplanes.

        this._midplanes = [];

        for ( var row = 0 ; row < this._machine_info.rows ; ++row ) {
            for ( var col = 0 ; col < this._machine_info.columns ; ++col ) {
                for ( var mp = 0 ; mp <= 1 ; ++mp ) {
                    var mp_loc = "R" + row + col + "-M" + mp;
                    this._midplanes.push( mp_loc );
                }
            }
        }

        this._fetched_callback();
    }

} );


return b_System;

} );
