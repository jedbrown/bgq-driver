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
    "dojo/dom",
    "dojo/dom-class",
    "dojo/_base/array",
    "dijit/registry"
],
function(
        d_dom,
        d_class,
        d_array,
        j_registry
    )
{


var _AMP_RE = /&/g;
var _GT_RE = />/g;
var _LT_RE = /</g;

var _HIDDEN_CLASS_NAME = "dijitHidden";


var _calcNode = function( /** DomNode | Dijit | String */ arg )
{
    if ( (typeof arg === "string") && j_registry.byId( arg ) )  return j_registry.byId( arg ).domNode;
    if ( (typeof arg === "string") && d_dom.byId( arg ) )  return d_dom.byId( arg );
    if ( arg.domNode )  return arg.domNode;
    return arg;
};


var _calcNodes = function( /** DomNode | Dijit | String ... */ )
{
    var res = [];
    d_array.forEach( arguments, function( a ) {
        res.push( _calcNode( a ) );
    } );
    return res;
};


var _setHiddenClass = function( args, b )
{
    var nodes = _calcNodes.apply( this, args );

    d_array.forEach( nodes, function( n ) {
        d_class.toggle( n, _HIDDEN_CLASS_NAME, b );
    } );
};

var _hide = function( /** DomNode | Dijit | String ... */ )
{
    _setHiddenClass( arguments, true );
};

var _show = function( /** DomNode | Dijit | String ... */ )
{
    _setHiddenClass( arguments, false );
};

/** @name bluegene^utility
 *  @namespace Utility functions and classes.
 */
var b_utility =

/** @lends bluegene^utility */
{

    /** Escape special HTML characters in a string so will display in HTML. */
    escapeHtml : function( text )
    {
        if ( text == null )  return "";

        var text_escaped = (text.replace( _AMP_RE, "&amp;" ).replace( _LT_RE, "&lt;" ).replace( _GT_RE, "&gt;" ));

        return text_escaped;
    },


    /** Combine two date objects, taking date part from one and time part from the other. */
    combineDates : function( /**Date*/ date_date, /**Date*/ date_time )
    {
        return new Date( date_date.getFullYear(), date_date.getMonth(), date_date.getDate(), date_time.getHours(), date_time.getMinutes(), date_time.getSeconds(), date_time.getMilliseconds() );
    },


    /** Returns true iff the object has no enumerable properties. */
    isEmpty : function( /**Object*/ o )
    {
        for ( p in o ) {
            return false;
        }

        return true;
    },

    /** Returns a DomNode given the argument. */
    calcNode : _calcNode,

    /** Returns the DomNodes as an array for each argument. */
    calcNodes : _calcNodes,

    /** Returns a Boolean, true iff the element is visible. */
    isVisible : function( /** DomNode | Dijit | String */ arg )
    {
        var elem = _calcNode( arg );
        return (! d_class.contains( elem, _HIDDEN_CLASS_NAME ));
    },

    /** Sets the argument to be visible. */
    show : _show,

    /** Sets the argument to be hidden. */
    hide : _hide,

    /** Sets the 'show' to be visible and the 'hide' to be hidden */
    setVisibility : function( obj )
    {
        if ( obj.hide )  _hide.apply( this, obj.hide );
        if ( obj.show )  _show.apply( this, obj.show );
    }

};

return b_utility;


} );
