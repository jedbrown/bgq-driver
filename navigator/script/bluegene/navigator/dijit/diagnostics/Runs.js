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
    "dojo/_base/lang",
    "dojo/text!./templates/Runs.html",
    "module",

    // Used only in template.
    "../MultiSelectCodes",
    "../TimeInterval",
    "../../format",
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
        ll_AbstractTemplatedContainer,
        ll_MonitorActiveMixin,
        b_navigator_FilterOptions,
        b_navigator_xlate,
        d_declare,
        d_lang,
        template,
        module
    )
{


var _formToQuery = function( form_obj )
    {
        var query_obj = b_navigator_FilterOptions.formToQueryHelper( form_obj, [ "end" ] );

        if ( ! (query_obj.end) )  query_obj.end = "-null"; // end is -null if not set.

        query_obj.status = (form_obj.status || "CFL");

        if ( form_obj.locations !== "" ) {
            // split on , or ' '
            // prefix _DIAGS_ to each location to get block ID.

            query_obj.blocks = "_DIAGS_" + form_obj.locations.split( /, / ).join( ",_DIAGS_" );
        }

        if ( form_obj.block !== "" ) {
            if ( ! ("blocks" in query_obj) ) query_obj.blocks = "";
            query_obj.blocks += form_obj.block;
        }

        return query_obj;
    };


var b_navigator_dijit_diagnostics_Runs = d_declare(
        [ ll_AbstractTemplatedContainer, ll_MonitorActiveMixin ],

{
    templateString : template,

    _store: null,

    _initialized: false,
    _dirty: true,


    setDiagnosticsRunsStore : function( new_store )
    {
        this._store = new_store;

        this._dirty = true;
        this._initialized = false;

        if ( this._isActive() )  this._notifyActive();
    },


    onRunSelected : function( args ) {},


    notifyRefresh : function()
    {
        if ( this._isActive() /*MonitorActiveMixin*/ ) {
            this._grid.render();
            return;
        }

        this._dirty = true;
    },


    // override.
    startup : function()
    {
        this.inherited( arguments );


        new b_navigator_FilterOptions( {
                name: "DiagnosticsRuns",
                grid: this._grid,
                form: this._foForm,
                dropdown: this._foDropDown,
                resetButton: this._resetButton,
                formToQuery: _formToQuery
            } );


        // Disable sorting for the Log File and Targets columns
        var orig_grid_cansort = this._grid.canSort;
        this._grid.canSort = function( inSortInfo ) {
                if ( Math.abs( inSortInfo ) === 4 || Math.abs( inSortInfo ) === 5 ) return false; // The log file and targets are the 4th and 5th columns.
                return orig_grid_cansort( arguments );
            };

        this._grid.on( "rowClick", d_lang.hitch( this, this._selectRun ) );
    },


    // override MonitorActiveMixin
    _notifyActive : function()
    {
        console.log( module.id + ": active" );

        if ( ! this._initialized ) {
            this._grid.setStore( this._store, { status: "CFL", end: "-null" } /* query */, null );
            this._initialized = true;
            this._dirty = false;
            return;
        }

        if ( this._dirty ) {
            this._grid.render();
            this._dirty = false;
            return;
        }
    },


    _selectRun : function()
    {
        var item = this._grid.selection.getFirstSelected();

        if ( item == null ) {
            console.log( module.id + ": selectRun with no item" );
            return;
        }

        var run_id = this._store.getValue( item, "runId", "" );
        var run_start = this._store.getValue( item, "start", "" );

        console.log( module.id + ": selected run " + run_id );

        this.onRunSelected( { runId: run_id, runStart: run_start } );
    },
    
    
    _b_navigator_xlate : b_navigator_xlate

} );

return b_navigator_dijit_diagnostics_Runs;

} );
