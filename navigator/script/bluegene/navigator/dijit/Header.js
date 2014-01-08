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
    "./AbstractTemplatedContainer",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dijit/Tooltip",
    "dojo/text!./templates/Header.html",

    // Used only in template.
    "../../dijit/ActionLink",
    "../../dijit/OutputText",
    "dijit/layout/ContentPane"
],
function(
        l_AbstractTemplatedContainer,
        d_declare,
        d_lang,
        j_Tooltip,
        template
    )
{

var b_navigator_dijit_Header = d_declare(
        "bluegene.navigator.dijit.Header",
        [ l_AbstractTemplatedContainer ],

{

    templateString : template,


    _username_tooltip : null,


    // override.
    startup : function()
    {
        this.inherited( arguments );

        this._endSessionButton.on( "click", d_lang.hitch( this, this._endSessionClicked ) );
    },


    setSystemName : function( new_name )
    {
        this._systemNameText.set( "value", new_name );
    },

    setUserName : function( new_name )
    {
        this._userNameText.set( "value", new_name );
    },

    setUserAuthDesc : function( auth_desc )
    {
        if ( this._username_tooltip ) {
            this._username_tooltip.destroyRecursive();
            this._username_tooltip = null;
        }

        this._username_tooltip = new j_Tooltip( {
                connectId: [ this._userNameText.id ],
                label: auth_desc,
                position: [ "below" ]
            } );
    },


    onEndSession : function()  {},


    _endSessionClicked : function()
    {
        var res = confirm( "Press OK to end your session." );

        if ( ! res )  return;

        this.onEndSession();
    }

} );

return b_navigator_dijit_Header;

} );
