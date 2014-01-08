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
    "./BgwsStore",
    "./UserInfo",
    "dojo/io-query",
    "dojo/json",
    "dojo/request",
    "dojo/topic",
    "dojo/when",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojox/encoding/base64",
    "dojox/rpc/Rest",
    "module"
],
function(
        b_BgwsStore,
        b_UserInfo,
        d_io_query,
        d_json,
        d_request,
        d_topic,
        d_when,
        d_declare,
        d_lang,
        x_base64,
        x_rpc_Rest,
        module
    )
{


var _jsonMediaType = "application/json";

var _acceptJsonHeader = { "Accept": _jsonMediaType };

var _HTTP_STATUS_BAD_REQUEST = 400;
var _HTTP_STATUS_SERVICE_TEMPORARILY_UNAVAILABLE = 503;


var _sessionIdHeaderName = "X-Bgws-Session-Id";


var _environmentals_path = "environmentals";
var _diagnostics_path = "diagnostics";
var _summary_path = "summary";

var _resourcePaths = {
        alerts : "alerts",
        blocks : "blocks",
        hardwareReplacements : "hardwareReplacements",
        jobs : "jobs",
        machine : "machine",
        perf : "perf",
        ras : "ras",
        serviceActions : "serviceActions",

        bgws : {
            sessions: "bgws/sessions"
        },

        diagnostics : {
            runs : _diagnostics_path + "/runs",
            locations : _diagnostics_path + "/locations",
            testcases : _diagnostics_path + "/testcases"
          },

        environmentals : {
            bulkPower : _environmentals_path + "/bulkPower",
            coolant : _environmentals_path + "/coolant",
            fan : _environmentals_path + "/fan",
            ioCard : _environmentals_path + "/ioCard",
            linkChip : _environmentals_path + "/linkChip",
            node : _environmentals_path + "/node",
            nodeCard : _environmentals_path + "/nodeCard",
            optical : _environmentals_path + "/optical",
            serviceCard : _environmentals_path + "/serviceCard",
            serviceCardTemp : _environmentals_path + "/serviceCardTemp"
          },

          summary : {
              machine : _summary_path + "/machine",
              system : _summary_path + "/system",
              jobs : _summary_path + "/jobs",
              ras : _summary_path + "/ras",
              utilization : _summary_path + "/utilization",
            }
    };


var _postHeaders = d_lang.mixin( { "Content-Type": _jsonMediaType }, _acceptJsonHeader );

var _ERROR_ID_INVALID_SESSION = "invalidSession";


var _loginError = {
        INVALID_AUTHORIZATION : 1,
        BGWS_NOT_AVAILABLE : 2,
        INVALID_SESSION : 3
    };


var _sessionEndedTopic = "/bg/Bgws/sessionEnded";


/**
 *  Returns true iff the response status, etc, indicates the session is not valid.
 */
var _checkXhrSessionInvalid = function( args )
    {
        if ( args.status === _HTTP_STATUS_SERVICE_TEMPORARILY_UNAVAILABLE ) {
            // Apache returns "Service Temporarily Unavailable" if the BGWS server isn't running.
            return _loginError.BGWS_NOT_AVAILABLE;
        }

        if ( args.contentType ) {

            if ( args.contentType === _jsonMediaType ) {
                // It's a BGWS response.

                var response_json = d_json.parse( args.responseText );

                if ( response_json.id === _ERROR_ID_INVALID_SESSION ) {
                    return _loginError.INVALID_SESSION;
                }

                return;
            }

            return;

        } else { // no contentType.

            if ( args.status === _HTTP_STATUS_BAD_REQUEST ) { // BGWS returns 400 with object with id==="invalidSession" if the session ID presented is not valid.

                var response_json = d_json.parse( args.responseText );
                if ( response_json ) {
                    if ( response_json.id === _ERROR_ID_INVALID_SESSION ) {
                        return _loginError.INVALID_SESSION;
                    }
                    return;
                }

                // response wasn't JSON.
                return;

            }

            // Response wasn't "Bad Request", so wouldn't be invalidSession.
            return;

        }
    };


/**
 *  Calculate the error message.
 *  <br/>
 *  If the response is a BGWS error response, then extracts the BGWS message.
 *  <br/>
 *  Otherwise, assumes the response is from the web server so just posts the response.
 *
 *  @name calculateErrorMessage
 *  @methodOf bluegene^Bgws
 *  @returns {string} error message
 */
var _calculateXhrErrorMessage = function( response, io_args )
    {
        if ( response.dojoType === "cancel" ) {
            return response.message;
        }

        var content_type = io_args.xhr.getResponseHeader( "Content-Type" );

        if ( content_type === _jsonMediaType ) {
            // It's a BGWS response.

            var bgws_err = d_json.parse( response.text );

            return bgws_err.text;
        }

        if ( io_args.xhr.status === _HTTP_STATUS_SERVICE_TEMPORARILY_UNAVAILABLE ) {
            return "The BGWS server is not available.";
        }

        return response.text;
    };


var _calculateRequestErrorMessage = function( error_data )
    {
        var response = error_data.response;

        var content_type = response.getHeader( "Content-Type" );

        if ( content_type === _jsonMediaType ) {
            // It's a BGWS response.

            var bgws_err = response.data;

            return bgws_err.text;
        }

        if ( response.status === _HTTP_STATUS_SERVICE_TEMPORARILY_UNAVAILABLE ) {
            return "The BGWS server is not available.";
        }

        return response.text;
    };


var _checkRequestSessionInvalid = function( response )
    {
        if ( response.status === _HTTP_STATUS_SERVICE_TEMPORARILY_UNAVAILABLE ) {
            // Apache returns "Service Temporarily Unavailable" if the BGWS server isn't running.
            return _loginError.BGWS_NOT_AVAILABLE;
        }

        var content_type = response.getHeader( "Content-Type" );

        if ( content_type ) {
    
            if ( content_type === _jsonMediaType ) {
                // It's a BGWS response.
    
                if ( response.data.id === _ERROR_ID_INVALID_SESSION ) {
                    return _loginError.INVALID_SESSION;
                }

                return;
            }

            return;

        } else { // no contentType.

            if ( response.status === _HTTP_STATUS_BAD_REQUEST ) { // BGWS returns 400 with object with id==="invalidSession" if the session ID presented is not valid.

                var response_json = d_json.parse( response.text );
                if ( response_json ) {
                    if ( response_json.id === _ERROR_ID_INVALID_SESSION ) {
                        return _loginError.INVALID_SESSION;
                    }
                    return;
                }

                // response wasn't JSON.
                return;

            }
    
            // Response wasn't "Bad Request", so wouldn't be invalidSession.
            return;
    
        }
    };


var _timestampRegex = /(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2}).(\d{6})/;


