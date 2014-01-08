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
    "./Optional",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang"
],
function(
        l_utility,
        l_Optional,
        d_array,
        d_declare,
        d_lang
    )
{


var b_utility_Stack = d_declare( null,

/** @lends bluegene^utility^Stack# */
{
    _node : null,
    _selected : null,

    _child_infos : null,


    /** @constructs */
    constructor : function( arg, node_fns )
    {
        this._node = l_utility.calcNode( arg );

        this._child_infos = [];

        d_array.forEach( this._node.children, d_lang.hitch( this, function( c ) {
            this._child_infos.push( {
                    node: c,
                    optional: new l_Optional( c )
                } );
        } ) );


        d_array.forEach( node_fns, d_lang.hitch( this, function( node_fn ) {
            var node = l_utility.calcNode( node_fn.node );

            for ( var i = 0 ; i < this._child_infos.length ; ++i ) {
                if ( this._child_infos[i].node !== node )  continue;
                this._child_infos[i].fn = node_fn.fn;
                break;
            }
        } ) );


        this._selectChild( this._node.children[0] );
    },


    getSelectedChild : function()  { return this._selected; },


    selectChild : function( arg, data )
    {
        this._selectChild( l_utility.calcNode( arg ), data );
    },


    _selectChild : function( elem, data )
    {
        this._selected = elem;

        var show_ci = null;

        d_array.forEach( this._child_infos, d_lang.hitch( this, function( ci ) {
            if ( ci.node === this._selected ) {
                show_ci = ci;
                return;
            }

            ci.optional.hide();
        } ) );

        if ( show_ci.fn )  show_ci.fn( data );
        show_ci.optional.show();
    }

} );


return b_utility_Stack;

} );
