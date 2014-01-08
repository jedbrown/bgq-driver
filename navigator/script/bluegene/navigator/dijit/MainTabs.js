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
    "dojo/topic",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/text!./templates/MainTabs.html",
    "module",

    // Used only in template.
    "./Blocks",
    "./Hardware",
    "./IoBlocks",
    "./JobHistory",
    "./Jobs",
    "./KnowledgeCenter",
    "./SystemSummary",
    "dijit/layout/TabContainer"
],
function(
        l_AbstractTemplatedContainer,
        d_topic,
        d_declare,
        d_lang,
        template,
        module
    )
{


var b_navigator_dijit_MainTabs = d_declare(
        [ l_AbstractTemplatedContainer ],

{

    templateString : template,

    _tab_dijits : null, // [] { dij : dijit, priority : number }
    _name_to_tab : null,


    getMachine : function()  { return this._machine; },

    getComputeBlocksTabDij : function()  { return this._computeBlocks; },
    getHardwareTabDij : function()  { return this._hardware; },
    getIoBlocksTabDij : function()  { return this._ioBlocks; },
    getJobsTabDij : function()  { return this._jobs; },
    getJobHistoryTabDij : function()  { return this._jobHistory; },
    getSystemSummaryTabDij : function()  { return this._systemSummary; },

    getTabDij : function( tab_name )
    {
        return this._name_to_tab[tab_name];
    },

    getComputeBlockDetailsDij : function()  { return this._computeBlocks.getDetailsDijit(); },
    getIoBlockDetailsDij : function()  { return this._ioBlocks.getDetailsDijit(); },
    getCurrentJobDetailsDij : function() { return this._jobs.getDetailsDijit(); },
    getHistoryJobDetailsDij : function() { return this._jobHistory.getDetailsDijit(); },


    onTabChanged : function( widget ) {},


    getSelectedTab : function()
    {
        return this._tabContainer.selectedChildWidget;
    },


    addTab : function( dijit, priority, name )
    {
        var i = 0;

        while ( i < this._tab_dijits.length && this._tab_dijits[i].priority < priority ) {
            ++i;
        }

        this._tabContainer.addChild( dijit, i );

        if ( name ) {
            this._name_to_tab[name] = dijit;
        }

        this._tab_dijits.splice( i, 0, { dij : dijit, priority : priority } );
    },

    removeTab : function( dijit )
    {
        this._tabContainer.removeChild( dijit );
    },

    switchTo : function( tab_name )
    {
        if ( ! (tab_name in this._name_to_tab) ) {
            console.log( module.id + ": don't have a tab for '" + tab_name + "'" );
            return;
        }
        var tab_dij = this._name_to_tab[tab_name];
        this._tabContainer.selectChild( tab_dij );
    },


    // override
    startup : function()
    {
        this.inherited( arguments );
        d_topic.subscribe( this._tabContainer.id + "-selectChild", d_lang.hitch( this, this._tabChanged ) );

        this._tab_dijits = [];
        this._tab_dijits.push( { dij : this._systemSummary, priority : 10 } );
        this._tab_dijits.push( { dij : this._jobs, priority : 30 } );
        this._tab_dijits.push( { dij : this._jobHistory, priority : 40 } );
        this._tab_dijits.push( { dij : this._computeBlocks, priority : 50 } );
        this._tab_dijits.push( { dij : this._ioBlocks, priority : 60 } );
        this._tab_dijits.push( { dij : this._hardware, priority : 90 } );
        this._tab_dijits.push( { dij : this._knowledgeCenter, priority : 140 } );

        this._name_to_tab = {
                "summary" : this._systemSummary,
                "jobs" : this._jobs,
                "job-history" : this._jobHistory,
                "blocks" : this._computeBlocks,
                "io-blocks" : this._ioBlocks,
                "hardware" : this._hardware,
                "knowledgeCenter" : this._knowledgeCenter
            };
    },

    _tabChanged : function( new_tab )
    {
        console.log( module.id + ": tab changed to", new_tab.id, "dij=", new_tab );
        this.onTabChanged( new_tab );
    }

} );


return b_navigator_dijit_MainTabs;

} );