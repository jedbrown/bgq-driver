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


// Note: I tried using relative module refs here but for some reason doesn't find them. Maybe something in DOH?
require(
[
    "bluegene/test/dojo",

    "bluegene/utility/test/calcNode",
    "bluegene/utility/test/combineDates",
    "bluegene/utility/test/escapeHtml",
    "bluegene/utility/test/visibility",
    "bluegene/utility/test/Optional",
    "bluegene/utility/test/Stack",
    "bluegene/utility/test/xlate",

    "bluegene/test/BlockBuilder",
    "bluegene/test/Duration",
    "bluegene/test/TimeInterval",

    "bluegene/dijit/test/ActionLink",
    "bluegene/dijit/test/Hideable",
    "bluegene/dijit/test/OutputFormat",
    "bluegene/dijit/test/OutputText",
    "bluegene/navigator/dijit/test/NumberCompareOption",
    "bluegene/navigator/dijit/test/TimeInterval"
],
function()
{
    for ( var i = 0 ; i < arguments.length ; ++i ) {
        arguments[i]();
    }
} );
