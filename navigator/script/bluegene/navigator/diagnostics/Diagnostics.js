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
    "../AbstractTab",
    "./ConfigureIoRun",
    "./ConfigureRun",
    "./Details",
    "./Locations",
    "./Runs",
    "./Summary",
    "../topic",
    "../dijit/Diagnostics",
    "dojo/request",
    "dojo/topic",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        b_navigator_AbstractTab,
        l_ConfigureIoRun,
        l_ConfigureRun,
        l_Details,
        l_Locations,
        l_Runs,
        l_Summary,
        ll_topic,
        b_navigator_dijit_Diagnostics,
        d_request,
        d_topic,
        d_array,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_diagnostics_Diagnostics = d_declare( [ b_navigator_AbstractTab ],

/** @lends bluegene^navigator^diagnostics^Diagnostics# */
{
    _bgws : null,
    _navigator : null,

    _diagnostics_dij : null,

    _configure_compute_run : null,
    _configure_io_run : null,

    _got_properties : false,


    /** @constructs */
    constructor : function( bgws, navigator )
    {
        this._bgws = bgws;
        this._navigator = navigator;

        new l_Details( this._bgws, this );
        new l_Locations( this._bgws, this );
        new l_Runs( this._bgws, this );
        new l_Summary( this._bgws, this );
        this._configure_io_run = new l_ConfigureIoRun( this._bgws, this );
        this._configure_compute_run = new l_ConfigureRun( this._bgws, this );
    },


    addTab : function( new_tab )
    {
        this._diagnostics_dij.addTab( new_tab );
    },


    removeTab : function( tab )
    {
        this._diagnostics_dij.removeTab( tab );
    },


    getSummaryDijit : function()
    {
        return this._diagnostics_dij.summaryTab;
    },

    getLocationsDijit : function()
    {
        return this._diagnostics_dij.locationsTab;
    },

    getRunsDijit : function()
    {
        return this._diagnostics_dij.runsTab;
    },

    getDetailsDijit : function()
    {
        return this._diagnostics_dij.detailsTab;
    },


    switchToDetails : function()
    {
        this._diagnostics_dij.switchToDetails();
    },


    updateMachineHighlighting : function()
    {
        this._updateMachineHighlighting();
    },


    // override from AbstractTab
    _getRequiredAuthority: function()  { return b_navigator_AbstractTab.requiredAuthority.HARDWARE_READ; },

    // override from AbstractTab
    _getMachineHighlightData : function()
    {
        var cur_tab = this._diagnostics_dij.tabContainer.selectedChildWidget;

        if ( cur_tab === this._configure_compute_run.getDijit() ) {
            return this._configure_compute_run.getMachineHighlightData();
        }

        if ( cur_tab === this._configure_io_run.getDijit() ) {
            return this._configure_io_run.getMachineHighlightData();
        }

        return null;
    },


    // override AbstractTab
    _loggedIn : function( args )
    {
        if ( args.userInfo.hasHardwareRead() ) {
            // User has authority to Diagnostics info.

            if ( this._diagnostics_dij ) {

                console.log( module.id + ": User has authority to diagnostics tab (already displayed)." );


            } else {

                console.log( module.id + ": User has authority to diagnostics, showing tab" );

                this._diagnostics_dij = new b_navigator_dijit_Diagnostics( { title: "Diagnostics" } );

                var TAB_PRIORITY = 110;
                var TAB_NAME = "diags";

                this._navigator.addTab( this._diagnostics_dij, TAB_PRIORITY, TAB_NAME, this );


                // Listen for the current tab to change.
                var select_subtab_topic = (this._diagnostics_dij.tabContainer.id + "-selectChild");
                d_topic.subscribe( select_subtab_topic, d_lang.hitch( this, this._diagsTabChanged ) );
            }

            ll_topic.publish( ll_topic.diags.loggedIn, args.userInfo.isAdministrator() );


            if ( (! this._got_properties) && args.userInfo.isAdministrator() ) {

                console.log( module.id + ": Fetching the tests.properties file." );

                // Load the tests.properties file.

                var promise = d_request( "resources/diags/tests.properties" );

                promise.then(
                        d_lang.hitch( this, this._gotTestsProperties )
                    );

                this._got_properties = true;

            }

        } else {
            // User doesn't have authority to Diagnostics info.

            ll_topic.publish( ll_topic.diags.loggedIn, null );

            if ( this._diagnostics_dij ) {

                console.log( module.id + ": User does not have authority to diagnostics and displayed, hiding." );

                this._navigator.removeTab( this._diagnostics_dij );

                this._diagnostics_dij = null;

            } else {

                console.log( module.id + ": User does not have authority to diagnostics and already hidden." );

            }
        }
    },


    // override AbstractTab
    notifyMachineClicked : function( loc )
    {
        var cur_tab = this._diagnostics_dij.tabContainer.selectedChildWidget;

        if ( cur_tab === this._configure_compute_run.getDijit() ) {
            this._configure_compute_run.notifyMachineClicked( loc );
        }

        if ( cur_tab === this._configure_io_run.getDijit() ) {
            this._configure_io_run.notifyMachineClicked( loc );
        }
    },


    _diagsTabChanged : function( widget )
    {
        this._updateMachineHighlighting();
    },


    _gotTestsProperties : function( result )
    {
        // console.log( module.id + ": Got tests.properties", result );

        var config = this._parseTestsProperties( result );

        ll_topic.publish( ll_topic.diags.gotTestsConfiguration, config );
    },


    _parseTestsProperties : function( properties_contents )
    {
        var buckets = {};
        var tests = {};

        var cur_obj = {};

        var lines = properties_contents.split( "\n" );

        d_array.forEach( lines, function( line ) {
            // console.log( module.id + ": line from tests.properties: " + line );

            // remove everything after #.
            line = line.replace( /#.*/, "" );

            var section_name_m = /\[(.*)\]/.exec( line );
            if ( section_name_m ) {

                // Set up the new section.

                var section_name = section_name_m[1];

                var split_section_name_m = /(.*)\.(.*)/.exec( section_name );

                var type = split_section_name_m[1];
                var name = split_section_name_m[2];

                // console.log( module.id + ": new section " + section_name + " -> type:" + type + " name:" + name );

                cur_obj = {};

                if ( type == "buckets" ) {
                    buckets[name] = cur_obj;
                } else {
                    tests[name] = cur_obj;
                }

                return; // break
            }

            var property_m = /\s*(\S+)\s*=\s*(.*)/.exec(line);
            if ( property_m ) {
                var prop_name = property_m[1];
                var prop_val = property_m[2];

                // console.log( module.id + ": new property '" + prop_name + "' -> '" + prop_val + "'" );

                if ( prop_name == "tests" ) {
                    prop_val = prop_val.split( /\s*,\s*/ );
                } else if ( prop_name == "timeout" ) {
                    prop_val = Number( prop_val );
                }

                cur_obj[prop_name] = prop_val;

                return;
            }
        } );

        return {
                buckets: buckets,
                tests: tests
            };
    }

} );


return b_navigator_diagnostics_Diagnostics;

} );
