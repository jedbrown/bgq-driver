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
    "dojo/_base/lang"
],
function(
        d_lang
    )
{


var _xlate = function( map, val ) {
    if ( ! (val in map) )  return val;
    return map[val];
};


/** @name bluegene^utility^xlate
 *  @namespace
 */
var b_utility_xlate =

/** @lends bluegene^utility^xlate */
{

    xlate : _xlate,

    xlator : function( map ) {
        return d_lang.partial( _xlate, map );
    }

};

return b_utility_xlate;

} );