var b_Bgws = d_declare( null,

/** @lends bluegene^Bgws# */
{

    resourceUris : null, // a structure containing all the BGWS resource URIs.

    _blocks_service: null,
    _jobs_service: null,

    _session_header: null,
    _user_info: null,

    _base_path: "/bg",


    /**
     *  Constructor.
     *
     *  @class Encapsulates communication with the BGWS server.
     *  @constructs
     */
    constructor: function( args )
    {
        console.log( module.id + ": Created with args=", args );

        if ( args.path )  this._base_path = args.path;

        console.log( module.id + ": blocks path=", this.calcResourcePath( "blocks" ) );


        this.resourceUris = {
            alerts : this.calcResourcePath( _resourcePaths.alerts ),
            blocks : this.calcResourcePath( _resourcePaths.blocks ),
            hardwareReplacements : this.calcResourcePath( _resourcePaths.hardwareReplacements ),
            jobs : this.calcResourcePath( _resourcePaths.jobs ),
            machine : this.calcResourcePath( _resourcePaths.machine ),
            perf : this.calcResourcePath( _resourcePaths.perf ),
            ras : this.calcResourcePath( _resourcePaths.ras ),
            serviceActions : this.calcResourcePath( _resourcePaths.serviceActions ),

            bgws : {
                sessions: this.calcResourcePath( _resourcePaths.bgws.sessions )
            },

            diagnostics : {
                runs : this.calcResourcePath( _resourcePaths.diagnostics.runs ),
                locations : this.calcResourcePath( _resourcePaths.diagnostics.locations ),
                testcases : this.calcResourcePath( _resourcePaths.diagnostics.testcases )
              },

            environmentals : {
                bulkPower : this.calcResourcePath( _resourcePaths.environmentals.bulkPower ),
                bulkPowerTemp : this.calcResourcePath( _resourcePaths.environmentals.bulkPowerTemp ),
                coolant : this.calcResourcePath( _resourcePaths.environmentals.coolant ),
                fan : this.calcResourcePath( _resourcePaths.environmentals.fan ),
                ioCard : this.calcResourcePath( _resourcePaths.environmentals.ioCard ),
                linkChip : this.calcResourcePath( _resourcePaths.environmentals.linkChip ),
                node : this.calcResourcePath( _resourcePaths.environmentals.node ),
                nodeCard : this.calcResourcePath( _resourcePaths.environmentals.nodeCard ),
                optical : this.calcResourcePath( _resourcePaths.environmentals.optical ),
                serviceCard : this.calcResourcePath( _resourcePaths.environmentals.serviceCard ),
                serviceCardTemp : this.calcResourcePath( _resourcePaths.environmentals.serviceCardTemp )
              },

            summary : {
                machine : this.calcResourcePath( _resourcePaths.summary.machine ),
                system : this.calcResourcePath( _resourcePaths.summary.system ),
                jobs : this.calcResourcePath( _resourcePaths.summary.jobs ),
                ras : this.calcResourcePath( _resourcePaths.summary.ras ),
                utilization : this.calcResourcePath( _resourcePaths.summary.utilization )
              }
          };


        this._blocks_service = this._createService( this.resourceUris.blocks + "/" );
        this._jobs_service = this._createService( this.resourceUris.jobs + "/" );
    },


    calcResourcePath : function( resource_path_part )  { return (this._base_path + "/" + resource_path_part); },


    /** Returns truthy iff there is a session in progress. */
    hasSession : function()
    {
        return this._session_header;
    },

    /** Returns an object to be mixed in to the request headers, or null if no current session. */
    getSessionHeaderObj : function()
    {
        return this._session_header;
    },


    notifyfetchError : function( error )
    {
        console.log( module.id + ": FETCH ERROR. error=", error );

        var error_type_opt = this._checkFetchErrorSessionInvalid( error );

        if ( ! error_type_opt ) {
            // Nothing to do.
            return;
        }

        this._sessionEnded( error_type_opt );
    },


    /** Creates a dojo/store-style store (new style) */
    createStore : function( args )
    {
        return new b_BgwsStore( args, this );
    },


    /** Creates a dojo/data-style store (old style) */
    createDataStore : function( args )
    {
        return b_BgwsStore.createObjectStore( args, this );
    },


    _alerts_data_store : null,

    getAlertsDataStore : function()
    {
        if ( ! this._alerts_data_store ) {
            this._alerts_data_store = this.createDataStore( {
                    target: _resourcePaths.alerts
                } );
        }

        return this._alerts_data_store;
    },


    _blocks_data_store : null,

    getBlocksDataStore : function()
    {
        if ( ! this._blocks_data_store ) {
            this._blocks_data_store = this.createDataStore( {
                    target: _resourcePaths.blocks
                } );
        }

        return this._blocks_data_store;
    },


    _hardware_replacements_data_store : null,

    getHardwareReplacementsDataStore : function()
    {
        if ( ! this._hardware_replacements_data_store ) {
            this._hardware_replacements_data_store = this.createDataStore( {
                    target: _resourcePaths.hardwareReplacements
                } );
        }

        return this._hardware_replacements_data_store;
    },


    _jobs_data_store : null,

    getJobsDataStore : function()
    {
        if ( ! this._jobs_data_store ) {
            this._jobs_data_store = this.createDataStore( {
                target: _resourcePaths.jobs
            } );
        }

        return this._jobs_data_store;
    },


    _ras_data_store : null,

    getRasDataStore : function()
    {
        if ( ! this._ras_data_store ) {
            this._ras_data_store = this.createDataStore( {
                    target: _resourcePaths.ras
                } );
        }

        return this._ras_data_store;
    },


    _service_actions_store : null,

    getServiceActionsStore : function()
    {
        if ( ! this._service_actions_store ) {
            this._service_actions_store = this.createDataStore( {
                    target: _resourcePaths.serviceActions
                } );
        }

        return this._service_actions_store;
    },


    _diagnostics_locations_data_store : null,

    getDiagnosticsLocationsDataStore : function()
    {
        if ( ! this._diagnostics_locations_data_store ) {
            this._diagnostics_locations_data_store = this.createDataStore( {
                    target: _resourcePaths.diagnostics.locations
                } );
        }

        return this._diagnostics_locations_data_store;
    },


    _diagnostics_runs_data_store : null,

    getDiagnosticsRunsDataStore : function()
    {
        if ( ! this._diagnostics_runs_data_store ) {
            this._diagnostics_runs_data_store = this.createDataStore( {
                    target: _resourcePaths.diagnostics.runs,
                    idProperty: "runId"
                } );
        }

        return this._diagnostics_runs_data_store;
    },


    _sessionEnded : function( error_type )
    {
        if ( ! this._session_header ) {
            console.log( module.id + ": Session is already invalid." );
            return;
        }

        // Invalidate the session.
        this._session_header = null;

        // Notify any listeners...
        d_topic.publish( _sessionEndedTopic, { errorType: error_type } );
    },


    _createService : function( uri )
    {
        return x_rpc_Rest(
                uri,
                true /* JSON */,
                null /* schema */,
                d_lang.hitch( this, this._getRequest, uri )
            );
    },


    /** Start a BGWS session.
     *
     *  @return deferred
     */
    startSession: function(
            /**String*/ username,
            /**String*/ password
        )
    {
        this._session_header = null;

        console.log( module.id + ": sending login request as " + username );

        // Create an array with "username:password" converted to ascii codes, then base64-encode it and send it to server.

        var auth_arr = [];
        for ( var i = 0 ; i < username.length ; ++i )  auth_arr.push( username.charCodeAt( i ) );
        auth_arr.push( ":".charCodeAt( 0 ) ); // : converted to ascii code.
        for ( var i = 0 ; i < password.length ; ++i )  auth_arr.push( password.charCodeAt( i ) );

        var auth_b64 = x_base64.encode( auth_arr );

        return this.startSessionAuth( auth_b64 );
    },

    /** Start a BGWS session given auth info.
     *
     *  @return deferred
     */
    startSessionAuth: function( /**String*/ auth_b64 )
    {
        var userinfo_json_text = d_json.stringify( { auth: auth_b64 } );

        var req_promise = d_request(
                this.resourceUris.bgws.sessions,
                {
                    method: "POST",
                    headers: _postHeaders,
                    data: userinfo_json_text,
                    handleAs: "json"
                }
            );

        var promise2 = req_promise.response.then(
                d_lang.hitch( this, this._onLoginComplete ),
                d_lang.hitch( this, this._onLoginFailure )
            );

        return promise2;
    },

    getUserInfo : function()
    {
        return this._user_info;
    },

    /** End the BGWS session. */
    endSession: function()
    {
        if ( ! this._session_header ) {
            return;
        }

        var req_promise = d_request(
                (this.resourceUris.bgws.sessions + "/current"),
                {
                    method: "DELETE",
                    headers:  d_lang.mixin( {},
                            _acceptJsonHeader,
                            this._session_header
                        ),
                    handleAs: "json",
                    sync: true
                }
            );

        this._session_header = null;

        return req_promise;
    },


    /** Create a block
     *
     *  @param create_block_info  block info as allowed by BGWS.
     */
    createBlock: function( /**Object*/ create_block_info ) /**dojo.Deferred*/
    {
        var block_json_text = d_json.stringify( create_block_info );

        console.log( module.id + ": posting create block to " + this.resourceUris.blocks + ": " + block_json_text );

        var req_promise = d_request(
                this.resourceUris.blocks,
                {
                    method: "POST",
                    headers: d_lang.mixin( {},
                            _postHeaders,
                            this._session_header
                        ),
                    data: block_json_text,
                    handleAs: "json"
                }
            );

        var promise = req_promise.then(
                d_lang.hitch( this, function( response ) {
                    console.log( module.id + ": successfully created block '" + create_block_info.id + "'" );
                    return response;
                } ),
                d_lang.hitch( this, function( error_data ) {
                    console.log( module.id + ": create block failed. error_data=", error_data );
                    this._processRequestError( error_data );
                } )
            );

        return promise;
    },

    deleteBlock: function( id )
    {
        var def = this._blocks_service['delete']( id );

        d_when( def,
                null,
                d_lang.hitch( this, function( error ) {
                    console.log( module.id + ": delete block failed. error=", error );
                    this._processErrorResponse( error, def );
                } )
            );

        return def;
    },

    fetchBlockDetails: function( id )
    {
        var def = this._blocks_service( id );

        d_when( def,
                null,
                d_lang.hitch( this, function( error ) {
                    console.log( module.id + ": fetch block details failed. error=", error );
                    this._processErrorResponse( error, def );
                } )
            );

        return def;
    },

    fetchJobDetails: function( id )
    {
        var def = this._jobs_service( id );

        d_when( def,
                null,
                d_lang.hitch( this, function( error ) {
                    console.log( module.id + ": fetch job details failed. error=", error );
                    this._processErrorResponse( error, def );
                } )
            );

        return def;
    },


    fetch: function( resource_path_part )
    {
        console.log( module.id + ": fetching", resource_path_part );

        var resource_path = this.calcResourcePath( resource_path_part );

        var req_promise = d_request(
                resource_path,
                {
                    headers: d_lang.mixin( {},
                            _acceptJsonHeader,
                            this._session_header
                        ),
                    handleAs: "json"
                }
            );

        req_promise.then(
                null, // ignore success.
                d_lang.hitch( this, function( error_data ) {
                    console.log( module.id + ": fetch " + resource_path + " failed. error_data=", error_data );
                    this._processRequestError( error_data );
                } )
            );

        return req_promise;
    },


    fetchPerformanceData: function( args )
    {
        var query_str = "";

        for ( n in args.query ) {
            if ( query_str === "" )  query_str = "?";
            else  query_str += "&";

            query_str += encodeURIComponent( n ) + "=" + encodeURIComponent( args.query[n] );
        }

        console.log( module.id + ": fetchPerformanceData args=", args, " url=", (this.resourceUris.perf + query_str) );

        var req_promise = d_request(
                (this.resourceUris.perf + query_str),
                {
                    headers: d_lang.mixin( args.headers,
                            _acceptJsonHeader,
                            this._session_header
                        ),
                    handleAs: "json"
                }
            );

        req_promise.then(
                null, // ignore success.
                d_lang.hitch( this, function( error_data ) {
                    console.log( module.id + ": fetch perf data failed. error_data=", error_data );
                    this._processRequestError( error_data );
                } )
            );

        return req_promise;
    },


    submitDiagnosticsRun: function( submit_data )
    {
        var json_str = d_json.stringify( submit_data );

        console.log( module.id + ": posting submit diagnostics to " + this.resourceUris.diagnostics.runs + ": " + json_str );

        var req_promise = d_request(
                this.resourceUris.diagnostics.runs,
                {
                    method: "POST",
                    headers: d_lang.mixin( {},
                            _postHeaders,
                            this._session_header
                        ),
                    data: json_str,
                    handleAs: "json"
                }
            );

        var promise = req_promise.response.then(

                // successful callback.
                d_lang.hitch( this, function( response ) {
                    var location = response.getHeader( "Location" );
                    console.log( module.id + ": successfully started diagnostics", location );

                    var location_parts = location.split( "/" );

                    var run_id = unescape( location_parts.pop() );

                    return { runId: run_id };
                } ),

                // Error callback.
                d_lang.hitch( this, function( error_data ) {
                    console.log( module.id + ": start diagnostics failed error_data=", error_data );

                    this._processRequestError( error_data );
                } )

            );

        return promise;
    },


    cancelDiagnosticsRun: function( run_id )
    {
        var run_uri = (this.resourceUris.diagnostics.runs + "/" + run_id);

        console.log( module.id + ": cancel submit diagnostics to " + run_uri );

        var json_str = d_json.stringify( { "operation" : "cancel" } );

        var req_promise = d_request(
                run_uri,
                {
                    method: "POST",
                    headers: d_lang.mixin( {},
                            _postHeaders,
                            this._session_header
                        ),
                    data: json_str,
                    handleAs: "json"
                }
            );

        var promise = req_promise.then(

                // successful callback.
                d_lang.hitch( this, function() {
                    console.log( module.id + ": cancel diagnostics run " + run_id + " successful.", arguments );
                    return { runId: run_id };
                } ),

                // Error callback.
                d_lang.hitch( this, function( error_data ) {
                    console.log( module.id + ": cancel diagnostics failed error_data=", error_data );

                    this._processRequestError( error_data );
                } )
            );

        return promise;
    },


    submitPrepareForService : function( location )
    {
        var submit_obj = { location: location };

        var json_str = d_json.stringify( submit_obj );

        console.log( module.id + ": posting submit prepare for service to " + this.resourceUris.serviceActions + ": " + json_str );

        var req_promise = d_request(
                this.resourceUris.serviceActions,
                {
                    method: "POST",
                    headers: d_lang.mixin( {},
                            _postHeaders,
                            this._session_header
                        ),
                    data: json_str,
                    handleAs: "json"
                }
            );

        var promise = req_promise.response.then(

                // successful callback.
                d_lang.hitch( this, function( response ) {
                    var location = response.getHeader( "Location" );
                    console.log( module.id + ": successfully started prepare service action", location );

                    var location_parts = location.split( "/" );

                    var id = unescape( location_parts.pop() );

                    return { id: id };
                } ),

                // Error callback.
                d_lang.hitch( this, function( error_data ) {
                    console.log( module.id + ": start service action failed error_data=", error_data );

                    this._processRequestError( error_data );
                } )
            );

        return promise;
    },

    submitEndServiceAction : function( id )
    {
        return this._submitServiceActionOperation( id, "end" );
    },

    submitCloseServiceAction : function( id )
    {
        return this._submitServiceActionOperation( id, "close" );
    },


    closeAlert : function( id )
    {
        var alert_uri = (this.resourceUris.alerts + "/" + id);

        var submit_obj = { "operation" : "close" };

        var json_str = d_json.stringify( submit_obj );

        console.log( module.id + ": posting submit close alert to " + alert_uri + ": " + json_str );

        var req_promise = d_request(
                alert_uri,
                {
                    method: "POST",
                    headers: d_lang.mixin( {},
                            _postHeaders,
                            this._session_header
                        ),
                    data: json_str,
                    handleAs: "json"
                }
            );

        var promise = req_promise.then(
                // successful callback.
                d_lang.hitch( this, function() {
                    console.log( module.id + ": successfully closed alert", id );

                    return;
                } ),

                // Error callback.
                d_lang.hitch( this, function( error_data ) {
                    console.log( module.id + ": close alert failed error=", error_data );

                    this._processRequestError( error_data );
                } )
            );

        return promise;
    },


    removeAlert : function( id )
    {
        var alert_uri = (this.resourceUris.alerts + "/" + id);

        console.log( module.id + ": removing alert, DELETE " + alert_uri );

        var req_promise = d_request(
                alert_uri,
                {
                    method: "DELETE",
                    headers: d_lang.mixin( {},
                            _acceptJsonHeader,
                            this._session_header
                        ),
                    handleAs: "json"
                }
            );

        var promise = req_promise.then(

                // successful callback.
                d_lang.hitch( this, function() {
                    console.log( module.id + ": successfully removed alert", id );

                    return;
                } ),

                // Error callback.
                d_lang.hitch( this, function( error_data ) {
                    console.log( module.id + ": remove alert failed error_data=", error_data );

                    this._processRequestError( error_data );
                } )
            );

        return promise;
    },


    _getRequest: function( path, id, args )
    {
        var url = path;

        if( d_lang.isObject( id ) ) {

            var query_obj = {};

            d_lang.mixin( query_obj, id );

            if( args && args.sort && (! args.queryStr) ){
                query_obj.sort = [];
                for( var i = 0 ; i < args.sort.length ; i++ ) {
                    var sort = args.sort[i];

                    query_obj.sort.push( (sort.descending ? "-" : "") + sort.attribute );
                }
            }

            var query_str = d_io_query.objectToQuery( query_obj );
            url += query_str.length === 0 ? "" : ("?" + query_str);

        } else {
            url += id;
        }

        var request = {
                url: url,
                handleAs: 'json',
                contentType: _jsonMediaType,
                headers: d_lang.mixin( {},
                        _acceptJsonHeader,
                        this._session_header
                    )
            };
        if ( args && (args.start >= 0 || args.count >= 0) ) {
            request.headers.Range = "items=" + (args.start || '0') + '-' + ((args.count && args.count != Infinity && (args.count + (args.start || 0) - 1)) || '');
        }
        return request;
    },


    _onLoginComplete: function( response )
    {
        // Extract the session ID from the Location header in the response.
        var location = response.getHeader( "Location" );
        var location_parts = location.split( "/" );

        var session_id = unescape( location_parts.pop() );

        this._session_header = {};
        this._session_header[_sessionIdHeaderName] = session_id;

        console.log( module.id + ": login successful, session ID:", session_id, "response:", response );

        this._user_info = new b_UserInfo( response.data );

        return { sessionId: session_id, response: response };
    },

    _onLoginFailure: function( error_data )
    {
        console.log( module.id + ": Login failed. error_data=", error_data );

        this._session_header = null;

        // figure out if was invalid username and password or some other problem,
        // it's invalid username and password if status is "400 Bad Request" and get back BGWS error response and error ID is "invalidUserPass".

        var error_type;

        var response = error_data.response;

        console.log( module.id + ": status=", response.status, "Content-Type=", response.getHeader( "Content-Type" ) );

        if ( (response.status === 400) && (response.getHeader( "Content-Type" ) === _jsonMediaType) ) {

            error_type = (response.data.id === "invalidUserPass" ? _loginError.INVALID_AUTHORIZATION : _loginError.BGWS_NOT_AVAILABLE);

        } else {

            error_type = _loginError.BGWS_NOT_AVAILABLE;

        }

        var new_e = new Error( "login failed" );
        new_e.errorType = error_type;
        new_e.response = response;

        throw new_e;
    },


    /** Processes an error response from a dojo/xhr request. Always throws. */
    _processErrorResponse: function( /**Error*/ error, /**Dojo.Deferred*/ deferred )
    {
        var text = _calculateXhrErrorMessage( error, deferred.ioArgs );

        var error_type_opt = this._checkDeferredErrorSessionInvalid( deferred );
        if ( error_type_opt ) {
            this._sessionEnded( error_type_opt );
        }

        throw new Error( text );
    },

    /** Processes an error response from a dojo/request request. Always throws. */
    _processRequestError : function( error_data )
    {
        var text = _calculateRequestErrorMessage( error_data );

        if ( "response" in error_data ) {
            var error_type_opt = _checkRequestSessionInvalid( error_data.response );
            if ( error_type_opt ) {
                this._sessionEnded( error_type_opt );
            }
        }

        throw new Error( text );
    },


    /** Returns bluegene.Bgws.loginError if error indicates the session is no longer valid and should be ended, otherwise returns undefined. */
    _checkDeferredErrorSessionInvalid : function( deferred )
    {
        var xhr = deferred.ioArgs.xhr;

        var content_type = xhr.getResponseHeader( "Content-Type" );

        return _checkXhrSessionInvalid( {
                status: xhr.status,
                contentType: content_type,
                responseText: xhr.responseText
            } );
    },

    /** Returns message if error indicates the session is no longer valid and should be ended, otherwise returns undefined. */
    _checkFetchErrorSessionInvalid : function( error )
    {
        return _checkXhrSessionInvalid( {
                status: error.status,
                responseText: error.responseText
            } );
    },


    _submitServiceActionOperation : function( id, operation_name )
    {
        var service_action_uri = (this.resourceUris.serviceActions + "/" + id);

        var submit_obj = { "operation" : operation_name };

        var json_str = d_json.stringify( submit_obj );

        console.log( module.id + ": posting submit " + operation_name + " service action to " + service_action_uri + ": " + json_str );

        var req_promise = d_request(
                service_action_uri,
                {
                    method: "POST",
                    headers: d_lang.mixin( {},
                            _postHeaders,
                            this._session_header
                        ),
                    data: json_str,
                    handleAs: "json"
                }
            );

        var promise = req_promise.then(

                // successful callback.
                d_lang.hitch( this, function() {
                    console.log( module.id + ": successfully started end service action", id );

                    return {};
                } ),

                // Error callback.
                d_lang.hitch( this, function( error_data ) {
                    console.log( module.id + ": end service action failed error_data=", error_data );

                    this._processRequestError( error_data );
                } )
            );

        return promise;
    }

} );


