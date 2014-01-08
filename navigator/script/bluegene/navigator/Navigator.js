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
    "./Alerts",
    "./Blocks",
    "./BlockBuilder",
    "./Charts",
    "./Environmentals",
    "./EventsMonitorMixin",
    "./Hardware",
    "./HardwareReplacements",
    "./IoBlocks",
    "./Jobs",
    "./JobHistory",
    "./Login",
    "./Machine",
    "./PerformanceMonitoring",
    "./PrepareServiceAction",
    "./Ras",
    "./ServiceActions",
    "./SystemSummary",
    "./topic",
    "./diagnostics/Diagnostics",
    "../Bgws",
    "../System",
    "dojo/dom-construct",
    "dojo/io-query",
    "dojo/query",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dijit/registry",
    "dijit/Tooltip",
    "module"
],
function(
        l_Alerts,
        l_Blocks,
        l_BlockBuilder,
        l_Charts,
        l_Environmentals,
        l_EventsMonitorMixin,
        l_Hardware,
        l_HardwareReplacements,
        l_IoBlocks,
        l_Jobs,
        l_JobHistory,
        l_Login,
        l_Machine,
        l_PerformanceMonitoring,
        l_PrepareServiceAction,
        l_Ras,
        l_ServiceActions,
        l_SystemSummary,
        l_topic,
        l_diagnostics_Diagnostics,
        b_Bgws,
        b_System,
        d_construct,
        d_ioquery,
        d_query,
        d_declare,
        d_lang,
        j_registry,
        j_Tooltip,
        module
    )
{


var b_navigator_Navigator = d_declare( null,

/** @lends bluegene^navigator^Navigator# */
{
    _navigator_dij : null,

    _initial_tab_name : null,

    _bgws: null,
    _login: null,

    _system: null,

    _environmentals : null,

    _header_dij : null,
    _footer_dij : null,

    _tab_id_to_controller : null,
    _current_controller : null,

    _location_search_obj : null,


    /**
     *  ctor
     *
     *  @class Navigator web application UI.
     *  @constructs
     */
    constructor: function()
    {
        this._navigator_dij = j_registry.byId( "navigator" );

        this._navigator_dij.on( "tabChanged", d_lang.hitch( this, this._tabChanged ) );


        // Act on the URL.

        this._location_search_obj = {};

        if ( window.location.search.length > 1 && window.location.search[0] === "?" ) {
            var search_str = window.location.search.substr( 1 );
            this._location_search_obj = d_ioquery.queryToObject( search_str );

            console.log( module.id + ": search=" + window.location.search + " -> search_obj=", this._location_search_obj );
        }


        var location_hash_obj = null;

        if ( window.location.hash.length > 1 && window.location.hash[0] === "#" ) {
            var hash_str = window.location.hash.substr( 1 );
            location_hash_obj = d_ioquery.queryToObject( hash_str );

            console.log( module.id + ": hash=" + window.location.hash + " -> hash_obj=", location_hash_obj );
        }

        if ( location_hash_obj != null && "tab" in location_hash_obj ) {
            this._initial_tab_name = location_hash_obj.tab;
        }


        var machine_region = "left";

        if ( this._location_search_obj.machineRegion )  machine_region = this._location_search_obj.machineRegion;

        this._navigator_dij.setMachineRegion( machine_region );


        this._header_dij = this._navigator_dij.header;
        this._header_dij.on( "endSession", d_lang.hitch( this, this._endSessionRequested ) );

        this._footer_dij = this._navigator_dij.footer;
        this._footer_dij.on( "refresh", function() { l_EventsMonitorMixin.refresh(); } );


        // Setup global stuff.

        var bgws_args = {};

        if ( this._location_search_obj && this._location_search_obj.bgwsPath ) {
            bgws_args.path = this._location_search_obj.bgwsPath;
        }

        this._bgws = new b_Bgws( bgws_args );


        // Create controllers for all the parts.

        this._login = new l_Login(
                this._bgws,
                this._location_search_obj
            );

        new l_Charts( this._bgws );
        new l_Machine();

        this._tab_id_to_controller = {};

        l_topic.subscribe( l_topic.tabControllerChange, d_lang.hitch( this, this._onTabControllerChange ) );

        new l_Alerts( this._bgws, this );
        new l_BlockBuilder( this._bgws, this );
        new l_Blocks( this._bgws );
        new l_diagnostics_Diagnostics( this._bgws, this );
        this._environmentals = new l_Environmentals( this._bgws, this );
        new l_Hardware( this._bgws );
        new l_HardwareReplacements( this._bgws, this );
        new l_IoBlocks( this._bgws );
        new l_JobHistory( this._bgws );
        new l_Jobs( this._bgws );
        new l_PerformanceMonitoring( this._bgws, this );
        new l_PrepareServiceAction( this._bgws, this );
        new l_Ras( this._bgws, this );
        new l_ServiceActions( this._bgws, this );
        new l_SystemSummary( this._bgws );


        // Start listening for global events.

        l_topic.subscribe( l_topic.loggedIn, d_lang.hitch( this, this._loggedIn ) );
        l_topic.subscribe( l_topic.loginComplete, d_lang.hitch( this, this._loginComplete ) );
        l_topic.subscribe( l_topic.midplane, d_lang.hitch( this, this._machineClicked ) );
        l_topic.subscribe( l_topic.updateMachineHighlighting, d_lang.hitch( this, this._updateMachineHighlighting ) );
        l_topic.subscribe( b_Bgws.sessionEndedTopic, d_lang.hitch( this, this._bgwsSessionEnded ) );

        // Start login prompt.

        this._login.start();


        if ( this._location_search_obj && this._location_search_obj.dashboard && this._location_search_obj.dashboard === "hidden" ) {
            this._footer_dij.setChartsVisible( false );
        }


        var cur_tab_id = this._navigator_dij.getSelectedTab().id;
        this._current_controller = this._tab_id_to_controller[cur_tab_id];
    },


    addTab : function( dijit, priority, name, controller )
    {
        this._navigator_dij.addTab( dijit, priority, name );

        this._onTabControllerChange( { id: dijit.id, controller: controller } );
    },


    removeTab : function( dijit )
    {
        this._onTabControllerChange( { id: dijit.id } );

        this._navigator_dij.removeTab( dijit );
    },


    _loggedIn : function( args )
    {
        this._header_dij.setUserName( args.userName );

        var user_info = args.userInfo;

        var user_tooltip_text = "";


        if ( args.userName === "tbudnik" ) {
            user_tooltip_text = "<img src='resources/image/user_info.jpeg' style='height: 78px; width: 78px;'/>";
        } else {
            if ( user_info.isAdministrator() ) {
                user_tooltip_text = "User is Blue Gene Administrator";
            } else {

                var authorities = [];

                if ( user_info.hasHardwareRead() ) {
                    authorities.push( "hardware read" );
                }
                if ( user_info.hasBlockCreate() ) {
                    authorities.push( "block create" );
                }

                if ( authorities.length > 0 ) {
                    user_tooltip_text = "User has " + authorities.join( ", " );
                } else {
                    user_tooltip_text = "User has no special authorities";
                }
            }
        }

        this._header_dij.setUserAuthDesc( user_tooltip_text );


        if ( ! this._system ) {
            this._system = new b_System(
                    this._bgws,
                    d_lang.hitch( this, this._gotSystemInfo )
                );

            this._system.fetch();
        }
    },


    _loginComplete : function()
    {
        console.log( module.id + ": login complete..." );

        if ( this._initial_tab_name ) {

            if ( this._initial_tab_name === "block-builder" ) {
                l_topic.publish( l_topic.displayBlockBuilder );
            } else if ( this._initial_tab_name === "prepare-service-action" ) {
                l_topic.publish( l_topic.displayPrepareServiceAction );
            } else {
                this._navigator_dij.switchTo( this._initial_tab_name );
            }

            this._initial_tab_name = null;

        }

    },


    _gotSystemInfo: function()
    {
        var machine_info = this._system.getMachineInfo();

        this._header_dij.setSystemName( machine_info.name );
        document.title = machine_info.name;

        l_topic.publish( l_topic.gotMachineInfo, { machineInfo: machine_info } );


        // Set the measurement system (US or SI).

        // If the value is set in the URL (via ?measurements=) then use that,
        // otherwise, if the machine info has a setting use that,
        // otherwise, default to US.

        var measurement_system = "US";

        if ( this._location_search_obj && this._location_search_obj.measurements ) {
            measurement_system = this._location_search_obj.measurements;
        } else if ( machine_info.measurementSystem ) {
            measurement_system = machine_info.measurementSystem;
        }

        this._environmentals.setMeasurementSystem( measurement_system );
    },

    _endSession: function()
    {
        this._bgws.endSession();
    },

    _endSessionRequested : function()
    {
        l_EventsMonitorMixin.endSession();

        console.log( module.id + ": end session requested." );
        this._bgws.endSession();

        this._login.start();
    },

    _bgwsSessionEnded : function( args )
    {
        console.log( module.id + ": notified that session ended. args=", args );

        l_EventsMonitorMixin.endSession();

        this._login.start( args.errorType );
    },

    _hasSession : function()
    {
        // returns true iff there is a session in progress.
        return this._bgws.hasSession();
    },


    _onTabControllerChange : function( args )
    {
        if ( args.controller ) {
            console.log( module.id + ": New tab controller for", args.id,"controller=", args.controller );
            this._tab_id_to_controller[args.id] = args.controller;
        } else {
            console.log( module.id + ": Removed tab controller mapping for", args.id, "was", this._tab_id_to_controller[args.id] );
            delete this._tab_id_to_controller[args.id];
        }
    },


    _tabChanged : function( widget )
    {
        this._current_controller = this._tab_id_to_controller[widget.id];

        if ( ! this._current_controller ) {
            console.log( module.id + ": _tabChanged, no controller for ", widget.id );
            return;
        }

        console.log( module.id + ": _tabChanged. widget=", widget.id, "controller=", this._current_controller );

        var machine_highlighting = this._current_controller.getMachineHighlightData();

        l_topic.publish(
                l_topic.highlightHardware,
                machine_highlighting
            );
    },


    _updateMachineHighlighting : function( controller )
    {
        if ( this._current_controller !== controller ) {
            console.log( module.id + ": Got request to update highlighting from different controller, discarding." );
            return;
        }

        var machine_highlighting = this._current_controller.getMachineHighlightData();

        l_topic.publish(
                l_topic.highlightHardware,
                machine_highlighting
            );
    },

    _machineClicked : function( location )
    {
        console.log( module.id + ": machine clicked @", location, "controller=", this._current_controller );

        if ( ! this._current_controller ) {
            console.log( module.id + ": _machineClicked, no current controller" );
            return;
        }

        this._current_controller.notifyMachineClicked( location );
    }

} );


/* Class data members */

b_navigator_Navigator.instance = null;


/* Class functions */

b_navigator_Navigator.createInstance = function()
{
    b_navigator_Navigator.instance = new b_navigator_Navigator();
};


b_navigator_Navigator.onLoad = function()
{
    console.log( module.id + ": onLoad called..." );

    d_construct.destroy( "overlay" );

    b_navigator_Navigator.createInstance();

    window.onbeforeunload = function( e ) {
            if ( ! b_navigator_Navigator.instance._hasSession() )  return;

            var e2 = e || window.event;
            if ( e2 ) {
            	e2.returnValue = "Leaving this page will end the Navigator session";
            }
            return "Leaving this page will end the Navigator session";
        };

    console.log( module.id + ": onLoad complete." );
};


b_navigator_Navigator.onUnload = function()
{
    b_navigator_Navigator.instance._endSession();
};


return b_navigator_Navigator;

} );
