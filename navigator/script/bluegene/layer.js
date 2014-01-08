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

// Layer file, see http://www.dojotoolkit.org/reference-guide/build/index.html

// Any classes referenced in navigator.html need to be here.


define(
[
    "dojo/parser",


    "bluegene/navigator/Navigator",

    "bluegene/navigator/dijit/Login",
    "bluegene/navigator/dijit/Navigator"

],
function(
        d_parser,
        b_navigator_Navigator,
        b_navigator_dijit_Login,
        b_navigator_dijit_Navigator
)
{


var b_layer = {
        dojo: {
            parser: d_parser
        },
        bluegene: {
            navigator: {
                Navigator: b_navigator_Navigator,

                dijit: {
                    Login: b_navigator_dijit_Login,
                    Navigator: b_navigator_dijit_Navigator
                }
            }
        }
};

return b_layer;

} );
