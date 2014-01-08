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
    "./topic",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_topic,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_Machine = d_declare( null,

/** @lends bluegene^navigator^Machine# */
{
    _machine_dij : null,


    /** @constructs */
    constructor : function(
            machine_dij
        )
    {
        this._machine_dij = machine_dij;

        l_topic.subscribe( l_topic.gotMachineInfo, d_lang.hitch( this, this._gotMachineInfo ) );
        l_topic.subscribe( l_topic.highlightHardware, d_lang.hitch( this, this._highlightHardware ) );
    },


    _gotMachineInfo : function( /* Object */ args )
    {
        this._machine_dij.setLayout( args.machineInfo );
    },

    _highlightHardware: function( /* Object */ data )
    {
        if ( ! this._machine_dij )  return;

        console.log( module.id + ": highlight hardware notification with data=", data );

        if ( data ) {
            this._machine_dij.set( "loading", data.loading );
            this._machine_dij.setHighlighting( data.highlighting );
        } else {
            this._machine_dij.set( "loading", false );
            this._machine_dij.setHighlighting( null );
        }
    }

} );


return b_navigator_Machine;

} );
