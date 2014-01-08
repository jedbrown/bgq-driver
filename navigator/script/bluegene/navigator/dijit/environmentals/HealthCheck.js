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
    "../AbstractTemplatedContainer",
    "../MonitorActiveMixin",
    "../../FilterOptions",
    "../../xlate",
    "dojo/_base/declare",
    "dojo/data/ObjectStore",
    "dojo/store/Memory",
    "dojo/text!./templates/HealthCheck.html",

    // Used only in template.
    "../MultiSelectCodes",
    "../MultiWildcardTextBox",
    "../TimeInterval",
    "../../format",
    "dijit/TooltipDialog",
    "dijit/form/Button",
    "dijit/form/DropDownButton",
    "dijit/form/Form",
    "dijit/form/Select",
    "dijit/layout/BorderContainer",
    "dijit/layout/ContentPane",
    "dojox/grid/EnhancedGrid"
],
function(
        ll_AbstractTemplatedContainer,
        ll_MonitorActiveMixin,
        b_navigator_FilterOptions,
        b_navigator_xlate,
        d_declare,
        d_data_ObjectStore,
        d_store_Memory,
        template
    )
{


var _formToQuery = function( form_obj )
    {
        var query_obj = b_navigator_FilterOptions.formToQueryHelper( form_obj, [ "interval", "severity", "location" ] );

        query_obj.msgid = "00061*";

        if ( form_obj.category != "Any" )  query_obj.category = form_obj.category;

        return query_obj;
    };


var b_navigator_dijit_environmentals_HealthCheck = d_declare(
        [ ll_AbstractTemplatedContainer, ll_MonitorActiveMixin ],

{

    templateString : template,

    _store : null,

    _initialized: false,
    _dirty: true,

    _filter_options: null,


    setStore : function( new_store )
    {
        this._store = new_store;
        this._initialized = false;
        this._refresh();
    },


    notifyRefresh : function()
    {
        this._refresh();
    },


    // override.
    startup : function()
    {
        this.inherited( arguments );

        this._foInterval.set( "value", "P1D" );

        var category_store = new d_store_Memory( {
                data: [
                        { id: "Any", label: "Any" },
                        { id: "Node_Board", label: "Node Board" },
                        { id: "Service_Card", label: "Service Card" },
                        { id: "AC_TO_DC_PWR", label: "AC_TO_DC_PWR" },
                        { id: "IO_Board", label: "I/O Board" },
                        { id: "Coolant_Monitor", label: "Coolant Monitor" },
                        { id: "DCA", label: "DCA" },
                        { id: "Optical_Module", label: "Optical Module" }
                    ]
            } );

        var category_adapter = new d_data_ObjectStore({
                objectStore: category_store
            });

        this._foCategory.setStore( category_adapter, "Any" );


        this._filter_options = new b_navigator_FilterOptions( {
                name: "HealthCheck",
                grid: this._grid,
                form: this._foForm,
                dropdown: this._foDropDown,
                resetButton: this._foResetButton,
                formToQuery: _formToQuery
            } );

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

            var query = this._filter_options.calcQuery();

            this._grid.setStore( this._store, query, null );

            this._initialized = true;

        }

        this._dirty = false;
    },


    _b_navigator_xlate : b_navigator_xlate

} );

return b_navigator_dijit_environmentals_HealthCheck;

} );
