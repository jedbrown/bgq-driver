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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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
    "./topic",
    "dojo/_base/declare",
    "dojo/_base/lang"
],
function(
        l_topic,
        d_declare,
        d_lang
    )
{


var b_navigator_EventsMonitorMixin = d_declare( [],

{
    constructor : function()
    {
        l_topic.subscribe( l_topic.loggedIn, d_lang.hitch( this, this._loggedIn ) );
        l_topic.subscribe( l_topic.refresh, d_lang.hitch( this, this._refresh ) );
        l_topic.subscribe( l_topic.endSession, d_lang.hitch( this, this._sessionEnded ) );
    },


    // Override, called when session starts.
    _loggedIn : function( args ) {},

    // Override, called when refresh.
    _refresh : function() {},

    // Override, called when session ended.
    _sessionEnded : function() {}

} );


b_navigator_EventsMonitorMixin.refresh = function()
    {
        l_topic.publish( l_topic.refresh );
    };


b_navigator_EventsMonitorMixin.endSession = function()
    {
        l_topic.publish( l_topic.endSession );
    };


return b_navigator_EventsMonitorMixin;

} );