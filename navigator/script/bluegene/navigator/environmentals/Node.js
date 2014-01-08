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
    "./AbstractEnv",
    "../../Bgws",
    "dojo/_base/declare"
],
function(
        l_AbstractEnv,
        b_Bgws,
        d_declare
    )
{


var b_navigator_environmentals_Node = d_declare( [ l_AbstractEnv ],

/** @lends bluegene^navigator^environmentals^Node# */
{

    /** @constructs */
    constructor : function()  { /* Nothing to do */ },

    // override
    _getName: function()  { return "Node"; },

    // override
    _getId: function()  { return "node"; },

    // override
    _getUri: function()  { return b_Bgws.resourcePaths.environmentals.node; }

} );


return b_navigator_environmentals_Node;

} );
