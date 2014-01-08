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
    "./AbstractTab",
    "./dijit/PerformanceMonitoring",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_AbstractTab,
        l_dijit_PerformanceMonitoring,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_PerformanceMonitoring = d_declare( [ l_AbstractTab ],

/** @lends bluegene^navigator^PerformanceMonitoring# */
{
    _bgws: null,
    _navigator : null,

    _performance_monitoring_dij : null,


    /** @constructs */
    constructor : function( bgws, navigator )
    {
        this._bgws = bgws;
        this._navigator = navigator;
    },


    // override from AbstractTab
    _refresh : function()
    {
        if ( this._performance_monitoring_dij ) {
            this._performance_monitoring_dij.notifyRefresh();
        }
    },


    // override from AbstractTab
    _loggedIn : function( args )
    {
        if ( args.userInfo.hasHardwareRead() ) {
            // User has authority to performance monitoring

            if ( this._performance_monitoring_dij ) {

                console.log( module.id + ": User has authority to performance monitoring tab (already displayed)." );

            } else {

                console.log( module.id + ": User has authority to performance monitoring, showing tab" );

                this._performance_monitoring_dij = new l_dijit_PerformanceMonitoring( { title: "Performance Monitoring" } );

                var TAB_PRIORITY = 130;
                var TAB_NAME = "perf";
                this._navigator.addTab( this._performance_monitoring_dij, TAB_PRIORITY, TAB_NAME, this );

            }

            this._performance_monitoring_dij.setFetchPerformanceDataFn( d_lang.hitch( this, this._fetchPerformanceData ) );

        } else {
            // User doesn't have authority to performance monitoring

            if ( this._performance_monitoring_dij ) {

                console.log( module.id + ": User does not have authority to performance monitoring and displayed, hiding." );

                this._navigator.removeTab( this._performance_monitoring_dij );

                this._performance_monitoring_dij = null;

            } else {

                console.log( module.id + ": User does not have authority to performance monitoring and already hidden." );

            }

        }
    },


    _fetchPerformanceData : function( args )
    {
        return this._bgws.fetchPerformanceData( args );
    }

} );


return b_navigator_PerformanceMonitoring;

} );
