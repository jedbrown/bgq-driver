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

            doh.register( "bluegene/utility/test/combineDates", [

            function test1() {
                doh.assertEqual( new Date( 2011, 0, 2, 1, 7, 30 ), b_utility.combineDates( new Date( 2011, 0, 2, 0, 0, 0 ), new Date( 2011, 0, 1, 1, 7, 30 ) ) );
            }

            ] );

        };
} );
