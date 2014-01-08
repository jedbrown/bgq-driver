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
    "../format",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/data/ObjectStore",
    "dojo/store/Memory",
    "dojo/text!./templates/RasResults.html",
    "module",

    // Used only in template.
    "./MultiSelectCodes",
    "./MultiWildcardTextBox",
    "./TimeInterval",
    "../xlate",
    "dijit/TooltipDialog",
    "dijit/form/Button",
    "dijit/form/CheckBox",
    "dijit/form/DropDownButton",
    "dijit/form/Form",
    "dijit/form/Select",
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
        ll_format,
        d_array,
        d_declare,
        d_lang,
        d_data_ObjectStore,
        d_store_Memory,
        template,
        module
    )
{


var _FIELD_ID_TO_CELL = {
        "recid" : { field: "id", name: "Record ID", width: "8em", styles: "text-align: right;" },
        "time" : { field: "eventTime", name: "Time", width: "16em", styles: "text-align: right;", cellStyles: "text-align: right;", formatter: ll_format.timestampLong },
        "severity" : { field: "severity", name: "Severity", width: "7em", styles: "text-align: center;" },
        "category" : { field: "category", name: "Category", width: "7em" },
        "component" : { field: "component", name: "Component", width: "9em" },
        "msgid" : { field: "msgId", name: "Message ID", width: "9em" },
        "block" : { field: "block", name: "Block", width: "180px" },
        "location" : { field: "location", name: "Location", width: "12em" },
        "serialnumber" : { field: "serialNumber", name: "Serial Number", width: "13em" },
        "jobid" : { field: "jobId", name: "Job ID", width: "7em" },
        "controlAction" : { field: "controlAction", name: "Control Action", width: "20em", styles: "text-align: center;" },
        "count" : { field: "count", name: "Count", width: "6em", styles: "text-align: right;" },
        "message" : { field: "message", name: "Message", width: "30em" }
    };


var _formToQuery = function( form_obj )
    {
        var query_obj = ll_FilterOptions.formToQueryHelper( form_obj,
                [ "interval", "severity", "block", "category", "component", "job", "message", "msgid", "location", "serialnumber" ]
            );

        if ( form_obj.controlAction != "any" ) query_obj.controlAction = form_obj.controlAction;

        return query_obj;
    };


var b_navigator_dijit_RasResults = d_declare(
        [ l_AbstractTemplatedContainer, l_MonitorActiveMixin ],

{

    templateString : template,

    _store : null,

    _filter_options : null,

    _initialized: false,
    _dirty: true,


    setRasStore : function( store )
    {
        this._store = store;
        this._refresh();
    },

    setFilter : function( args )
    {
        console.log( module.id + ": selected with args=", args );

        var new_form_obj = {
                block : "",
                category : "",
                component : "",
                job : "",
                message : "",
                msgid : "",
                location : "",
                serialnumber : "",
                controlAction : "any",
                severity : args.severities.join(),
                interval : args.interval.toIso()
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

    onEventSelected : function( recid )  {},


    // override
    startup : function()
    {
        this.inherited( arguments );

        var control_actions_store = new d_store_Memory( {
                data: [
                        { id: "any", label: "Any" },
                        { id: "action", label: "Any action" },
                        { id: "COMPUTE_IN_ERROR", label: "COMPUTE_IN_ERROR" },
                        { id: "BOARD_IN_ERROR", label: "BOARD_IN_ERROR" },
                        { id: "CABLE_IN_ERROR", label: "CABLE_IN_ERROR" },
                        { id: "DCA_IN_ERROR", label: "DCA_IN_ERROR" },
                        { id: "END_JOB", label: "END_JOB" },
                        { id: "FREE_COMPUTE_BLOCK", label: "FREE_COMPUTE_BLOCK" },
                        { id: "SOFTWARE_IN_ERROR", label: "SOFTWARE_IN_ERROR" },
                        { id: "BQL_SPARE", label: "BQL_SPARE" },
                        { id: "RACK_IN_ERROR", label: "RACK_IN_ERROR" }
                  ]
            } );

        var control_actions_adapter = new d_data_ObjectStore({
                objectStore: control_actions_store
            });

        this._filterOptionsControlActionSelect.setStore( control_actions_adapter, "any" );


        this._filterOptionsTime.set( "value", "P1D" );

        this._grid.on( "rowClick", d_lang.hitch( this, this._selectRecord ) );


        var DEFAULT_FIELDS = [
                "recid", "time", "severity", "category", "msgid", "block", "location", "jobid", "message"
           ];

        var DEFAULT_VALUE = {
                "field" : DEFAULT_FIELDS
            };

        this._fieldsForm.set( "value", DEFAULT_VALUE );
        this._fieldsForm.on( "submit", d_lang.hitch( this, this._handleFieldsSubmit ) );


        this._filter_options = new ll_FilterOptions( {
                name: "RAS",
                grid: this._grid,
                form: this._filterOptionsForm,
                dropdown: this._filterOptionsDropDown,
                resetButton: this._filterOptionsResetButton,
                formToQuery: _formToQuery
            } );
    },


    // override MonitorActiveMixin
    _notifyActive : function()
    {
        this._loadIfReady();
    },


    _handleFieldsSubmit: function( e )
    {
        e.preventDefault();

        if ( ! this._fieldsForm.isValid() )  return;

        this._updateStructure();

        this._fieldsDropDown.closeDropDown();
    },


    _refresh : function()
    {
        this._dirty = true;
        this._loadIfReady();
    },


    _loadIfReady : function()
    {
        if ( ! this._store )  return;
        if ( ! this._dirty )  return; // Don't have to do anything if clean.
        if ( ! this._isActive() /*MonitorActiveMixin*/ )  return;

        if ( ! this._initialized ) {
            this._updateStructure();

            var query = this._filter_options.calcQuery();
            this._grid.setStore( this._store, query, null );

            this._initialized = true;
            this._dirty = false;

            return;
        }

        this._grid.render();
        this._dirty = false;
    },


    _updateStructure: function()
    {
        var form_obj = this._fieldsForm.get( "value" );

        console.log( module.id + ": new fields=", form_obj );

        var subrow = [];

        d_array.forEach( form_obj.field, d_lang.hitch( this, function( field_id ) {
           subrow.push( _FIELD_ID_TO_CELL[field_id] );
        } ) );

        var view = { rows: subrow };
        var structure = [ view ];

        this._grid.set( "structure", structure );
    },


    _selectRecord : function( ev )
    {
        var item = this._grid.selection.getFirstSelected();

        if ( item == null ) {
            console.log( module.id + ": _selectRecord with no item" );
            return;
        }

        var recid = this._store.getValue( item, "id", "" );

        console.log( module.id + ": selected ", recid );

        this.onEventSelected( recid );
    }

} );

return b_navigator_dijit_RasResults;

} );
