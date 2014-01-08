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
    "dojo/dom-construct",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dijit/form/MultiSelect",
    "dojo/text!./templates/HardwareReplacements.html",
    "module",

    // Used only in template.
    "./MultiWildcardTextBox",
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
        d_construct,
        d_declare,
        d_lang,
        j_form_MultiSelect,
        template,
        module
    )
{


var _formToQuery = function( form_obj )
    {
        var query_obj = ll_FilterOptions.formToQueryHelper( form_obj, [ "interval", "ecid", "location", "serialnumber" ] );

        var NUMBER_OF_TYPES = 12;

        if ( form_obj.types.length !== 0 && form_obj.types.length !== NUMBER_OF_TYPES )  { query_obj.types = form_obj.types.join( "" ); }

        return query_obj;
    };


var b_navigator_dijit_HardwareReplacements = d_declare(
        [ l_AbstractTemplatedContainer, l_MonitorActiveMixin ],

{

    templateString : template,

    _store : null,

    _initialized : false,
    _dirty : true,


    // override
    startup : function()
    {
        this.inherited( arguments );


        d_construct.create( "option", { innerHTML: "Midplane", value: "M" }, this._filterOptionsTypes );
        d_construct.create( "option", { innerHTML: "Node Board", value: "B" }, this._filterOptionsTypes );
        d_construct.create( "option", { innerHTML: "Node Board DCA", value: "A" }, this._filterOptionsTypes );
        d_construct.create( "option", { innerHTML: "Node", value: "N" }, this._filterOptionsTypes );
        d_construct.create( "option", { innerHTML: "I/O Node", value: "O" }, this._filterOptionsTypes );
        d_construct.create( "option", { innerHTML: "I/O Rack", value: "R" }, this._filterOptionsTypes );
        d_construct.create( "option", { innerHTML: "I/O Drawer", value: "D" }, this._filterOptionsTypes );
        d_construct.create( "option", { innerHTML: "Service Card", value: "S" }, this._filterOptionsTypes );
        d_construct.create( "option", { innerHTML: "Clock Card", value: "C" }, this._filterOptionsTypes );
        d_construct.create( "option", { innerHTML: "Link Chip", value: "L" }, this._filterOptionsTypes );
        d_construct.create( "option", { innerHTML: "I/O Link Chip", value: "H" }, this._filterOptionsTypes );
        d_construct.create( "option", { innerHTML: "Bulk Power", value: "P" }, this._filterOptionsTypes );

        new j_form_MultiSelect(
                { name: 'types', size: 4 },
                this._filterOptionsTypes
            );


        new ll_FilterOptions( {
                name : "HardwareReplacements",
                grid : this._grid,
                form : this._filterOptionsForm,
                dropdown : this._filterOptionsDropDownButton,
                resetButton: this._filterOptionsResetButton,
                formToQuery : _formToQuery
            } );

    },


    setStore : function( new_store )
    {
        this._store = new_store;
        this._dirty = true;
        this._refresh();
    },


    notifyRefresh : function()
    {
        this._dirty = true;
        this._refresh();
    },


    // override MonitorActiveMixin
    _notifyActive : function()
    {
        console.log( module.id + ": now active." );
        this._refresh();
    },

    _refresh : function()
    {
        if ( ! this._store )  return;

        if ( ! this._isActive() /*MonitorActiveMixin*/ ) {
            this._dirty = true;
            return;
        }

        if ( ! this._dirty )  return;

        this._dirty = false;

        if ( ! this._initialized ) {
            this._initialized = true;

            var query = {};
            this._grid.setStore( this._store, query, null );

            return;
        }

        this._grid.render();
    }

} );

return b_navigator_dijit_HardwareReplacements;

} );
