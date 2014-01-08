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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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
    "dojo/when",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        d_when,
        d_array,
        d_declare,
        d_lang,
        module
    )
{

var b_utility_Requests = d_declare( [],

{

    _requests : null,


    constructor : function()
    {
        this._requests = [];
    },


    monitor : function( request )
    {
        this.add( request );

        // console.log( module.id + ": monitoring request, now have ", this._requests.length, "request=", request );

        d_when(
                request,
                d_lang.hitch( this, this.complete, request ),
                d_lang.hitch( this, this.complete, request )
            );
    },


    add : function( request )
    {
        this._requests.push( request );
    },


    complete : function( request )
    {
        var i = this._requests.indexOf( request );
        if ( i >= 0 ) {
            this._requests.splice( i, 1 );
        }

        // console.log( module.id + ": request complete, now have ", this._requests.length, "request=", request );
    },


    cancel : function()
    {
        console.log( module.id + ": canceling", this._requests.length, "requests!" );
        d_array.forEach( this._requests, function( r ) {
            r.cancel();
        } );
    }
} );


return b_utility_Requests;

} );