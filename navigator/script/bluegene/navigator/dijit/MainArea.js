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
    "./Machine",
    "./MainTabs",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/text!./templates/MainArea.html",
    "module",

    // Used only in template.
    "dijit/layout/BorderContainer"
],
function(
        l_AbstractTemplatedContainer,
        l_Machine,
        l_MainTabs,
        d_declare,
        d_lang,
        template,
        module
    )
{


var b_navigator_dijit_MainArea = d_declare(
        [ l_AbstractTemplatedContainer ],

{

    templateString : template,

    getMachine : function()  { return this._machine; },

    getComputeBlocksTabDij : function()  { return this._mainTabs.getComputeBlocksTabDij(); },
    getHardwareTabDij : function()  { return this._mainTabs.getHardwareTabDij(); },
    getIoBlocksTabDij : function()  { return this._mainTabs.getIoBlocksTabDij(); },
    getJobsTabDij : function()  { return this._mainTabs.getJobsTabDij(); },
    getJobHistoryTabDij : function()  { return this._mainTabs.getJobHistoryTabDij(); },
    getSystemSummaryTabDij : function()  { return this._mainTabs.getSystemSummaryTabDij(); },

    getTabDij : function( tab_name )  { return this._mainTabs.getTabDij( tab_name ); },

    getComputeBlockDetailsDij : function()  { return this._mainTabs.getComputeBlockDetailsDij(); },
    getIoBlockDetailsDij : function()  { return this._mainTabs.getIoBlockDetailsDij(); },
    getCurrentJobDetailsDij : function() { return this._mainTabs.getCurrentJobDetailsDij(); },
    getHistoryJobDetailsDij : function() { return this._mainTabs.getHistoryJobDetailsDij(); },


    onTabChanged : function( widget ) {},


    getSelectedTab : function()  { return this._mainTabs.getSelectedTab(); },


    addTab : function( dijit, priority, name )  { this._mainTabs.addTab( dijit, priority, name ); },
    removeTab : function( dijit )  { this._mainTabs.removeTab( dijit ); },
    switchTo : function( tab_name )  { this._mainTabs.switchTo( tab_name ); },


    _machine : null,


    // override
    startup : function()
    {
        this.inherited( arguments );
        this._mainTabs.on( "tabChanged", d_lang.hitch( this, this._tabChanged ) );
    },


    setMachineRegion : function( new_region )
    {
        if ( ! new_region )  return;


        this._set( "machineRegion", new_region );

        this._machine = new l_Machine( { region: this.machineRegion, style: "overflow: auto;" } );

        this._borderContainer.addChild( this._machine );
    },


    _tabChanged : function( new_tab )
    {
        this.onTabChanged( new_tab );
    }

} );


return b_navigator_dijit_MainArea;

} );