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
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dijit/_TemplatedMixin",
    "dijit/_Widget",
    "dijit/_WidgetsInTemplateMixin",
    "dijit/form/NumberTextBox", // Used in template.
    "dijit/form/Select", // Used in template.
    "dojo/text!./templates/NumberCompareOption.html"
],
function(
        d_declare,
        d_lang,
        j__TemplatedMixin,
        j__Widget,
        j__WidgetsInTemplateMixin,
        j_form_NumberTextBox,
        j_form_Select,
        template
    )
{


var _OPTIONS = [
        { value: "ge", label: "greater than" },
        { value: "le", label: "less than" }
    ];

var _VALUE_RE = /(..)(.*)/;


var b_navigator_dijit_NumberCompareOption = d_declare(
        [ j__Widget, j__TemplatedMixin, j__WidgetsInTemplateMixin ],

/** @lends bluegene^navigator^dijit^NumberCompareOption# */
{
    /** Label for the input to be displayed.
     *  @type string
     */
    label : "",

    /** Name in form.
     *  @type string
     */
    name : "",

    /** Current value.
     *  Format is like "(ge|le)###", "" if not set.
     *  @type string
     *  @default ""
     */
    value : "",

    /** Only allow integers.
     *  If true, the value must be an integer.
     *  @type Boolean
     *  @default false
     */
    integerOnly : false,


    templateString : template,


    /** @class Widget that can be used in Filter Options for BGWS number comparison parameters.
     *  @constructs
     */
    constructor : function() { /* Nothing to do */ },


    // override
    postCreate : function()
    {
        this.inherited( arguments );

        this._cmp_dij.addOption( d_lang.clone( _OPTIONS ) ); // clone is important here. The dijit/form/Select must keep ref and modify it.

        if ( this.value !== "" ) {
            console.log( "postCreate: value=", this.value );
            var m = _VALUE_RE.exec( this.value );
            this._cmp_dij.set( "value", m[1] );
        }

        if ( this.integerOnly ) {
            this._val_dij.set( "constraints", { places: 0 } );
            this._val_dij.set( "placeHolder", "1" );
        } else {
            this._val_dij.set( "placeHolder", "1.0" );
        }
    },


    _getValueAttr : function()
    {
        // console.log( "GET: name=", this.name, " cmp=", this._cmp_dij.get( "value" ), " val=", this._val_dij.get( "value" ) );

        if ( isNaN( this._val_dij.get( "value" ) ) )  return "";

        return (this._cmp_dij.get( "value" ) + this._val_dij.get( "value" ));
    },


    _setValueAttr : function( new_val )
    {
        // console.log( "SET1: name=" + this.name + " new_val=", new_val );

        this._set( "value", new_val );

        if ( new_val === "" || ((typeof new_val !== "string") && isNaN( new_val )) ) {
            // console.log( "SET2: name=" + this.name + " default" );
            this._cmp_dij.set( "value", _OPTIONS[0].value );
            this._val_dij.set( "value", "" );
            return;
        }

        var m = _VALUE_RE.exec( new_val );

        // console.log( "SET3: name=" + this.name + " new_val=", new_val, " m=", m, " cmp->", m[1], " val->", +(m[2]) );

        this._cmp_dij.set( "value", m[1] );
        this._val_dij.set( "value", +(m[2]) );

        // console.log( "SET4: name=" + this.name + " cmp=", this._cmp_dij.get( "value" ), " val=", this._val_dij.get( "value" ) );
    }

} );

return b_navigator_dijit_NumberCompareOption;

} );
