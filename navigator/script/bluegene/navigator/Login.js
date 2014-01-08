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
    "./topic",
    "./dijit/Login",
    "dojo/when",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_topic,
        l_dijit_Login,
        d_when,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_Login = d_declare( null,

/** @lends bluegene^navigator^Login# */
{
    _bgws: null,
    _username: "",
    _auth_str: null,
    _login_dij : null,
    _login_def : null, /** Deferred */


    /**
     *  ctor
     *
     *  @class Login dialog.
     *  @constructs
     */
    constructor: function(
            /**bluegene^Bgws*/ bgws,
            /**Object*/ location_search_obj,
            login_dij
        )
    {
        this._bgws = bgws;

        this._login_dij = login_dij;

        if ( location_search_obj ) {
            if ( "user" in location_search_obj ) {
                this._login_dij.setUser( location_search_obj.user );
                this._username = location_search_obj.user;
            }
            if ( "auth" in location_search_obj ) {
                this._auth_str = location_search_obj.auth;
            }
        }

        this._login_dij.on( "login", d_lang.hitch( this, this._login ) );
    },


    /** Start processing the login dialog.
     */
    start: function( error_type )
    {
        if ( this._auth_str ) {
            console.log( module.id + ": auto login." );

            this._login_dij.setState( l_dijit_Login.states.AUTHENTICATING, error_type );

            if ( ! this._username )  this._username = "user";

            this._login_def = d_when(
                    this._bgws.startSessionAuth(
                            this._auth_str
                        ),
                    d_lang.hitch( this, this._loginSuccess ),
                    d_lang.hitch( this, this._loginFailed )
                );

            return;
        }

        console.log( module.id + ": waiting for user to log in." );
        this._login_dij.setState( l_dijit_Login.states.WAITING, error_type );
    },

    _login: function( login_value )
    {
        if ( this._login_def ) {
            console.log( module.id + ": _login called when already logging in?" );
            return;
        }

        this._login_dij.setState( l_dijit_Login.states.AUTHENTICATING );

        this._username = login_value.username;

        this._login_def = d_when(
                this._bgws.startSession(
                    login_value.username,
                    login_value.password
                ),
                d_lang.hitch( this, this._loginSuccess ),
                d_lang.hitch( this, this._loginFailed )
            );
    },

    _loginSuccess: function()
    {
        this._login_def = null;

        this._login_dij.setState( l_dijit_Login.states.DONE );

        l_topic.publish( l_topic.loggedIn, { userInfo: this._bgws.getUserInfo(), userName: this._username } );
        l_topic.publish( l_topic.loginComplete );
    },

    _loginFailed: function( error )
    {
        this._login_def = null;

        this._login_dij.setState( l_dijit_Login.states.WAITING, error.errorType );
    }

} );


return b_navigator_Login;

} );
