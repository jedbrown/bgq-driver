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
    "../../Bgws",
    "dojo/keys",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dijit/_WidgetBase",
    "dijit/_TemplatedMixin",
    "dijit/_WidgetsInTemplateMixin",
    "dijit/_base/focus",
    "dojo/text!./templates/Login.html",
    "module",

    // Used in the template only.
    "../../dijit/Hideable",
    "dijit/Dialog",
    "dijit/form/Button",
    "dijit/form/TextBox"
],
function(
        b_Bgws,
        d_keys,
        d_declare,
        d_lang,
        j__WidgetBase,
        j__TemplatedMixin,
        j___WidgetsInTemplateMixin,
        j_focus,
        template,
        module
    )
{


var _states = {
        WAITING: "waiting",
        AUTHENTICATING: "authenticating",
        DONE: "done"
    };


var b_navigator_dijit_Login = d_declare(
        [ j__WidgetBase, j__TemplatedMixin, j___WidgetsInTemplateMixin ],

{

    _startupHandled : false,

    templateString : template,


    startup : function()
    {
        this.inherited( arguments );
        if ( this._startupHandled )  return;
        this._startupHandled = true;

        this._usernameInput.on( "keyPress", d_lang.hitch( this, this._userKeypress ) );
        this._passwordInput.on( "keyPress", d_lang.hitch( this, this._passwordKeypress ) );
        this._loginButton.on( "click", d_lang.hitch( this, this._login ) );

        // Prevent the dialog's close button from doing anything.
        this._loginDialog.onCancel = function() { /* Do Nothing */ };
    },


    setUser : function( user_name )
    {
        this._usernameInput.set( "value", user_name );
    },


    setState : function( new_state, error_type )
    {
        this._invalidUserPassText.hide();
        this._notAvailableText.hide();
        this._invalidSessionText.hide();

        if ( error_type === b_Bgws.loginError.INVALID_AUTHORIZATION )  this._invalidUserPassText.show();
        else if ( error_type === b_Bgws.loginError.BGWS_NOT_AVAILABLE )  this._notAvailableText.show();
        else if ( error_type === b_Bgws.loginError.INVALID_SESSION )  this._invalidSessionText.show();


        if ( new_state === _states.WAITING ) {

            this._authenticatingText.hide();

            this._usernameInput.set( "disabled", false );
            this._passwordInput.set( "disabled", false );
            this._loginButton.set( "disabled", false );

            this._loginDialog.show();

            if ( this._usernameInput.get( "value" ) !== "" ) {
                console.log( module.id + ": have user name so want to switch focus to password..." );
                window.setTimeout( d_lang.hitch( this, this._setFocusToPassword ), 500 );
            }

        } else if ( new_state === _states.AUTHENTICATING ) {

            this._usernameInput.set( "disabled", true );
            this._passwordInput.set( "disabled", true );
            this._loginButton.set( "disabled", true );

            this._authenticatingText.show();

            this._loginDialog.show();

        } else if ( new_state === _states.DONE ) {

            this._passwordInput.set( "value", "" ); // Clear the login password.

            this._loginDialog.hide();

        }

        this._state = new_state;
    },


    /** Event. Called when should login. */
    onLogin : function( data )  { /* Do nothing */ },


    _userKeypress : function( ev )
    {
        // ignore everything but ENTER.
        if ( ev.keyCode !== d_keys.ENTER ) {
            return;
        }

        if ( this._passwordInput.get( "value" ).length === 0 ) {
            // No password yet so set focus to the password field.
            this._passwordInput.focus();
            return;
        }

        this._login();
    },


    _passwordKeypress : function( ev )
    {
        // ignore everything but ENTER.
        if ( ev.keyCode !== d_keys.ENTER ) {
            return;
        }

        this._login();
    },


    _login : function()
    {
        var login_value = this._loginDialog.get( "value" );
        this.onLogin( login_value );
    },

    _setFocusToPassword: function()
    {
        j_focus.focus( this._passwordInput.focusNode );

        if ( j_focus.getFocus().node && j_focus.getFocus().node === this._passwordInput.focusNode )  return;

        window.setTimeout( d_lang.hitch( this, this._setFocusToPassword ), 100 );
    }

} );


b_navigator_dijit_Login.states = _states;


return b_navigator_dijit_Login;

} );
