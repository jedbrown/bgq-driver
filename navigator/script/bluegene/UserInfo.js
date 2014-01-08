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
    "dojo/_base/declare"
],
function(
        d_declare
    )
{


var b_UserInfo = d_declare( null,

/** @lends bluegene^UserInfo# */
{

    _user_info : null,


    /** @constructs */
    constructor : function( user_info )
    {
        this._user_info = user_info;
    },


    isAdministrator : function()
    {
        return (("isAdministrator" in this._user_info) && this._user_info.isAdministrator);
    },

    hasHardwareRead : function()
    {
        return (("hardwareRead" in this._user_info) && this._user_info.hardwareRead);
    },

    hasBlockCreate : function()
    {
        return (("blockCreate" in this._user_info) && this._user_info.blockCreate);
    }

} );

return b_UserInfo;

} );
