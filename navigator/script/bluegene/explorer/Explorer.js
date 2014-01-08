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
    "bluegene/Bgws",
    "dojo/cookie",
    "dojo/dom-prop",
    "dojo/json",
    "dojo/request",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/data/ObjectStore",
    "dojo/store/Memory",
    "dijit/registry",
    "dijit/form/ComboBox",
    "module",

    // Used in explorer.html
    "./dijit/Explorer"
],
function(
        b_Bgws,
        d_cookie,
        d_prop,
        d_json,
        d_request,
        d_array,
        d_declare,
        d_lang,
        d_data_ObjectStore,
        d_store_Memory,
        j_registry,
        j_form_ComboBox,
        module
    )
{


var b_explorer_Explorer =  d_declare( null,

/** @lends bluegene^explorer^Explorer# */
{
    _bgws : null,

    _session_id: null,

    _request_headers_obj: {},

    _stored_requests: {},


    /** @constructs */
    constructor : function()
    {
        this._bgws = new b_Bgws( {} );


        var stored_username = d_cookie( "username" );

        var stored_requests_json = d_cookie( "requests" );

        if ( stored_requests_json ) {
            this._stored_requests = d_json.parse( stored_requests_json );
        }

        console.log( module.id + ": creating. stored_username=", stored_username, "stored_requests=", this._stored_requests );


        if ( stored_username != null ) {
            j_registry.byId( "start-session-username-input" ).set( "value", stored_username );
        }


        this._setStoredQueriesOptions();


        var bgws_uris_store = new d_store_Memory( {
                data: [
                    { url: "/bg/blocks/" },
                    { url: "/bg/jobs/" },
                    { url: "/bg/alerts/" },
                    { url: "/bg/ras/" },
                    { url: "/bg/machine/" },
                    { url: "/bg/diagnostics/blocks/" },
                    { url: "/bg/diagnostics/locations" },
                    { url: "/bg/diagnostics/runs/" },
                    { url: "/bg/diagnostics/testcases" },
                    { url: "/bg/environmentals/bulkPower" },
                    { url: "/bg/environmentals/coolant" },
                    { url: "/bg/environmentals/fan" },
                    { url: "/bg/environmentals/ioCard" },
                    { url: "/bg/environmentals/linkChip" },
                    { url: "/bg/environmentals/node" },
                    { url: "/bg/environmentals/nodeCard" },
                    { url: "/bg/environmentals/optical" },
                    { url: "/bg/environmentals/serviceCard" },
                    { url: "/bg/environmentals/serviceCardTemp" },
                    { url: "/bg/hardwareReplacements" },
                    { url: "/bg/serviceActions" },
                    { url: "/bg/summary/jobs" },
                    { url: "/bg/summary/machine" },
                    { url: "/bg/summary/ras" },
                    { url: "/bg/summary/system" },
                    { url: "/bg/summary/utilization" },
                    { url: "/bg/perf" },
                    { url: "/bg/bgwsServer" },
                    { url: "/bg/bgwsServer/logging" },
                    { url: "/bg/bgws/sessions/" }
                ]
            } );

        new j_form_ComboBox(
                { name: "uri", searchAttr: "url", store: bgws_uris_store, style: "width: 500px;", value: "/bg/blocks/" },
                "request-url"
            );


        var header_names_store = new d_store_Memory( {
                data: [
                       { name: "Range" }
                ]
            } );

        var new_header_name_dijit = new j_form_ComboBox(
                { name: "newHeaderName", searchAttr: "name", store: header_names_store, style: "width: 200px;", value: "" },
                "new-header-name"
            );
        new_header_name_dijit.startup();

        new_header_name_dijit.on( "change", d_lang.hitch( this, this._newHeaderNameChanged ) );

        this._request_headers_obj = {
                "Accept": { value: b_Bgws.jsonMediaType, selected: true },
                "Content-Type": { value: b_Bgws.jsonMediaType, selected: false }
            };

        this._setRequestHeaders( this._request_headers_obj );


        j_registry.byId( "new-header-form" ).on( "submit", d_lang.hitch( this, function( e ) { e.preventDefault(); this._addHeader(); } ) );
        j_registry.byId( "request-body-text" ).on( "change", d_lang.hitch( this, this._bodyChanged ) );
        j_registry.byId( "request-form" ).on( "submit", d_lang.hitch( this, function( e ) { e.preventDefault(); this._requestSubmit(); } ) );
        j_registry.byId( "request-operation-select" ).on( "change", d_lang.hitch( this, this._requestOperationChanged ) );
        j_registry.byId( "request-restore-button" ).on( "click", d_lang.hitch( this, this._restoreRequest ) );
        j_registry.byId( "request-save-button" ).on( "click", d_lang.hitch( this, this._saveRequest ) );
        j_registry.byId( "start-session-form" ).on( "submit", d_lang.hitch( this, function( e ) { e.preventDefault(); this._startSessionSubmit(); } ) );
    },


    _setStoredQueriesOptions : function()
    {
        var stored_requests_items = [];

        for ( request_name in this._stored_requests ) {
            stored_requests_items.push( { name: request_name } );
        }

        console.log( module.id + ": stored_requests_items=", stored_requests_items );

        var stored_requests_store = new d_store_Memory( {
                data: stored_requests_items
            } );

        var stored_requests_adapter = new d_data_ObjectStore( {
                objectStore: stored_requests_store
            } );

        j_registry.byId( "request-restore-select" ).setStore( stored_requests_adapter, (stored_requests_items.length == 0 ? "" : stored_requests_items[0].name), null );
    },


    _requestOperationChanged : function( new_value )
    {
        console.log( module.id + ": _requestOperationChanged", new_value, arguments );
        var enable_content_type = (new_value == "POST" || new_value == "PUT");
        this._request_headers_obj["Content-Type"] = { value: b_Bgws.jsonMediaType, selected: enable_content_type };
        this._setRequestHeaders( this._request_headers_obj );
    },

    _newHeaderNameChanged : function( new_value )
    {
        console.log( module.id + ": _newHeaderNameChanged", arguments );

        if ( new_value == "Range" ) {
            j_registry.byId( "new-header-value-input" ).set( "value", "items=0-1");
        }
    },

    _addHeader : function()
    {
        var form_val = j_registry.byId( "new-header-form" ).get( "value" );

        console.log( module.id + ": add header", form_val );

        this._request_headers_obj[form_val.newHeaderName] = { value: form_val.newHeaderValue, selected: true };

        this._setRequestHeaders( this._request_headers_obj );
    },

    _bodyChanged : function( new_value )
    {
        var json_ok = true;

        try {

            d_json.parse( new_value );

        } catch ( e ) {

            json_ok = false;

        }

        j_registry.byId( "request-body-invalid-hideable" ).set( "visibility", (json_ok ? "hidden" : "displayed") );
    },

    _startSessionSubmit : function()
    {
        var form_val = j_registry.byId( "start-session-form" ).get( "value" );

        var promise = this._bgws.startSession(
                form_val.username,
                form_val.password
            );

        promise.then( 
                d_lang.hitch( this, this._sessionStarted ),
                d_lang.hitch( this, this._sessionFailed )
            );
    },


    _sessionStarted : function( response )
    {
        console.log( module.id + ": session started. response=", response );

        this._session_id = response.sessionId;

        this._request_headers_obj[b_Bgws.sessionIdHeaderName] = { value: this._session_id, selected: true };

        // Store the username in a cookie...
        d_cookie( "username", j_registry.byId( "start-session-username-input" ).get( "value" ), { expire: 14 } );

        this._setRequestHeaders( this._request_headers_obj );

        this._updateResponse( response.response );
    },

    _sessionFailed : function( error )
    {
        console.log( module.id + ": session failed. error=", error );

        this._session_id = null;
        delete this._request_headers_obj[b_Bgws.sessionIdHeaderName];

        this._setRequestHeaders( this._request_headers_obj );

        this._updateResponse( error.response );
    },


    _requestSubmit : function()
    {
        var form_val = j_registry.byId( "request-form" ).get( "value" );

        // go through the request headers in the grid and pick out the ones that are enabled.

        var headers = {};

        var headers_grid = j_registry.byId( "request-headers-grid" );

        for ( var i = 0 ; i < headers_grid.selection.selected.length ; ++i ) {
            if ( ! headers_grid.selection.selected[i] )  continue; // not enabled.

            var header_name = headers_grid.store.getValue( headers_grid.getItem( i ), "name" );
            var header_value = headers_grid.store.getValue( headers_grid.getItem( i ), "value" );

            headers[header_name] = header_value;
        }

        console.log( module.id + ": submit form=", form_val, "headers=", headers );

 
        var req_options = { headers: headers };

        
        if ( form_val.operation == "GET" ) {

            // Nothing to do.
            
        } else if ( form_val.operation == "DELETE" ) {

            req_options.method = "DELETE";

        } else if ( form_val.operation == "POST" ) {

            req_options.method = "POST";
            req_options.data = form_val.body;

        } else if ( form_val.operation == "PUT" ) {

            req_options.method = "PUT";
            req_options.data = form_val.body;

        } else {
            return;
        }


        var req_promise = d_request(
                form_val.uri,
                req_options
            );

        req_promise.response.then(
                d_lang.hitch( this, this._requestSuccess ),
                d_lang.hitch( this, this._requestFailed )
            );

    },


    _requestSuccess : function( response )
    {
        console.log( module.id + ": request was successful, response=", response );
        
        this._updateResponse( response );
    },

    _requestFailed : function( error_data )
    {
        console.log( module.id + ": request failed. error_data=", error_data );

        this._updateResponse( error_data.response );
    },


    _saveRequest : function()
    {
        var name = j_registry.byId( "request-name" ).get( "value" );
        console.log( module.id + ": save request! name=", name );

        var form_obj = j_registry.byId( "request-form" ).get( "value" );

        if ( name == null || name == "" ) {
            return;
        }

        var obj = {
            operation: form_obj.operation,
            url: form_obj.uri
        };

        if ( form_obj.body != "" ) {
            obj.body = form_obj.body;
        }

        // go through the request headers in the grid and pick out the ones that are enabled, skipping session ID!

        var headers = {};

        var headers_grid = j_registry.byId( "request-headers-grid" );

        for ( var i = 0 ; i < headers_grid.selection.selected.length ; ++i ) {
            if ( ! headers_grid.selection.selected[i] )  continue; // not enabled.

            var item = headers_grid.getItem( i );

            var header_name = headers_grid.store.getValue( item, "name" );
            var header_value = headers_grid.store.getValue( item, "value" );

            if ( header_name == b_Bgws.sessionIdHeaderName )  continue; // don't save session ID.

            headers[header_name] = header_value;
        }

        obj.headers = headers;

        this._stored_requests[name] = obj;

        console.log( module.id + ": storing", this._stored_requests );

        var stored_requests_json = d_json.stringify( this._stored_requests );


        // Store the requests in a cookie!

        d_cookie( "requests", stored_requests_json, { expire: 14 } );


        // Update the stored requests dropdown.
        this._setStoredQueriesOptions();
    },

    _restoreRequest : function()
    {
        var name = j_registry.byId( "request-restore-select" ).get( "value" );

        var req_obj = this._stored_requests[name];

        var form_obj = {
                operation: req_obj.operation,
                uri: req_obj.url
            };

        if ( "body" in req_obj ) {
            form_obj.body = req_obj.body;
        }

        j_registry.byId( "request-form" ).set( "value", form_obj );

        // headers...
        for ( header_name in req_obj.headers ) {
            this._request_headers_obj[header_name]= { value: req_obj.headers[header_name], selected: true };
        }

        this._setRequestHeaders( this._request_headers_obj );
    },

    _updateResponse : function ( response )
    {
        d_prop.set( "response-status", "innerHTML", ("" + response.status + " " + response.xhr.statusText) );
        d_prop.set( "response-headers", "innerHTML", response.xhr.getAllResponseHeaders() );

        if ( response.getHeader( "Content-Type" ) == "application/json" ) {

            var obj = d_json.parse( response.text );

            d_prop.set( "response-body", "innerHTML", d_json.stringify( obj, null, " " ) );

        } else {

            d_prop.set( "response-body", "innerHTML", response.data );

        }
    },

    
    _calcRequestHeaderItemsSelecteds : function( headers_obj )
    {
        var items = [];
        var selecteds = [];

        for ( header_name in headers_obj ) {
            var header = headers_obj[header_name];
            items.push( { name: header_name, value: header.value } );
            selecteds.push( header.selected );
        }

        return { items: items, selecteds: selecteds };
    },

    _setRequestHeaders : function( headers_obj )
    {
        var items_selecteds = this._calcRequestHeaderItemsSelecteds( headers_obj );

        var store = new d_store_Memory({
                idProperty: "name",
                data: items_selecteds.items
            });

        var adapter = new d_data_ObjectStore({
                objectStore: store
            });

        var grid = j_registry.byId( "request-headers-grid" );

        grid.setStore( adapter );

        d_array.forEach( items_selecteds.selecteds, function( selected, i ) {
                grid.rowSelectCell.toggleRow( i, selected );
            } );
    }

} );


b_explorer_Explorer.instance = null;


b_explorer_Explorer.createInstance = function() {
        b_explorer_Explorer.instance = new b_explorer_Explorer();
    };


b_explorer_Explorer.onLoad = function() {
        console.log( module.id + ": onLoad" );
        b_explorer_Explorer.createInstance();
    };


return b_explorer_Explorer;

} );
