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
    "dojo/text!./templates/RasDetails.html",
    "module",

    // Used only in template.
    "./RasFields",
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


var b_navigator_dijit_RasDetails = d_declare(
        [ l_AbstractTemplatedContainer, l_MonitorActiveMixin ],

{

    templateString : template,

    _fetch_details_fn : null,
    _id : null,

    _dirty : false,
    _fetch_deferred: null,


    setFetchDetailsFn : function( fn )
    {
        this._fetch_details_fn = fn;
        this._refresh();
    },

    setRecId : function( rec_id )
    {
        this._id = rec_id;
        this._refresh();
    },


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
        this._loadIfReady();
    },


    _refresh : function()
    {
        this._dirty = true;
        this._loadIfReady();
    },


    _loadIfReady : function()
    {
        if ( ! this._fetch_details_fn )  return;
        if ( ! this._dirty ) return;
        if ( ! this._isActive() /*MonitorActiveMixin*/ )  return;

        console.log( module.id + ": requesting details for ", this._id );

        // Cancel any previous request.
        if ( this._fetch_deferred !== null ) {
            this._fetch_deferred.cancel();
            this._fetch_deferred = null;
        }

        if ( this._id === null ) {
            this._dirty = false;
            this._setDisplay( "none" );
            return;
        }

        // Start loading the event data...

        this._fetch_deferred = this._fetch_details_fn( this._id );

        d_when(
                this._fetch_deferred,
                d_lang.hitch( this, this._gotEventDetails ),
                d_lang.hitch( this, this._fetchDetailsFailed )
            );

        this._setDisplay( "loading" );
    },


    _gotEventDetails : function( result )
    {
        this._fetch_deferred = null;

        console.log( module.id + ": got details for ", this._id, result );

        this._setDisplay( "loaded", result );
    },


    _fetchDetailsFailed : function( error )
    {
        var io_args = this._fetch_deferred.ioArgs;

        this._fetch_deferred = null;

        var err_text = b_Bgws.calculateErrorMessage( error, io_args );

        console.log( module.id + ": Failed to get details for " + this._id + ", " + err_text );

        this._setDisplay( "error", { message: err_text } );
    },


    _setDisplay : function( which, args )
    {
        if ( which === "none" ) {
            this._stackContainer.selectChild( this._noEventContainer );
            return;
        }

        if ( which === "loading" ) {
            this._loadingFmt.set( "value", { id: this._id } );
            this._stackContainer.selectChild( this._loadingContainer );
            return;
        }

        if ( which === "error" ) {
            this._errorFmt.set( "value", { id: this._id, msg: args.message } );
            this._stackContainer.selectChild( this._errorContainer );
            return;
        }

        if ( which === "loaded" ) {
            args.id = this._id;
            this._loadedContainer.set( "value", args );
            this._stackContainer.selectChild( this._loadedContainer );
            return;
        }
    }


} );

return b_navigator_dijit_RasDetails;

} );
