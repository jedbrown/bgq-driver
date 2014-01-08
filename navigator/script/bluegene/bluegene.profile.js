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

var profile = (function()
{
    return {
        resourceTags: {
            test: function( filename, mid )
            {
                return /^bluegene\/test/.test( mid ) ||
                       /^bluegene\/dijit\/test/.test( mid ) ||
                       /^bluegene\/navigator\/dijit\/test/.test( mid ) ||
                       /^bluegene\/utility\/test/.test( mid );
            },

            copyOnly: function( filename, mid )
            {
                if ( /\.json$/.test( mid ) )  return true;
                return false;
            },

            amd: function( filename, mid )
            {
                if ( /\.js$/.test( filename ) )  return true;
                return false;
            },

            miniExclude: function( filename, mid ) 
            {
                return false;
            }
        },

        trees: [
                [ ".", ".", /(\/\.)|(~$)|(\.svn)/ ]
            ]
    };
})();
