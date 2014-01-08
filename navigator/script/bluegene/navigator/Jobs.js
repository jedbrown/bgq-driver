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
    "./JobDetails",
    "./topic",
    "../Bgws",
    "dojo/_base/declare",
    "dojo/_base/lang"
],
function(
        l_AbstractTab,
        l_JobDetails,
        l_topic,
        b_Bgws,
        d_declare,
        d_lang
    )
{


var b_navigator_Jobs = d_declare( [ l_AbstractTab ],

/** @lends bluegene^navigator^Jobs# */
{

    _bgws: null,
    _jobs_dijit : null,


    /**
     *  Create instance of Jobs.
     *
     *  @class Stuff for the Jobs tab.
     *  @constructs
     */
    constructor: function(
            /**bluegene^Bgws*/ bgws,
            jobs_dij,
            current_job_details_dij
        )
    {
        this._bgws = bgws;
        this._jobs_dijit = jobs_dij;

        this._jobs_dijit.on( "jobSelected", d_lang.hitch( this, this._jobSelected ) );


        new l_JobDetails(
                this._bgws,
                current_job_details_dij
            );
    },


    // override AbstractTab
    _getTabDijitName : function() { return "jobs"; },

    // override AbstractTab
    _refresh: function()
    {
        this._jobs_dijit.notifyRefresh();
    },

    // override from AbstractTab
    _loggedIn : function( args )
    {
        this._jobs_dijit.setJobsStore(
                this._bgws.getJobsDataStore()
            );
    },


    _jobSelected: function( args )
    {
        l_topic.publish( l_topic.jobSelected, args );
    }

} );


return b_navigator_Jobs;

} );
