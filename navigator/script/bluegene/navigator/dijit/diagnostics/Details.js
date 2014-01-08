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
    "../AbstractTemplatedContainer",
    "../MonitorActiveMixin",
    "../../format",
    "../../../dijit/ActionLink",
    "../../../dijit/OutputText",
    "dojo/dom-construct",
    "dojo/when",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/text!./templates/Details.html",
    "module",

    // Used only in template.
    "../Table",
    "../../../dijit/Hideable",
    "../../../dijit/OutputFormat",
    "dijit/form/Button",
    "dijit/layout/ContentPane",
    "dijit/layout/StackContainer"
],
function(
        l_AbstractTemplatedContainer,
        ll_MonitorActiveMixin,
        b_navigator_format,
        b_dijit_ActionLink,
        b_dijit_OutputText,
        d_construct,
        d_when,
        d_array,
        d_declare,
        d_lang,
        template,
        module
    )
{


var b_navigator_dijit_diagnostics_Details = d_declare(
        [ l_AbstractTemplatedContainer, ll_MonitorActiveMixin ],

{

    templateString : template,


    _fetch_data_fn : null,
    _testcases_store : null,

    _request : null,

    _is_active: false,
    _dirty: true,

    _cur: null,

    _history: null, // []
    _forward_history: null, // []

    _block_run_selected_on_handle : null, // dojo/on handle.


    constructor : function()
    {
        this._history = [];
        this._forward_history = [];
    },


    setFetchDataFn : function( fn )
    {
        this._fetch_data_fn = fn;
    },

    setTestcasesStore : function( store )
    {
        this._testcases_store = store;
    },

    notifySelected : function( obj )
    {
        console.log( module.id + ": notified", obj );

        this._setCurrentNoHistory( obj );

        this._history = [];
        this._forward_history = [];
        this._checkHistoryButtonState();
    },


    notifyRefresh : function()
    {
        if ( ! this._isActive() /*MonitorActiveMixin*/ ) {
            this._dirty = true;
            return;
        }

        this._setCurrentNoHistory( this._cur );
    },


    // override
    startup : function()
    {
        this.inherited( arguments );

        this._blockBackBtn.on( "click", d_lang.hitch( this, this._back ) );
        this._blockForwardBtn.on( "click", d_lang.hitch( this, this._forward ) );

        this._testsBackBtn.on( "click", d_lang.hitch( this, this._back ) );
        this._testsForwardBtn.on( "click", d_lang.hitch( this, this._forward ) );

        this._runBackBtn.on( "click", d_lang.hitch( this, this._back ) );
        this._runForwardBtn.on( "click", d_lang.hitch( this, this._forward ) );

        this._locationBackBtn.on( "click", d_lang.hitch( this, this._back ) );
        this._locationForwardBtn.on( "click", d_lang.hitch( this, this._forward ) );

        this._tcLocBackBtn.on( "click", d_lang.hitch( this, this._back ) );
        this._tcLocForwardBtn.on( "click", d_lang.hitch( this, this._forward ) );

        this._checkHistoryButtonState();

        this._setupBlockRunsTable();
        this._setupBlockTestsTable();
        this._setupTestsTable();
        this._setupRunTables();
        this._setupLocationTable();
        this._setupTestLocTable();
    },


    // override
    destroy : function()
    {
        if ( this._request ) {
            console.log( module.id + ": canceling operation in progress because destroyed." );
            this._request.cancel();
            this._request = null;
        }

        this.inherited( arguments );
    },


    // override MonitorActiveMixin
    _notifyActive : function()
    {
        if ( this._dirty && (! this._request) ) {
            this._setCurrentNoHistory( this._cur ); // reload
        }
    },


    _setupBlockRunsTable : function()
    {
        var executed_calc_properties = d_lang.hitch( this, function( run )
            {
                var is_current = (run.runId === this._cur.runId);
                return { label: b_navigator_format.timestamp( run.start ), disabled: is_current };
            } );

        var executed_cb = d_lang.hitch( this, function( dij, run )
            {
                var is_current = (run.runId === this._cur.runId);
                if ( is_current )  return;
                dij.on( "click", d_lang.hitch( this, this._setCurrent, { blockId: this._cur.blockId, runId: run.runId } ) );
            } );

        var current_ind_formatter = d_lang.hitch( this, function( runId )
            {
                var is_current = (runId === this._cur.runId);
                return (is_current ? ">" : "");
            } );

        var layout = { columns: [
                 { title: "Executed", dijitType: b_dijit_ActionLink, calcProperties: executed_calc_properties, createCb: executed_cb },
                 { title: "Result",
                   field: "hardwareStatus",
                   dijitType: b_dijit_OutputText,
                   dijitProperties: {
                       formatter: b_navigator_format.diagnostics.hardwareStatus,
                       style: "text-align: center"
                     }
                 },
                 { field: "runId", dijitType: b_dijit_OutputText, dijitProperties: { formatter: current_ind_formatter }  }
             ] };

         this._blockRunsTable.set( "layout", layout );
    },


    _setupBlockTestsTable : function()
    {
        var calc_properties = function( field, tc )
            {
                return { label: (field in tc ? "" + tc[field] : ""), style: "text-align: right", disabled: (! (field in tc) || tc[field] === 0) };
            };

        var create_cb = d_lang.hitch( this, function( field, hw_status, dij, tc )
            {
                if ( ! (field in tc) || tc[field] === 0 )  return;

                dij.on( "click", d_lang.hitch( this, this._setCurrent,
                        { blockId: this._cur.blockId, runId: this._cur.runId, runStart: this._cur.runStart, testcase: tc.testcase, hardwareStatus: hw_status }
                    ) );
            } );


        var layout = { columns: [
                { title: "Testcase", dijitType: b_dijit_OutputText, field: "testcase" },
                { title: "Passed",
                  dijitType: b_dijit_ActionLink,
                  calcProperties: d_lang.hitch( this, calc_properties, "passed" ),
                  createCb: d_lang.hitch( this, create_cb, "passed", "success" )
                },
                { title: "Marginal",
                  dijitType: b_dijit_ActionLink,
                  calcProperties: d_lang.hitch( this, calc_properties, "marginal" ),
                  createCb: d_lang.hitch( this, create_cb, "marginal", "marginal" )
                },
                { title: "Unknown",
                  dijitType: b_dijit_ActionLink,
                  calcProperties: d_lang.hitch( this, calc_properties, "unknown" ),
                  createCb: d_lang.hitch( this, create_cb, "unknown", "unknown" )
                },
                { title: "Failed",
                  dijitType: b_dijit_ActionLink,
                  calcProperties: d_lang.hitch( this, calc_properties, "failed" ),
                  createCb: d_lang.hitch( this, create_cb, "failed", "failed" )
                }
            ] };

        this._blockTestsTable.set( "layout", layout );
    },


    _setupTestsTable : function()
    {
        var create_location_fn = d_lang.hitch( this, function( dij, tc )
            {
                dij.on( "click", d_lang.hitch( this, this._setCurrent, { location: tc.location } ) );
            } );

        var layout = { columns: [
                { title: "Location",
                  dijitType: b_dijit_ActionLink,
                  calcProperties: function( tc )  { return { label: tc.location }; },
                  createCb: d_lang.hitch( this, create_location_fn )
                },
                { title: "Serial Number", field: "serialnumber", dijitType: b_dijit_OutputText,
                  dijitProperties: { formatter: function( val )  { return (val ? val : ""); } }
                },
                { title: "Replace", field: "hardwareReplace", dijitType: b_dijit_OutputText,
                  dijitProperties: { formatter: function( val )  { return (val ? "Yes" : "No"); }, style: "text-align: center;" }
                },
                { title: "Executed", field: "end", dijitType: b_dijit_OutputText,
                  dijitProperties: { formatter: b_navigator_format.timestamp, style: "text-align: center;" }
                },
                { title: "Log File", field: "logfile", dijitType: b_dijit_OutputText,
                  dijitProperties: { formatter: function( val )  { return val ? val : ""; } }
                },
                { title: "Analysis", field: "analysis", dijitType: b_dijit_OutputText,
                  dijitProperties: { formatter: function( val )  { return val ? val : ""; }, style: "font-family: monospacee; white-space: pre;" }
                }
            ] };

        this._testsTable.set( "layout", layout );
    },


    _setupRunTables : function()
    {
        var create_location_fn = d_lang.hitch( this, function( dij, block )
            {
                var new_cur_obj = { runId: this._cur.runId, blockId: block.blockId };
                dij.on( "click", d_lang.hitch( this, this._setCurrent, new_cur_obj ) );
            } );

        var opt_time_fmt_fn = function( ts_val_opt )
            {
                return (ts_val_opt ? b_navigator_format.timestamp( ts_val_opt ) : "");
            };

        var hw_status_col_info = {
                title: "Hardware Status",
                field: "hardwareStatus",
                dijitType: b_dijit_OutputText,
                dijitProperties: {
                    formatter: b_navigator_format.diagnostics.hardwareStatus,
                    style: "text-align: center;"
                  }
            };

        var start_time_col_info = {
                title: "Start Time",
                field: "start",
                dijitType: b_dijit_OutputText,
                dijitProperties: { formatter: opt_time_fmt_fn, style: "text-align: right;" }
            };

        var end_time_col_info = {
                title: "End Time",
                field: "end",
                dijitType: b_dijit_OutputText,
                dijitProperties: { formatter: opt_time_fmt_fn, style: "text-align: right;" }
            };

        var mp_io_layout = { columns: [
                { title: "Location",
                  dijitType: b_dijit_ActionLink, calcProperties: function( block )  { return { label: block.location }; },
                  createCb: d_lang.hitch( this, create_location_fn )
                },
                hw_status_col_info,
                start_time_col_info,
                end_time_col_info
            ] };

        this._runMidplanesTable.set( "layout", mp_io_layout );
        this._runIoDrawersTable.set( "layout", mp_io_layout );


        var b_layout = { columns: [
                { title: "Block ID",
                  dijitType: b_dijit_ActionLink, calcProperties: function( block )  { return { label: block.blockId }; },
                  createCb: d_lang.hitch( this, create_location_fn )
                },
                hw_status_col_info,
                start_time_col_info,
                end_time_col_info
            ] };

        this._runBlocksTable.set( "layout", b_layout );
    },


    _setupLocationTable : function()
    {
        var create_tc_fn = d_lang.hitch( this, function( dij, tc )
            {
                dij.on( "click", d_lang.hitch( this, this._setCurrent,
                        { testcase: tc.testcase, location: this._cur.location }
                    ) );
            } );

        var create_end_fn = d_lang.hitch( this, function( dij, tc )
            {
                dij.on( "click", d_lang.hitch( this, this._setCurrent,
                        { runId: tc.runId, blockId: tc.blockId }
                    ) );
            } );

        var layout = { columns: [
                { title: "Testcase", dijitType: b_dijit_ActionLink,
                  calcProperties: function( tc )  { return { label: tc.testcase }; },
                  createCb : d_lang.hitch( this, create_tc_fn )
                },
                { title: "Most Recent Execution", dijitType: b_dijit_ActionLink,
                  calcProperties: function( tc )  { return { label: b_navigator_format.timestamp( tc.end ), style: "text-align: right;" }; },
                  createCb : d_lang.hitch( this, create_end_fn )
                },
                { title: "Hardware Result",
                  field: "hardwareStatus",
                  dijitType: b_dijit_OutputText,
                  dijitProperties: {
                      formatter: b_navigator_format.diagnostics.hardwareStatus,
                      style: "text-align: center;"
                    }
                },
                { title: "Replace", field: "hardwareReplace", dijitType: b_dijit_OutputText,
                  dijitProperties: { formatter: function( val ) { return (val ? "Yes" : "No"); }, style: { style: "text-align: center;" } }
                },
                { title: "Log File", field: "logfile", dijitType: b_dijit_OutputText,
                  dijitProperties: { formatter: function( val ) { return (val ? val : ""); } }
                },
                { title: "Analysis", field: "analysis", dijitType: b_dijit_OutputText,
                  dijitProperties: { formatter: function( val ) { return (val ? val : ""); }, style: "font-family: monospacee; white-space: pre;" }
                }
            ] };

        this._locationTable.set( "layout", layout );
    },


    _setupTestLocTable : function()
    {
        var create_exec_fn = d_lang.hitch( this, function( dij, res )
            {
                dij.on( "click", d_lang.hitch( this, this._setCurrent,
                        { runId: res.runId, blockId: res.blockId }
                    ) );
            } );

        var layout = { columns: [
                { title: "Executed", dijitType: b_dijit_ActionLink,
                  calcProperties: function( res )  { return { label: b_navigator_format.timestamp( res.end ), style: "text-align: right;" }; },
                  createCb : d_lang.hitch( this, create_exec_fn )
                },
                { title: "Hardware Status",
                  field: "hardwareStatus",
                  dijitType: b_dijit_OutputText,
                  dijitProperties: {
                      formatter: b_navigator_format.diagnostics.hardwareStatus,
                      style: "text-align: center;"
                    }
                },
                { title: "Replace", field: "hardwareReplace", dijitType: b_dijit_OutputText,
                  dijitProperties: { formatter: function( val )  { return (val ? "Yes" : "No"); }, style: "text-align: center;" }
                },
                { title: "Log File", field: "logfile", dijitType: b_dijit_OutputText,
                  dijitProperties: { formatter: function( val )  { return val ? val : ""; } }
                },
                { title: "Analysis", field: "analysis", dijitType: b_dijit_OutputText,
                  dijitProperties: { formatter: function( val )  { return val ? val : ""; }, style: "font-family: monospacee; white-space: pre;" }
                }
            ] };

        this._tcLocTable.set( "layout", layout );
    },


    _setCurrent : function( obj )
    {
        if ( obj == null ) {
            this._history = [];
            this._forward_history = [];
            this._checkHistoryButtonState();
            this._setCurrentNoHistory( obj );
            return;
        }

        if ( this._cur != null ) {
            this._history.push( this._cur );
            this._forward_history = [];
            this._checkHistoryButtonState();
        }

        this._setCurrentNoHistory( obj );

        if ( this._cur == null ) {
            // invalid object, shouldn't get here.
            this._history = [];
            this._forward_history = [];
            this._checkHistoryButtonState();
            return;
        }
    },


    _setCurrentNoHistory : function( obj )
    {
        console.log( module.id + ": setting current=", obj );

        this._cur = obj;

        if ( this._cur == null ) {
            this._setDisplay( this._noneContainer );
            return;
        }

        if ( ("blockId" in this._cur) && ("runId" in this._cur) && ("runStart" in this._cur) && ("testcase" in this._cur) && ("hardwareStatus" in this._cur) ) {
            // testcases for a block given result.

            var title_fields = {
                    hardwareStatus: this._cur.hardwareStatus,
                    testcase: this._cur.testcase,
                    runTime: this._cur.runStart
                };

            var block_id = this._cur.blockId;

            if ( block_id.substr( 0, 7 ) == "_DIAGS_" ) {

                var location = block_id.substr( 7 );

                if ( location.match( /R..-M./ ) ) {
                    type = "midplane";
                } else if ( location.match( /[QR]..-I./) ) {
                    type = "ioDrawer";
                }

                title_fields.blockType = (type == "midplane" ? "Midplane" : "I/O drawer");
                title_fields.blockOrLocation = location;

            } else {

                title_fields.blockType = "Block";
                title_fields.blockOrLocation = this._cur.blockId;

            }

            this._testsTitleFmt.set( "value", title_fields );


            var query = {
                    runId: this._cur.runId,
                    blockId: this._cur.blockId,
                    testcase: this._cur.testcase,
                    hardwareStatus: this._cur.hardwareStatus
                };

            console.log( module.id + ": [" + this.id + "] setting tests table store. query=", query );

            this._testsTable.setStore( this._testcases_store, query );

            this._dirty = false;
            if ( this._request ) { this._request.cancel(); this._request = null; }

            this._setDisplay( this._testsContainer );
            return;
        }

        if ( "blockId" in this._cur ) {
            // Details for a diagnostics block.

            var block_id = this._cur.blockId;

            var location = null;
            var type = "user";

            var title_fields = {};

            if ( block_id.substr( 0, 7 ) == "_DIAGS_" ) {

                location = block_id.substr( 7 );

                if ( location.match( /R..-M./ ) ) {
                    type = "midplane";
                } else if ( location.match( /[QR]..-I./) ) {
                    type = "ioDrawer";
                }

                title_fields = { blockType: type == "midplane" ? "Midplane" : "I/O drawer", blockOrLocation: location };

            } else {

                title_fields = { blockType: "Block", blockOrLocation: this._cur.blockId };

            }

            this._blockTitleFmt.set( "value", title_fields );

            this._blockSchedTxt.set( "value", null );
            this._blockTimeTxt.set( "value", null );
            this._blockResultTxt.set( "value", null );

            this._blockRunResultTxt.set( "disabled" , true );
            this._blockRunResultTxt.set( "label", "Loading..." );


            var block_url = "blocks/" + this._cur.blockId + "?runId=" + this._cur.runId;

            if ( this._request )  this._request.cancel();
            this._request = d_when(
                    this._fetch_data_fn( block_url ),
                    d_lang.hitch( this, this._gotBlock )
                );

            this._setDisplay( this._blockContainer );

            return;
        }

        if ( ("runId" in this._cur) && ("runStart" in this._cur) ) {
            // Display details for the run.

            this._runTitleFmt.set( "value", { runTime: this._cur.runStart } );

            this._runStatusTxt.set( "value", null );
            this._runStartTxt.set( "value", null );
            this._runLogfileTxt.set( "value", null );
            this._runEndContainer.set( "visibility", "hidden" );
            this._runUserContainer.set( "visibility", "hidden" );

            this._runMidplanesContainer.set( "visibility", "hidden" );
            this._runIoDrawersContainer.set( "visibility", "hidden" );
            this._runBlocksContainer.set( "visibility", "hidden" );

            var run_url = "runs/" + this._cur.runId;

            if ( this._request )  this._request.cancel();
            this._request = d_when(
                    this._fetch_data_fn( run_url ),
                    d_lang.hitch( this, this._gotRun )
                );

            this._setDisplay( this._runContainer );

            return;
        }

        if ( ("location" in this._cur) && ("testcase" in this._cur) ) {

            this._tcLocTitleFmt.set( "value",
                    { testcase: this._cur.testcase, location: this._cur.location }
                );


            var query = {
                    location: this._cur.location,
                    testcase: this._cur.testcase
                };

            console.log( module.id + ": [" + this.id + "] setting test at location table store. query=", query );

            this._tcLocTable.setStore( this._testcases_store, query );

            this._dirty = false;
            if ( this._request ) { this._request.cancel(); this._request = null; }

            this._setDisplay( this._tcLocContainer );
            return;
        }

        if ( "location" in this._cur ) {

            this._locationTitleFmt.set( "value", { location: this._cur.location } );


            var query = {
                    location: this._cur.location
                };

            console.log( module.id + ": [" + this.id + "] setting tests for location table store. query=", query );

            this._locationTable.setStore( this._testcases_store, query );

            this._dirty = false;
            if ( this._request ) { this._request.cancel(); this._request = null; }


            this._setDisplay( this._locationContainer );
            return;
        }

        this._cur = null;
        this._setDisplay( this._noneContainer );
    },


    _gotRun: function( result )
    {
        this._request = null;
        this._dirty = false;

        console.log( module.id + ": got run ", result );

        this._runStatusTxt.set( "value", result.status );
        this._runStartTxt.set( "value", b_navigator_format.timestamp( result.start ) );
        this._runLogfileTxt.set( "value", b_navigator_format.diagnostics.calcRunLogFile( result.logDirectory ) );

        if ( "end" in result ) {
            this._runEndTxt.set( "value", b_navigator_format.timestamp( result.end ) );
            this._runEndContainer.set( "visibility", "visible" );
        }

        if ( "user" in result ) {
            this._runUserTxt.set( "value", result.user );
            this._runUserContainer.set( "visibility", "visible" );
        }

        var midplanes_data = [];
        var io_drawers_data = [];
        var user_data = [];

        d_array.forEach( result.blocks, function( block ) {
            if ( ("location" in block) && (block.location.match( /R..-M./ )) ) {
                midplanes_data.push( block );
            } else if ( ("location" in block) && (block.location.match( /[QR]..-I./ )) ) {
                io_drawers_data.push( block );
                io_drawers_visible = true;
            } else {
                user_data.push( block );
                user_visible = true;
            }
        } );


        this._runMidplanesContainer.set( "visibility", "hidden" );
        this._runIoDrawersContainer.set( "visibility", "hidden" );
        this._runBlocksContainer.set( "visibility", "hidden" );

        this._runMidplanesTable.set( "data", { data: midplanes_data, idProperty: "blockId" } );
        this._runIoDrawersTable.set( "data", { data: io_drawers_data, idProperty: "blockId" } );
        this._runBlocksTable.set( "data", { data: user_data, idProperty: "blockId" } );

        if ( midplanes_data.length )  this._runMidplanesContainer.set( "visibility", "visible" );
        if ( io_drawers_data.length )  this._runIoDrawersContainer.set( "visibility", "visible" );
        if ( user_data.length )  this._runBlocksContainer.set( "visibility", "visible" );
    },


    _gotBlock : function( result )
    {
        this._request = null;
        this._dirty = false;

        console.log( module.id + ": got block ", result );

        this._cur.runId = result.runId;
        this._cur.runStart = result.runStart;

        if ( result.start ) {
            this._blockSchedTxt.set( "value", "Run at" );
            this._blockTimeTxt.set( "value", result.start );
        } else {
            if ( result.runEnd ) {
                this._blockSchedTxt.set( "value", "Did not start on completed run started" );
            } else {
                this._blockSchedTxt.set( "value", "Pending for run started" );
            }
            this._blockTimeTxt.set( "value", result.runStart );
        }
        this._blockResultTxt.set( "value", result.hardwareStatus );


        this._blockRunResultTxt.set( "label", result.runStatus );
        this._blockRunResultTxt.set( "disabled", false );

        if ( this._block_run_selected_on_handle ) { this._block_run_selected_on_handle.remove(); console.log( module.id + ": removed old handle." ); }
        this._block_run_selected_on_handle = this._blockRunResultTxt.on( "click", d_lang.hitch( this, this._setCurrent, { runId: result.runId, runStart: result.runStart } ) );


        this._blockTestsTable.set( "data", { data: result.testcases, idProperty: "testcase" } );
        this._blockRunsTable.set( "data", { data: result.history, idProperty: "runId" } );
    },


    _setDisplay : function( which )
    {
        this._stackContainer.selectChild( which );
    },


    _back : function()
    {
        console.log( module.id + ": going back. history.length=", this._history.length );

        if ( this._history.length == 0 ) {
            return;
        }

        this._forward_history.push( this._cur );

        this._setCurrentNoHistory( this._history.pop() );
        this._checkHistoryButtonState();
    },


    _forward : function()
    {
        console.log( module.id + ": going forward. forward.length=", this._forward_history.length );

        if ( this._forward_history.length == 0 ) {
            return;
        }

        this._history.push( this._cur );

        this._setCurrentNoHistory( this._forward_history.pop() );
        this._checkHistoryButtonState();
    },


    _checkHistoryButtonState : function()
    {
        var back_disabled = (this._history.length == 0);

        this._blockBackBtn.set( "disabled", back_disabled );
        this._testsBackBtn.set( "disabled", back_disabled );
        this._runBackBtn.set( "disabled", back_disabled );
        this._locationBackBtn.set( "disabled", back_disabled );
        this._tcLocBackBtn.set( "disabled", back_disabled );

        var forward_disabled = (this._forward_history.length == 0);

        this._blockForwardBtn.set( "disabled", forward_disabled );
        this._testsForwardBtn.set( "disabled", forward_disabled );
        this._runForwardBtn.set( "disabled", forward_disabled );
        this._locationForwardBtn.set( "disabled", forward_disabled );
        this._tcLocForwardBtn.set( "disabled", forward_disabled );
    },
    
    
    _b_navigator_format : b_navigator_format


} );

return b_navigator_dijit_diagnostics_Details;

} );
