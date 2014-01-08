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
    "bluegene/utility/xlate",
    "doh/runner"
],
function(
        b_utility_xlate,
        doh
    )
{
    return function() {

var map1 = {
        "a" : "abc",
        "b" : "def",
        "1" : "ghi"
    };


doh.register( "bluegene/utility/test/xlate", [

function xlate_simple() {
    // If call xlate with the value in the map returns the mapped value

    doh.is( "abc", b_utility_xlate.xlate( map1, "a" ) );
},

function xlate_notpresent() {
    // If call xlate with a value not in the map returns the original value.

    doh.is( "notavalue", b_utility_xlate.xlate( map1, "notavalue" ));
},

function xlate_null() {
    // If call xlate with null returns null.

    doh.is( null, b_utility_xlate.xlate( map1, null ));
},


function xlate_number() {
    // If pass a number, should be converted to a string and get the value.

    doh.is( "ghi", b_utility_xlate.xlate( map1, 1 ));
},


function xlator_simple() {
    // an xlator is like xlate but stores the map.

    var xlator = b_utility_xlate.xlator( map1 );
    doh.is( "abc", xlator( "a" ) );
    doh.is( "def", xlator( "b" ) );
}

] );

        };
} );
