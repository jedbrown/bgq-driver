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
    "dijit/_WidgetBase",
    "dijit/_TemplatedMixin",
    "dijit/_WidgetsInTemplateMixin",
    "dojo/_base/declare",
    "dojo/text!./templates/MultiWildcardTextBox.html",
    "module",

    // Used in template.
    "dijit/Tooltip",
    "dijit/form/TextBox"
],
function(
        j__WidgetBase,
        j__TemplatedMixin,
        j__WidgetsInTemplateMixin,
        d_declare,
        template,
        module
    )
{


var b_navigator_dijit_MultiWildcardTextBox = d_declare(
        [ j__WidgetBase, j__TemplatedMixin, j__WidgetsInTemplateMixin ],

/** @lends bluegene^navigator^dijit^MultiWildcardTextBox# */
{
    /** Property, the name for the value in the form.
     *  @type string
     */
    name : null,

    /** Property, the current value.
     *  @type string
     */
    value: "",

    placeHolder: "",


    templateString : template,


    /** @constructs */
    constructor : function()
    {
        // Nothing to do.
    },


    // override
    startup : function()
    {
        this.inherited( arguments );

        this._input.set( "placeHolder", this.placeHolder );
    },


    _getValueAttr : function()
    {
        return this._input.get( "value" );
    },


    _setValueAttr : function( val )
    {
        this._input.set( "value", val );
    }

} );


return b_navigator_dijit_MultiWildcardTextBox;

} );
