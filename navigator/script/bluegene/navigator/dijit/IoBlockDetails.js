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
    "./MonitorActiveMixin",
    "../../Bgws",
    "dojo/when",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/text!./templates/IoBlockDetails.html",
    "module",

    // Used only in template.
    "./BlockFields",
    "../../dijit/OutputFormat",
    "dijit/layout/ContentPane",
    "dijit/layout/StackContainer"
],
function(
        l_AbstractTemplatedContainer,
        l_MonitorActiveMixin,
        b_Bgws,
        d_when,
        d_declare,
        d_lang,
        template,
        module
    )
{


var b_navigator_dijit_IoBlockDetails = d_declare(
        [ l_AbstractTemplatedContainer, l_MonitorActiveMixin ],

{

    templateString : template,


    _fetch_block_details_fn : null,
    _block_id : null,

    _dirty : true,

    _fetch_deferred : null,


    setFetchBlockDetailsFn : function( fetch_block_details_fn )
    {
        this._fetch_block_details_fn = fetch_block_details_fn;
        this._refresh();
    },


    setBlockId : function( block_id )
    {
        this._block_id = block_id;
        this._refresh();

        if ( block_id === null ) {
            this.onGotBlockDetails( null );
        }
    },


    notifyRefresh : function()
    {
        this._refresh();
    },


    onLoading : function() {},
    onGotBlockDetails : function( block_info ) {},


    // override
    destroy : function()
    {
        if ( this._fetch_deferred ) {
            this._fetch_deferred.cancel();
            this._fetch_deferred = null;
        }

        this.inherited( arguments );
    },


    // override MonitorActiveMixin
    _notifyActive : function()
    {
        console.log( module.id + ": now active!" );
        this._loadIfDirtyAndActive();
    },


    _refresh : function()
    {
        this._dirty = true;
        this._loadIfDirtyAndActive();
    },


    _loadIfDirtyAndActive : function()
    {
        if ( ! this._fetch_block_details_fn )  return;

        if ( ! this._isActive() /*MonitorActiveMixin*/ )  return;
        if ( ! this._dirty )  return;

        this._dirty = false;

        if ( this._fetch_deferred ) {
            this._fetch_deferred.cancel();
            this._fetch_deferred = null;
        }

        if ( this._block_id === null ) {
            this._setDisplayed( this._noBlockPane );
            return;
        }

        this._fetch_deferred = this._fetch_block_details_fn( this._block_id );

        this._setDisplayed( this._loadingPane );

        this.onLoading();

        d_when(
                this._fetch_deferred,
                d_lang.hitch( this, this._fetchSuccess ),
                d_lang.hitch( this, this._fetchFailed )
            );
    },


    _fetchSuccess : function( response )
    {
        this._fetch_deferred = null;

        this._loadedPane.set( "value", response );
        this._setDisplayed( this._loadedPane );

        this.onGotBlockDetails( response );
    },


    _fetchFailed : function( response )
    {
        var io_args = this._fetch_deferred.ioArgs;

        this._fetch_deferred = null;

        var err_text = b_Bgws.calculateErrorMessage( response, io_args );

        console.log( module.id + ": Failed to get details for '" + this._block_id + "', " + err_text );

        this._errorFmt.set( "value", { name: this._block_id, "msg": err_text } );

        this._setDisplayed( this._errorPane );

        this.onGotBlockDetails( null );
    },


    _setDisplayed : function( new_pane )
    {
        this._detailsStack.selectChild( new_pane );
    }

} );

return b_navigator_dijit_IoBlockDetails;

} );
