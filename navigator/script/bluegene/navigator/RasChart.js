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
    "./topic",
    "../Bgws",
    "dojo/_base/declare",
    "dojo/_base/lang"
],
function(
        l_topic,
        b_Bgws,
        d_declare,
        d_lang
    )
{


var b_navigator_RasChart = d_declare( null,

/** @lends bluegene^navigator^RasChart# */
{
    _bgws: null,

    _ras_chart_dij : null,


    /** @constructs */
    constructor: function(
            bgws,
            ras_chart_dij
        )
    {
        this._bgws = bgws;
        this._ras_chart_dij = ras_chart_dij;

        this._ras_chart_dij.on( "selected", d_lang.hitch( this, this._selected ) );

        l_topic.subscribe( l_topic.loggedIn, d_lang.hitch( this, this._loggedIn ) );
        l_topic.subscribe( l_topic.refreshCharts, d_lang.hitch( this, this._refresh ) );
    },


    _fetchRasData : function()
    {
        return this._bgws.fetch( b_Bgws.resourcePaths.summary.ras );
    },


    _loggedIn : function()
    {
        if ( ! this._ras_chart_dij )  return;

        this._ras_chart_dij.setFetchRasDataFn( d_lang.hitch( this, this._fetchRasData ) );
    },


    _refresh: function()
    {
        if ( ! this._ras_chart_dij )  return;

        this._ras_chart_dij.notifyRefresh();
    },


    _selected : function( args )
    {
        l_topic.publish( l_topic.rasEventsSelected, args );
    }

} );


return b_navigator_RasChart;

} );
