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
    "../../dijit/ActionLink",
    "../../dijit/Hideable",
    "../../dijit/OutputText",
    "dojo/dom-construct",
    "dojo/when",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/text!./templates/Hardware.html",
    "module",

    // Used only in template.
    "./Table",
    "dijit/form/Button",
    "dijit/form/Form",
    "dijit/form/ValidationTextBox",
    "dijit/layout/BorderContainer",
    "dijit/layout/ContentPane",
    "dijit/layout/StackContainer"
],
function(
        l_AbstractTemplatedContainer,
        l_MonitorActiveMixin,
        ll_format,
        b_dijit_ActionLink,
        b_dijit_Hideable,
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


var _common_location_calc_properties = function( o )
    {
        return { label: o.location };
    };


var _common_status_column_definition = {
        title: "Status",
        field: "status",
        dijitType: b_dijit_OutputText,
        dijitProperties: { formatter: ll_format.hardwareStatus, classFn: ll_format.hardwareStatusCalcClass, nullText: "Loading..." }
    };


var _na_formatter = function( val )
    {
        return (val ? val : "n/a");
    };


var _calcParent = function( location )
    {
        var parent = null;

        var parent_res = /^(.+)-.+$/.exec( location );

        if ( parent_res != null ) {
            parent = parent_res[1];
        }

        return parent;
    };


var b_navigator_dijit_Hardware = d_declare(
        [ l_AbstractTemplatedContainer, l_MonitorActiveMixin ],

{

    templateString : template,

    _fetch_data_fn : null,

    _dirty: true,

    _current_location: null,

    _previous_location: null,

    _hardware_highlighting: null,

    _rack_clock_card_supplies_dijs : null, // [] dijits

    _rack_clock_card_source: null,
    _clock_source: null,

    _cur_req : null, // Deferred, current request.
    _machine_req : null, // Deferred, request to get machine data.

    _midplane_count : 1, // Guess of how many midplanes there are on the system.

    _common_location_column_definition : null,


    constructor : function()
    {
        var loc_cb = d_lang.hitch( this, function( dij, o )
            {
                dij.on( "click", d_lang.hitch( this, function() { this._setLocation( o.location ); } ) );
            } );

        this._common_location_column_definition = { title: "Location", dijitType: b_dijit_ActionLink, calcProperties: _common_location_calc_properties, createCb: loc_cb };
    },


    setFetchDataFn : function( fn )
    {
        this._fetch_data_fn = fn;
        this._refresh();
    },


    getMachineHighlightData: function()
    {
        var ret = {
                loading: this._machine_req,
                highlighting: this._hardware_highlighting
            };

        return ret;
    },

    onMachineHighlightingChanged : function() {},


    notifyLocationSelected : function( loc )
    {
        this._locationSelected( loc );
    },


    notifyRefresh : function()
    {
        this._refresh();
    },


    // override
    startup: function( bgws )
    {
        this.inherited( arguments );


        this._rack_clock_card_supplies_dijs = [];


        this._parentButton.on( "click", d_lang.hitch( this, this._parentClicked ) );
        this._jumpToForm.on( "submit", d_lang.hitch( this, this._onJumpToFormSubmit ) );


        this._setupRackMidplanesTable();
        this._setupRackIoDrawersTable();
        this._setupRackBpmsTable();

        this._setupMidplaneNodeBoardsTable();

        this._setupNodeBoardComputeCardsTable();
        this._setupNodeBoardDcasTable();

        this._setupIoRackDrawersTable();
        this._setupIoRackClockCardsTable();
        this._setupIoRackPowerModulesTable();

        this._setupIoDrawerNodesTable();


        this._rackClockCardSource.on( "click", d_lang.hitch( this, function() { this._setLocation( this._rack_clock_card_source.substr( 0, 3 ) ); } ) );

        this._midplaneClockSource.on( "click", d_lang.hitch( this, function() { this._setLocation( this._clock_source.substr( 0, 3 ) ); } ) );
        this._ioDrawerClockSourceHideable.on( "click", d_lang.hitch( this, function() { this._setLocation( this._clock_source.substr( 0, 3 ) ); } ) );
    },


    // override
    destroy : function()
    {
        if ( this._cur_req ) {
            this._cur_req.cancel();
            this._cur_req = null;
        }
        if ( this._machine_req ) {
            this._machine_req.cancel();
            this._machine_req = null;
        }

        this.inherited( arguments );
    },


    // override MonitorActiveMixin.
    _notifyActive: function()
    {
        if ( ! this._dirty )  return;

        this._setLocation( this._current_location );
    },


    _setupRackMidplanesTable : function()
    {
        var layout = { columns: [
                this._common_location_column_definition,
                _common_status_column_definition
            ] };

        this._rackMidplanes.set( "layout", layout );
    },


    _setupRackIoDrawersTable : function()
    {

        var layout = { columns: [
                this._common_location_column_definition,
                _common_status_column_definition
            ] };

        this._rackIoDrawersTable.set( "layout", layout );
    },


    _setupRackBpmsTable : function()
    {
        var layout = { columns : [
                { title: "Location", field: "location", dijitType: b_dijit_OutputText },
                _common_status_column_definition,
                { title: "Serial Number", field: "serialNumber", dijitType: b_dijit_OutputText },
                { title: "Product ID", field: "productId", dijitType: b_dijit_OutputText }
            ] };

        this._rackBpmsTable.set( "layout", layout );
    },


    _setupMidplaneNodeBoardsTable : function()
    {
        var node_status_calc_properties = function( nb_data )
            {
                if ( ! nb_data )  return { value: "" };

                var msg_parts = [];
                if ( "notAvailableNodeCount" in nb_data ) {
                    msg_parts.push( "" + nb_data.notAvailableNodeCount + " " + (nb_data.notAvailableNodeCount == 1 ? "node is" : "nodes are" ) + " not available" );
                }
                if ( "notAvailableDCACount" in nb_data ) {
                    msg_parts.push( "" + nb_data.notAvailableDCACount + " " + (nb_data.notAvailableDCACount == 1 ? "DCA is" : "DCAs are" ) + " not available" );
                }

                return { value: msg_parts.join( ", " ) };
            };

        var layout = { columns : [
                this._common_location_column_definition,
                _common_status_column_definition,
                { title: "Node Status", dijitType: b_dijit_OutputText, calcProperties: node_status_calc_properties }
            ] };

        this._midplaneNodeBoardsTable.set( "layout", layout );
    },


    _setupNodeBoardComputeCardsTable : function()
    {
        var io_link_calc_properties = function( o )
            {
                if ( "ioLinkNode" in o ) {
                    var props = { label: o.ioLinkNode };

                    if ( ("ioLinkStatus" in o && o.ioLinkStatus !== "A") ||
                         ("ioLinkNodeStatus" in o && o.ioLinkNodeStatus !== "A") )
                    {
                        props["class"] = "bgHardwareNotAvailable";
                    }

                    return props;
                } else {
                    return { label: "", disabled: true };
                }
            };

        var io_link_cb = d_lang.hitch( this, function( dij, o )
            {
                if ( ! ("ioLinkNode" in o) )  return;

                var parent_location = _calcParent( o.ioLinkNode );

                dij.on( "click", d_lang.hitch( this, function() { this._setLocation( parent_location ); } ) );
            } );


        var layout = { columns : [
                { title: "Location", field: "location", dijitType: b_dijit_OutputText },
                _common_status_column_definition,
                { title: "Serial Number", field: "serialNumber", dijitType: b_dijit_OutputText, dijitProperties: { formatter: _na_formatter } },
                { title: "Product ID", field: "productId", dijitType: b_dijit_OutputText, dijitProperties: { formatter: _na_formatter } },
                { title: "Memory Size", field: "memorySize", dijitType: b_dijit_OutputText },
                { title: "Voltage", field: "voltage", dijitType: b_dijit_OutputText },
                { title: "I/O Link", dijitType: b_dijit_ActionLink, calcProperties: io_link_calc_properties, createCb: io_link_cb }
            ] };

        this._nodeBoardComputeCardsTable.set( "layout", layout );
    },


    _setupNodeBoardDcasTable : function()
    {
        var layout = { columns : [
                { title: "Location", field: "location", dijitType: b_dijit_OutputText },
                _common_status_column_definition,
                { title: "Serial Number", field: "serialNumber", dijitType: b_dijit_OutputText, dijitProperties: { formatter: _na_formatter } },
                { title: "Product ID", field: "productId", dijitType: b_dijit_OutputText, dijitProperties: { formatter: _na_formatter } }
            ] };

        this._nodeBoardDcasTable.set( "layout", layout );
    },


    _setupIoRackDrawersTable : function()
    {
        var layout = { columns: [
                this._common_location_column_definition,
                _common_status_column_definition
            ] };

        this._ioRackDrawersTable.set( "layout", layout );
    },


    _setupIoRackClockCardsTable : function()
    {
        var layout = { columns: [
                { title: "Location", field: "location", dijitType: b_dijit_OutputText },
                _common_status_column_definition,
                { title: "Serial Number", field: "serialNumber", dijitType: b_dijit_OutputText, dijitProperties: { formatter: _na_formatter } },
                { title: "Product ID", field: "productId", dijitType: b_dijit_OutputText, dijitProperties: { formatter: _na_formatter } },
                { title: "Clock Hz", field: "clockHz", dijitType: b_dijit_OutputText }
            ] };

        this._ioRackClockCardsTable.set( "layout", layout );
    },


    _setupIoRackPowerModulesTable : function()
    {
        var layout = { columns: [
                { title: "Location", field: "location", dijitType: b_dijit_OutputText },
                _common_status_column_definition,
                { title: "Serial Number", field: "serialNumber", dijitType: b_dijit_OutputText, dijitProperties: { formatter: _na_formatter } },
                { title: "Product ID", field: "productId", dijitType: b_dijit_OutputText, dijitProperties: { formatter: _na_formatter } }
            ] };

        this._ioRackPowerModulesTable.set( "layout", layout );
    },


    _setupIoDrawerNodesTable : function()
    {
        var io_link_calc_properties = function( link_location_field_name, link_status_field_name, o )
            {
                if ( link_location_field_name in o ) {
                    var props = { label: o[link_location_field_name] };

                    if ( link_status_field_name in o && o[link_status_field_name] !== "A" ) {
                        props["class"] = "bgHardwareNotAvailable";
                    }

                    return props;
                } else {
                    return { label: "", disabled: true };
                }
            };

        var io_link_cb = d_lang.hitch( this, function( link_location_field_name, dij, o )
            {
                if ( ! (link_location_field_name in o) )  return;

                var parent_location = _calcParent( o[link_location_field_name] );

                dij.on( "click", d_lang.hitch( this, function() { this._setLocation( parent_location ); } ) );
            } );

        var layout = { columns: [
                { title: "Location", field: "location", dijitType: b_dijit_OutputText },
                _common_status_column_definition,
                { title: "IP address", field: "ipAddress", dijitType: b_dijit_OutputText },
                { title: "Serial Number", field: "serialNumber", dijitType: b_dijit_OutputText, dijitProperties: { formatter: _na_formatter } },
                { title: "Product ID", field: "productId", dijitType: b_dijit_OutputText, dijitProperties: { formatter: _na_formatter } },
                { title: "Memory Size", field: "memorySize", dijitType: b_dijit_OutputText, dijitProperties: { formatter: _na_formatter } },
                { title: "Voltage", field: "voltage", dijitType: b_dijit_OutputText, dijitProperties: { formatter: _na_formatter } },
                {
                  title: "Compute Node 1",
                  dijitType: b_dijit_ActionLink,
                  calcProperties: d_lang.partial( io_link_calc_properties, "cnLink1Location", "cnLink1Status" ),
                  createCb: d_lang.partial( io_link_cb, "cnLink1Location" )
                },
                {
                  title: "Compute Node 2",
                  dijitType: b_dijit_ActionLink,
                  calcProperties: d_lang.partial( io_link_calc_properties, "cnLink2Location", "cnLink2Status" ),
                  createCb: d_lang.partial( io_link_cb, "cnLink2Location" )
                }
            ] };

        this._ioDrawerNodesTable.set( "layout", layout );
    },


    _refresh: function()
    {
        if ( ! this._isActive() /*MonitorActiveMixin*/ ) {
            this._dirty = true;
            return;
        }

        this._setLocation( this._current_location );
    },


    _onJumpToFormSubmit : function( e )
    {
        e.preventDefault();
        if ( ! this._jumpToForm.isValid() ) {
            this._jumpToInput.validate( false );
            return;
        }
        this._jumpToLocation();
    },


    _locationSelected: function( location )
    {
        if ( ! this._isActive() /*MonitorActiveMixin*/ )  return;

        var new_location = this._calcNewLocation( location );

        console.log( module.id + ": selected " + location + (new_location != null ? " -> " + new_location : "") );

        if ( new_location != null ) {
            this._setLocation( new_location );
        }
    },

    _calcNewLocation: function( selected_location )
    {
        // If looking at machine, display the selected rack.
        if ( this._current_location == null )  return selected_location.substr( 0, 3 );

        if ( this._current_location == selected_location )  return null; // clicked where looking, don't change.

        var selected_rack = selected_location.substr( 0, 3 );
        var current_rack = this._current_location.substr( 0, 3 );

        // if clicked on a different rack, switch to that rack.
        if ( selected_rack != current_rack )  return selected_location.substr( 0, 3 );

        // otherwise clicked in the same rack...
        var selected_midplane = selected_location.substr( 0, 6 );
        var current_midplane = this._current_location.substr( 0, 6 );

        if ( selected_midplane != current_midplane )  return selected_midplane;

        return selected_location; // selected a different node board.
    },

    _parentClicked: function()
    {
        var parent = _calcParent( this._current_location );

        console.log( module.id + ": parent button clicked " + this._current_location + " -> " + parent );

        this._setLocation( parent );
    },

    _jumpToLocation: function()
    {
        var target_location = this._jumpToInput.get( "value" );

        console.log( module.id + ": jump to button clicked, target=", target_location );

        var display_location = null;

        if ( target_location.match( /^R..-M.-N../ ) ) {
            display_location = target_location.substr( 0, 10 );
        } else if ( target_location.match( /^R..-M./ ) ) {
            display_location = target_location.substr( 0, 6 );
        } else if ( target_location.match( /^R..-I./ ) ) {
            display_location = target_location.substr( 0, 6 );
        } else if ( target_location.match( /^R../ ) ) {
            display_location = target_location.substr( 0, 3 );
        } else if ( target_location.match( /^Q..-I./ ) ) {
            display_location = target_location.substr( 0, 6 );
        } else if ( target_location.match( /^Q../ ) ) {
            display_location = target_location.substr( 0, 3 );
        }

        this._setLocation( display_location );
    },

    _setLocation: function( new_location )
    {
        if ( ! this._fetch_data_fn )  return; // Not ready to fetch yet.

        this._nowLoading();

        console.log( module.id + ": setting location to " + new_location );

        if ( new_location == null ) {
            // If new location is null then display the machine details.

            this._previous_location = this._current_location;
            this._current_location = new_location;
            this._setMachine();
            this._parentButton.set( "disabled", true );
            return;
        }

        if ( new_location.match( /^R..$/ ) ) {
            this._previous_location = this._current_location;
            this._current_location = new_location;
            this._setRack();
            this._parentButton.set( "disabled", false );
            return;
        }

        if ( new_location.match( /^R..-M.$/ ) ) {
            this._previous_location = this._current_location;
            this._current_location = new_location;
            this._setMidplane();
            this._parentButton.set( "disabled", false );
            return;
        }

        if ( new_location.match( /^R..-M.-N..$/ ) ) {
            this._previous_location = this._current_location;
            this._current_location = new_location;
            this._setNodeBoard();
            this._parentButton.set( "disabled", false );
            return;
        }

        if ( new_location.match( /^[QR]..-I.$/ ) ) {
            this._previous_location = this._current_location;
            this._current_location = new_location;
            this._setIoDrawer();
            this._parentButton.set( "disabled", false );
            return;
        }

        if ( new_location.match( /^Q..$/ ) ) {
            this._previous_location = this._current_location;
            this._current_location = new_location;
            this._setIoRack();
            this._parentButton.set( "disabled", false );
            return;
        }

        console.log( module.id + ": Unexpected location " + new_location );
    },

    _setMachine: function()
    {
        this._setDisplayValues( {
                "_machineStatus": null
            } );

        this._show( this._machineContainer );

        if ( this._cur_req ) {
            this._cur_req.cancel();
            this._cur_req = null;
        }

        if ( this._machine_req )  this._machine_req.cancel();
        this._machine_req = d_when(
                this._fetch_data_fn( null ),
                d_lang.hitch( this, this._gotMachine )
            );

        this.onMachineHighlightingChanged();
    },

    _gotMachine: function( machine_data )
    {
        this._doneLoading( false );
        this._machine_req = null;

        this._setDisplayValues( {
                "_machineStatus": machine_data.status,
                "_machineSerialNumber": machine_data.serialNumber,
                "_machineProductId": machine_data.productId,
                "_machineDescription": ("description" in machine_data ? machine_data.description : "N/A"),
                "_machineHasEthGw": machine_data.hasEthernetGateway,
                "_machineMtu": machine_data.mtu,
                "_machineClockhz": machine_data.clockHz,
                "_machineBringupOption": ("bringupOptions" in machine_data ? machine_data.bringupOptions : "N/A")
            } );

        this._machineDescriptionHideable.set( "visibility", "description" in machine_data ? b_dijit_Hideable.Visibility.VISIBLE : b_dijit_Hideable.Visibility.HIDDEN );


        this._hardware_highlighting = {};

        var problems = {};

        if ( "notAvailable" in machine_data ) {
            for ( loc in machine_data.notAvailable ) {
                if ( loc.match( /^R..$/ ) ) {
                    // It's a compute rack.

                    var mp_0_loc = loc + "-M0";
                    var mp_1_loc = loc + "-M1";

                    if ( mp_0_loc in this._hardware_highlighting ) {
                        this._hardware_highlighting[mp_0_loc].color = "red";
                    } else {
                        this._hardware_highlighting[mp_0_loc] = { color: "red", nodeBoards: {} };
                    }

                    if ( mp_1_loc in this._hardware_highlighting ) {
                        this._hardware_highlighting[mp_1_loc].color = "red";
                    } else {
                        this._hardware_highlighting[mp_1_loc] = { color: "red", nodeBoards: {} };
                    }

                    if ( ! (loc in problems) )  problems[loc] = {};

                    if ( "powerModuleCount" in machine_data.notAvailable[loc] ) {
                        problems[loc].powerModuleCount = machine_data.notAvailable[loc].powerModuleCount;
                    }

                } else if ( loc.match( /^Q..$/ ) ) {
                    // It's an I/O rack.

                    this._hardware_highlighting[loc] = { color: "red" };

                    if ( ! (loc in problems) )  problems[loc] = {};
                    problems[loc].notAvailable = true;

                } else if ( loc.match( /^R..-M.$/ ) ) {
                    // it's a midplane.

                    if ( loc in this._hardware_highlighting ) {
                        this._hardware_highlighting[loc].color = "red";
                    } else {
                        this._hardware_highlighting[loc] = { color: "red", nodeBoards: {} };
                    }

                    var rack_loc = loc.substr( 0, 3 );

                    if ( ! (rack_loc in problems) )   problems[rack_loc] = {};

                    if ( "status" in machine_data.notAvailable[loc] ) {
                        if ( ! ("midplaneCount" in problems[rack_loc] ) )  problems[rack_loc].midplaneCount = 0;
                        ++(problems[rack_loc].midplaneCount);
                    }
                    if ( "serviceCardStatus" in machine_data.notAvailable[loc] ) {
                        if ( ! ("serviceCardCount" in problems[rack_loc] ) )  problems[rack_loc].serviceCardCount = 0;
                        ++(problems[rack_loc].serviceCardCount);
                    }
                    if ( "switchCount" in machine_data.notAvailable[loc] ) {
                        if ( ! ("switchCount" in problems[rack_loc] ) )  problems[rack_loc].switchCount = 0;
                        problems[rack_loc].switchCount += machine_data.notAvailable[loc].switchCount;
                    }

                } else if ( loc.match( /^R..-M.-N..$/ ) ) {
                    // it's a node board.

                    var parts = /^(R..-M.)-(N..)$/.exec( loc );
                    var midplane = parts[1];
                    var nb_pos = parts[2];

                    if ( ! (midplane in this._hardware_highlighting) ) {
                        this._hardware_highlighting[midplane] = { nodeBoards: {} };
                    } else if ( ! ("nodeBoards" in this._hardware_highlighting[midplane]) ) {
                        this._hardware_highlighting[midplane].nodeBoards = {};
                    }

                    this._hardware_highlighting[midplane].nodeBoards[nb_pos] = "red";

                    var rack_loc = loc.substr( 0, 3 );

                    if ( "status" in machine_data.notAvailable[loc] ) {
                        if ( ! (rack_loc in problems) )   problems[rack_loc] = {};
                        if ( ! ("nodeboardCount" in problems[rack_loc] ) )  problems[rack_loc].nodeboardCount = 0;
                        ++(problems[rack_loc].nodeboardCount);
                    }
                    if ( "nodeCount" in machine_data.notAvailable[loc] ) {
                        if ( ! (rack_loc in problems) )   problems[rack_loc] = {};
                        if ( ! ("nodeCount" in problems[rack_loc] ) )  problems[rack_loc].nodeCount = 0;
                        problems[rack_loc].nodeCount += machine_data.notAvailable[loc].nodeCount;
                    }
                    if ( "DCACount" in machine_data.notAvailable[loc] ) {
                        if ( ! (rack_loc in problems) )   problems[rack_loc] = {};
                        if ( ! ("DCACount" in problems[rack_loc] ) )  problems[rack_loc].DCACount = 0;
                        problems[rack_loc].DCACount += machine_data.notAvailable[loc].DCACount;
                    }

                } else if ( loc.match( /^[QR]..-I.$/ ) ) {
                    // It's an I/O drawer, highlight the drawer.

                    this._hardware_highlighting[loc] = { color: "red" };

                    var rack_loc = loc.substr( 0, 3 );

                    if ( "status" in machine_data.notAvailable[loc] ) {
                        if ( ! (rack_loc in problems) )  problems[rack_loc] = {};
                        if ( ! ("ioDrawerCount" in problems[rack_loc]) )  problems[rack_loc].ioDrawerCount = 0;
                        ++(problems[rack_loc].ioDrawerCount);
                    }
                    if ( "ioNodeCount" in machine_data.notAvailable[loc] ) {
                        if ( ! (rack_loc in problems) )  problems[rack_loc] = {};
                        if ( ! ("ioNodeCount" in problems[rack_loc]) )  problems[rack_loc].ioNodeCount = 0;
                        problems[rack_loc].ioNodeCount += machine_data.notAvailable[loc].ioNodeCount;
                    }
                }
            }
        }

        { // update problem list
            d_construct.empty( this._machineProblemsList );

            var problem_locs = [];

            for ( loc in problems ) {
                problem_locs.push( loc );
            }

            problem_locs.sort();

            var any_problems = false;

            d_array.forEach( problem_locs, d_lang.hitch( this, function( loc ) {
                any_problems = true;

                var e1 = d_construct.create( "li", { "innerHTML": loc }, this._machineProblemsList );
                var e2 = d_construct.create( "ul", {}, e1 );

                if ( "midplaneCount" in problems[loc] ) {
                    d_construct.create(
                            "li",
                            { "innerHTML": "" + problems[loc].midplaneCount + " " + (problems[loc].midplaneCount == 1 ? "midplane is" : "midplanes are") + " not available" },
                            e2
                        );
                }
                if ( "serviceCardCount" in problems[loc] ) {
                    d_construct.create(
                            "li",
                            { "innerHTML": "" + problems[loc].serviceCardCount + " " + (problems[loc].serviceCardCount == 1 ? "service card is" : "service cards are") + " not available" },
                            e2
                        );
                }
                if ( "nodeboardCount" in problems[loc] ) {
                    d_construct.create(
                            "li",
                            { "innerHTML": "" + problems[loc].nodeboardCount + " " + (problems[loc].nodeboardCount == 1 ? "node board is" : "node boards are") + " not available" },
                            e2
                        );
                }
                if ( "nodeCount" in problems[loc] ) {
                    d_construct.create(
                            "li",
                            { "innerHTML": "" + problems[loc].nodeCount + " " + (problems[loc].nodeCount == 1 ? "node is" : "nodes are") + " not available" },
                            e2
                        );
                }
                if ( "DCACount" in problems[loc] ) {
                    d_construct.create(
                            "li",
                            { "innerHTML": "" + problems[loc].DCACount + " node board " + (problems[loc].DCACount == 1 ? "DCA is" : "DCAs are") + " not available" },
                            e2
                        );
                }
                if ( "ioDrawerCount" in problems[loc] ) {
                    d_construct.create(
                            "li",
                            { "innerHTML": "" + problems[loc].ioDrawerCount + " " + (problems[loc].ioDrawerCount == 1 ? "I/O drawer is" : "I/O drawers are") + " not available" },
                            e2
                        );
                }
                if ( "ioNodeCount" in problems[loc] ) {
                    d_construct.create(
                            "li",
                            { "innerHTML": "" + problems[loc].ioNodeCount + " " + (problems[loc].ioNodeCount == 1 ? "I/O node is" : "I/O nodes are") + " not available" },
                            e2
                        );
                }
                if ( "powerModuleCount" in problems[loc] ) {
                    d_construct.create(
                            "li",
                            { "innerHTML": "" + problems[loc].powerModuleCount + " " + (problems[loc].powerModuleCount == 1 ? "power module is" : "power modules are") + " not available" },
                            e2
                        );
                }
                if ( "switchCount" in problems[loc] ) {
                    d_construct.create(
                            "li",
                            { "innerHTML": "" + problems[loc].switchCount + " " + (problems[loc].switchCount == 1 ? "switch is" : "switches are") + " not available" },
                            e2
                        );
                }
            } ) );

            if ( any_problems ) {
                this._machineProblemsContainer.show();
            } else {
                this._machineProblemsContainer.hide();
            }
        }

        this.onMachineHighlightingChanged();
    },


    _setRack: function()
    {
        console.log( module.id + ": displaying rack ", this._current_location );

        var set_obj = {
                "_rackLocation": this._current_location,
                "_rackClockCardLocation": (this._current_location  + "-K"),
                "_rackClockCardStatus": null,
                "_rackClockCardFreq": null,
                "_rackClockCardSerialNumber": null,
                "_rackClockCardProductId": null,
                "_rackClockCardSource": null
            };

        this._setDisplayValues( set_obj );


        var midplanes_data = [
                { location: (this._current_location + "-M0"), status: null }
            ];

        if ( this._midplane_count === 2 ) {
            midplanes_data.push( { location: (this._current_location + "-M1"), status: null } );
        }

        this._rackMidplanes.set( "data", { data: midplanes_data, idProperty: "location" } );

        this._rackClockCardSourceHideable.hide();
        this._rackClockCardSuppliesHideable.hide();


        this._rackIoDrawersTable.setStore( null, {} /* query */ );


        // Show bulk power modules with status loading
        var bpm_data = [];

        for ( var bpm_no = 0 ; bpm_no < 36 ; ++bpm_no ) {
            var bp_supply = Math.floor( bpm_no / 9 );
            var bpm = bpm_no % 9;

            bpm_data.push(
                    { location: (this._current_location + "-B" + bp_supply + "-P" + bpm), status: null, serialNumber: null, productId: null }
                );
        }

        this._rackBpmsTable.set( "data", { data: bpm_data, idProperty: "location" } );


        this._rackProblemsContainer.hide();

        this._show( this._rackContainer );

        if ( this._cur_req )  this._cur_req.cancel();
        this._cur_req = d_when(
                this._fetch_data_fn( this._current_location ),
                d_lang.hitch( this, this._gotRack ),
                d_lang.hitch( this, this._fetchFailed )
            );

        if ( ! this._machine_req ) {
            this._machine_req = d_when(
                    this._fetch_data_fn( null ),
                    d_lang.hitch( this, this._gotMachine )
                );
            this.onMachineHighlightingChanged();
        }
    },

    _gotRack: function( rack_data )
    {
        console.log( module.id + ": Got rack data", rack_data );

        this._doneLoading( false );


        var set_obj = {};


        var midplanes_data = [
                { location: (this._current_location + "-M0"), status: rack_data.midplanes[0].status }
            ];

        if ( rack_data.midplanes.length === 2 ) {
            midplanes_data.push( { location: (this._current_location + "-M1"), status: rack_data.midplanes[1].status } );
        }

        this._midplane_count = rack_data.midplanes.length;

        this._rackMidplanes.set( "data", { data: midplanes_data, idProperty: "location" } );


        if ( rack_data.clockCard ) {

            this._rackClockCardContainer.show();

            set_obj["_rackClockCardStatus"] = rack_data.clockCard.status;
            set_obj["_rackClockCardFreq"] = rack_data.clockCard.clockHz;
            set_obj["_rackClockCardSerialNumber"] = rack_data.clockCard.serialNumber;
            set_obj["_rackClockCardProductId"] = rack_data.clockCard.productId;

            if ( "source" in rack_data.clockCard ) {
                set_obj["_rackClockCardSource"] = rack_data.clockCard.source;
                this._rack_clock_card_source = rack_data.clockCard.source;
                this._rackClockCardSourceHideable.show();
            } else {
                this._rackClockCardSourceHideable.hide();
            }

            d_array.forEach( this._rack_clock_card_supplies_dijs, function( j ) { j.destroyRecursive(); } );
            this._rack_clock_card_supplies_dijs = [];

            d_construct.empty( this._rackClockCardSupplies );

            if ( rack_data.clockCard.supplies ) {
                var first = true;

                d_array.forEach( rack_data.clockCard.supplies, d_lang.hitch( this, function( loc ) {

                    if ( first ) first = false;
                    else d_construct.create( "span", { innerHTML: ", " }, this._rackClockCardSupplies );

                    var action_link = new b_dijit_ActionLink( { label : loc } );
                    action_link.placeAt( this._rackClockCardSupplies );
                    action_link.startup();

                    var target_loc = (loc[loc.length-1] === 'K' ? loc.substr(0,3) : loc); // if the target is a clock card, then location should be the rack.
                    action_link.on( "click", d_lang.hitch( this, function( ev ) { this._setLocation( target_loc ); } ) );

                    this._rack_clock_card_supplies_dijs.push( action_link );

                } ) );

                this._rackClockCardSuppliesHideable.show();
            } else {
                this._rackClockCardSuppliesHideable.hide();
            }


        } else {

            this._rackClockCardContainer.hide();

        }


        this._setDisplayValues( set_obj );


        var probs = {};

        for ( var i = 0 ; i < rack_data.midplanes.length ; ++i ) { // for each midplane.
            var mp_loc = this._current_location + "-M" + i;

            if ( rack_data.midplanes[i].serviceCardStatus != "A" ) {
                if ( ! (mp_loc in probs) )  probs[mp_loc] = {};
                probs[mp_loc].serviceCardStatus = rack_data.midplanes[i].serviceCardStatus;
            }
            if ( "notAvailableSwitchCount" in rack_data.midplanes[i] ) {
                if ( ! (mp_loc in probs) )  probs[mp_loc] = {};
                probs[mp_loc].switchCount = rack_data.midplanes[i].notAvailableSwitchCount;
            }
            if ( "notAvailableNodeBoardCount" in rack_data.midplanes[i] ) {
                if ( ! (mp_loc in probs) )  probs[mp_loc] = {};
                probs[mp_loc].nodeBoardCount = rack_data.midplanes[i].notAvailableNodeBoardCount;
            }
            if ( "notAvailableNodeCount" in rack_data.midplanes[i] ) {
                if ( ! (mp_loc in probs) )  probs[mp_loc] = {};
                probs[mp_loc].nodeCount = rack_data.midplanes[i].notAvailableNodeCount;
            }
            if ( "notAvailableDCACount" in rack_data.midplanes[i] ) {
                if ( ! (mp_loc in probs) )  probs[mp_loc] = {};
                probs[mp_loc].DCACount = rack_data.midplanes[i].notAvailableDCACount;
            }
        }

        if ( "ioDrawers" in rack_data ) {

            this._rackIoDrawersTable.set( "data", { data: rack_data.ioDrawers, idProperty: "location" } );

            this._rackIoDrawersContainer.show();

        } else {
            // No I/O drawers.
            this._rackIoDrawersContainer.hide();
        }


        this._rackBpmsTable.set( "data", { data: rack_data.powerModules, idProperty: "location" } );


        { // update problem list
            d_construct.empty( this._rackProblemsList );

            var any_probs = false;

            for ( loc in probs ) {
                any_probs = true;

                var e1 = d_construct.create( "li", { "innerHTML": loc }, this._rackProblemsList );
                var e2 = d_construct.create( "ul", {}, e1 );

                if ( "serviceCardStatus" in probs[loc] ) {
                    d_construct.create(
                            "li",
                            { "innerHTML": "Service card is not available" },
                            e2
                        );
                }
                if ( "switchCount" in probs[loc] ) {
                    d_construct.create(
                            "li",
                            { "innerHTML": "" + probs[loc].switchCount + " " + (probs[loc].switchCount == 1 ? "switch is" : "switches are") + " not available" },
                            e2
                        );
                }
                if ( "nodeBoardCount" in probs[loc] ) {
                    d_construct.create(
                            "li",
                            { "innerHTML": "" + probs[loc].nodeBoardCount + " " + (probs[loc].nodeBoardCount == 1 ? "node board is" : "node boards are") + " not available" },
                            e2
                        );
                }
                if ( "nodeCount" in probs[loc] ) {
                    d_construct.create(
                            "li",
                            { "innerHTML": "" + probs[loc].nodeCount + " " + (probs[loc].nodeCount == 1 ? "node is" : "nodes are") + " not available" },
                            e2
                        );
                }
                if ( "DCACount" in probs[loc] ) {
                    d_construct.create(
                            "li",
                            { "innerHTML": "" + probs[loc].DCACount + " " + (probs[loc].DCACount == 1 ? "DCA is" : "DCAs are") + " not available" },
                            e2
                        );
                }
                if ( "ioNodeCount" in probs[loc] ) {
                    d_construct.create(
                            "li",
                            { "innerHTML": "" + probs[loc].ioNodeCount + " " + (probs[loc].ioNodeCount == 1 ? "I/O node is" : "I/O nodes are") + " not available" },
                            e2
                        );
                }
            }

            d_array.forEach( rack_data.powerModules, function( pm ) {
                    if ( pm.status == "A" )  return;

                    any_probs = true;

                    d_construct.create( "li",
                            { "innerHTML": "Power module at " + pm.location + " is not available" },
                            this._rackProblemsList
                        );
                } );

            if ( any_probs )  this._rackProblemsContainer.show();
            else  this._rackProblemsContainer.hide();
        }
    },

    _setMidplane: function()
    {
        console.log( module.id + ": displaying ", this._current_location );

        var set_obj = {
                "_midplaneLocation": this._current_location,
                "_midplaneStatus": null,
                "_midplaneSerialNumber": null,
                "_midplaneProductId": null,
                "_midplaneTorusCoords": null,
                "_midplaneClockSource": null,

                "_midplaneServiceCardLocation": (this._current_location + "-S"),
                "_midplaneServiceCardStatus": null
            };

        this._setDisplayValues( set_obj );


        var nb_data = [];
        for ( var i = 0 ; i < 16 ; ++i ) {
            nb_data.push( {
                    location: (this._current_location + "-N" + (i < 10 ? "0" + i : "" + i)),
                    status: null
                } );
        }

        this._midplaneNodeBoardsTable.set( "data", { data: nb_data, idProperty: "location" } );


        this._midplaneSwitchProblemsHideable.hide();

        this._show( this._midplaneContainer );

        if ( this._cur_req )  this._cur_req.cancel();
        this._cur_req = d_when(
                this._fetch_data_fn( this._current_location  ),
                d_lang.hitch( this, this._gotMidplane ),
                d_lang.hitch( this, this._fetchFailed )
            );

        if ( ! this._machine_req ) {
            this._machine_req = d_when(
                    this._fetch_data_fn( null ),
                    d_lang.hitch( this, this._gotMachine )
                );
            this.onMachineHighlightingChanged();
        }
    },


    _gotMidplane: function( mp_data )
    {
        console.log( module.id + ": Got midplane data, ", mp_data );

        this._doneLoading( false );

        var set_obj = {
                "_midplaneStatus": mp_data.status,
                "_midplaneSerialNumber": ("serialNumber" in mp_data ? mp_data.serialNumber : "N/A"),
                "_midplaneProductId": mp_data.productId,
                "_midplaneTorusCoords": ("[" + mp_data.torusCoord[0] + "," + mp_data.torusCoord[1] + "," + mp_data.torusCoord[2] + "," + mp_data.torusCoord[3] + "]"),

                "_midplaneServiceCardStatus": mp_data.serviceCard.status,
                "_midplaneServiceCardSerialNumber": ("serialNumber" in mp_data.serviceCard ? mp_data.serviceCard.serialNumber : "N/A"),
                "_midplaneServiceCardProductId": mp_data.serviceCard.productId
            };

        if ( "clockSource" in mp_data ) {
            this._clock_source = mp_data.clockSource;
            set_obj["_midplaneClockSource"] = mp_data.clockSource;
            this._midplaneClockSourceHideable.show();
        } else {
            this._midplaneClockSourceHideable.hide();
        }


        this._midplaneNodeBoardsTable.set( "data", { data: mp_data.nodeBoards, idProperty: "location" } );


        var switch_prob_parts = [];

        for ( var i = 0 ; i < 4 ; ++i ) {
            if ( mp_data.switches[i].status != "A" ) {
                switch_prob_parts.push( mp_data.switches[i].id + " is not available" );
            }
        }

        set_obj["_midplaneSwitchProblems"] = switch_prob_parts.join( ", " );

        this._setDisplayValues( set_obj );

        if ( switch_prob_parts.length != 0 ) {
            this._midplaneSwitchProblemsHideable.show();
        }
    },


    _setNodeBoard: function()
    {
        console.log( module.id + ": displaying ", this._current_location );

        var set_obj = {
                "_nodeBoardLocation": this._current_location,
                "_nodeBoardStatus": null,
                "_nodeBoardSerialNumber": null,
                "_nodeBoardProductId": null
            };

        this._setDisplayValues( set_obj );


        var cc_data = [];

        for ( var i = 0 ; i < 32 ; ++i ) {

            cc_data.push(
                    {
                      location: (this._current_location + "-J" + (i < 10 ? "0" + i : "" + i)),
                      status: null
                    }
                );
        }

        this._nodeBoardComputeCardsTable.set( "data", { data: cc_data, idProperty: "location" } );


        var dca_data = [
                { location: (this._current_location + "-D0"), status: null },
                { location: (this._current_location + "-D1"), status: null }
            ];

        this._nodeBoardDcasTable.set( "data", { data: dca_data, idProperty: "location" } );


        this._show( this._nodeBoardContainer );


        if ( this._cur_req )  this._cur_req.cancel();
        this._cur_req = d_when(
                this._fetch_data_fn( this._current_location  ),
                d_lang.hitch( this, this._gotNodeBoard ),
                d_lang.hitch( this, this._fetchFailed )
            );

        if ( ! this._machine_req ) {
            this._machine_req = d_when(
                    this._fetch_data_fn( null ),
                    d_lang.hitch( this, this._gotMachine )
                );
            this.onMachineHighlightingChanged();
        }
    },

    _gotNodeBoard: function( nb_data )
    {
        console.log( module.id + ": got node board data ", nb_data );

        this._doneLoading( false );

        var set_obj = {
                "_nodeBoardStatus": nb_data.status,
                "_nodeBoardSerialNumber": ("serialNumber" in nb_data ? nb_data.serialNumber : "N/A"),
                "_nodeBoardProductId": nb_data.productId
            };

        this._setDisplayValues( set_obj );


        this._nodeBoardComputeCardsTable.set( "data", { data: nb_data.computeCards, idProperty: "location" } );
        this._nodeBoardDcasTable.set( "data", { data: nb_data.DCAs, idProperty: "location" } );
    },

    _setIoDrawer: function()
    {
        var set_obj = {
                "_ioDrawerLocation": this._current_location,
                "_ioDrawerStatus": null,
                "_ioDrawerSerialNumber": null,
                "_ioDrawerProductId": null,
                "_ioDrawerClockSourceHideable": null
            };

        this._setDisplayValues( set_obj );


        // Fill in node with empty data while loading.
        var node_data = [];

        for ( var i = 0 ; i < 8 ; ++i ) {
            node_data.push(
                    {
                        location: (this._current_location + "-J" + (i < 10 ? "0" + i : "" + i)),
                        ipAddress : "",
                        status: null,
                        serialNumber: " ",
                        productId: " ",
                        memorySize: " ",
                        voltage: " "
                    }
                );
        }

        this._ioDrawerNodesTable.set( "data", { data: node_data, idProperty: "location" } );


        this._show( this._ioDrawerContainer );

        if ( this._cur_req )  this._cur_req.cancel();
        this._cur_req = d_when(
                this._fetch_data_fn( this._current_location  ),
                d_lang.hitch( this, this._gotIoDrawer ),
                d_lang.hitch( this, this._fetchFailed )
            );

        if ( ! this._machine_req ) {
            this._machine_req = d_when(
                    this._fetch_data_fn( null ),
                    d_lang.hitch( this, this._gotMachine )
                );
            this.onMachineHighlightingChanged();
        }
    },

    _gotIoDrawer: function( iod_data )
    {
        console.log( module.id + ": got I/O drawer data ", iod_data );

        this._doneLoading( false );

        var set_obj = {
                "_ioDrawerStatus": iod_data.status,
                "_ioDrawerSerialNumber": ("serialNumber" in iod_data ? iod_data.serialNumber : "N/A"),
                "_ioDrawerProductId": iod_data.productId
            };

        if ( "clockSource" in iod_data ) {
            this._clock_source = iod_data.clockSource;
            set_obj["_ioDrawerClockSource"] = iod_data.clockSource;
            this._ioDrawerClockSourceHideable.show();
        } else {
            this._ioDrawerClockSourceHideable.hide();
        }

        this._setDisplayValues( set_obj );


        this._ioDrawerNodesTable.set( "data", { data: iod_data.computeCards, idProperty: "location" } );
    },


    _setIoRack : function()
    {
        var set_obj = {
                "_ioRackLocation": this._current_location,
                "_ioRackStatus": null,
                "_ioRackSerialNumber": null,
                "_ioRackProductId": null
            };

        this._setDisplayValues( set_obj );


        this._ioRackDrawersTable.set( "data", null );


        this._show( this._ioRackContainer );

        if ( this._cur_req )  this._cur_req.cancel();
        this._cur_req = d_when(
                this._fetch_data_fn( this._current_location  ),
                d_lang.hitch( this, this._gotIoRack ),
                d_lang.hitch( this, this._fetchFailed )
            );

        if ( ! this._machine_req ) {
            this._machine_req = d_when(
                    this._fetch_data_fn( null ),
                    d_lang.hitch( this, this._gotMachine )
                );
            this.onMachineHighlightingChanged();
        }
    },


    _gotIoRack : function( io_rack_data )
    {
        this._doneLoading( false );

        var set_obj = {
                "_ioRackStatus": io_rack_data.status,
                "_ioRackSerialNumber": io_rack_data.serialNumber,
                "_ioRackProductId": io_rack_data.productId
            };

        this._setDisplayValues( set_obj );


        this._ioRackDrawersTable.set( "data", { data: io_rack_data.ioDrawers, idProperty: "location" } );
        this._ioRackClockCardsTable.set( "data", { data: io_rack_data.clockCards, idProperty: "location" } );
        this._ioRackPowerModulesTable.set( "data", { data: io_rack_data.powerModules, idProperty: "location" } );

    },


    _fetchFailed: function( error )
    {
        console.log( module.id + ": Failed to fetch " + this._current_location + " error=", error );
        this._setLocation( this._previous_location );
        this._previous_location = null;
        this._doneLoading( true );
    },


    _nowLoading : function()
    {
        this._jumpToInput.set( "disabled", true );
        this._jumpToButton.set( "disabled", true );
        this._jumpToLoadingHideable.show();
        this._jumpToFailedHideable.hide();
    },

    _doneLoading : function( failed_ind )
    {
        this._cur_req = null;
        this._dirty = false;

        this._jumpToInput.set( "disabled", false );
        this._jumpToButton.set( "disabled", false );
        this._jumpToLoadingHideable.hide();
        if ( failed_ind )  this._jumpToFailedHideable.show();
    },

    _setDisplayValues: function( values )
    {
        for ( elem_name in values ) {
            var dij = this[elem_name];

            if ( "label" in dij ) {
                dij.set( "label", values[elem_name] );
            } else {
                dij.set( "value", values[elem_name] );
            }
        }
    },

    _show: function( show_hideable )
    {
        this._detailsStack.selectChild( show_hideable );
    },
    
    
    _ll_format : ll_format

} );

return b_navigator_dijit_Hardware;

} );
