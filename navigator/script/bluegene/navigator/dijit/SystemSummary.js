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
    "./MonitorActiveMixin",
    "../format",
    "../../BlueGene",
    "../../utility/Stack",
    "dojo/dom-construct",
    "dojo/number",
    "dojo/when",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/text!./templates/SystemSummary.html",
    "module",

    // Used only in template.
    "../../dijit/Hideable",
    "../../dijit/OutputFormat",
    "dijit/form/CheckBox",
    "dijit/layout/ContentPane"
],
function(
        l_AbstractTemplatedContainer,
        l_MonitorActiveMixin,
        ll_format,
        b_BlueGene,
        b_utility_Stack,
        d_construct,
        d_number,
        d_when,
        d_array,
        d_declare,
        d_lang,
        template,
        module
    )
{


var _statusToColor = {
        "AVAILABLE" : "white",
        "BLOCK_BUSY" : "#85a34a", // green
        "BLOCK_READY" : "#677b2a", // darker green
        "JOB_RUNNING" : "#2f718b", // blue
        "DIAGNOSTICS" : "#e7e486", // yellow
        "SERVICE_ACTION" : "#ebcd78", // orange
        "NOT_AVAILABLE" : "#9f5a5a" // red
    };


var _calcColorForStatus = function( status ) {
        if ( status in _statusToColor ) {
            return _statusToColor[status];
        }
        return "lightgray";
    };


var b_navigator_dijit_SystemSummary = d_declare(
        [ l_AbstractTemplatedContainer, l_MonitorActiveMixin ],

{

    templateString : template,

    _fetch_summary_fn : null,

    _system_cpu_count: 1024,
    _io_drawers : null, // [] I/O drawer locations.

    _promise: null, // not null when loading.

    _auto_update: false,
    _timeout_id: null,

    _current_hw: null,

    _dirty: true,

    _alerts_stack : null,


    getMachineHighlightData : function()
    {
        var ret = {
                loading: this._promise,
                highlighting: this._current_hw
            };

        return ret;
    },


    onMachineHighlightDataChanged : function()  {},


    setMachineInfo : function( args )
    {
        console.log( module.id + ": setMachineInfo. args=", args );

        this._system_cpu_count = args.machineInfo.systemCpuCount;
        this._io_drawers = args.machineInfo.ioDrawers;

        console.log( module.id + ": calling update from setMachineInfo." );
        this._update();
    },


    setFetchSummaryFn : function( fn )
    {
        this._fetch_summary_fn = fn;

        if ( this._fetch_summary_fn ) {

            // If auto updating, start the timer again.
            if ( this._auto_update ) {
                this._update();
            }

        } else {

            console.log( module.id + ": session ended so canceling auto-refresh. timeout_id=", this._timeout_id );
            if ( this._timeout_id != null ) {
                window.clearTimeout( this._timeout_id );
                this._timeout_id = null;
            }

        }
    },


    notifyRefresh : function()
    {
        if ( this._auto_update )  return; // don't do anything

        if ( ! this._isActive() /*MonitorActiveMixin*/ ) {
            this._dirty = true;
            return;
        }

        console.log( module.id + ": calling update from notifyRefresh." );
        this._update();
    },


    // override
    startup : function()
    {
        this.inherited( arguments );

        this._alerts_stack = b_utility_Stack( this._alertsStack,
                [
                  {
                    node: this._alertsText,
                    fn: d_lang.hitch( this, function( data ) {
                        this._alertsText.set( "value", data );
                      } )
                  }
                ]
            );


        this._autoUpdateCheckbox.on( "change", d_lang.hitch( this, this._autoUpdateChanged ) );
    },


    destroy : function()
    {
        if ( this._promise ) {
            this._promise.cancel();
            this._promise = null;
        }

        this.inherited( arguments );
    },


    // override MonitorActiveMixin
    _notifyActive : function()
    {
        if ( this._auto_update )  return;

        if ( ! this._dirty )  return;

        console.log( module.id + ": calling update from _notifyActive." );
        this._update();
    },


    _autoUpdateChanged : function( new_setting )
    {
        if ( new_setting ) {
            if ( this._auto_update )  return; // Don't update again.

            console.log( module.id + ": now auto-updating" );

            this._auto_update = true;
            this._update();
        } else {
            this._auto_update = false;

            console.log( module.id + ": no longer auto-updating" );

            if ( this._timeout_id != null ) {
                window.clearTimeout( this._timeout_id );
                this._timeout_id = null;
            }
        }
    },


    _update : function()
    {
        if ( ! this._fetch_summary_fn )  return;


        if ( this._promise != null ) {
            console.log( module.id + ": Already loading." );
            return;
        }

        if ( this._timeout_id != null ) {
            window.clearTimeout( this._timeout_id );
            this._timeout_id = null;
        }

        this._promise = d_when(
                this._fetch_summary_fn(),
                d_lang.hitch( this, this._handleSummaryData ),
                d_lang.hitch( this, function() { this._promise = null; } )
            );
        this.onMachineHighlightDataChanged();
    },


    _handleSummaryData : function( result )
    {
        console.log( module.id + ": _handleSummaryData. result=", result );

        this._promise = null;
        this._dirty = false;

        this._gotJobData( result );
        this._gotAlertsSummary( result );
        this._gotHardwareSummary( result );
        this._gotDiagsSummary( result );
        this._gotServiceActionsSummary( result );

        this._setCurrentHardware( result );

        if ( this._auto_update ) {
            this._timeout_id = window.setTimeout( d_lang.hitch( this, this._update ), 5000 );
        }
    },


    _gotJobData: function( result )
    {
        var new_value = {
                jobCount: result.jobCount,
                jobCpuCount: result.jobCpuCount,
                systemCpuCount: this._system_cpu_count,
                jobPct: (result.jobCpuCount / this._system_cpu_count)
            };

        this._jobsText.set( "value", new_value );
    },

    _gotAlertsSummary : function( result )
    {
        if ( result.alertCount == null || result.alertCount == 0 ) {
            this._alerts_stack.selectChild( this._alertsNoneText );
            return;
        }

        this._alerts_stack.selectChild( this._alertsText, { count: result.alertCount } );
    },

    _gotHardwareSummary : function( result )
    {
        if ( (! ("hardwareNotifications" in result)) || (result.hardwareNotifications.length == 0) ) {
            // No problems to list, nothing to do.
        } else {
            // hardware problems.

            d_construct.empty( this._hwProblemsList );

            d_array.forEach( result.hardwareNotifications, d_lang.hitch( this, function( hw ) {

                var msg = "";

                if ( "nodeCount" in hw ) {
                    msg = hw.location + " has " + hw.nodeCount + " nodes unavailable";
                } else {
                    var status_description = (hw.status == 'M' ? "is missing" : "in error");

                    var hw_type = b_BlueGene.hardwareTypes.forLocation( hw.location );

                    if ( hw_type == b_BlueGene.hardwareTypes.LINK ) {
                        var link_re = /(.*),(.*)/;
                        var m = link_re.exec( hw.location );
                        msg = "Link from " + m[1] + " to " + m[2] + " " + status_description;
                    } else {
                        var hw_type_name =
                            (hw_type == b_BlueGene.hardwareTypes.BULK_POWER_SUPPLY ? "Bulk Power Supply" :
                             hw_type == b_BlueGene.hardwareTypes.CLOCK_CARD ? "Clock Card" :
                             hw_type == b_BlueGene.hardwareTypes.COMPUTE_RACK ? "Compute Rack" : // Not going to happen but included for completeness.
                             hw_type == b_BlueGene.hardwareTypes.FAN_MODULE ? "Fan Module" :
                             hw_type == b_BlueGene.hardwareTypes.IO_ADAPTER ? "I/O Adapter" :
                             hw_type == b_BlueGene.hardwareTypes.IO_DRAWER ? "I/O Drawer" :
                             hw_type == b_BlueGene.hardwareTypes.IO_NODE ? "I/O Node" :
                             hw_type == b_BlueGene.hardwareTypes.IO_RACK ? "I/O Rack" :
                             hw_type == b_BlueGene.hardwareTypes.IO_LINK_CHIP ? "I/O Link Chip" :
                             hw_type == b_BlueGene.hardwareTypes.LINK_CHIP ? "Link Chip" :
                             hw_type == b_BlueGene.hardwareTypes.MIDPLANE ? "Midplane" :
                             hw_type == b_BlueGene.hardwareTypes.NODE_BOARD ? "Node Board" :
                             hw_type == b_BlueGene.hardwareTypes.NODE_BOARD_DCA ? "Node Board DC Assembly" :
                             hw_type == b_BlueGene.hardwareTypes.SERVICE_CARD ? "Service Card" :
                             hw_type == b_BlueGene.hardwareTypes.SWITCH ? "Switch" :
                             hw_type == b_BlueGene.hardwareTypes.CLOCK_CARD ? "Clock Card" :
                             (hw_type + " (unexpected)"));

                        msg = hw_type_name + " at " + hw.location + " " + status_description;
                    }
                }

                d_construct.create( "li", { innerHTML: msg }, this._hwProblemsList );
            } ) );
        }

        this._hwLoading.hide();
        if ( (! ("hardwareNotifications" in result)) || (result.hardwareNotifications.length == 0) ) {
            this._hwProblems.hide();
            this._hwAllAvailable.show();
        } else {
            this._hwAllAvailable.hide();
            this._hwProblems.show();
        }
    },


    _gotDiagsSummary : function( result )
    {
        this._diagsLoading.hide();

        if ( (! ("diagnostics" in result)) || result.diagnostics.length == 0 ) {
            this._diagsContainer.hide();
            this._diagsNone.show();
            return;
        }

        // There's at least one diagnostics to show here.

        d_construct.empty( this._diagsList );

        d_array.forEach( result.diagnostics, d_lang.hitch( this, function( d ) {
            var msg;

            if ( d.location ) {
                msg = d.location;
            } else {
                msg = "Block " + d.block;
            }
            d_construct.create( "li", { innerHTML: msg }, this._diagsList );
        } ) );

        this._diagsNone.hide();
        this._diagsContainer.show();
    },


    _gotServiceActionsSummary : function( result )
    {
        this._saLoading.hide();

        if ( (! ("serviceActions" in result)) || result.serviceActions.length == 0 ) {
            this._saContainer.hide();
            this._saNone.show();
            return;
        }

        d_construct.empty( this._saList );

        d_array.forEach( result.serviceActions, d_lang.hitch( this, function( sa ) {
            var msg = sa.action + " on " + sa.location + " by " + sa.prepareUser;
            d_construct.create( "li", { innerHTML: msg }, this._saList );
        } ) );

        this._saNone.hide();
        this._saContainer.show();
    },


    _setCurrentHardware : function( data )
    {
        this._current_hw = {};

        for ( mp_location in data.mps ) {
            var mp = data.mps[mp_location];

            this._current_hw[mp_location] = {};

            if ( "status" in mp ) {
                this._current_hw[mp_location].color = _calcColorForStatus( mp.status );
            }

            if ( "nodeBoards" in mp ) {
                this._current_hw[mp_location].nodeBoards = {};

                for ( nb_pos in mp.nodeBoards ) {
                    this._current_hw[mp_location].nodeBoards[nb_pos] =
                        _calcColorForStatus( mp.nodeBoards[nb_pos] );
                }
            }
        }


        // Note that if the I/O drawer is BLOCK_READY, it's not included in the data.
        // So need to mark all I/O drawers as BLOCK_READY and then update with data from the server.

        // Initially, mark all the I/O drawers as BLOCK_READY.
        d_array.forEach( this._io_drawers, d_lang.hitch( this, function( drawer_location ) {
            this._current_hw[drawer_location] = { color: _calcColorForStatus( "BLOCK_READY" ) };
        } ) );

        for ( rack_location in data.racks ) {
            var rack_info = data.racks[rack_location];

            if ( rack_info.status ) {
                this._current_hw[rack_location] = { color: _calcColorForStatus( rack_info.status ) };
            }

            for ( drawer_pos in rack_info.drawers ) {

                var drawer_location = (rack_location + "-" + drawer_pos);
                this._current_hw[drawer_location] = { color: _calcColorForStatus( rack_info.drawers[drawer_pos] ) };

            }
        }

        this.onMachineHighlightDataChanged();
    },


    _d_number : d_number,
    _ll_format : ll_format

} );

return b_navigator_dijit_SystemSummary;

} );
