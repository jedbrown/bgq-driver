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
    "./AbstractTemplatedContainer",
    "./JobRasGrid",
    "./MonitorActiveMixin",
    "../../Bgws",
    "../../TimeInterval",
    "dojo/when",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/text!./templates/JobDetails.html",
    "module",

    // Used only in template.
    "./JobFields",
    "../../dijit/OutputFormat",
    "dijit/layout/ContentPane",
    "dijit/layout/StackContainer",
    "dijit/layout/TabContainer"
],
function(
        l_AbstractTemplatedContainer,
        l_JobRasGrid,
        l_MonitorActiveMixin,
        b_Bgws,
        b_TimeInterval,
        d_when,
        d_declare,
        d_lang,
        template,
        module
    )
{


var b_navigator_dijit_JobDetails = d_declare(
        [ l_AbstractTemplatedContainer, l_MonitorActiveMixin ],

{

    templateString : template,

    _fetch_details_fn : null,

    _ras_dij : null,
    _ras_store : null,

    _job_info : null,

    _dirty : true,

    _fetch_deferred : null,



    setFetchDetailsFn : function( fn )
    {
        this._fetch_details_fn = fn;
        this._refresh();
    },

    setShowRas : function( show_ras )
    {
        if ( show_ras ) {

            if ( ! this._ras_dij ) {
                this._ras_dij = new l_JobRasGrid( { title: "RAS" } );

                this._loadedPane.addChild( this._ras_dij );
            }

            this._refresh();

        } else {

            if( this._ras_dij ) {
                this._loadedPane.removeChild( this._ras_dij );
                this._ras_dij = null;
            }

        }
    },

    setRasStore : function( ras_store )
    {
        this._ras_store = ras_store;
        this._refresh();
    },

    setJobInfo : function( job_info )
    {
        this._job_info = job_info;
        this._refresh();
    },


    notifyRefresh : function()
    {
        this._refresh();
    },


    // override
    destroy : function()
    {
        if ( this._fetch_deferred ) {
            this._fetch_deferred.cancel();
            this._fetch_deferred = null;
        }

        this.inherited( arguments );
    },


    // Override MonitorActiveMixin
    _notifyActive : function()
    {
        this._loadIfReady();
    },


    _refresh : function()
    {
        this._dirty = true;
        this._loadIfReady();
    },


    _loadIfReady : function()
    {
        if ( ! this._fetch_details_fn )  return;
        if ( ! this._ras_store )  return;
        if ( ! this._dirty )  return;
        if ( ! this._isActive() /*MonitorActiveMixin*/ )  return;

        if ( this._fetch_deferred ) {
            this._fetch_deferred.cancel();
            this._fetch_deferred = null;
        }

        if ( this._job_info === null ) {
            this._displayStack.selectChild( this._noJobPane );
            return;
        }

        this._fetch_deferred = this._fetch_details_fn( this._job_info.id );

        this._dirty = false;

        this._displayStack.selectChild( this._loadingPane );

        d_when(
                this._fetch_deferred,
                d_lang.hitch( this, this._jobDetailsLoaded ),
                d_lang.hitch( this, this._fetchDetailsFailed )
            );


        if ( this._ras_dij ) {

            var interval = new b_TimeInterval( {
                    start: this._job_info.startTime,
                    end: new Date( (new Date()).getTime() + (24*60*60*1000) ) // Some time in the future, tomorrow.
                } );

            var query = {
                    job: this._job_info.id,
                    interval: interval.toIso()
                };

            this._ras_dij.setStore( this._ras_store, query, null );

        }

    },


    _jobDetailsLoaded : function( result )
    {
        this._fetch_deferred = null;

        console.log( module.id + ": got job details ", result );

        this._detailsTab.set( "value", result );
        this._displayStack.selectChild( this._loadedPane );
    },


    _fetchDetailsFailed : function( error )
    {
        this._fetch_deferred = null;

        var io_args = this._fetch_deferred.ioArgs;

        var err_text = b_Bgws.calculateErrorMessage( error, io_args );

        console.log( module.id + ": failed to fetch details for job " + this._job_info.id + ", error=" + err_text );

        this._errorFmt.set( "value", { id: ("" + this._job_info.id), msg: err_text } );
        this._displayStack.selectChild( this._errorPane );
    }

} );

return b_navigator_dijit_JobDetails;

} );
