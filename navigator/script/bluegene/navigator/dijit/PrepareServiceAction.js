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
    "dojo/when",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/data/ObjectStore",
    "dojo/store/Memory",
    "dojo/text!./templates/PrepareServiceAction.html",
    "module",

    // Used only in template.
    "../format",
    "../../dijit/ActionLink",
    "../../dijit/Hideable",
    "../../dijit/OutputText",
    "dojo/number",
    "dijit/form/Button",
    "dijit/form/Form",
    "dijit/form/Select",
    "dijit/layout/BorderContainer",
    "dijit/layout/ContentPane",
    "dijit/layout/StackContainer",
    "dojox/grid/EnhancedGrid"
],
function(
        l_AbstractTemplatedContainer,
        l_MonitorActiveMixin,
        d_when,
        d_array,
        d_declare,
        d_lang,
        d_data_ObjectStore,
        d_store_Memory,
        template,
        module
    )
{


/* Maps service action type to information about that type of service action. */
var _TYPE = {
        "allNodeBoards": {
                prompt: "Select the midplane to service node boards",
                inputs: [ "midplane" ],
                calcLocation: function( form_obj )  { return (form_obj.midplane + "-N"); }
            },
        "computeClockCard": {
                prompt: "Select the compute rack with the clock card to service",
                inputs: [ "computeRack" ],
                calcLocation: function( form_obj )  { return (form_obj.computeRack + "-K"); }
            },
        "computeBulkPowerModule": {
                prompt: "Select the bulk power module to service",
                inputs: [ "computeRack", "computeBulkPowerModule" ],
                calcLocation: function( form_obj )  { return (form_obj.computeRack + "-" + form_obj.computeBulkPowerAssembly + "-" + form_obj.computeBulkPowerModule ); }
            },
        "computeIoDrawer": {
                prompt: "Select the I/O drawer in compute rack to service",
                inputs: [ "computeIoDrawerRack", "ioDrawer" ],
                calcLocation: function( form_obj )  { return (form_obj.computeIoDrawerRack + "-" + form_obj.ioDrawer); }
            },
        "computeRack": {
                prompt: "Select the compute rack to service",
                inputs: [ "computeRack" ],
                calcLocation: function( form_obj )  { return form_obj.computeRack; }
            },
        "ioBulkPowerModule": {
                prompt: "Select the bulk power module in an I/O rack to service",
                inputs: [ "ioRack", "ioBulkPowerModule" ],
                calcLocation: function( form_obj )  { return (form_obj.ioRack + "-B-" + form_obj.ioBulkPowerModule); }
            },
        "ioClockCard": {
                prompt: "Select the clock card in I/O rack to service",
                inputs: [ "ioRack" ],
                calcLocation: function( form_obj )  { return (form_obj.ioRack + "-K"); }
            },
        "ioIoDrawer": {
                prompt: "Select the I/O drawer in an I/O rack to service",
                inputs: [ "ioIoDrawerRack", "ioDrawer" ],
                calcLocation: function( form_obj )  { return (form_obj.ioIoDrawerRack + "-" + form_obj.ioDrawer); }
            },
        "ioRack": {
                prompt: "Select the I/O rack to service",
                inputs: [ "ioRack" ],
                calcLocation: function( form_obj )  { return form_obj.ioRack; }
            },
        "midplane": {
                prompt: "Select the midplane to service",
                inputs: [ "midplane" ],
                calcLocation: function( form_obj )  { return form_obj.midplane; }
            },
        "nodeBoard": {
                prompt: "Select the node board to service",
                inputs: [ "midplane", "nodeBoard" ],
                calcLocation: function( form_obj )  { return (form_obj.midplane + "-" + form_obj.nodeBoard); }
            },
        "nodeDca": {
                prompt: "Select the DCA on a node board to service",
                inputs: [ "midplane", "nodeBoard", "nodeDca" ],
                calcLocation: function( form_obj )  { return (form_obj.midplane + "-" + form_obj.nodeBoard + "-" + form_obj.nodeDca); }
            }
    };


var b_navigator_dijit_PrepareServiceAction = d_declare(
        [ l_AbstractTemplatedContainer, l_MonitorActiveMixin ],

{

    templateString : template,

    _store : null,
    _submit_prepare_for_service_fn : null,

    _type : null,

    _grid : null,

    _io_drawers: null, // map of rack location to array of I/O drawer positions., "R00": [ "IC", "ID" ], "Q00" : [ "I0", "I1", ... ], ...

    _promise : null,


    constructor : function()
    {
        this._io_drawers = {};
    },


    setJobsStore : function( new_store )
    {
        this._store = new_store;
        this._locationChanged(); // causes a reload.
    },


    setSubmitPrepareForServiceFn : function( fn )
    {
        this._submit_prepare_for_service_fn = fn;
    },


    setMachineInfo : function( args )
    {
        var machine_info = args.machineInfo;


        console.log( module.id + ": machine_info=", machine_info );

        // Create stores for selecting midplanes and racks.

        var mp_locs = [];
        var rack_locs = [];
        var racks_inserted = {};

        d_array.forEach( machine_info.midplanes, function( mp_a ) {
            d_array.forEach( mp_a, function( mp_b ) {
                d_array.forEach( mp_b, function( mp_c ) {
                    d_array.forEach( mp_c, function( mp ) {
                        mp_locs.push( mp );

                        var rack_loc = mp.substr( 0, 3 );

                        if ( ! (rack_loc in racks_inserted) ) {
                            rack_locs.push( mp.substr( 0, 3 ) );
                            racks_inserted[rack_loc] = true;
                        }
                    } );
                } );
            } );
        } );

        mp_locs.sort();

        var midplane_items = [];

        d_array.forEach( mp_locs, function( mp_loc ) {
            midplane_items.push( { location: mp_loc } );
        } );

        var midplanes_store = new d_store_Memory( {
                idProperty: "location",
                data: midplane_items
            } );

        var midplanes_adapter = new d_data_ObjectStore({
                labelProperty: "location",
                objectStore: midplanes_store
            });

        this._midplaneSelect.setStore( midplanes_adapter, "R00-M0" );


        rack_locs.sort();

        var rack_items = [];

        d_array.forEach( rack_locs, function( rack_loc ) {
           rack_items.push( { location: rack_loc } );
        } );

        var racks_store = new d_store_Memory( {
                idProperty: "location",
                data: rack_items
            } );

        var racks_adapter = new d_data_ObjectStore({
                labelProperty: "location",
                objectStore: racks_store
            });

        this._computeRackSelect.setStore( racks_adapter, "R00" );


        if ( "ioDrawers" in machine_info ) {

            this._io_drawers = {};

            d_array.forEach( machine_info.ioDrawers, d_lang.hitch( this, function( io_drawer ) {
                var rack_loc = io_drawer.substr( 0, 3 );
                var io_drawer_pos = io_drawer.substr( 4, 2 );

                if ( ! (rack_loc in this._io_drawers) )  this._io_drawers[rack_loc] = [];
                this._io_drawers[rack_loc].push( io_drawer_pos );
            } ) );

            var compute_racks_with_io_drawers = [];
            var io_racks_with_io_drawers = [];

            for ( rack_loc in this._io_drawers ) {
                if ( rack_loc[0] == 'R' )  compute_racks_with_io_drawers.push( rack_loc );
                else  io_racks_with_io_drawers.push( rack_loc );
            }

            if ( compute_racks_with_io_drawers.length != 0 ) {
                compute_racks_with_io_drawers.sort();

                var compute_io_drawer_rack_items = [];

                d_array.forEach( compute_racks_with_io_drawers, function( rack_loc ) {
                    compute_io_drawer_rack_items.push( { pos: rack_loc } );
                } );

                var compute_io_drawer_rack_store = new d_store_Memory( {
                        idProperty: "pos",
                        data: compute_io_drawer_rack_items
                    } );

                var compute_io_drawer_rack_adapter = new d_data_ObjectStore({
                        labelProperty: "pos",
                        objectStore: compute_io_drawer_rack_store
                    });

                this._computeIoDrawerRackSelect.setStore( compute_io_drawer_rack_adapter, compute_racks_with_io_drawers[0] );

            } else {

                console.log( module.id + ": no I/O drawers in compute racks in this machine." );

                this._selectComputeIoDrawerContainer.hide();
            }


            if ( io_racks_with_io_drawers.length != 0 ) {
                io_racks_with_io_drawers.sort();

                var io_io_drawer_rack_items = [];

                d_array.forEach( io_racks_with_io_drawers, function( rack_loc ) {
                    io_io_drawer_rack_items.push( { pos: rack_loc } );
                } );

                console.log( module.id + ": io_io_drawer_rack_items=", io_io_drawer_rack_items );

                var io_io_drawer_rack_store = new d_store_Memory( {
                        idProperty: "pos",
                        data: io_io_drawer_rack_items
                    } );

                var io_io_drawer_rack_adapter = new d_data_ObjectStore({
                        labelProperty: "pos",
                        objectStore: io_io_drawer_rack_store
                    });

                this._ioIoDrawerRackSelect.setStore( io_io_drawer_rack_adapter, io_racks_with_io_drawers[0] );

                this._ioRackSelect.setStore( io_io_drawer_rack_adapter, io_racks_with_io_drawers[0] );

            } else {
                console.log( module.id + ": no I/O drawers in I/O racks in this machine." );

                this._selectIoBpmContainer.hide();
                this._selectIoClockCardContainer.hide();
                this._selectIoIoDrawerContainer.hide();
                this._selectIoRackContainer.hide();
            }

        } else {

            console.log( module.id + ": no I/O drawers in this machine??" );

            this._selectComputeIoDrawerContainer.hide();

        }

    },


    notifyRefresh : function()
    {
        if ( ! this._isActive() ) {
            return;
        }

        this._locationChanged();
    },


    notifyReset : function()
    {
        this._selectType( null );
    },


    // override
    startup : function()
    {
        this.inherited( arguments );


        var node_boards_store = new d_store_Memory({
                idProperty: "pos",
                data: [ { pos: "N00" }, { pos: "N01" }, { pos: "N02" }, { pos: "N03" },
                        { pos: "N04" }, { pos: "N05" }, { pos: "N06" }, { pos: "N07" },
                        { pos: "N08" }, { pos: "N09" }, { pos: "N10" }, { pos: "N11" },
                        { pos: "N12" }, { pos: "N13" }, { pos: "N14" }, { pos: "N15" }
                   ]
            });

        var node_boards_adapter = new d_data_ObjectStore({
                labelProperty: "pos",
                objectStore: node_boards_store
            });

        this._nodeBoardSelect.setStore( node_boards_adapter, "N00" );


        var node_dca_store = new d_store_Memory({
                idProperty: "pos",
                data: [ { pos: "D0" }, { pos: "D1" } ]
            });

        var node_dca_adapter = new d_data_ObjectStore({
                labelProperty: "pos",
                objectStore: node_dca_store
            });

        this._nodeDcaSelect.setStore( node_dca_adapter, "D0" );


        var compute_bulk_power_assembly_store = new d_store_Memory({
                idProperty: "pos",
                data: [ { pos: "B0" }, { pos: "B1" }, { pos: "B2" }, { pos: "B3" } ]
            });

        var compute_bulk_power_assembly_adapter = new d_data_ObjectStore({
                labelProperty: "pos",
                objectStore: compute_bulk_power_assembly_store
            });

        this._computeBpaSelect.setStore( compute_bulk_power_assembly_adapter, "B0" );


        var compute_bulk_power_module_store = new d_store_Memory( {
                idProperty: "pos",
                data:[ { pos: "P0" }, { pos: "P1" }, { pos: "P2" },
                       { pos: "P3" }, { pos: "P4" }, { pos: "P5" },
                       { pos: "P6" }, { pos: "P7" }, { pos: "P8" }
                    ]
            } );

        var compute_bulk_power_module_adapter = new d_data_ObjectStore({
                labelProperty: "pos",
                objectStore: compute_bulk_power_module_store
            });

        this._computeBpmSelect.setStore( compute_bulk_power_module_adapter, "P0" );


        var io_bulk_power_module_store = new d_store_Memory( {
                idProperty: "pos",
                data: [ { pos: "P0" }, { pos: "P1" }, { pos: "P2" }, { pos: "P3" }, { pos: "P4" }, { pos: "P5" } ]
            } );

        var io_bulk_power_module_adapter = new d_data_ObjectStore({
                labelProperty: "pos",
                objectStore: io_bulk_power_module_store
            });

        this._ioBpmSelect.setStore( io_bulk_power_module_adapter, "P0" );


        this._selectAllNodeBoards.on( "click", d_lang.hitch( this, this._selectType, "allNodeBoards" ) );
        this._selectComputeBpm.on( "click", d_lang.hitch( this, this._selectType, "computeBulkPowerModule" ) );
        this._selectComputeClockCard.on( "click", d_lang.hitch( this, this._selectType, "computeClockCard" ) );
        this._selectComputeIoDrawer.on( "click", d_lang.hitch( this, this._selectType, "computeIoDrawer" ) );
        this._selectComputeRack.on( "click", d_lang.hitch( this, this._selectType, "computeRack" ) );
        this._selectIoBpm.on( "click", d_lang.hitch( this, this._selectType, "ioBulkPowerModule" ) );
        this._selectIoClockCard.on( "click", d_lang.hitch( this, this._selectType, "ioClockCard" ) );
        this._selectIoIoDrawer.on( "click", d_lang.hitch( this, this._selectType, "ioIoDrawer" ) );
        this._selectIoRack.on( "click", d_lang.hitch( this, this._selectType, "ioRack" ) );
        this._selectMidplane.on( "click", d_lang.hitch( this, this._selectType, "midplane" ) );
        this._selectNodeBoard.on( "click", d_lang.hitch( this, this._selectType, "nodeBoard" ) );
        this._selectNodeDca.on( "click", d_lang.hitch( this, this._selectType, "nodeDca" ) );

        this._computeBpaSelect.on( "change", d_lang.hitch( this, this._locationChanged ) );
        this._computeBpmSelect.on( "change", d_lang.hitch( this, this._locationChanged ) );
        this._computeRackSelect.on( "change", d_lang.hitch( this, this._locationChanged ) );
        this._ioBpmSelect.on( "change", d_lang.hitch( this, this._locationChanged ) );
        this._ioRackSelect.on( "change", d_lang.hitch( this, this._locationChanged ) );
        this._midplaneSelect.on( "change", d_lang.hitch( this, this._locationChanged ) );
        this._nodeBoardSelect.on( "change", d_lang.hitch( this, this._locationChanged ) );
        this._nodeDcaSelect.on( "change", d_lang.hitch( this, this._locationChanged ) );

        this._computeIoDrawerRackSelect.on( "change", d_lang.hitch( this, this._ioDrawerRackChanged ) );
        this._ioIoDrawerRackSelect.on( "change", d_lang.hitch( this, this._ioDrawerRackChanged ) );
        this._ioDrawerSelect.on( "change", d_lang.hitch( this, this._locationChanged ) );

        this._backButton.on( "click", d_lang.hitch( this, this._selectType, null ) );
        this._submitButton.on( "click", d_lang.hitch( this, this._submit ) );
    },


    // override
    destroy : function()
    {
        if ( this._promise ) {
            console.log( module.id + ": destroy when starting service action, will cancel." );
            this._promise.cancel();
            this._promise = null;
        }
        this.inherited( arguments );
    },


    // override MonitorActiveMixin
    _notifyActive : function()
    {
        this._locationChanged();
    },


    _selectType : function( type )
    {
        console.log( module.id + ": selected type", type );

        if ( (type != null) && (type in _TYPE) ) {
            this._type = type;
        } else {
            this._type = null;
        }

        this._type = type;

        if ( this._type == null ) {
            this._stackContainer.selectChild( this._typePane );
            return;
        }

        var type_info = _TYPE[type];

        this._setPromptInputs(
                type_info.prompt,
                type_info.inputs
            );

        this._stackContainer.selectChild( this._detailsPane );
        this._locationChanged(); // start loading.
    },


    _setPromptInputs : function( prompt, inputs )
    {
        // Hide the ones that aren't displayed.
        var input_inds = {};
        d_array.forEach( inputs, function( input ) {
            input_inds[input] = true;
        });

        if ( ! ("computeBulkPowerModule" in input_inds) )  this._selectComputeBpmHideable.hide();
        if ( ! ("computeIoDrawerRack" in input_inds) )  this._selectComputeIoDrawerRackHideable.hide();
        if ( ! ("computeRack" in input_inds) )  this._selectComputeRackHideable.hide();
        if ( ! ("ioBulkPowerModule" in input_inds) )  this._selectIoBpmHideable.hide();
        if ( ! ("ioDrawer" in input_inds) )  this._selectIoDrawerHideable.hide();
        if ( ! ("ioIoDrawerRack" in input_inds) )  this._ioIoDrawerRackHideable.hide();
        if ( ! ("ioRack" in input_inds) )  this._selectIoRackHideable.hide();
        if ( ! ("midplane" in input_inds) )  this._selectMidplaneHideable.hide();
        if ( ! ("nodeBoard" in input_inds) )  this._selectNodeBoardHideable.hide();
        if ( ! ("nodeDca" in input_inds) )  this._selectNodeDcaHideable.hide();

        this._promptText.set( "value", prompt );

        // Show the ones that are displayed.
        if ( "computeBulkPowerModule" in input_inds )  this._selectComputeBpmHideable.show();
        if ( "computeIoDrawerRack" in input_inds )  this._selectComputeIoDrawerRackHideable.show();
        if ( "computeRack" in input_inds )  this._selectComputeRackHideable.show();
        if ( "ioBulkPowerModule" in input_inds )  this._selectIoBpmHideable.show();
        if ( "ioDrawer" in input_inds )  this._selectIoDrawerHideable.show();
        if ( "ioIoDrawerRack" in input_inds )  this._ioIoDrawerRackHideable.show();
        if ( "ioRack" in input_inds )  this._selectIoRackHideable.show();
        if ( "midplane" in input_inds )  this._selectMidplaneHideable.show();
        if ( "nodeBoard" in input_inds )  this._selectNodeBoardHideable.show();
        if ( "nodeDca" in input_inds )  this._selectNodeDcaHideable.show();
    },


    _locationChanged : function()
    {
        if ( this._type == null )  return;
        if ( this._store === null )  return;

        var location = this._calcLocation();

        var query = {
                serviceLocation: location
            };

        console.log( module.id + ": type=", this._type, "query=", query );

        this._jobsGrid.setStore( this._store, query, null );
    },


    _calcLocation : function()
    {
        var form_obj = this._locationForm.get( "value" );

        console.log( module.id + ": _calcLocation, form_obj=", form_obj );

        return _TYPE[this._type].calcLocation( form_obj );
    },


    _ioDrawerRackChanged : function( rack_loc )
    {
        console.log( module.id + ": _ioDrawerRackChanged", rack_loc );

        // Set the options for prepare-service-action-io-drawer-select to the I/O drawers at the new location.

        var items = [];

        d_array.forEach( this._io_drawers[rack_loc], function( io_drawer ) {
            items.push( { pos: io_drawer } );
        } );

        var store = new d_store_Memory( {
                idProperty: "pos",
                data: items
            } );

        var adapter = new d_data_ObjectStore({
                labelProperty: "pos",
                objectStore: store
            });

        this._ioDrawerSelect.setStore( adapter, this._io_drawers[rack_loc][0] );
    },


    _submit : function()
    {
        var location = this._calcLocation();

        var res = confirm( "Press OK to start service action on " + location + "." );

        if ( ! res ) {
            return;
        }

        console.log( module.id + ": submit type=", this._type, "location=", location );

        this._promise = this._submit_prepare_for_service_fn( location );

        d_when(
                this._promise,
                d_lang.hitch( this, this._submitComplete ),
                d_lang.hitch( this, this._submitFailed )
            );

        this._startedHideable.hide();
        this._failedHideable.hide();

        this._submitButton.set( "disabled", true );
        this._startingHideable.show();

        this._detailsPane.layout();
    },


    _submitComplete : function( result )
    {
        console.log( module.id + ": submit complete", result );

        this._promise = null;

        this._startingHideable.hide();

        this._failedHideable.hide();

        this._startedIdText.set( "value", "" + result.id );
        this._startedHideable.show();

        this._submitButton.set( "disabled", false );

        this._detailsPane.layout();
    },


    _submitFailed : function( error )
    {
        this._promise = null;

        this._startingHideable.hide();

        var err_text = error.message;

        console.log( module.id + ": failed to started service action with error text=", err_text );

        this._startedHideable.hide();

        this._failedText.set( "value", err_text );
        this._failedHideable.show();

        this._submitButton.set( "disabled", false );

        this._detailsPane.layout();
    }

} );

return b_navigator_dijit_PrepareServiceAction;

} );
