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
    "dojo/text!./templates/Locations.html",
    "module",

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
        d_lang,
        template,
        module
    )
{


var _formToQuery = function( form_obj )
    {
        var query_obj = b_navigator_FilterOptions.formToQueryHelper( form_obj, [ "hardwareStatus", "interval", "location" ] );

        if ( form_obj.replace !== "Any" )  query_obj.replace = form_obj.replace;

        return query_obj;
    };


var b_navigator_dijit_diagnostics_Locations = d_declare(
        [ ll_AbstractTemplatedContainer, ll_MonitorActiveMixin ],

{

    templateString : template,

    _store: null,

    _initialized: false,
    _dirty: true,


    setDiagnosticsLocationsStore : function( new_store )
    {
        this._store = new_store;

        this._initialized = false;
        this._dirty = true;

        if ( this._isActive() /*MonitorActiveMixin*/ ) {
            this._notifyActive();
        }
    },


    onLocationSelected : function( location ) {},


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

        this._grid.on( "rowClick", d_lang.hitch( this, this._selectLocation ) );

        new b_navigator_FilterOptions( {
                name: "DiagnosticsLocations",
                grid: this._grid,
                form: this._foForm,
                dropdown: this._foDropDown,
                resetButton: this._resetButton,
                formToQuery: _formToQuery
            } );

    },


    // override MonitorActiveMixin
    _notifyActive : function()
    {
        if ( ! this._store )  return;

        if ( ! this._initialized ) {
            this._grid.setStore( this._store, {} /* query */, null );
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


    _selectLocation : function( ev )
    {
        var item = this._grid.selection.getFirstSelected();

        if ( item == null ) {
            console.log( module.id + ": selectLocation with no item" );
            return;
        }

        var location = this._store.getValue( item, "location", "" );

        console.log( module.id + ": selected location " + location );

        this.onLocationSelected( location );
    },
    
    
    _b_navigator_xlate : b_navigator_xlate

} );

return b_navigator_dijit_diagnostics_Locations;

} );
