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
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module",

    // Used only in template.
    "../MultiWildcardTextBox",
    "../NumberCompareOption",
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
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_dijit_environmentals_AbstractQueryResultsDijit = d_declare(
        [ ll_AbstractTemplatedContainer, ll_MonitorActiveMixin ],

{
    __store : null,
    __filter_options : null,

    __dirty : true,


    setStore : function( new_store )
    {
        this.__store = new_store;
        this.__refresh();
    },


    notifyRefresh : function()
    {
        this.__refresh();
    },


    // override.
    startup : function()
    {
        this.inherited( arguments );

        this.__filter_options = new b_navigator_FilterOptions( {
                name: ("AbstractEnvQRDijit-" + this.id),
                grid: this._grid,
                form: this._foForm,
                dropdown: this._foDropDown,
                resetButton: this._foResetButton,
                formToQuery: d_lang.hitch( this, this.__formToQuery )
            } );

    },


    // override MonitorActiveMixin
    _notifyActive : function()
    {
        this.__loadIfReady();
    },


    // override to return the extra field names to extract from the form.
    _getExtraFieldNames : function() { return []; },


    __formToQuery : function( form_obj )
    {
        var query_obj = b_navigator_FilterOptions.formToQueryHelper( form_obj, [ "interval", "location" ].concat( this._getExtraFieldNames() ) );

        console.log( module.id + ": [" + this.id + "] _formToQuery. form=", form_obj, "query=", query_obj );

        return query_obj;
    },


    __refresh : function()
    {
        this.__dirty = true;
        this.__loadIfReady();
    },


    __loadIfReady : function()
    {
        if ( ! this.__store )  return;
        if ( ! this.__dirty )  return;
        if ( ! this._isActive() /*MonitorActiveMixin*/ )  return;

        this._grid.setStore( this.__store, this.__filter_options.calcQuery(), null );

        this.__dirty = false;
    }

} );

return b_navigator_dijit_environmentals_AbstractQueryResultsDijit;

} );
