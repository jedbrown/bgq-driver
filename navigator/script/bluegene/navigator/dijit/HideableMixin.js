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
    "dojo/dom-class",
    "dojo/_base/declare"
],
function(
        d_class,
        d_declare
    )
{

var b_navigator_dijit_HideableMixin = d_declare( [],

{

    onVisibilityChanged : function( is_visible ) {},


    hide : function()
    {
        d_class.add( this.domNode, "dijitHidden" );
        this.onVisibilityChanged( false );
    },


    show : function()
    {
        d_class.remove( this.domNode, "dijitHidden" );
        this.onVisibilityChanged( true );
    }

} );


return b_navigator_dijit_HideableMixin;

} );
