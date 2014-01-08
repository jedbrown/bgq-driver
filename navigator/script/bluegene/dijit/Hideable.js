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
    "dojo/dom-class",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dijit/_WidgetBase"
],
function(
        d_class,
        d_declare,
        d_lang,
        j__WidgetBase
    )
{


/** Visibility enum.
 *
 *  @name Visibility
 *  @memberOf bluegene^dijit^Hideable
 *  @namespace Visibility enum.
 */
var _Visibility =

/** @lends bluegene^dijit^Hideable.Visibility */
{
    /** @constant
     *  @type string */
    HIDDEN: "hidden",

    /** @constant
     *  @type string */
    VISIBLE: "visible"
};


var _HIDDEN_CLASS_NAME = "dijitHidden";

var b_dijit_Hideable = d_declare(
        [ j__WidgetBase ],

/** @lends bluegene^dijit^Hideable# */
{
    /** Property, indicates whether displayed or not.
     *  Values are "visible" or "hidden".
     *  @default "visible"
     *  @type string
     */
    visibility : "",


    /**
     *  @constructs
     *
     *  @class Dijit that can be either visible or hidden.
     */
    constructor : function( params, srcNodeRef )
    {
        this.visibility = _Visibility.VISIBLE;
        this.containerNode = srcNodeRef;
    },


    _setVisibilityAttr : function( visibility )
    {
        this.setVisibility( visibility );
    },


    /** Set the visibility. */
    setVisibility : function( /**bluegene^dijit^Hideable.Visibility*/ visibility )
    {
        this.visibility = visibility;

        d_class.toggle( this.domNode, _HIDDEN_CLASS_NAME, (this.visibility === _Visibility.HIDDEN) );
        if ( this.visibility === _Visibility.HIDDEN ) {
            this.containerNode = null;
        } else {
            this.containerNode = this.domNode; // This makes j__WidgetBase.getChildren return the child dijits under the Hideable.
        }
    },


    /** Show the element. */
    show : function()
    {
        this.setVisibility( _Visibility.VISIBLE );
    },


    /** Hide the element. */
    hide : function()
    {
        this.setVisibility( _Visibility.HIDDEN );
    }

} );


d_lang.mixin( b_dijit_Hideable,
{
    Visibility : _Visibility
} );


return b_dijit_Hideable;

} );
