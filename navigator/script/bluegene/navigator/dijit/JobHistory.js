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
    "./MonitorActiveMixin",
    "../FilterOptions",
    "../xlate",
    "../../Bgws",
    "../../BlueGene",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/text!./templates/JobHistory.html",
    "module",

    // Used only in template.
    "./JobDetails",

    "./MultiSelectCodes",
    "./TimeInterval",
    "dijit/TooltipDialog",
    "dijit/form/Button",
    "dijit/form/DropDownButton",
    "dijit/form/Form",
    "dijit/form/NumberTextBox",
    "dijit/form/TextBox",
    "dijit/form/ValidationTextBox",
    "dijit/layout/BorderContainer",
    "dijit/layout/ContentPane",
    "dojox/grid/EnhancedGrid"
],
function(
        l_AbstractTemplatedContainer,
        l_MonitorActiveMixin,
        ll_FilterOptions,
        ll_xlate,
        b_Bgws,
        b_BlueGene,
        d_declare,
        d_lang,
        template,
        module
    )
{


var _formToQuery = function( form_obj )
    {
        var query_obj = ll_FilterOptions.formToQueryHelper( form_obj, [ "block", "user", "executable", "exitStatus", "endTime", "id", "startTime" ] );

        if ( form_obj.status === "" )  query_obj.status = [b_BlueGene.jobStatus.ERROR, b_BlueGene.jobStatus.TERMINATED].join( "" );
        else query_obj.status = form_obj.status;

        return query_obj;
    };


var b_navigator_dijit_JobHistory = d_declare(
        [ l_AbstractTemplatedContainer, l_MonitorActiveMixin ],

{

    templateString : template,

    _store : null,

    _dirty : true,
    _initialized : false,


    getDetailsDijit : function()  { return this._details; },


    setJobsStore : function( new_store )
    {
        this._store = new_store;
        this._refresh();
    },

    setFilter : function( args )
    {
        var new_form_obj = {
                block : "",
                user : "",
                executable : "",
                exitStatus : NaN,
                status : [b_BlueGene.jobStatus.ERROR, b_BlueGene.jobStatus.TERMINATED].join(),
                endTime : null,
                startTime: args.startTime.toIso()
            };

        this._filterOptionsForm.set( "value", new_form_obj );

        if ( this._isActive() ) {
            this._filter_options.apply();
        } else {
            this._dirty = true;
        }
    },

    notifyRefresh : function()
    {
        this._refresh();
    },

    onJobSelected : function( job_info ) {},


    // override
    startup : function()
    {
        this.inherited( arguments );

        this._filterOptionsEndTime.set( "value", "P1D" );

        this._filter_options = new ll_FilterOptions( {
                name : "JobHistory",
                grid : this._grid,
                form : this._filterOptionsForm,
                dropdown : this._filterOptionsDropDownButton,
                resetButton: this._filterOptionsResetButton,
                formToQuery : _formToQuery
            } );

        this._grid.on( "rowClick", d_lang.hitch( this, this._rowClicked ) );
    },


    // override MonitorActiveMixin
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
        if ( ! this._store )  return;
        if ( ! this._dirty )  return;
        if ( ! this._isActive() )  return;

        this._grid.selection.clear();

        if ( ! this._initialized ) {
            this._grid.setStore( this._store, this._filter_options.calcQuery(), null );
            this._initialized = true;
        } else {
            this._grid.render();
        }

        this._dirty = false;
    },


    _rowClicked : function( e )
    {
        var item = this._grid.selection.getFirstSelected();

        if ( item == null ) {
            console.log( module.id + ": _onRowClick with no item" );
            return;
        }

        var job_id = this._store.getValue( item, "id", "" );
        var start_time_str = this._store.getValue( item, "startTime", "" );
        var end_time_str = this._store.getValue( item, "endTime", "" );

        var start_time = b_Bgws.parseTimestamp( start_time_str );
        var end_time = b_Bgws.parseTimestamp( end_time_str );

        console.log( module.id + ": selected job", job_id, "started at", start_time, "end time=", end_time );

        this.onJobSelected( { id: job_id, startTime: start_time, endTime: end_time } );
    },


    _ll_xlate : ll_xlate

} );

return b_navigator_dijit_JobHistory;

} );
