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
    "./EventsMonitorMixin",
    "./topic",
    "../Bgws",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_EventsMonitorMixin,
        l_topic,
        b_Bgws,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_JobHistoryDetails = d_declare( [ l_EventsMonitorMixin ],

/** @lends bluegene^navigator^JobHistoryDetails# */
{
    _bgws: null,

    _details_dij : null,


    /**
     *  @class Job history details section.
     *  @constructs
     */
    constructor: function(
            /** bluegene^Bgws */ bgws,
            history_job_details_dij
        )
    {
        this._bgws = bgws;
        this._details_dij = history_job_details_dij;

        l_topic.subscribe( l_topic.historyJobSelected, d_lang.hitch( this, this._jobSelected ) );
    },


    // override EventsMonitorMixin
    _refresh: function()
    {
        this._details_dij.notifyRefresh();
    },


    // override EventsMonitorMixin
    _loggedIn : function( args )
    {
        // If the user doesn't have hardware READ the RAS tab should be hidden, otherwise displayed.

        this._details_dij.setFetchDetailsFn( d_lang.hitch( this, this._fetchDetails ) );

        this._details_dij.setRasStore(
                this._bgws.getRasDataStore()
            );

        if ( args.userInfo.hasHardwareRead() ) {
            this._details_dij.setShowRas( true );
        } else {
            console.log( module.id + ": user doesn't have hardware READ, hiding jobs details RAS tab." );
            this._details_dij.setShowRas( false );
        }
    },


    _fetchDetails: function( job_id )
    {
        return this._bgws.fetchJobDetails( job_id );
    },


    _jobSelected: function( /**object,optional*/ job_info )
    {
        console.log( module.id + ": selected ", job_info );

        this._details_dij.setJobInfo( job_info );
    }

} );


return b_navigator_JobHistoryDetails;

} );
