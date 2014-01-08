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
    "dojo/text!./templates/Jobs.html",
    "module",

    // Used only in template.
    "./JobDetails",

    "./MultiSelectCodes",
    "./TimeInterval",
    "../format",
    "dijit/TooltipDialog",
    "dijit/form/Button",
    "dijit/form/DropDownButton",
    "dijit/form/Form",
    "dijit/form/TextBox",
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


var _DEFAULT_QUERY = {
        status: [
                 b_BlueGene.jobStatus.SETUP,
                 b_BlueGene.jobStatus.LOADING,
                 b_BlueGene.jobStatus.STARTING,
                 b_BlueGene.jobStatus.RUNNING,
                 b_BlueGene.jobStatus.DEBUG,
                 b_BlueGene.jobStatus.CLEANUP
             ].join( "" )
    };


var _formToQuery = function( form_obj )
    {
        var query_obj = ll_FilterOptions.formToQueryHelper( form_obj, [ "user", "block", "executable", "startTime" ] );

        if ( ! form_obj.status )  query_obj.status = _DEFAULT_QUERY.status;
        else  query_obj.status = form_obj.status;

        return query_obj;
    };


var b_navigator_dijit_Jobs = d_declare(
        [ l_AbstractTemplatedContainer, l_MonitorActiveMixin ],

{

    templateString : template,

    _store : null,

    _initialized : false,
    _dirty : true,


    getDetailsDijit : function()
    {
        return this._details;
    },

    setJobsStore : function( new_store )
    {
        this._store = new_store;
        this._refresh();
    },


    notifyRefresh : function()
    {
        this._refresh();
    },


    onJobSelected : function( args /* { id: job_id, startTime: timestamp } */ ) {},


    // override
    startup : function()
    {
        this.inherited( arguments );


        this._filter_options = new ll_FilterOptions( {
                name : "Jobs",
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
        if ( ! this._isActive() /*MonitorActiveMixin*/ )  return;

        if ( this._initialized ) {
            this._grid.render();
        } else {
            this._grid.setStore( this._store, _DEFAULT_QUERY, null );
            this._initialized = true;
        }

        this._dirty = false;
    },


    _rowClicked : function( e )
    {
        if ( this._store === null )  return;

        var item = this._grid.selection.getFirstSelected();

        if ( item === null ) {
            console.log( module.id + ": _onRowClick with no item" );
            return;
        }

        var job_id = this._store.getValue( item, "id", "" );
        var start_time_str = this._store.getValue( item, "startTime", "" );

        var start_time = b_Bgws.parseTimestamp( start_time_str );

        console.log( module.id + ": selected job", job_id, "started at", start_time );

        this.onJobSelected( { id : job_id, startTime: start_time } );
    },
    
    
    _ll_xlate : ll_xlate

} );

return b_navigator_dijit_Jobs;

} );
