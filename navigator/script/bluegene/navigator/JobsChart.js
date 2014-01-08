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
    "dojo/_base/lang",
    "module"
],
function(
        l_topic,
        b_Bgws,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_JobsChart = d_declare( null,

/** @lends bluegene^navigator^JobsChart */
{
    _bgws: null,

    _jobs_chart_dij : null,


    /** @constructs */
    constructor: function(
            bgws,
            jobs_chart_dij
        )
    {
        this._bgws = bgws;
        this._jobs_chart_dij = jobs_chart_dij;

        this._jobs_chart_dij.on( "selected", d_lang.hitch( this, this._onSelected ) );

        l_topic.subscribe( l_topic.loggedIn, d_lang.hitch( this, this._loggedIn ) );
        l_topic.subscribe( l_topic.refreshCharts, d_lang.hitch( this, this._refresh ) );
    },


    _fetchData : function()
    {
        return this._bgws.fetch( b_Bgws.resourcePaths.summary.jobs );
    },


    _loggedIn : function()
    {
        if ( ! this._jobs_chart_dij )  return;

        this._jobs_chart_dij.setFetchDataFn( d_lang.hitch( this, this._fetchData ) );
    },


    _refresh: function()
    {
        if ( ! this._jobs_chart_dij )  return;

        this._jobs_chart_dij.notifyRefresh();
    },


    _onSelected : function( obj )
    {
        console.log( module.id + ": selected. obj=", obj );

        l_topic.publish( l_topic.jobHistorySelected, obj );
    }

} );


return b_navigator_JobsChart;

} );
