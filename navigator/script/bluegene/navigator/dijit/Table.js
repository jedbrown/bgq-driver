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
    "dojo/dom-construct",
    "dojo/when",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/store/Memory",
    "dijit/_WidgetBase",
    "dijit/_TemplatedMixin",
    "dijit/form/CheckBox",
    "dojo/text!./templates/Table.html",
    "module"
],
function(
        d_construct,
        d_when,
        d_array,
        d_declare,
        d_lang,
        d_store_Memory,
        j__WidgetBase,
        j__TemplatedMixin,
        j_form_Checkbox,
        template,
        module
    )
{

var b_navigator_dijit_Table = d_declare(
        [ j__WidgetBase, j__TemplatedMixin ],

/** @lends bluegene^navigator^dijit^Table# */
{

    templateString : template,


    /** Property; text to display when the store has no values.
     *
     *  @default ""
     *  @type String
     */
    emptyText : "",


    /** Property; the layout for the table.
     *
     * <p>
     * Contents are:
     * <ul>
     * <li> columns: an array of column definitions. The order of the column definitions is the order of the columns in the table.
     * </ul>
     *
     * <p>
     * Column definition: An object, where each object has
     * <ul>
     * <li> title: Text displayed in the header for the column. If not present then displays nothing in the header.
     * <li> field: The field from the store object to use.
     *          Optional, if set this field from the object is passed to as the value, otherwise the object is used as the value for the property.
     *          Not used if calcProperties is set.
     * <li> dijitType: A dijit class. An instance of this class will be created for the cell.
     * <li> dijitProperties : An object containing extra properties for the cell. Optional, if not set then no extra properties are set. Not used if calcProperties is set.
     * <li> calcProperties: A function, called to calculate the properties that are set on the dijit.
     *          The function is called for each row and passed the object from the store for that row.
     *          The default properties are
     *            a) the properties from dijitProperties for the cell, plus
     *            b) { value : o[field] }, where o is the object and 'field' is the field for the cell if field is set or { value : o } if field is not set.
     * <li> createCb: A function, called after the dijit is created. The arguments are the dijit and the object for the row.
     * </ul>
     *
     * @default null
     * @type Object
     */
    layout : null,


    /** Property; Selection mode.
     *
     * <p>
     * Values are
     * <ul>
     * <li>none : No selection.</li>
     * <li>indirect : Checkboxes at front of each row to select the row.</li>
     * </ul>
     *
     * @default null
     * @type String
     */
    selection : "none",


    /** Property; If true, a header with the field titles will be shown, otherwise the header is not displayed.
     *
     *  @default true
     *  @type Boolean
     */
    showHeader : true,


    /** Property; The current value.
     *
     *  @default []
     *  @type String[]
     */
    value : null,


    /** Property; this dijit gets created and put in the footer.
     *
     *  If null, no dijit is created, otherwise the widget is created with the supplied footerProperties.
     *
     *  @default null
     *  @type Dijit
     */
    footerDijitType : null,

    /** Property; the initial properties for the footer dijit.
     *
     *  @default {}
     *  @type Object
     */
    footerProperties : null,

    // This is here so that gets array when set( "value" ) in a dijit.form.Form.
    multiple: true,

    _store : null,
    _query : null,

    _promise : null,

    _dijits : null,
    _selection_cbs : null,

    _column_count : 0,



    /**
     *  @class Widget for a simple table.
     *
     *  @constructs
     */
    constructor : function()
    {
        this.value = [];
        this.footerProperties = {};

        this._dijits = [];
        this._selection_cbs = [];
    },


    /** Set the store from which the data for the table will be retrieved. */
    setStore : function(
            /** dojo/store */ new_store,
            /** Object */ new_query
        )
    {
        this._store = new_store;
        this._query = new_query;

        this._draw();
    },


    // Override.
    destroy : function()
    {
        this.inherited( arguments );

        if ( this._promise ) {
            this._promise.cancel();
            this._promise = null;
        }

        this._destroyDijits();
    },


    _setLayoutAttr : function( new_layout )
    {
        console.log( module.id + ": ["+this.id+"] setting layout=", new_layout );
        this.layout = new_layout;

        d_construct.empty( this._thead );

        this._column_count = (this.layout.columns.length + (this.selection === "indirect" ? 1 : 0));

        if ( this.showHeader ) {

            if ( this.title ) {
                var tr_elem = d_construct.create( "tr", {}, this._thead );
                d_construct.create( "th", { innerHTML: this.title, colspan: this._column_count }, tr_elem );
            }

            var tr_elem = d_construct.create( "tr", {}, this._thead );


            if ( this.selection === "indirect" ) {
                d_construct.create( "th", {}, tr_elem );
            }


            d_array.forEach( this.layout.columns, function( col ) {
                var title_text = (col.title ? col.title : "");
                d_construct.create( "th", { innerHTML: title_text }, tr_elem );
            } );

        }

        if ( this.footerDijitType ) {
            console.log( module.id + ": [" + this.id + "] Have footerDijitType..." );

            var footer_row = d_construct.create( "tr", {}, this._tfoot );

            var footer_td = d_construct.create( "th", { colspan: this._column_count }, footer_row );

            console.log( module.id + ": [" + this.id + "] Creating footerDijit..." );

            this.footerDijit = new this.footerDijitType( this.footerProperties, footer_td );

            console.log( module.id + ": [" + this.id + "] Calling startup..." );

            this.footerDijit.startup();

            console.log( module.id + ": [" + this.id + "] Done!" );
        }

        this._draw();
    },


    _setDataAttr : function( args )
    {
        // args is like { data: data /*array*/, idProperty: "id" /*string*/ }

        var store = new d_store_Memory( args );
        var query = {};
        this.setStore( store, query );
    },


    _draw : function()
    {
        this._clearTable();


        if ( this._promise ) {
            this._promise.cancel();
            this._promise = null;
        }


        if ( ! this.layout )  return;
        if ( ! this._store ) {

            var tr_elem = d_construct.create( "tr", {}, this._tbody );
            d_construct.create( "td", { colspan: this._column_count, innerHTML: "Loading..." }, tr_elem );

            return;
        }

        this._promise = this._store.query( this._query );


        var tr_elem = d_construct.create( "tr", {}, this._tbody );
        d_construct.create( "td", { colspan: this._column_count, innerHTML: "Loading..." }, tr_elem );


        d_when(
                this._promise,
                d_lang.hitch( this, this._gotData ),
                d_lang.hitch( this, this._loadError )
            );
    },


    _destroyDijits : function()
    {
        d_array.forEach( this._dijits, function( j )  { j.destroyRecursive(); } );
        this._dijits = [];

        d_array.forEach( this._selection_cbs, function( j )  { j.destroyRecursive(); } );
        this._selection_cbs = [];
    },


    _clearTable : function()
    {
        // console.log( module.id + ": ["+this.id+"] Destroying", this._dijits.length, "dijits" );
        this._destroyDijits();

        d_construct.empty( this._tbody );
    },


    _gotData : function( data )
    {
        this._promise = null;

        this._clearTable();

        if ( data.length === 0 ) {
            var tr_elem = d_construct.create( "tr", {}, this._tbody );
            d_construct.create( "td", { colspan: this._column_count, innerHTML: this.emptyText, style: "font-style: italic;" }, tr_elem );

            return;
        }

        data.forEach( d_lang.hitch( this, function( o ) {
            this._populateRow( o );
        } ) );
    },


    _loadError : function( data )
    {
        this._clearTable();

        var tr_elem = d_construct.create( "tr", {}, this._tbody );
        d_construct.create( "td", { colspan: this._column_count, innerHTML: "Error loading" }, tr_elem );
    },


    _populateRow : function( o )
    {
        var tr_elem = d_construct.create( "tr", {}, this._tbody );

        if ( this.selection === "indirect" ) {
            var td_elem = d_construct.create( "td", {}, tr_elem );

            var cb_dij = new j_form_Checkbox( { value: o[this._store.idProperty] } );

            cb_dij.placeAt( td_elem );
            cb_dij.startup();

            cb_dij.on( "change", d_lang.hitch( this, this._cbChanged ) );

            this._selection_cbs.push( cb_dij );
        }

        d_array.forEach( this.layout.columns, d_lang.hitch( this, function( col ) {
            this._createField( col, o, tr_elem );
        } ) );
    },


    _createField : function( column_info, o, tr_elem )
    {
        var dijitType = column_info.dijitType;


        var properties;

        if ( column_info.calcProperties )  properties = column_info.calcProperties( o );
        else {
            var value = (column_info.field ? o[column_info.field] : o);
            properties = { value: value };

            if ( column_info.dijitProperties ) {
                properties = d_lang.mixin( properties, column_info.dijitProperties );
            }
        }


        var cell_dijit = new dijitType(
                properties,
                d_construct.create( "td", {}, tr_elem )
            );

        this._dijits.push( cell_dijit );

        cell_dijit.startup();

        if ( column_info.createCb )  column_info.createCb( cell_dijit, o );
    },


    _cbChanged : function()
    {
        var new_value = this._calcValue();

        this._set( "value", new_value );
    },


    _calcValue : function()
    {
        var ret = [];

        d_array.forEach( this._selection_cbs, function( cb ) { if ( cb.checked ) ret.push( cb.value ); } );

        return ret;
    },


    _setValueAttr : function( new_value )
    {
        // console.log( module.id + ": [" + this.id + "] setting value=", new_value );

        var vals = {};
        d_array.forEach( new_value, function( val ) { vals[val] = 1; } );

        d_array.forEach( this._selection_cbs, function( cb ) { cb.set( "checked", (cb.value in vals) ); } );

        this._set( "value", new_value );
    }

} );

return b_navigator_dijit_Table;

} );