/* Class data members */

b_Bgws.resourcePaths = _resourcePaths;


b_Bgws.sessionIdHeaderName = _sessionIdHeaderName;

b_Bgws.jsonMediaType = _jsonMediaType;

b_Bgws.acceptJsonHeader = _acceptJsonHeader;

b_Bgws.postHeaders = _postHeaders;

b_Bgws.HTTP_STATUS_BAD_REQUEST = _HTTP_STATUS_BAD_REQUEST;
b_Bgws.HTTP_STATUS_SERVICE_TEMPORARILY_UNAVAILABLE = _HTTP_STATUS_SERVICE_TEMPORARILY_UNAVAILABLE;

b_Bgws.ERROR_ID_INVALID_SESSION = _ERROR_ID_INVALID_SESSION;


b_Bgws.calculateErrorMessage = _calculateXhrErrorMessage;


/**
 *  Parse a BGWS timestamp.
 *  @memberOf bluegene^Bgws
 */
b_Bgws.parseTimestamp = function( /**string*/ timestamp_str )
    {
        if ( ! timestamp_str )  return;

        var matches = _timestampRegex.exec( timestamp_str );
        var yy = Number( matches[1] );
        var MM = Number( matches[2] );
        var dd = Number( matches[3] );
        var hh = Number( matches[4] );
        var mm = Number( matches[5] );
        var ss = Number( matches[6] );

        return new Date( yy, MM - 1, dd, hh, mm, ss );
    };


b_Bgws.loginError = _loginError;


/**
 *  Data is { errorType: error_type }.
 *  @memberOf bluegene^Bgws
 */
b_Bgws.sessionEndedTopic = _sessionEndedTopic;


return b_Bgws;

} );
