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
    "dojo/when",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/text!./templates/ServiceActions.html",
    "module",

    // Used only in template.
    "./MultiSelectCodes",
    "./MultiWildcardTextBox",
    "./TimeInterval",
    "../format",
    "../../dijit/Hideable",
    "../../dijit/OutputText",
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
        d_when,
        d_declare,
        d_lang,
        template,
        module
    )
{


var _formToQuery = function( form_obj )
    {
        var query_obj = ll_FilterOptions.formToQueryHelper( form_obj, [ "action", "endTime", "status", "prepareTime", "location", "prepareUser", "endUser" ] );

        return query_obj;
    };


var b_navigator_dijit_ServiceActions = d_declare(
        [ l_AbstractTemplatedContainer, l_MonitorActiveMixin ],

{

    templateString : template,

    _store : null,

    _user_allowed_operations : false,

    _end_service_action_fn : null,
    _close_service_action_fn : null,

    _initialized : false,
    _dirty : true,

    _filter_options: null,

    _operation_promise : null,


    setServiceActionsStore : function( new_store )
    {
        this._store = new_store;
        this._initialized = false;
        this._refresh();
    },

    setUserAllowedOperations : function( operations_allowed )
    {
        this._user_allowed_operations = operations_allowed;

        if ( this._user_allowed_operations ) {

            this._actionButtonsContainer.show();

        } else {

            console.log( module.id + ": Hiding action buttons." );

            this._actionButtonsContainer.hide();

        }
    },

    setEndServiceActionFn : function( fn )
    {
        this._end_service_action_fn = fn;
    },

    setCloseServiceActionFn : function( fn )
    {
        this._close_service_action_fn = fn;
    },

    notifyRefresh : function()
    {
        this._refresh();
    },

    onPrepareServiceAction : function()  {},


    // override.
    startup : function()
    {
        this.inherited( arguments );

        this._foPrepared.set( "value", "P7D" ); // default filter is 7 days.

        this._filter_options = new ll_FilterOptions( {
                name: "ServiceActions",
                grid: this._grid,
                form: this._foForm,
                dropdown: this._foDropDown,
                resetButton: this._foResetButton,
                formToQuery: _formToQuery,
                onChange: d_lang.hitch( this, this._checkUpdateButtonStates )
            } );

        this._prepareButton.on( "click", d_lang.hitch( this, function() { this.onPrepareServiceAction(); } ) );
        this._endButton.on( "click", d_lang.hitch( this, this._endButtonClicked ) );
        this._closeButton.on( "click", d_lang.hitch( this, this._closeButtonClicked ) );
        this._grid.on( "rowClick", d_lang.hitch( this, this._selectRow ) );
    },


    // override
    destroy : function()
    {
        if ( this._operation_promise ) {
            this._operation_promise.cancel();
            this._operation_promise = null;
        }

        this.inherited( arguments );
    },


    // override MonitorActiveMixin
    _notifyActive : function()
    {
        if ( ! this._store )  return;
        if ( ! this._dirty )  return;

        if ( ! this._initialized ) {
            var query = this._filter_options.calcQuery();
            this._grid.setStore( this._store, query, null );
            this._initialized = true;
        } else {
            this._grid.render();
        }

        this._dirty = false;
    },


    _refresh : function()
    {
        this._grid.selection.clear();
        this._checkUpdateButtonStates();

        if ( ! this._isActive() ) {
            this._dirty = true;
            return;
        }

        if ( ! this._store )  return;

        if ( ! this._initialized ) {
            var query = this._filter_options.calcQuery();
            this._grid.setStore( this._store, query, null );
            this._initialized = true;
        } else {
            this._grid.render();
        }

        if ( this._operation_promise === null ) {
            this._progressText.set( "value", null );
        }
    },


    _endButtonClicked : function()
    {
        var item = this._grid.selection.getFirstSelected();

        if ( item == null ) {
            console.log( module.id + ": _endButtonClicked with no item" );
            return;
        }

        var id = this._store.getValue( item, "id", "" );
        var location = this._store.getValue( item, "location", "" );


        var res = window.confirm( "Press OK to end service action " + id + " on " + location + "." );

        if ( ! res ) {
            return;
        }


        console.log( module.id + ": requested to end service action", id );

        this._operation_promise = this._end_service_action_fn( id );

        this._checkUpdateButtonStates();

        d_when(
                this._operation_promise,
                d_lang.hitch( this, this._submitEndComplete, id, location ),
                d_lang.hitch( this, this._endFailed, id, location )
            );

        this._progressText.set( "value", "Starting end service action " + id + " on " + location + "..." );
    },


    _submitEndComplete : function( id, location, result )
    {
        console.log( module.id + ": submit end complete. result=", result );

        this._operation_promise = null;

        this._refresh();

        this._checkUpdateButtonStates();

        this._progressText.set( "value", "End of service action " + id + " on " + location + " started." );
    },


    _endFailed : function( id, location, error )
    {
        console.log( module.id + ": end service action failed. error=", error );

        this._operation_promise = null;

        this._refresh();

        this._checkUpdateButtonStates();

        this._progressText.set( "value", "Failed to end service action " + id + " on " + location + ". The error is " + error.message + "." );
    },


    _closeButtonClicked : function()
    {
        console.log( module.id + ": close service action button pressed." );
        if ( this._operation_promise )  return; // operation already in progress. Shouldn't happen because button should be disabled!


        var item = this._grid.selection.getFirstSelected();

        if ( item == null ) {
            console.log( module.id + ": _closeButtonClicked with no item" );
            return;
        }

        var id = this._store.getValue( item, "id", "" );
        var location = this._store.getValue( item, "location", "" );


        var res = window.confirm( "Press OK to force service action " + id + " on " + location + " closed." );

        if ( ! res ) {
            return;
        }


        console.log( module.id + ": requested to force close service action", id );

        this._operation_promise = this._close_service_action_fn( id );

        this._checkUpdateButtonStates();

        d_when(
                this._operation_promise,
                d_lang.hitch( this, this._submitCloseComplete, id, location ),
                d_lang.hitch( this, this._closeFailed, id, location )
            );

        this._progressText.set( "value", "Starting force service action " + id + " on " + location + " closed..." );
    },


    _submitCloseComplete : function( id, location, result )
    {
        console.log( module.id + ": submit close complete. result=", result );

        this._operation_promise = null;

        this._refresh();

        this._checkUpdateButtonStates();

        this._progressText.set( "value", "Forced service action " + id + " on " + location + " closed." );
    },


    _closeFailed : function( id, location, result )
    {
        console.log( module.id + ": close service action failed. error=", error );

        this._operation_promise = null;

        this._refresh();

        this._checkUpdateButtonStates();

        this._progressText.set( "value", "Failed to force service action " + id + " on " + location + " closed. The error is " + error.message + "." );
    },


    _selectRow : function()
    {
        this._checkUpdateButtonStates();
    },


    _checkUpdateButtonStates : function()
    {
        if ( ! this._user_allowed_operations )  return;

        this._checkUpdateEndButtonState();
        this._checkUpdateCloseButtonState();
    },


    _checkUpdateEndButtonState : function()
    {
        if ( this._operation_promise ) {
            // disabled because operation already in progress.
            this._endButton.set( "disabled", true );
            return;
        }

        var item = this._grid.selection.getFirstSelected();
        if ( ! item ) {
            // disabled because none selected.
            this._endButton.set( "disabled", true );
            return;
        }

        var status = this._store.getValue( item, "status", "" );

        if ( status !== "P" ) {
            // Disabled because not prepared.
            this._endButton.set( "disabled", true );
            return;
        }

        this._endButton.set( "disabled", false );
    },


    _checkUpdateCloseButtonState : function()
    {
        if ( this._operation_promise ) {
            // disabled because operation already in progress.
            this._closeButton.set( "disabled", true );
            return;
        }

        var item = this._grid.selection.getFirstSelected();
        if ( ! item ) {
            // disabled because none selected.
            this._closeButton.set( "disabled", true );
            return;
        }

        var status = this._store.getValue( item, "status", "" );

        if ( status !== "E" ) {
            // Disabled because not prepared.
            this._closeButton.set( "disabled", true );
            return;
        }

        this._closeButton.set( "disabled", false );
    },


    _ll_xlate : ll_xlate

} );

return b_navigator_dijit_ServiceActions;

} );
