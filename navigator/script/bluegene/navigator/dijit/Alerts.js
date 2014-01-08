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
    "../../BlueGene",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/text!./templates/Alerts.html",
    "module",

    // Used only in template.
    "./TimeInterval",
    "./MultiSelectCodes",
    "./MultiWildcardTextBox",
    "../format",
    "../../dijit/Hideable",
    "../../dijit/OutputFormat",
    "../../dijit/OutputText",
    "dijit/TooltipDialog",
    "dijit/form/Button",
    "dijit/form/DropDownButton",
    "dijit/form/Form",
    "dijit/form/RadioButton",
    "dijit/layout/BorderContainer",
    "dijit/layout/ContentPane",
    "dojox/grid/EnhancedGrid"
],
function(
        l_AbstractTemplatedContainer,
        l_MonitorActiveMixin,
        ll_FilterOptions,
        ll_xlate,
        b_BlueGene,
        d_declare,
        d_lang,
        template,
        module
    )
{

var _INITIAL_FORM_OBJ = { state: "" + b_BlueGene.tealAlertState.OPEN };
var _INITIAL_QUERY_OBJ = { state: "" + b_BlueGene.tealAlertState.OPEN };


var _formToQuery = function( form_obj )
    {
        var query_obj = ll_FilterOptions.formToQueryHelper( form_obj, [ "interval", "location", "locationType", "severity", "state", "urgency" ] );

        if ( form_obj.duplicates === "show" ) {
            query_obj.dup = "T";
        }

        return query_obj;
    };


var b_navigator_dijit_Alerts = d_declare(
        [ l_AbstractTemplatedContainer, l_MonitorActiveMixin ],

{

    templateString : template,

    _can_perform_operations : null, // null === don't know.
    _store : null,

    _initialized : false,
    _dirty : true,

    _operation_in_progress : false,


    // override
    startup : function()
    {
        this.inherited( arguments );


        // Disable sorting for the "Duplicate Of" column.
        var orig_grid_cansort = this._grid.canSort;
        this._grid.canSort = function( inSortInfo ) {
                if ( Math.abs( inSortInfo ) === 9 || Math.abs( inSortInfo ) === 13 ) return false; // The "Dupliate Of" column is the 9th, "Raw Data" is 13th.
                return orig_grid_cansort( arguments );
            };


        this._filterOptionsForm.set( "value", _INITIAL_FORM_OBJ );

        new ll_FilterOptions( {
            name: "Alerts",
            grid: this._grid,
            form: this._filterOptionsForm,
            dropdown: this._filterOptionsDropDownButton,
            formToQuery: _formToQuery,
            resetButton: this._filterOptionsResetButton,
            onChange: d_lang.hitch( this, this._checkUpdateOperationButtonsStatus )
        } );

        this._grid.on( "rowClick", d_lang.hitch( this, this._selectAlert ) );
        this._closeButton.on( "click", d_lang.hitch( this, this._closeAlert ) );
        this._removeButton.on( "click", d_lang.hitch( this, this._removeAlert ) );
        this._opResultDismissButton.on( "click", d_lang.hitch( this, this._dismissResult ) );
    },


    setStore : function( new_store )
    {
        this._store = new_store;
        if ( this._isActive() /*MonitorActiveMixin*/ )  this._notifyActive();
    },


    setUserCanPerformOperations : function( new_value )
    {
        this._can_perform_operations = new_value;

        if ( this._can_perform_operations ) {

            this._actionButtonsContainer.show();

        } else {

            console.log( module.id + ": user isn't administrator so hiding the alert operation buttons." );

            this._actionButtonsContainer.hide();

        }

        if ( this._isActive() /*MonitorActiveMixin*/ )  this._notifyActive();
    },


    setOperationInProgress : function( new_value )
    {
        if ( "success" in new_value ) { // previous operation was successful.

            this._operation_in_progress = false;

            this._opInProgressHideable.hide();
            this._opSuccessFormat.set( "value", { opType: (new_value.opType === "close" ? "Closed" : "Removed"), id: ("" + new_value.id)} );
            this._opSuccessHideable.show();
            this._opResultDismissHideable.show();

            this._refresh();

        } else if ( "error" in new_value ) { // previous operation failed.

            this._operation_in_progress = false;

            this._opInProgressHideable.hide();
            this._opErrorFormat.set( "value", { opType : (new_value.opType === "close" ? "close" : "remove"), message: new_value.error } );
            this._opErrorHideable.show();
            this._opResultDismissHideable.show();

            this._refresh();

        } else {
            // Operation is now in progress.

            this._operation_in_progress = true;

            this._dismissResult();
            this._opInProgressFormat.set( "value", { opType: (new_value.opType === "close" ? "Closing" : "Removing"), id: ("" + new_value.id) } );
            this._opInProgressHideable.show();

        }

        this._checkUpdateOperationButtonsStatus();
    },


    notifyRefresh : function()
    {
        this._refresh();
    },


    onClose : function( id )  {},
    onRemove : function( id )  {},


    // override MonitorActiveMixin
    _notifyActive : function()
    {
        if ( ! this._store )  return; // no store yet.
        if ( this._can_perform_operations === null )  return; // don't know if can perform operations yet.

        if ( ! this._dirty )  return;

        if ( ! this._initialized ) {
            this._grid.setStore( this._store, _INITIAL_QUERY_OBJ, null );
        } else if ( this._dirty ) {
            this._grid.render();
        }

        this._initialized = true;
        this._dirty = false;
    },


    _selectAlert : function()
    {
        this._checkUpdateOperationButtonsStatus();
    },


    _closeAlert : function()
    {
        console.log( module.id + ": Close alert clicked" );

        var item = this._grid.selection.getFirstSelected();

        if ( item == null ) {
            console.log( module.id + ": no item selected" );
            return;
        }

        var id = this._store.getValue( item, "recId" );

        if ( id == null ) {
            console.log( module.id + ": item has no recId" );
            return;
        }

        var res = window.confirm( "Press OK to close alert " + id + "." );

        if ( ! res )  return;

        console.log( module.id + ": Calling onClose with id=", id );

        this.onClose( id );
    },


    _removeAlert : function()
    {
        console.log( module.id + ": Remove alert clicked" );

        var item = this._grid.selection.getFirstSelected();

        if ( item == null ) {
            console.log( module.id + ": no item selected" );
            return;
        }

        var id = this._store.getValue( item, "recId" );

        if ( id == null ) {
            console.log( module.id + ": item has no recId" );
            return;
        }

        var res = window.confirm( "Press OK to remove alert " + id + "." );

        if ( ! res )  return;

        console.log( module.id + ": Calling onRemove with id=", id );

        this.onRemove( id );
    },


    _checkUpdateOperationButtonsStatus : function()
    {
        if ( ! this._can_perform_operations )  return;

        this._closeButton.set( "disabled", this._closeButtonShouldBeDisabled() );
        this._removeButton.set( "disabled", this._removeButtonShouldBeDisabled() );
    },


    _closeButtonShouldBeDisabled : function()
    {
        // Check if operation already in progress.
        if ( this._operation_in_progress ) {
            return true;
        }

        // Check if an item is selected.
        var item = this._grid.selection.getFirstSelected();

        if ( ! item ) {
            return true;
        }

        // Check the item's state.
        var state = this._store.getValue( item, "state" );

        console.log( module.id + ": selected item state=", state );

        if ( state === b_BlueGene.tealAlertState.CLOSED ) {
            return true;
        }

        // Check if is duplicate.
        var duplicateOf = this._store.getValue( item, "duplicateOf" );

        console.log( module.id + ": selected item duplicateOf=", duplicateOf );

        if ( duplicateOf ) {
            return true;
        }

        return false;
    },


    _removeButtonShouldBeDisabled : function()
    {
        // Check if operation already in progress.
        if ( this._operation_in_progress ) {
            return true;
        }

        // Check if an item is selected.
        var item = this._grid.selection.getFirstSelected();

        if ( ! item ) {
            return true;
        }

        // Check the item's state.
        var state = this._store.getValue( item, "state" );

        console.log( module.id + ": selected item state=", state );

        if ( state === b_BlueGene.tealAlertState.OPEN ) {
            return true; // disabled if not closed.
        }

        // Check if is duplicate.
        var duplicateOf = this._store.getValue( item, "duplicateOf" );

        console.log( module.id + ": selected item duplicateOf=", duplicateOf );

        if ( duplicateOf ) {
            return true;
        }

        return false;
    },


    _dismissResult : function()
    {
        this._opSuccessHideable.hide();
        this._opErrorHideable.hide();
        this._opResultDismissHideable.hide();
    },


    _refresh : function()
    {
        if ( ! this._store )  return; // no store yet.
        if ( this._can_perform_operations === null )  return;

        this._grid.selection.clear();

        if ( ! this._isActive() /*MonitorActiveMixin*/ ) {
            this._dirty = true;
            return;
        }

        this._grid.render();

        this._checkUpdateOperationButtonsStatus();
    },
    
    
    _ll_xlate : ll_xlate


} );

return b_navigator_dijit_Alerts;

} );
