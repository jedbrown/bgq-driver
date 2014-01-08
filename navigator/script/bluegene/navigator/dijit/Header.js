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
    "module",

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
        template,
        module
    )
{

var b_navigator_dijit_Header = d_declare(
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


    onEndSession : function()  {},


    _setSystemNameAttr : function( new_name )
    {
        this._systemNameText.set( "value", new_name );
    },


    _setUserInfoAttr : function( args )
    {
        console.log( module.id + ": [" + this.id + "] Setting user name to '" + args.userName + "'" );

        this._userNameText.set( "value", args.userName );


        var user_tooltip_text = "";


        if ( args.userInfo.isAdministrator() ) {

            user_tooltip_text = "User is Blue Gene Administrator";

        } else {

            var authorities = [];

            if ( args.userInfo.hasHardwareRead() ) {
                authorities.push( "hardware read" );
            }
            if ( args.userInfo.hasBlockCreate() ) {
                authorities.push( "block create" );
            }

            if ( authorities.length > 0 ) {
                user_tooltip_text = "User has " + authorities.join( ", " );
            } else {
                user_tooltip_text = "User has no special authorities";
            }

        }

        if ( this._username_tooltip ) {
            this._username_tooltip.destroyRecursive();
            this._username_tooltip = null;
        }

        this._username_tooltip = new j_Tooltip( {
                connectId: [ this._userNameText.id ],
                label: user_tooltip_text,
                position: [ "below" ]
            } );

    },


    _endSessionClicked : function()
    {
        var res = confirm( "Press OK to end your session." );

        if ( ! res )  return;

        this.onEndSession();
    }

} );

return b_navigator_dijit_Header;

} );
