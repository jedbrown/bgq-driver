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
    "bluegene/Duration",
    "bluegene/TimeInterval",
    "doh/runner"
],
function(
        b_Duration,
        b_TimeInterval,
        doh
    )
{
    return function() {

doh.register( "bluegene/test/TimeInterval", [

function dateToIso() {
    doh.assertEqual( "20110101T000000", b_TimeInterval.dateToIso( new Date( 2011, 0, 1, 0, 0, 0 ) ) );
    doh.assertEqual( "20111010T111213", b_TimeInterval.dateToIso( new Date( 2011, 9, 10, 11, 12, 13 ) ) );
},

function parseIsoTimestamp() {
    doh.assertEqual( b_TimeInterval.dateToIso( new Date( 2011, 0, 1, 0, 0, 0 ) ), b_TimeInterval.dateToIso( b_TimeInterval.parseIsoTimestamp( "20110101T000000" ) ) );
    doh.assertEqual( b_TimeInterval.dateToIso( new Date( 2011, 9, 10, 11, 12, 13 ) ), b_TimeInterval.dateToIso( b_TimeInterval.parseIsoTimestamp( "20111010T111213" ) ) );
},

function startEnd() {
    doh.assertEqual( "20110101T000000/20110102T000000", new b_TimeInterval( { start: new Date( 2011, 0, 1, 0, 0, 0 ), end: new Date( 2011, 0, 2, 0, 0, 0 ) } ).toIso() );
},

function duration() {
    doh.assertEqual( "P1D", new b_TimeInterval( { duration: new b_Duration( { days: 1 } ) } ).toIso() );
},

function startDuration() {
    doh.assertEqual( "20110101T000000/P1D", new b_TimeInterval( { start: new Date( 2011, 0, 1, 0, 0, 0 ), duration: new b_Duration( { days: 1 } ) } ).toIso() );
},

function durationEnd() {
    doh.assertEqual( "P1D/20110102T000000", new b_TimeInterval( { duration: new b_Duration( { days: 1 } ), end: new Date( 2011, 0, 2, 0, 0, 0 ) } ).toIso() );
},

function parseStartEnd() {
    doh.assertEqual( "20110101T000000/20110102T000000", b_TimeInterval.parseIso( "20110101T000000/20110102T000000" ).toIso() );
},

function parseDuration() {
    doh.assertEqual( "P1D", b_TimeInterval.parseIso( "P1D" ).toIso() );
},

function parrseDurationStart() {
    doh.assertEqual( "20110101T000000/P1D", b_TimeInterval.parseIso( "20110101T000000/P1D" ).toIso() );
},

function parrseDurationEnd() {
    doh.assertEqual( "P1D/20110102T000000", b_TimeInterval.parseIso( "P1D/20110102T000000" ).toIso() );
}

] );

        };
} );
