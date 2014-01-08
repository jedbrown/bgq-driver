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


var b_navigator_UtilizationChart = d_declare( null,

/** @lends bluegene^navigator^UtilizationChart# */
{
    _bgws: null,

    _utilization_chart_dij : null,


    /** @constructs */
    constructor: function(
            bgws,
            utilization_chart_dij
        )
    {
        this._bgws = bgws;
        this._utilization_chart_dij = utilization_chart_dij;

        l_topic.subscribe( l_topic.loggedIn, d_lang.hitch( this, this._loggedIn ) );
        l_topic.subscribe( l_topic.refreshCharts, d_lang.hitch( this, this._refresh ) );
    },


    _loggedIn : function()
    {
        if ( ! this._utilization_chart_dij )  return;

        this._utilization_chart_dij.setFetchDataFn( d_lang.hitch( this, this._fetchData ) );
    },


    _fetchData : function()
    {
        return this._bgws.fetch( b_Bgws.resourcePaths.summary.utilization );
    },


    _refresh: function()
    {
        if ( ! this._utilization_chart_dij )  return;

        this._utilization_chart_dij.notifyRefresh();
    }

} );


return b_navigator_UtilizationChart;

} );
