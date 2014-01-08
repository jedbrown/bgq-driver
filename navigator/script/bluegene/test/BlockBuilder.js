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
    "bluegene/BlockBuilder",
    "doh/runner"
],
function(
        b_BlockBuilder,
        doh
    )
{
    return function() {

var midplanes_1x1 = [[[["R00-M0","R00-M1"]]]];

var midplanes_4x4 = [[[["R00-M0","R00-M1"],["R01-M0","R01-M1"]],
                      [["R20-M0","R20-M1"],["R21-M0","R21-M1"]],
                      [["R30-M0","R30-M1"],["R31-M0","R31-M1"]],
                      [["R10-M0","R10-M1"],["R11-M0","R11-M1"]]],
                     [[["R02-M0","R02-M1"],["R03-M0","R03-M1"]],
                      [["R22-M0","R22-M1"],["R23-M0","R23-M1"]],
                      [["R32-M0","R32-M1"],["R33-M0","R33-M1"]],
                      [["R12-M0","R12-M1"],["R13-M0","R13-M1"]]]];


doh.register( "bluegene/test/BlockBuilder", [


function t_initially_invalid() {
    var bb = new b_BlockBuilder( midplanes_1x1 );

    doh.assertFalse( bb.isValid() );
},


function t_1x1_r000() {
    var bb = new b_BlockBuilder( midplanes_1x1 );

    bb.add( "R00-M0" );

    doh.assertTrue( bb.isValid() );

    var cbi = bb.calcCreateBlockInfo();

    doh.assertEqual( "R00-M0", cbi.midplane );
    doh.assertEqual( "1", cbi.midplanes[0] );
    doh.assertEqual( "1", cbi.midplanes[1] );
    doh.assertEqual( "1", cbi.midplanes[2] );
    doh.assertEqual( "1", cbi.midplanes[3] );
    doh.assertEqual( "ABCDE", cbi.torus );
},


function t_1x1_r001() {
    var bb = new b_BlockBuilder( midplanes_1x1 );

    bb.add( "R00-M1" );

    doh.assertTrue( bb.isValid() );

    var cbi = bb.calcCreateBlockInfo();

    doh.assertEqual( "R00-M1", cbi.midplane );
    doh.assertEqual( "1", cbi.midplanes[0] );
    doh.assertEqual( "1", cbi.midplanes[1] );
    doh.assertEqual( "1", cbi.midplanes[2] );
    doh.assertEqual( "1", cbi.midplanes[3] );
    doh.assertEqual( "ABCDE", cbi.torus );
},


function t_1x1_r00() {
    var bb = new b_BlockBuilder( midplanes_1x1 );

    bb.add( "R00-M0" );
    bb.add( "R00-M1" );

    doh.assertTrue( bb.isValid() );

    var cbi = bb.calcCreateBlockInfo();

    doh.assertEqual( "R00-M0", cbi.midplane );
    doh.assertEqual( "1", cbi.midplanes[0] );
    doh.assertEqual( "1", cbi.midplanes[1] );
    doh.assertEqual( "1", cbi.midplanes[2] );
    doh.assertEqual( "11", cbi.midplanes[3] );
    doh.assertEqual( "ABCDE", cbi.torus );
},


function t_4x4_mesh() {
    var bb = new b_BlockBuilder( midplanes_4x4 );

    bb.add( "R00-M0" );
    bb.add( "R20-M0" );

    doh.assertTrue( bb.isValid() );

    var cbi = bb.calcCreateBlockInfo();

    doh.assertEqual( "R00-M0", cbi.midplane );
    doh.assertEqual( "1", cbi.midplanes[0] );
    doh.assertEqual( "11", cbi.midplanes[1] );
    doh.assertEqual( "1", cbi.midplanes[2] );
    doh.assertEqual( "1", cbi.midplanes[3] );
    doh.assertEqual( "ACDE", cbi.torus );
},


function t_4x4_pt() {
    var bb = new b_BlockBuilder( midplanes_4x4 );

    bb.add( "R00-M0" );
    bb.add( "R20-M0" );
    bb.add( "R30-M0" );
    bb.add( "R20-M0" ); // sets R20-M0 to passthrough

    doh.assertTrue( bb.isValid() );

    var cbi = bb.calcCreateBlockInfo();

    doh.assertEqual( "R00-M0", cbi.midplane );
    doh.assertEqual( "1", cbi.midplanes[0] );
    doh.assertEqual( "101", cbi.midplanes[1] );
    doh.assertEqual( "1", cbi.midplanes[2] );
    doh.assertEqual( "1", cbi.midplanes[3] );
    doh.assertEqual( "ACDE", cbi.torus );
}

] );

        };
} );
