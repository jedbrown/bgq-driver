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
    "./JobsChart",
    "./RasChart",
    "./topic",
    "./UtilizationChart",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_JobsChart,
        l_RasChart,
        l_topic,
        l_UtilizationChart,
        d_array,
        d_declare,
        d_lang,
        module
    )
{


var _REFRESH_INTERVAL_MILLIS = (5 * 60 * 1000); // 5 minutes


var b_navigator_Charts = d_declare( null,

/** @lends bluegene^navigator^Charts# */
{
    _charts_dij : null,

    _charts : null,

    _refresh_interval_id : null,

    _logged_in : false,

    _displayed : true,


    /** @constructs */
    constructor : function(
            bgws,
            charts_dij
        )
    {
        this._charts_dij = charts_dij;

        this._charts_dij.on( "visibilityChanged", d_lang.hitch( this, this._visibilityChanged ) );

        this._charts = [
                new l_RasChart( bgws, charts_dij.ras ),
                new l_JobsChart( bgws, charts_dij.jobs ),
                new l_UtilizationChart( bgws, charts_dij.utilization )
            ];

        l_topic.subscribe( l_topic.loggedIn, d_lang.hitch( this, this._loggedIn ) );
        l_topic.subscribe( l_topic.endSession, d_lang.hitch( this, this._sessionEnded ) );
    },


    _loggedIn : function()
    {
        this._logged_in = true;

        if ( ! this._displayed )  return;

        this._cancelRefreshIfActive();

        // Start a timer to refresh chart data on an interval.
        this._refresh_interval_id = window.setInterval( function() { l_topic.publish( l_topic.refreshCharts ); }, _REFRESH_INTERVAL_MILLIS );
    },


    _sessionEnded : function()
    {
        this._logged_in = false;
        this._cancelRefreshIfActive();
    },


    _visibilityChanged : function( is_visible )
    {
        this._displayed = is_visible;

        console.log( module.id + ": visibility changed, ", is_visible );

        if ( this._displayed ) {

            // Charts are now displayed. Request new updates if logged in.

            if ( ! this._logged_in ) {
                return;
            }

            // Force a refresh now.
            l_topic.publish( l_topic.refreshCharts );

            // Start a timer to refresh chart data every 5 minutes.
            this._refresh_interval_id = window.setInterval( function() { l_topic.publish( l_topic.refreshCharts ); }, _REFRESH_INTERVAL_MILLIS );

        } else {

            // Charts are now hidden. End the update interval if it's active.
            this._cancelRefreshIfActive();

        }
    },


    _cancelRefreshIfActive : function()
    {
        if ( ! this._refresh_interval_id ) {
            return;
        }

        window.clearInterval( this._refresh_interval_id );
        this._refresh_interval_id = null;
    }

} );


return b_navigator_Charts;

} );
