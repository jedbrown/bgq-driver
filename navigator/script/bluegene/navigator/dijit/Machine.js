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
    "dojo/_base/lang",
    "dojo/_base/NodeList",
    "dojo/_base/window",
    "dijit/registry",
    "dojo/text!./templates/Machine.html",
    "module",

    // Used only in template.
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
        d_lang,
        d_NodeList,
        d_window,
        j_registry,
        template,
        module
    )
{


var b_navigator_dijit_Machine = d_declare(
        "bluegene.navigator.dijit.Machine",
        [ l_AbstractTemplatedContainer ],

/** @lends bluegene^navigator^dijit^Machine */
{

    templateString : template,


    _visible: true,

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
      /* {}
       * represents midplanes and node boards on the machine,
       * map of location_string -> element
       */

    _highlighted_elems: null,
       /* dojo/NodeList
        * elements that are currently highlighted.
        */

    _overlay_elems: null,
        /* dojo/NodeList
         * overlay elements.
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
    },

    startup : function()
    {
        this.inherited( arguments );

        console.log( module.id + ": [" + this.id + "] startup. region=", this.region );

        d_on( this._toggleVisibilityButton, "click", d_lang.hitch( this, this._toggleVisibility ) );
    },

    setLayout : function( machine_layout )
    {
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

        for ( mp_location in this._data ) {
            var mp_data = this._data[mp_location];

            if ( "nodeBoards" in mp_data ) {
                if ( "color" in mp_data ) {
                    this._highlight( mp_location, { color: mp_data.color, include_nodeboards: false } );
                }

                for ( nb_pos in mp_data.nodeBoards ) {
                    this._highlight( mp_location + "-" + nb_pos, { color: mp_data.nodeBoards[nb_pos] } );
                }
            } else if ( "color" in mp_data ) {
                this._highlight( mp_location, { color: mp_data.color, include_nodeboards: true } );
            }

            if ( "overlay" in mp_data ) {
                this._overlay( mp_location, mp_data.overlay );
            }
        }
    },


    _drawMachine: function()
    {
        // Special case when the machine is 1x1x1x1 (half-rack configuration)
        if ( this._machine_info.midplanes.length === 1 &&
             this._machine_info.midplanes[0].length === 1 &&
             this._machine_info.midplanes[0][0].length == 1 &&
             this._machine_info.midplanes[0][0][0].length == 1
           )
        {
            console.log( module.id + ": machine is half-rack configuration" );

            this._drawHalfRack();
            return;
        }

        // It's not a half-rack.
        for ( var row = 0 ; row < this._machine_info.rows ; ++row ) {
            var row_elem = d_construct.create( "tr", null, this._tBodyElem );

            for ( var col = 0 ; col < this._machine_info.columns ; ++col ) {
                var rack_loc = b_BlueGene.rackCoordinateToLocationString( { row: row, col: col } );

                var rack_td_elem = d_construct.create( "td", {}, row_elem );
                var rack_table_elem = d_construct.create( "table", { cellspacing: "0", cellpadding: "0" }, rack_td_elem );

                var rack_id_row_elem = d_construct.create( "tr", {}, rack_table_elem );
                var rack_id_td_elem = d_construct.create( "td", { style: "background-color: #dedede; text-align: center;" }, rack_id_row_elem );

                d_construct.place( d_window.doc.createTextNode( rack_loc ), rack_id_td_elem );

                for ( var mp = 1 ; mp > -1 ; --mp ) {
                    // Loop through columns and put a table for each midplane for the node cards in the midplane.

                    var mp_loc = b_BlueGene.midplaneCoordinateToLocationString( { row: row, col: col, mp: mp } );

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
                        }
                    }
                }

            }
        }
    },


    _drawHalfRack : function()
    {
        var row_elem = d_construct.create( "tr", null, this._tBodyElem );


        var mp_loc = "R00-M0";

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

    _toggleVisibility : function()
    {
        console.log( module.id + ": _toggleVisibility. visible=", this._visible );
        this._visible = ! this._visible;

        if ( ! this._visible ) {
            // Hide.
            d_class.add( this._tableElem, "dijitHidden" );
            this._toggleVisibilityButton.set( "iconClass", "bgToggleVisibilityIcon bgToggleVisibilityIconRight" );
        } else {
            // Show.
            d_class.remove( this._tableElem, "dijitHidden" );
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
