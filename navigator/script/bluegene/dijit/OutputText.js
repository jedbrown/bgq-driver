/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
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
    "dojo/dom-class",
    "dojo/dom-construct",
    "dojo/_base/declare",
    "dojo/_base/window",
    "dijit/_WidgetBase"
],
function(
        d_class,
        d_construct,
        d_declare,
        d_window,
        j__WidgetBase
    )
{

var _DEFAULT_FORMATTER_FN = function( value )
    {
        if ( value === null )  return "";
        if ( typeof value === "string" )  return value;
        if ( typeof value === "object" && ("join" in value) )  return value.join( ", " );
        return ("" + value);
    };


var b_dijit_OutputText = d_declare(
        [ j__WidgetBase ],

/** @lends bluegene^dijit^OutputText# */
{
    /** Property; Set to a function that calculates the class for the element based on the value.
     *
     *  The argument is the value, which is whatever you set value to, which includes null if value isn't set.
     *  The return value is a string of class names separated by spaces or an array of class names, see "dojo/dom-class".replace.
     *
     *  @type Function
     */
    classFn : null,

    /** Property; If true, any HTML chars in the value are escaped, otherwise HTML chars are not escaped.
     *
     *  If HTML chars are not escaped then it's up to the user to properly escape HTML characters.
     *
     *  @default true
     *  @type Boolean
     */
    escapeHtml : true,

    /** Property; Function to format the value.
     *
     *  The formatter will be passed the value and must return a string.
     *
     *  The default formatter works like this:
     *  <ul>
     *  <li> Strings are returned unchanged.</li>
     *  <li> Numbers are converted to strings without formatting (i.e, no ,s)</li>
     *  <li> Arrays of strings are each value as a string comma-separated.</li>
     *  <li> null returns empty string.</li>
     *  </ul>
     *
     *  @default formatter function as in the description.
     *  @type function
     */
    formatter : null,

    /** Property; The text displayed when the value is null.
     *
     *  If defined declaratively, the null text is the contents of the element by default.
     *
     *  @default The contents
     *  @type String
     */
    nullText : null,

    /** Property; If true, the nullText value is HTML and will not be escaped, otherwise it will be escaped.
     *
     *  @default false.
     *  @type Boolean
     */
    nullTextHtml : false,

    /** Property; The value.
     */
    value : null,


    /**
     * @class Widget to display a value as text on the page.
     * Use the value property to set the current value.
     * The text displayed is the formatted value, as calculated by the formatter property.
     * If the escapeHtml property is true then any HTML will be escaped,
     * otherwise the formatted value is displayed with HTML elements and formatting intact.
     *
     * Note that the type of the element for this dijit is the same as srcNodeRef.
     * If no srcNodeRef then the type of the element is a SPAN.
     *
     * @constructs
     */
    constructor : function( /** Object? */ params, /** DomNode | String */ srcNodeRef )
    {
        this.formatter = _DEFAULT_FORMATTER_FN;
    },


    // override
    postMixInProperties : function()
    {
        this.inherited( arguments );

        if ( this.srcNodeRef && this.nullText === null && this.srcNodeRef.innerHTML !== "" ) {
            this.nullText = this.srcNodeRef.innerHTML;
            this.nullTextHtml = true;
        }
    },

    // override
    buildRendering : function()
    {
        // buildRendering creates 'div' by default. OutputText should be a span.
        if ( ! this.domNode ) {
            this.domNode = this.srcNodeRef || d_construct.create( 'span' );
        }

        this.inherited( arguments );
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

        d_construct.empty( this.domNode );

        if ( this.value === null ) {

            if ( this.nullText !== null ) {
                if ( this.nullTextHtml ) {
                    this.domNode.innerHTML = this.nullText;
                } else {
                    this.domNode.appendChild( d_window.doc.createTextNode( this.nullText ) );
                }
            } else if ( this.formatter ) {
                this.domNode.appendChild( d_window.doc.createTextNode( this.formatter( null ) ) );
            }

        } else {

            var formatted_value;
            if ( this.formatter !== null ) {
                formatted_value = this.formatter( this.value );
            } else {
                formatted_value = this.value;
            }

            if ( this.escapeHtml ) {
                this.domNode.appendChild( d_window.doc.createTextNode( formatted_value ) );
            } else {
                this.domNode.innerHTML = formatted_value;
            }

        }


        if ( this.classFn !== null ) {
            var classes = this.classFn( this.value );

            d_class.replace( this.domNode, classes );
        }
    }

} );

return b_dijit_OutputText;

} );
