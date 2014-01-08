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
],
function(
    )

{
    var _ISO_8601_DATE_RE = /(\d{4})(\d{2})(\d{2})/;

    /** @name bluegene^navigator^ChartsCommon
     *  @namespace
     */
    var b_navigator_ChartsCommon =

    /** @lends bluegene^navigator^ChartsCommon */
    {
            parseDate : function( date_str )
            {
                var m = _ISO_8601_DATE_RE.exec( date_str );
                return new Date( +m[1], +m[2] - 1, +m[3] );
            },

            formatDate: function( date_str )
            {
                var m = _ISO_8601_DATE_RE.exec( date_str );
                return ("" + +m[2] + "/" + +m[3] );
            }
        };

    return b_navigator_ChartsCommon;
} );
