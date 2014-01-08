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

// Profile file, see http://www.dojotoolkit.org/reference-guide/build/index.html


var profile = {
    stripConsole: "none",
    packages: [
       {
           name: "bluegene",
           location: "script/bluegene"
       },
       {
            name: "dojo",
            location: "build/dojo/dojo"
        },
        {
            name: "dijit",
            location: "build/dojo/dijit"
        },
        {
            name: "dojox",
            location: "build/dojo/dojox"
        }
    ],
    layers: {

            "dojo/moreDojoLayer": {
                    exclude: [],
                    include: [ "bluegene/moreDojoLayer" ]
                },

            "bluegene/bluegeneLayer": {
                    copyright: "bgCopyright.js",
                    exclude: [ "bluegene/moreDojoLayer" ],
                    include: [ "bluegene/layer" ]
                }
        }
};
