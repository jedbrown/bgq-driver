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
    "./EventsMonitorMixin",
    "./topic",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_EventsMonitorMixin,
        l_topic,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_AbstractTab = d_declare(
        [ l_EventsMonitorMixin ],

/** @lends bluegene^navigator^AbstractTab# */
{

    _navigator_dij : null,


    /** @constructs */
    constructor: function()
    {
        // Nothing to do.
    },


    setNavigatorDij : function( navigator_dij )
    {
        this._navigator_dij = navigator_dij;

        var tab_dij = this._getTabDijit();

        if ( tab_dij ) {
            // Notify the Navigator that this instance is controlling its tab dijit
            l_topic.publish( l_topic.tabControllerChange, { id: tab_dij.id, controller: this } );
        }
    },


    getMachineHighlightData : function()
    {
        return this._getMachineHighlightData();
    },


    // override this method, provide the tab dijit's field name (in the Navigator dijit).
    _getTabDijitName : function()  { return null; },

    // Optionally, override this method to get the data to display in the machine.
    _getMachineHighlightData: function()  { return null; },


    // Call this to get the dijit for the tab in the Navigator.
    _getTabDijit : function()
    {
        if ( ! this._getTabDijitName() )  return null;
        return this._navigator_dij.getTabDij( this._getTabDijitName() );
    },


    // Call this method to update the machine highlighting (i.e., if your internal state changed)
    _updateMachineHighlighting: function()
    {
        l_topic.publish( l_topic.updateMachineHighlighting, this );
    },


    // override, called when machine clicked.
    notifyMachineClicked : function( location ) {}

} );


return b_navigator_AbstractTab;

} );
