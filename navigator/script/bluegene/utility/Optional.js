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
    "./utility",
    "dojo/dom-class",
    "dojo/_base/array",
    "dojo/_base/declare"
],
function(
        l_utility,
        d_class,
        d_array,
        d_declare
    )
{


var b_utility_Optional = d_declare(
        "bluegene.utility.Optional",
        null,

/** @lends bluegene^utility^Optional# */
{
    _node : null,


    /** @constructs */
    constructor : function( element, initial_visibility )
    {
        this._node = l_utility.calcNode( element );

        if ( initial_visibility === bluegene.utility.Optional.HIDDEN ) {
            this.hide();
        }
    },


    getNode : function()  { return this._node; },

    isVisible : function()  { return (! d_class.contains( this._node, bluegene.utility.Optional._HIDDEN_CLASS_NAME )); },

    hide : function()  { this._setHiddenClass( true ); },

    show : function()  { this._setHiddenClass( false ); },


    _setHiddenClass : function( b ) { d_class.toggle( this._node, bluegene.utility.Optional._HIDDEN_CLASS_NAME, b ); }
} );


b_utility_Optional._HIDDEN_CLASS_NAME = "dijitHidden";

b_utility_Optional.HIDDEN = "hidden";
b_utility_Optional.VISIBLE = "visible";


b_utility_Optional.setVisibility = function( obj )
    {
        d_array.forEach( obj.hide, function( o ) { o.hide(); } );
        d_array.forEach( obj.show, function( o ) { o.show(); } );
    };


return b_utility_Optional;

} );
