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
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/text!./templates/Navigator.html",
    "module",

    // Used only in template.
    "./MainArea",
    "./Footer",
    "./Header",
    "dijit/layout/BorderContainer"
],
function(
        l_AbstractTemplatedContainer,
        d_declare,
        d_lang,
        template,
        module
    )
{


var b_navigator_dijit_Navigator = d_declare(
        [ l_AbstractTemplatedContainer ],

{

    templateString : template,


    getMachine : function()  { return this._mainArea.getMachine(); },

    getChartsDij : function()  { return this._footer.charts; },

    getComputeBlocksTabDij : function()  { return this._mainArea.getComputeBlocksTabDij(); },
    getHardwareTabDij : function()  { return this._mainArea.getHardwareTabDij(); },
    getIoBlocksTabDij : function()  { return this._mainArea.getIoBlocksTabDij(); },
    getJobsTabDij : function()  { return this._mainArea.getJobsTabDij(); },
    getJobHistoryTabDij : function()  { return this._mainArea.getJobHistoryTabDij(); },
    getSystemSummaryTabDij : function()  { return this._mainArea.getSystemSummaryTabDij(); },

    getTabDij : function( tab_name )  { return this._mainArea.getTabDij( tab_name ); },

    getComputeBlockDetailsDij : function()  { return this._mainArea.getComputeBlockDetailsDij(); },
    getIoBlockDetailsDij : function()  { return this._mainArea.getIoBlockDetailsDij(); },
    getCurrentJobDetailsDij : function() { return this._mainArea.getCurrentJobDetailsDij(); },
    getHistoryJobDetailsDij : function() { return this._mainArea.getHistoryJobDetailsDij(); },


    onTabChanged : function( widget ) {},


    getSelectedTab : function()  { return this._mainArea.getSelectedTab(); },
    addTab : function( dijit, priority, name )  { this._mainArea.addTab( dijit, priority, name ); },
    removeTab : function( dijit )  { this._mainArea.removeTab( dijit ); },
    switchTo : function( tab_name )  { this._mainArea.switchTo( tab_name ); },


    setMachineRegion : function( new_region )
    {
        this._mainArea.setMachineRegion( new_region );
    },


    onEndSession : function()  {},
    onRefresh : function() {},


    // override
    startup : function()
    {
        this.inherited( arguments );

        this._mainArea.on( "tabChanged", d_lang.hitch( this, this._tabChanged ) );

        this._footer.on( "refresh", d_lang.hitch( this, this._refresh ) );
        this._header.on( "endSession", d_lang.hitch( this, this._endSession ) );
    },


    _setChartsVisibleAttr : function( is_visible )
    {
        this._footer.setChartsVisible( is_visible );
    },


    _setSystemNameAttr : function( system_name )
    {
        this._header.set( "systemName", system_name );
    },


    _setUserInfoAttr : function( args )
    {
        this._header.set( "userInfo", args );
    },


    _tabChanged : function( new_tab )
    {
        console.log( module.id + ": tab changed to", new_tab.id, "dij=", new_tab );
        this.onTabChanged( new_tab );
    },


    _endSession : function()
    {
        this.onEndSession();
    },


    _refresh : function()
    {
        this.onRefresh();
    }

} );

return b_navigator_dijit_Navigator;

} );
