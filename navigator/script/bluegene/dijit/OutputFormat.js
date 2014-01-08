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
    "dijit/_WidgetBase",
    "dojo/dom-construct",
    "dojo/string",
    "dojo/_base/declare",
    "dojo/_base/window"
],
function(
        j__WidgetBase,
        d_construct,
        d_string,
        d_declare,
        d_window
    )
{


var b_dijit_OutputFormat = d_declare(
        [ j__WidgetBase ],

/** @lends bluegene^dijit^OutputFormat# */
{
    /** Property; The format string, format is like the template parameter of "dojo/string".substitute().
     *
     *  If this widget is created declaratively, will get the format from the element's contents
     *  if it's not set in data-dojo-props.
     *
     *  @type String
     */
    format : null,

    /** Property; This text is displayed if the value is null.
     *  @type String
     */
    nullText : null,

    /** Property; The value to format.
     *
     *  The value gets passed to the format function.
     *
     *  @type Object|Array|null
     */
    value : null,

    /** Property; Object to pass as thisObject to substitute.
     */
    thisObject : null,


    /**
     * @class Widget that uses a format string to format a value or values onto the page.
     *
     * <p>
     * Use the value property to set the current value.
     * The text displayed is the formatted value, as formatted by the format template.
     * The formatted value has any HTML elements escaped.
     *
     * <p>
     * The type of the element for this dijit is the same as srcNodeRef.
     * If no srcNodeRef then the type of the element is a SPAN.
     *
     * @constructs
     */
    constructor : function( /** Object? */ params, /** DomNode | String */ srcNodeRef )
    {
        // Nothing to do.
    },


    // override
    buildRendering : function()
    {
        // buildRendering creates 'div' by default. OutputFormat should be a span.
        if ( ! this.domNode ) {
            this.domNode = this.srcNodeRef || d_construct.create( 'span' );
        }

        this.inherited( arguments );
    },

    // override
    postMixInProperties : function()
    {
        this.inherited( arguments );

        if ( this.srcNodeRef && this.format === null && this.srcNodeRef.innerHTML !== "" ) {
            this.format = this.srcNodeRef.innerHTML;
            d_construct.empty( this.srcNodeRef );
        }
    },

    // override
    startup : function()
    {
        this.inherited( arguments );

        this._updateDisplayedText();
    },


    _setValueAttr : function( new_val )
    {
        this._set( "value", new_val );

        this._updateDisplayedText();
    },


    _updateDisplayedText : function()
    {
        if ( ! this._started )  return; // This makes sure that the display element isn't updated until all values are set. this._started is set to true in startup().

        if ( this.format === null )  return;

        d_construct.empty( this.domNode );

        if ( this.value === null ) {

            if ( this.nullText === null )  return;

            this.domNode.appendChild( d_window.doc.createTextNode( this.nullText ) );

            return;
        }

        var text = d_string.substitute(
                this.format,
                this.value,
                null, /* transform */
                this.thisObject
            );

        this.domNode.appendChild( d_window.doc.createTextNode( text ) );
    }


} );


return b_dijit_OutputFormat;

} );
