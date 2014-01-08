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
    "doh/runner"
],
function(
        b_Duration,
        doh
    )
{
    return function() {

doh.register( "bluegene/test/Duration", [

function years()  { doh.assertEqual( "P1Y", new b_Duration( { years: 1 } ).toIso() ); },
function months()  { doh.assertEqual( "P1M", new b_Duration( { months: 1 } ).toIso() ); },
function days()  { doh.assertEqual( "P1D", new b_Duration( { days: 1 } ).toIso()); },
function hours()  { doh.assertEqual( "PT1H", new b_Duration( { hours: 1 } ).toIso() ); },
function minutes()  { doh.assertEqual( "PT1M", new b_Duration( { minutes: 1 } ).toIso() ); },
function hoursMinutes()  { doh.assertEqual( "PT1H2M", new b_Duration( { hours: 1, minutes: 2 } ).toIso() ); },
function seconds()  { doh.assertEqual( "PT1S", new b_Duration( { seconds: 1 } ).toIso() ); },
function ymdhms()  { doh.assertEqual( "P1Y2M3DT4H5M6S", new b_Duration( { years: 1, months: 2, days: 3, hours: 4, minutes: 5, seconds: 6 } ).toIso() ); },

function parseP1Y()  { doh.assertEqual( new b_Duration( { years: 1 } ).toIso(), b_Duration.parseIso( "P1Y" ).toIso() ); },
function parseP1M()  { doh.assertEqual( new b_Duration( { months: 1 } ).toIso(), b_Duration.parseIso( "P1M" ).toIso() ); },
function parseP1D()  { doh.assertEqual( new b_Duration( { days: 1 } ).toIso(), b_Duration.parseIso( "P1D" ).toIso() ); },
function parsePT1H()  { doh.assertEqual( new b_Duration( { hours: 1 } ).toIso(), b_Duration.parseIso( "PT1H" ).toIso() ); },
function parsePT1M()  { doh.assertEqual( new b_Duration( { minutes: 1 } ).toIso(), b_Duration.parseIso( "PT1M" ).toIso() ); },
function parsePT1S()  { doh.assertEqual( new b_Duration( { seconds: 1 } ).toIso(), b_Duration.parseIso( "PT1S" ).toIso() ); },
function parseP2D()  { doh.assertEqual( new b_Duration( { days: 2 } ).toIso(), b_Duration.parseIso( "P2D" ).toIso() ); },
function parseYMDHMS()  { doh.assertEqual( new b_Duration( { years: 1, months: 10, days: 20, hours: 5, minutes: 40, seconds: 50 } ).toIso(), b_Duration.parseIso( "P1Y10M20DT5H40M50S" ).toIso() ); }

] );

        };
} );
