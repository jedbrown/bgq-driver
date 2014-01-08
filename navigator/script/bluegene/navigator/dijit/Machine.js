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
    "../topic",
    "../../BlueGene",
    "dojo/dom-class",
    "dojo/dom-construct",
    "dojo/dom-style",
    "dojo/on",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/event",
    "dojo/_base/json",
    "dojo/_base/lang",
    "dojo/_base/NodeList",
    "dojo/_base/window",
    "dojo/text!./templates/Machine.html",
    "module",

    "dojo/NodeList-dom", // extension for .toggleClass.

    // Used only in template.
    "../../dijit/ActionLink",
    "dijit/form/Button",
    "dijit/layout/ContentPane"
],
function(
        l_AbstractTemplatedContainer,
        l_topic,
        b_BlueGene,
        d_class,
        d_construct,
        d_style,
        d_on,
        d_array,
        d_declare,
        d_event,
        d_json,
        d_lang,
        d_NodeList,
        d_window,
        template,
        module
    )
{


var b_navigator_dijit_Machine = d_declare(
        [ l_AbstractTemplatedContainer ],

/** @lends bluegene^navigator^dijit^Machine */
{

    templateString : template,


    _visible: true,
    _display_io : false,

    _machine_info: null,
      /* Information about the machine,
       *  {
       *    rows: rowCount,
       *    cols: colCount,
       *    midplanes: [ [ [ [ "location" ], ... ], ... ], ... ]
       *  }
       *
       *  set once retrieved from Bgws.
       */

    _data : null,
      // Current highlighting data.

    _hardware: null,
      /* { location: element, ... }
       * represents midplanes, node boards, I/O racks, and I/O drawers on the machine,
       */

    _highlighted_elems: null,
       /* dojo/NodeList
        * elements that are currently highlighted.
        */

    _overlay_elems: null,
        /* dojo/NodeList
         * overlay elements.
         */

    _mp_table_nodelist : null,
        /* dojo/NodeList of the midplane tables.
         * So can hide all of them when showing I/O or display all when showing compute.
         */

    _tophat_table_nodelist : null,
        /* dojo/NodeList of top-hat tables.
         * So can hide all of them when showing compute or display all when showing I/O.
         */

    _loading : true,
    _loading_displayed : true,


    /** @constructs */
    constructor: function()
    {
        this.doLayout = false;

        this._hardware = {};
        this._highlighted_elems = new d_NodeList();
        this._overlay_elems = new d_NodeList();

        this._mp_table_nodelist = new d_NodeList();
        this._tophat_table_nodelist = new d_NodeList();
    },


    startup : function()
    {
        this.inherited( arguments );

        console.log( module.id + ": [" + this.id + "] startup. region=", this.region );

        d_on( this._toggleVisibilityButton, "click", d_lang.hitch( this, this._toggleVisibility ) );
        d_on( this._toggleIoButton, "click", d_lang.hitch( this, this._toggleIoCompute ) );
    },


    setLayout : function( machine_layout )
    {
        console.log( module.id + ": layout=", d_json.toJson( machine_layout ) );
        this._machine_info = machine_layout;
        this._drawMachine();

        this._layoutRoot();

        this._doHighlighting();

        this._checkLoading();
    },


    setHighlighting: function( /* Object */ data )
    {
        this._data = data;

        if ( ! this._machine_info ) {
            console.log( module.id + ": waiting for machine data to set highlighting. _data=", this._data );
            return;
        }

        this._doHighlighting();
    },


    _setLoadingAttr : function( loading )
    {
        this._loading = loading;

        this._checkLoading();
    },


    _checkLoading : function()
    {
        var loading = (this._visible && (this._loading || (! this._machine_info)));

        console.log( module.id + ": _checkLoading.",
                "loading=", loading, "displayed=", this._loading_displayed,
                "visible=", this._visible, "loading=", this._loading, "machine_data=", !!! this._machine_info
            );

        if ( loading && ! this._loading_displayed ) {
            d_class.remove( this._loadingContainer, "dijitHidden" );
            this._loading_displayed = true;
            return;
        }

        if ( (! loading) && this._loading_displayed ) {
            d_class.add( this._loadingContainer, "dijitHidden" );
            this._loading_displayed = false;
            return;
        }
    },


    _doHighlighting : function()
    {
        console.log( module.id + ": highlighting ", this._data );

        this._clear();

        if ( ! this._data ) {
            return;
        }

        for ( bg_location in this._data ) {

            var loc_data = this._data[bg_location];

            if ( bg_location[4] === 'M' ) {

                if ( "nodeBoards" in loc_data ) {
                    if ( "color" in loc_data ) {
                        this._highlight( bg_location, { color: loc_data.color, include_nodeboards: false } );
                    }

                    for ( nb_pos in loc_data.nodeBoards ) {
                        this._highlight( bg_location + "-" + nb_pos, { color: loc_data.nodeBoards[nb_pos] } );
                    }
                } else if ( "color" in loc_data ) {
                    this._highlight( bg_location, { color: loc_data.color, include_nodeboards: true } );
                }

                if ( "overlay" in loc_data ) {
                    this._overlay( bg_location, loc_data.overlay );
                }

            } else {

                this._highlight( bg_location, { color: loc_data.color, include_nodeboards: false } );

            }
        }
    },


    _drawMachine: function()
    {
        var calcIoData = d_lang.hitch( this, function()
            {
                var io_data = {};
                d_array.forEach( this._machine_info.ioDrawers, function( io_drawer_loc ) {
                    var calcRackLocation = function( io_drawer_loc ) { return io_drawer_loc.substr( 0, 3 ); };

                    var rack_loc = calcRackLocation( io_drawer_loc );

                    if ( ! (rack_loc in io_data) )  io_data[rack_loc] = [];
                    io_data[rack_loc].push( io_drawer_loc );
                } );

                var has_top_hats = false;
                var has_io_racks = false;

                // Sort the values, figure out if has top hats.
                for ( rack_loc in io_data ) {
                    var sortOrder = function( io_drawer_loc ) { return parseInt( io_drawer_loc.substr( 5, 1 ), 16 ); };
                    io_data[rack_loc].sort( function( a, b ) { return sortOrder(a) - sortOrder(b); } );

                    if ( rack_loc[0] === "R" )  has_top_hats = true;
                    if ( rack_loc[0] === "Q" )  has_io_racks = true;
                }

                io_data.hasTopHats = has_top_hats;
                io_data.hasIoRacks = has_io_racks;

                return io_data;
            } );

        var io_data = calcIoData();

        console.log( module.id + ": I/O rack info=", io_data );


        // Special case when the machine is 1x1x1x1 (half-rack configuration)
        if ( this._machine_info.midplanes.length === 1 &&
             this._machine_info.midplanes[0].length === 1 &&
             this._machine_info.midplanes[0][0].length == 1 &&
             this._machine_info.midplanes[0][0][0].length == 1
           )
        {
            console.log( module.id + ": machine is half-rack configuration" );

            this._drawHalfRack( io_data );

            // Get rid of the I/O toggle button since we won't need it.
            this._toggleIoButton.destroyRecursive();
            this._toggleIoButton = null;

            return;
        }


        this._drawMultiRacks( io_data );

        if ( ! io_data.hasTopHats ) {
            // Get rid of the I/O toggle button since we won't need it.
            this._toggleIoButton.destroyRecursive();
            this._toggleIoButton = null;
        }
    },


    _drawMultiRacks : function( io_data )
    {

        for ( var row = 0 ; row < this._machine_info.rows ; ++row ) {
            this._drawRow( row, io_data );
        }

    },


    _drawRow : function( row, io_data )
    {
        var row_elem = d_construct.create( "tr", null, this._tBodyElem );

        for ( var col = 0 ; col < this._machine_info.columns ; ++col ) {

            var rack_loc = b_BlueGene.rackCoordinateToLocationString( { row: row, col: col } );

            this._drawComputeRack( rack_loc, io_data, row_elem );

        }

        // If there's I/O racks in the column, draw them.
        var io_rack_col = this._machine_info.columns;

        while ( true ) {

            var calcIoRackLocation = function( row, io_rack_col )  { return ("Q" + row.toString(36).toUpperCase() + io_rack_col.toString(36).toUpperCase()); };
            var io_rack_loc = calcIoRackLocation( row, io_rack_col );
            console.log( module.id + ": looking for I/O rack", io_rack_loc, "=", io_data[io_rack_loc] );

            if ( ! (io_rack_loc in io_data) )  break;

            this._drawIoRack( io_rack_loc, io_data[io_rack_loc], row_elem );

            ++io_rack_col;
        }

    },


    _drawComputeRack : function( rack_loc, io_data, row_elem )
    {

        var rack_td_elem = d_construct.create( "td", {}, row_elem );
        var rack_table_elem = d_construct.create( "table", { cellspacing: "0", cellpadding: "0" }, rack_td_elem );

        // header with rack location

        var rack_id_row_elem = d_construct.create( "tr", {}, rack_table_elem );
        var rack_id_td_elem = d_construct.create( "td", { style: "background-color: #dedede; text-align: center;" }, rack_id_row_elem );

        d_construct.place( d_window.doc.createTextNode( rack_loc ), rack_id_td_elem );


        // I/O drawers in top-hat of rack

        var tophat_tr_elem = d_construct.create( "tr", { "class": "dijitHidden", style: "height: 58px;" }, rack_table_elem );
        var tophat_td_elem = d_construct.create( "td", { style: "background-color: white; padding: 2px; vertical-align: top;" }, tophat_tr_elem );

        this._drawTopHat( rack_loc, io_data[rack_loc], tophat_td_elem );

        this._tophat_table_nodelist.push( tophat_tr_elem );


        // Midplanes

        var mps_tr_elem = d_construct.create( "tr", {}, rack_table_elem );
        var mps_td_elem = d_construct.create( "td", {}, mps_tr_elem );
        var mps_table = d_construct.create( "table", { cellspacing: "0", cellpadding: "0" }, mps_td_elem );

        this._drawMidplane( rack_loc, 1, mps_table );
        this._drawMidplane( rack_loc, 0, mps_table );

        this._mp_table_nodelist.push( mps_tr_elem );
    },


    _drawTopHat : function( rack_loc, io_drawers, root_elem )
    {
        var tophat_table_elem = d_construct.create( "table", { style: "background-color: black;", cellspacing: "1" }, root_elem );

        var top_io_drawers_tr_elem = d_construct.create( "tr", {}, tophat_table_elem );

        var findLoc = function( rack_loc, io_drawer, io_drawers )
            {
                if ( ! io_drawers )  return false;

                var io_drawer_loc = (rack_loc + "-I" + io_drawer);
                for ( var i = 0 ; i < io_drawers.length ; ++i ) {
                    if ( io_drawers[i] === io_drawer_loc )  return true;
                }
                return false;
            };


        var e_present = findLoc( rack_loc, "E", io_drawers );
        var e_bg_col = (e_present ? "white" : "#ccc");
        var e_io_drawer_td_elem = d_construct.create( "td", { style: "background-color: " + e_bg_col + "; height: 10px; width: 11px;" }, top_io_drawers_tr_elem );
        if ( e_present ) {
            var io_drawer_loc = (rack_loc + "-IE");
            this._hardware[io_drawer_loc] = e_io_drawer_td_elem;
            this._setupPublish( e_io_drawer_td_elem, io_drawer_loc );
        }

        var f_present = findLoc( rack_loc, "F", io_drawers );
        var f_bg_col = (f_present ? "white" : "#ccc");
        var f_io_drawer_td_elem = d_construct.create( "td", { style: "background-color: " + f_bg_col + "; height: 10px; width: 11px;" }, top_io_drawers_tr_elem );
        if ( f_present ) {
            var io_drawer_loc = (rack_loc + "-IF");
            this._hardware[io_drawer_loc] = f_io_drawer_td_elem;
            this._setupPublish( f_io_drawer_td_elem, io_drawer_loc );
        }

        var bottom_io_drawers_tr_elem = d_construct.create( "tr", {}, tophat_table_elem );

        var c_present = findLoc( rack_loc, "C", io_drawers );
        var c_bg_col = (c_present ? "white" : "#ccc");
        var c_io_drawer_td_elem = d_construct.create( "td", { style: "background-color: " + c_bg_col + "; height: 10px; width: 11px;" }, bottom_io_drawers_tr_elem );
        if ( c_present ) {
            var io_drawer_loc = (rack_loc + "-IC");
            this._hardware[io_drawer_loc] = c_io_drawer_td_elem;
            this._setupPublish( c_io_drawer_td_elem, io_drawer_loc );
        }

        var d_present = findLoc( rack_loc, "D", io_drawers );
        var d_bg_col = (d_present ? "white" : "#ccc");
        var d_io_drawer_td_elem = d_construct.create( "td", { style: "background-color: " + d_bg_col + "; height: 10px; width: 11px;" }, bottom_io_drawers_tr_elem );
        if ( d_present ) {
            var io_drawer_loc = (rack_loc + "-ID");
            this._hardware[io_drawer_loc] = d_io_drawer_td_elem;
            this._setupPublish( d_io_drawer_td_elem, io_drawer_loc );
        }
    },


    _drawMidplane : function( rack_loc, mp, rack_table_elem )
    {
        var mp_loc = (rack_loc + "-M" + mp);

        var mp_row_elem = d_construct.create( "tr", null, rack_table_elem );

        var mp_elem = d_construct.create( "td", { style: "background-color: white; padding: 2px;" }, mp_row_elem );

        this._hardware[mp_loc] = mp_elem;

        this._setupPublish( mp_elem, mp_loc );

        // Create nodecards table for the midplane.
        var ncs_table_elem = d_construct.create( "table", { style: "background-color: black;", cellspacing: "1" }, mp_elem );

        for ( var nc_row = 0 ; nc_row < 4 ; ++nc_row ) {
            var ncs_row_elem = d_construct.create( "tr", null, ncs_table_elem );

            for ( var nc_col = 0 ; nc_col < 4 ; ++nc_col ) {
                var nb_loc = mp_loc + "-" + b_BlueGene.nodeBoardPositions[nc_row * 4 + nc_col];

                var nb_elem = d_construct.create(
                        "td",
                        { style: "width: 5px; height: 5px; background-color: white;" },
                        ncs_row_elem
                    );

                this._hardware[nb_loc] = nb_elem;
                this._setupPublish( nb_elem, nb_loc );
            } // for each col of node card
        } // for each row of node card
    },


    _drawIoRack : function( location, drawers, row_elem )
    {
        var td_elem = d_construct.create( "td", {}, row_elem );

        var table_elem = d_construct.create( "table", { cellspacing: "0", cellpadding: "0" }, td_elem );

        var id_row_elem = d_construct.create( "tr", {}, table_elem );
        var id_td_elem = d_construct.create( "td", { style: "background-color: #444; color: white; text-align: center" }, id_row_elem );

        d_construct.place( d_window.doc.createTextNode( location ), id_td_elem );

        var drawers_row_elem = d_construct.create( "tr", { style: "height: 58px;"}, table_elem );
        var drawers_td_elem = d_construct.create( "td", { style: "background-color: white; padding: 2px;"}, drawers_row_elem );

        this._hardware[location] = drawers_td_elem;
        this._setupPublish( drawers_td_elem, location );

        var drawers_table = d_construct.create( "table", { style: "background-color: black;", cellspacing: "1" }, drawers_td_elem );

        var drawer_height = (Math.floor(53/(drawers.length)) - 1);
        var drawer_height_text = "" + drawer_height + "px";

        for ( var i = 0 ; i < drawers.length ; ++i ) {
            var drawer_location = drawers[drawers.length - 1 - i];

            var drawer_row_elem = d_construct.create( "tr", {}, drawers_table );
            var drawer_elem = d_construct.create( "td", { style: "background-color: white; text-align: center; height: " + drawer_height_text + "; width: 20px;" }, drawer_row_elem );

            this._hardware[drawer_location] = drawer_elem;
            this._setupPublish( drawer_elem, drawer_location );
        }

    },


    _drawHalfRack : function( io_data )
    {
        var rack_loc = "R00";
        var mp_loc = (rack_loc + "-M0");


        if ( io_data.hasTopHats ) {
            var tophat_row_elem = d_construct.create( "tr", {}, this._tBodyElem );
            var tophat_td_elem = d_construct.create( "td", { style: "background-color: white; padding: 2px;", align: "center" }, tophat_row_elem );

            this._drawTopHat( rack_loc, io_data[rack_loc], tophat_td_elem );
        }


        var row_elem = d_construct.create( "tr", null, this._tBodyElem );

        var mp_elem = d_construct.create( "td", { style: "background-color: white; padding: 2px;", align: "center" }, row_elem );

        this._hardware[mp_loc] = mp_elem;

        this._setupPublish( mp_elem, mp_loc );

        // Create nodecards table for the midplane.
        var nbs_tabl_elem = d_construct.create( "table", { style: "background-color: black;", cellspacing: "1" }, mp_elem );

        for ( var nb_no = 0 ; nb_no < 16 ; ++nb_no ) {
            var nb_loc = mp_loc + "-" + b_BlueGene.nodeBoardPositions[15 - nb_no];

            var nb_row_elem = d_construct.create( "tr", null, nbs_tabl_elem );
            var nb_elem = d_construct.create(
                    "td",
                    {
                        style: "background-color: white; text-align: center; width: 70px; cursor: pointer;"
                    },
                    nb_row_elem
                );

            d_construct.place(
                    d_window.doc.createTextNode( nb_loc ),
                    nb_elem
                );

            this._hardware[nb_loc] = nb_elem;
            this._setupPublish( nb_elem, nb_loc );
        }
    },


    _setupPublish: function( element, location )
    {
        d_on( element, "click",
                function( evt ) {
                    d_event.stop( evt );
                    l_topic.publish( l_topic.midplane, location );
                }
            );
    },

    _highlight: function(
            /*String*/ location,
            /*Object*/ info
        )
    {
        // summary: highlights the element at the location in the color.
        //
        // location: either a midplane or node board location
        // info:
        //   {
        //     color: "color", // the color to set the elements.
        //     include_nodeboards: Boolean // if true, also highlight the node boards and location must be a midplane.
        //   }

        var elem = this._hardware[location];

        if ( ! elem ) {
            console.log( module.id + ": requested to highlight unknown location " + location );
            return;
        }

        d_style.set( elem, "backgroundColor", info.color );
        this._highlighted_elems.push( elem );

        if ( ("include_nodeboards" in info) && info.include_nodeboards ) {
            d_array.forEach( b_BlueGene.nodeBoardPositions, d_lang.hitch( this, function( nb_pos ) {
                var nb_loc = location + "-" + nb_pos;
                var nb_elem = this._hardware[nb_loc];
                d_style.set( nb_elem, "backgroundColor", info.color );
                this._highlighted_elems.push( nb_elem );
            } ) );
        }
    },

    _overlay: function(
            /*String*/ location,
            /*Object*/ info
        )
    {
        // summary: overlay
        // location: a midplane location
        // info:
        //   {
        //     text: "text", -- gets set as the text.
        //     color: "color", -- the color of the overlay
        //   }

        var elem = this._hardware[location];

        var span_elem = d_construct.create(
                "span",
                { style: "position: absolute; font-size: 16pt; font-weight: bold; color: " + info.color + ";" },
                elem,
                "first"
            );

        d_construct.place( d_window.doc.createTextNode( info.text ), span_elem );

        this._overlay_elems.push( span_elem );
    },

    _clear: function()
    {
        // summary: clears all highlighted elements and overlays.

        this._highlighted_elems.style( { backgroundColor: "white" } );

        this._highlighted_elems = new d_NodeList();

        this._overlay_elems.forEach(
                function( elem ) { d_construct.destroy( elem ); }
            );

        this._overlay_elems = new d_NodeList();
    },

    _toggleIoCompute : function()
    {
        console.log( module.id + ": toggle IO, io=", this._display_io );

        this._display_io = (! this._display_io);

        if ( this._display_io ) {

            this._mp_table_nodelist.toggleClass( "dijitHidden", true );
            this._tophat_table_nodelist.toggleClass( "dijitHidden", false );

            this._toggleIoButton.set( "label", "Compute" );

        } else {

            this._mp_table_nodelist.toggleClass( "dijitHidden", false );
            this._tophat_table_nodelist.toggleClass( "dijitHidden", true );

            this._toggleIoButton.set( "label", "I/O" );

        }
    },

    _toggleVisibility : function()
    {
        console.log( module.id + ": _toggleVisibility. visible=", this._visible );
        this._visible = ! this._visible;

        if ( ! this._visible ) {
            // Hide.

            d_class.add( this._tableElem, "dijitHidden" );

            if ( this._toggleIoButton ) {
                d_class.add( this._toggleIoContainer, "dijitHidden" );
            }

            this._toggleVisibilityButton.set( "iconClass", "bgToggleVisibilityIcon bgToggleVisibilityIconRight" );

        } else {
            // Show.

            d_class.remove( this._tableElem, "dijitHidden" );

            if ( this._toggleIoButton ) {
                d_class.remove( this._toggleIoContainer, "dijitHidden" );
            }

            this._toggleVisibilityButton.set( "iconClass", "bgToggleVisibilityIcon bgToggleVisibilityIconLeft" );

        }

        this._checkLoading();

        this._layoutRoot();
    },


    _layoutRoot : function()
    {
        // Force re-layout of root layout dijit.
        var dij = this;
        while ( dij.getParent() )  dij = dij.getParent();
        dij.layout();
    }

} );

return b_navigator_dijit_Machine;

} );
