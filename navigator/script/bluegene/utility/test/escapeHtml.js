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
    "bluegene/utility/utility",
    "doh/runner"
],
function(
        b_utility,
        doh
    )
{
    return function() {

doh.register( "bluegene/utility/test/escapeHtml", [


function escapeEmptyString() {
    doh.is( "", b_utility.escapeHtml( "" ) );
},

function escapeLt() {
    doh.is( "&lt;", b_utility.escapeHtml( "<" ) );
},

function escapeGt() {
    doh.is( "&gt;", b_utility.escapeHtml( ">" ) );
},

function escapeAmp() {
    doh.is( "&amp;", b_utility.escapeHtml( "&" ) );
},

function escapeNull() {
    doh.is( "", b_utility.escapeHtml( null ) );
},

function escapeString1() {
    doh.is( "Bits &amp; Bytes", b_utility.escapeHtml( "Bits & Bytes" ) );
},

function escapeString2() {
    doh.is( "1 &lt; 2", b_utility.escapeHtml( "1 < 2" ) );
},

function origStringNotModified() {
    // The string that's passed in isn't modified.

    var text = "Bits & Bytes";

    doh.is( "Bits &amp; Bytes", b_utility.escapeHtml( text ) );
    doh.is( "Bits & Bytes", text );
}


] );

        };
} );
