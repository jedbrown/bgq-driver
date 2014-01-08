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
    "dojo/on",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dijit/_WidgetBase"
],
function(
        d_class,
        d_construct,
        d_on,
        d_declare,
        d_lang,
        j__WidgetBase
    )
{


var _CLICKABLE_CLASSNAME = "bgActionLink";


var b_dijit_ActionLink = d_declare(
        [ j__WidgetBase ],

/** @lends bluegene^dijit^ActionLink# */
{
    /** Property; the text displayed in the action link.
     *
     *  If the label is not set on the properties it's gotten from the contents of the srcNodeRef.
     *
     *  @default ""
     *  @type String
     */
    label: null,

    /** Property; Whether the widget is enabled or disabled.
     *
     *  If the widget is disabled then it's not displayed as a link (just regular text) and clicking on it does not call your callback.
     *
     *  @default false
     *  @type Boolean
     */
    disabled: false,


    /** @class Widget for a link that you can click on.
     * <p>
     * Use .on( "click", ... ) to attach a handler function.
     * <p>
     * This widget uses the supplied node in srcNodeRef as the its node (it's not replaced like in most dijits),
     * so the type (e.g., li, span) doesn't change.
     * If no srcNodeRef is supplied then a span is used.
     *
     * @constructs
     */
    constructor: function( /** Object? */ params, /** DomNode|String? */ srcNodeRef )
    {
        // Nothing to do.
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
    postMixInProperties : function()
    {
        this.inherited( arguments );

        if ( this.label === null ) {
            if ( this.srcNodeRef && this.srcNodeRef.innerHTML !== "" ) {
                this.label = this.srcNodeRef.innerHTML;
            } else {
                this.label = "";
            }
        }
    },

    // override
    postCreate : function()
    {
        this.inherited( arguments );

        this._click_on_h = d_on( this.domNode, "click", d_lang.hitch( this, this._onClick ) );

        if ( ! this.disabled )  d_class.add( this.domNode, _CLICKABLE_CLASSNAME );
    },

    // override
    destroy: function()
    {
        this._click_on_h.remove();
        d_construct.destroy( this.domNode );

        this.inherited( arguments );
    },


    onClick : function()
    {
        return this.inherited( arguments );
    },


    _setDisabledAttr : function( new_value )
    {
        this._set( "disabled", new_value );

        if ( ! this.domNode )  return;

        if ( this.disabled ) {
            d_class.remove( this.domNode, _CLICKABLE_CLASSNAME );
        } else {
            d_class.add( this.domNode, _CLICKABLE_CLASSNAME );
        }
    },

    _setLabelAttr : function( new_value )
    {
        this.domNode.innerHTML = new_value;
    },


    _onClick : function( e )
    {
        if ( this.disabled )  return;

        this.onClick();
    }

} );

return b_dijit_ActionLink;

} );
