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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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
    "dijit/_Widget",
    "dijit/_TemplatedMixin",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/dom-construct",
    "dijit/form/CheckBox"
],
function(
        j__Widget,
        j__TemplatedMixin,
        d_array,
        d_declare,
        d_lang,
        d_c,
        j_f_CheckBox
    )
{


var b_navigator_dijit_MultiSelectCodes = d_declare(
        [ j__Widget, j__TemplatedMixin ],

/** @lends bluegene^navigator^dijit^MultiSelectCodes# */
{
    /** Property, the name for the value in the form.
     *  @type string
     */
    name : null,

    /** Property, the possible values for input.
     *  Either:
     *  <ul>
     *  <li>An object where the keys are the codes, and the values are the labels.
     *  Optionally, contains an _order member that's an array of codes in the order that the codes should appear in the inputs.
     *  </li>
     *  <li>
     *  An array of names which is the values and order from the map property. (Use when should not display all codes.)
     *  </li>
     *  </ul>
     *  @type object|array
     */
    values : null,

    /** Property, map of keys to labels.
     *  If <i>values</i> is an array, this is the map of codes -> labels.
     *  @type object
     */
    map : null,


    /** Property, the current value. A string containing the values that are selected.
     *  @type string
     */
    value: "",

    templateString : "<div style='display: inline;' dojoAttachPoint='cbsNode'></div>",

    _valToCb : null, // {} code -> CheckBox dijit


    /** @constructs
     *
     *  @class Input for a set of codes, where a code is turned on by checking next to the label.
     *  BGWS has several query parameters where the value is a set of codes in a string.
     */
    constructor : function( params, srcNodeRef )
    {
        this._valToCb = {};
    },


    postCreate: function()
    {
        this.inherited( arguments );

        var values_ord = this.values._order || this.values;
        var map = this.map || this.values;

        d_array.forEach( values_ord, d_lang.hitch( this, function( value ) {

            var cb_id = this.cbsNode.id + "-cb-" + value;

            var cb_dij = new j_f_CheckBox( { value: value, checked: true, id: cb_id } );
            cb_dij.placeAt( this.cbsNode );
            cb_dij.startup();

            d_c.create( "label", { innerHTML: map[ value ], "for": cb_id, style: "margin-right: 1em;" }, this.cbsNode );

            this._valToCb[value] = cb_dij;

        } ) );
    },


    _getValueAttr : function()
    {
        var all_checked = true;

        var codes = [];

        for ( val in this._valToCb ) {

            if ( this._valToCb[val].get( "checked" ) ) {
                codes.push( val );
            } else {
                all_checked = false;
            }

        }

        if ( (codes.length === 0) || all_checked )  return "";
        return codes.join( "" );
    },


    _setValueAttr : function( val )
    {
        var codes = val.split( "" );

        if ( codes.length === 0 ) {
            for ( val in this._valToCb ) {
                this._valToCb[val].set( "checked", true );
            }
            return;
        }

        var code_map = {};

        d_array.forEach( codes, function( code ) {
            code_map[code] = true;
        } );

        for ( val in this._valToCb ) {
            if ( code_map[val] )  this._valToCb[val].set( "checked", true );
            else this._valToCb[val].set( "checked", false );
        }
    }

} );


return b_navigator_dijit_MultiSelectCodes;

} );
