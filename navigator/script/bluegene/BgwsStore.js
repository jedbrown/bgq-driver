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
/* (C) Copyright IBM Corp.  2012, 2011                              */
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
    "dojo/json",
    "dojo/when",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/_base/xhr",
    "dojo/data/ObjectStore",
    "dojo/store/JsonRest",
    "dojo/store/util/QueryResults",
    "module"
],
function(
        d_json,
        d_when,
        d_declare,
        d_lang,
        d_xhr,
        d_data_ObjectStore,
        d_store_JsonRest,
        d_store_util_QueryResults,
        module
    )
{


var b_BgwsStore = d_declare( [ d_store_JsonRest ],

/** @lends bluegene^BgwsStore# */
{
    "-chains-": { constructor: "manual" },


    _bgws : null,


    /** @constructs */
    constructor : function(
            args,
            bgws
        )
    {
        // args:
        //   target: "url" -- required
        //   idProperty: "name"

        this._bgws = bgws;

        args.target = this._bgws.calcResourcePath( args.target );
        args.sortParam = "sort";

        this.inherited( arguments, [ args ] );
    },


    // override
    get : function( id, options )
    {
        var new_options = d_lang.mixin( options || {}, this._bgws.getSessionHeaderObj() );

        return this.inherited( arguments, [ id, new_options ] );
    },


    // override
    put : function( object, options )
    {
        options = options || {};
        var id = (("id" in options) ? options.id : this.getIdentity( object ));
        var hasId = (typeof id != "undefined");

        var req_headers = d_lang.mixin( {
                    "Content-Type": "application/json",
                    Accept: this.accepts,
                    "If-Match": options.overwrite === true ? "*" : null,
                    "If-None-Match": options.overwrite === false ? "*" : null
                },
                this._bgws.getSessionHeaderObj()
            );

        return d_xhr(
                hasId && ! options.incremental ? "PUT" : "POST",
                {
                    url: hasId ? this.target + id : this.target,
                    postData: d_json.stringify( object ),
                    handleAs: "json",
                    headers: req_headers
                }
            );
    },


    // override
    remove : function( id )
    {
        return d_xhr(
                "DELETE",
                {
                    url: this.target + id,
                    headers: this._bgws.getSessionHeaderObj()
                }
            );
    },


    // override
    query : function( query, options )
    {
        console.log( module.id + ": query " + this.target + " query=", query, " options=", options );

        var headers = d_lang.mixin(
                {
                    Accept: this.accepts
                },
                this._bgws.getSessionHeaderObj()
            );

        options = options || {};

        if ( options.start >= 0 || options.count >= 0 ) {
            headers.Range = ("items=" +
                    (options.start || '0') +
                    '-' +
                    (("count" in options && options.count != Infinity) ? (options.count + (options.start || 0) - 1) : ''))
                ;
        }

        if ( query && typeof query == "object" ){
            query = d_xhr.objectToQuery( query );
            query = query ? ("?" + query) : "";
        }

        if ( options && options.sort ){
            var sortParam = this.sortParam;
            query += (query ? "&" : "?") + (sortParam ? sortParam + '=' : "sort(");
            for( var i = 0 ; i < options.sort.length ; i++ ) {
                var sort = options.sort[i];
                query += (i > 0 ? "," : "") + (sort.descending ? '-' : '') + encodeURIComponent( sort.attribute ); // changed so no + on sort.
            }
            if( ! sortParam ) {
                query += ")";
            }
        }

        var results = d_xhr(
                "GET",
                {
                    url: this.target + (query || ""),
                    handleAs: "json",
                    headers: headers
                }
            );

        results.total = results.then(
                function() {

                    // We noticed a problem where if the BGWS server sends back a large total,
                    // the EnhancedGrid just displays nothing.
                    // To work aroun this, I'm going to set the max range size to 500,000,
                    // which appears to generally work.
                    // If EnhancedGrid is fixed, or switch to another type of grid,
                    // may want to revisit this.

                    var range = results.ioArgs.xhr.getResponseHeader( "Content-Range" );
                    if ( ! range )  return;
                    range = range.match( /\/(.*)/ );
                    if ( ! range ) return;
                    range = +range[1];

                    var MAX_SIZE = 500000;

                    if ( range > MAX_SIZE ) {
                        console.log( module.id + ": Hit max size for result total, truncating" );
                        return MAX_SIZE;
                    }
                    return range;
                }
            );

        d_when(
                results,
                null, // Don't care about success.
                d_lang.hitch( this, function( error ) {
                    this._bgws.notifyfetchError( error );
                } )
            );

        return d_store_util_QueryResults( results );
    }

} );


b_BgwsStore.createObjectStore = function( args, bgws )
    {
        var bgwsStore = new b_BgwsStore(
                args,
                bgws
            );

        return new d_data_ObjectStore( { objectStore: bgwsStore } );
    };


return b_BgwsStore;

} );
