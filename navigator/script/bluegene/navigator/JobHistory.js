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
    "./AbstractTab",
    "./JobHistoryDetails",
    "./topic",
    "../Bgws",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_AbstractTab,
        l_JobHistoryDetails,
        l_topic,
        b_Bgws,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_JobHistory = d_declare( [ l_AbstractTab ],

/** @lends bluegene^navigator^JobHistory# */
{
    _bgws: null,
    _navigator : null,
    _job_history_dij : null,


    /**
     *  @class Job History tab.
     *  @constructs
     */
    constructor: function(
            /**bluegene^Bgws*/ bgws,
            navigator,
            job_history_dij,
            history_job_details_dij
        )
    {
        this._bgws = bgws;
        this._navigator = navigator;
        this._job_history_dij = job_history_dij;

        this._job_history_dij.on( "jobSelected", d_lang.hitch( this, this._jobSelected ) );

        new l_JobHistoryDetails(
                this._bgws,
                history_job_details_dij
            );

        l_topic.subscribe( l_topic.jobHistorySelected, d_lang.hitch( this, this._selected ) );
    },


    // override AbstractTab
    _getTabDijitName : function() { return "job-history"; },

    // override AbstractTab
    _refresh: function()
    {
        this._job_history_dij.notifyRefresh();
    },

    _loggedIn : function()
    {
        this._job_history_dij.setJobsStore(
                this._bgws.getJobsDataStore()
            );
    },


    _jobSelected: function( job_info )
    {
        l_topic.publish( l_topic.historyJobSelected, job_info );
    },


    _selected : function( args )
    {
        console.log( module.id + ": selected with args=", args );

        this._job_history_dij.setFilter( args );
        this._navigator.switchTo( "job-history" );
    }

} );


return b_navigator_JobHistory;

} );
