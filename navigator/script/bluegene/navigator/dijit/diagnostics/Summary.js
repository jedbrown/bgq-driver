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
    "./SummaryRunningCell",
    "../AbstractTemplatedContainer",
    "../MonitorActiveMixin",
    "../../format",
    "../../../dijit/ActionLink",
    "../../../dijit/OutputText",
    "../../../utility/Requests",
    "dojo/when",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dijit/form/Button",
    "dojo/text!./templates/Summary.html",
    "module",

    // Used only in template.
    "../Table",
    "../../../dijit/Hideable",
    "dijit/layout/ContentPane"
],
function(
        l_SummaryRunningCell,
        ll_AbstractTemplatedContainer,
        ll_MonitorActiveMixin,
        b_navigator_format,
        b_dijit_ActionLink,
        b_dijit_OutputText,
        b_utility_Requests,
        d_when,
        d_array,
        d_declare,
        d_lang,
        j_form_Button,
        template,
        module
    )
{


var b_navigator_dijit_diagnostics_Summary = d_declare(
        [ ll_AbstractTemplatedContainer, ll_MonitorActiveMixin ],

{

    templateString : template,

    _machine_info : null,
    _fetch_diagnostics_data_fn : null,

    _dirty : true,

    _requests : null, // bluegene^utility^Requests.


    constructor : function()
    {
        this._requests = new b_utility_Requests( "diagnostics/Summary" );
    },


    setFetchDiagnosticsDataFn : function( new_fn )
    {
        this._fetch_diagnostics_data_fn = new_fn;

        if ( ! this._isActive() /*MonitorActiveMixin*/ ) {
            this._dirty = true;
            return;
        }

        this._load();
    },


    setCancelDiagnosticsRunFn : function( new_fn )
    {
        var RUNNING_CREATE_CB = d_lang.hitch( this, function( dij, o )
            {
                dij.setCancelDiagnosticsRunFn( new_fn );
                dij.on( "blockSelected", d_lang.hitch( this, function( o )
                    {
                        console.log( module.id + ": [" + this.id + "] Click from running, o=", o );
                        this.onBlockSelected( o );
                    } ) );
            } );

        var RUNNING_LAYOUT = { columns: [
                { dijitType : l_SummaryRunningCell, createCb: RUNNING_CREATE_CB }
            ] };

        this._runningTable.set( "layout", RUNNING_LAYOUT );
    },


    setMachineInfo : function( new_machine_info )
    {
        this._machine_info = new_machine_info;

        if ( ! this._isActive() /*MonitorActiveMixin*/ ) {
            this._dirty = true;
            return;
        }

        this._load();
    },


    onBlockSelected : function( obj )  {},


    notifyRefresh : function()
    {
        if ( ! this._isActive() /*MonitorActiveMixin*/ ) {
            this._dirty = true;
            return;
        }

        this._load();
    },


    // Override
    startup : function()
    {
        this.inherited( arguments );


        var HW_STATUS_FORMATTER = function( val )  {  return (val ? b_navigator_format.diagnostics.hardwareStatus( val ) : "Unknown"); };

        var HW_STATUS_COL = { title: "Hardware Status", field: "hardwareStatus", dijitType: b_dijit_OutputText, dijitProperties: { formatter: HW_STATUS_FORMATTER, style: "text-align: center" } };

        var RUN_TIME_FORMATTER = function( val ) { return (val ? b_navigator_format.timestamp( val ) : "Never"); };

        var RUN_TIME_COL = { title: "Run Time", field: "end", dijitType: b_dijit_OutputText, dijitProperties: { formatter: RUN_TIME_FORMATTER } };


        var DB_LOC_CALC_PROPERTIES = function( o )  { return { label: o.location }; };

        var DB_CREATE_LOC_CB = d_lang.hitch( this, function( dij, o )
            {
                if ( "runId" in o ) {
                    dij.on( "click", d_lang.hitch( this, this._blockClicked, { blockId: o.blockId, location: o.location, runId: o.runId } ) );
                } else {
                    dij.set( "disabled", true );
                }
            } );


        var DB_LAYOUT = { columns : [
                { title: "Location", field: "location", dijitType: b_dijit_ActionLink, calcProperties: DB_LOC_CALC_PROPERTIES, createCb: DB_CREATE_LOC_CB },
                HW_STATUS_COL,
                RUN_TIME_COL
            ] };

        this._midplanesTable.set( "layout", DB_LAYOUT );

        this._ioDrawersTable.set( "layout", DB_LAYOUT );


        var USER_LOC_CALC_PROPERTIES = function( o )  { return { label: o.blockId }; };

        var USER_CREATE_LOC_CB = d_lang.hitch( this, function( dij, o )
                {
                    if ( "runId" in o ) {
                        dij.on( "click", d_lang.hitch( this, this._blockClicked, { blockId: o.blockId, runId: o.runId } ) );
                    } else {
                        dij.set( "disabled", true );
                    }
                } );


        var USER_LAYOUT = { columns: [
                { title: "Location", field: "blockId", dijitType: b_dijit_ActionLink, calcProperties: USER_LOC_CALC_PROPERTIES, createCb: USER_CREATE_LOC_CB },
                HW_STATUS_COL,
                RUN_TIME_COL
            ] };

        this._userTable.set( "layout", USER_LAYOUT );
    },


    // override
    destroy : function()
    {
        this._requests.cancel();
        this.inherited( arguments );
    },


    // override MonitorActiveMixin
    _notifyActive : function()
    {
        if ( ! this._dirty ) {
            // Nothing to do.
            return;
        }

        // Start loading data from the bgws...
        this._load();
    },


    _load : function()
    {
        if ( ! this._fetch_diagnostics_data_fn )  return;
        if ( ! this._machine_info )  return;


        this._runningLoading.set( "visibility", "visible" );

        this._requests.monitor(
                d_when(
                        this._fetch_diagnostics_data_fn( "runs?end=null&sort=start" ),
                        d_lang.hitch( this, this._gotCurrentDiagnosticsRuns )
                    )
            );


        this._midplanesLoading.set( "visibility", "visible" );

        this._requests.monitor(
                d_when(
                        this._fetch_diagnostics_data_fn( "blocks?type=midplane" ),
                        d_lang.hitch( this, this._gotMiplanes )
                    )
            );


        this._ioDrawersLoading.set( "visibility", "visible" );

        this._requests.monitor(
                d_when(
                        this._fetch_diagnostics_data_fn( "blocks?type=ioDrawer" ),
                        d_lang.hitch( this, this._gotIoDrawers )
                    )
            );

        this._userLoading.set( "visibility", "visible" );

        this._requests.monitor(
                d_when(
                        this._fetch_diagnostics_data_fn( "blocks?type=user" ),
                        d_lang.hitch( this, this._gotUserBlocks )
                    )
            );
    },


    _gotCurrentDiagnosticsRuns : function( result )
    {
        this._runningLoading.set( "visibility", "hidden" );

        this._dirty = false;

        console.log( module.id + ": got current diagnostics runs ", result );


        this._runningTable.set( "data", { data: result, idProperty: "runId" } );
    },


    _gotMiplanes : function( result )
    {
        this._midplanesLoading.set( "visibility", "hidden" );

        this._dirty = false;

        console.log( module.id + ": got midplanes summary ", result );

        console.log( module.id + ": machine info=", this._machine_info );

        var mp_hash = {};

        d_array.forEach( this._machine_info.midplanes, function( mpa ) {
            d_array.forEach( mpa, function( mpb ) {
                d_array.forEach( mpb, function( mpc ) {
                    d_array.forEach( mpc, function( mp ) {
                        mp_hash[mp] = { location: mp };
                    } );
                } );
            } );
        } );

        d_array.forEach( result, function( diags_mp ) {
            mp_hash[diags_mp.location] = diags_mp;
        } );

        var mps = [];

        for ( mp_loc in mp_hash ) {
            mps.push( mp_hash[mp_loc] );
        }

        mps.sort( function( a, b ) { return (a.location > b.location ? 1 : -1); } );


        this._midplanesTable.set( "data", { data: mps, idProperty: "location" } );
    },


    _gotIoDrawers : function( result )
    {
        this._ioDrawersLoading.set( "visibility", "hidden" );

        this._dirty = false;

        console.log( module.id + ": got I/O drawer summary ", result );


        var io_drawer_hash = {};

        d_array.forEach( this._machine_info.ioDrawers, function( io_drawer_location ) {
            io_drawer_hash[io_drawer_location] = { location: io_drawer_location };
        } );

        d_array.forEach( result, function( io_drawer ) {
            io_drawer_hash[io_drawer.location] = io_drawer;
        } );

        var io_drawers = [];

        for ( var iod_loc in io_drawer_hash ) {
            io_drawers.push( io_drawer_hash[iod_loc] );
        }

        io_drawers.sort( function( a, b ) { return (a.location > b.location ? 1 : -1); } );


        this._ioDrawersTable.set( "data", { data: io_drawers, idProperty: "location" } );
    },


    _gotUserBlocks : function( result )
    {
        this._userLoading.set( "visibility", "hidden" );

        this._dirty = false;

        console.log( module.id + ": got user blocks summary ", result );

        if ( result.length == 0 ) {
            this._userHideable.set( "visibility", "hidden" );
        } else {
            this._userHideable.set( "visibility", "visible" );

            this._userTable.set( "data", { data: result, idProperty: "blockId" } );
        }
    },


    _blockClicked : function( obj )
    {
        console.log( module.id + ": block selected ", obj );

        this.onBlockSelected( obj );
    }

} );

return b_navigator_dijit_diagnostics_Summary;

} );
