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
    "./BlueGene",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        l_BlueGene,
        d_array,
        d_declare,
        d_lang,
        module
    )
{


var _countIncludes = function( includes_arr )
    {
        var sum = 0;
        d_array.forEach( includes_arr, function( e ) { if ( e ) ++sum; } );
        return sum;
    };


var _includesToString = function( includes_arr )
    {
        var res = "";
        d_array.forEach( includes_arr, function( e ) { res += (e ? "1" : "0"); } );
        return res;
    };


var b_BlockBuilder = d_declare( null,

/** @lends bluegene^BlockBuilder# */
{
    _machine: null,
        // Static info about the machine.
        // {
        //   midplanes: [[[mp_loc,...],...],...], -- 4D array with midplane location at [a][b][c][d].
        //   mp_loc_to_coords: { mp_loc: [a_coord,b_coord,c_coord,d_coord],... } -- Maps midplane location to coordinates.
        //   shape: [a_size,b_size,c_size,d_size] -- The size of the machine in each dimension
        // }

    _hw: null,
        // The hardware that's currently selected.
        // {
        //    midplane: mp_loc,
        //    includes: [ a_incs, b_incs, c_incs, d_incs ], -- large block, incs is [ bool, ... ] indicating whether midplane included or not.
        //    -- or
        //    nodeBoards: [nb_pos,...] -- small block
        // }

    _next_mp_dim: null,
        // The next midplane in each dimension plus the dimension.
        // { 'mp_loc': dimension, ... }


    /**
     *  Keeps track of data for constructing a block.
     *
     *  @class Block builder.
     *  @constructs
     */
    constructor: function( midplanes )
    {
        // console.log( module.id + ": midplanes= ", midplanes );

        // Store info about the machine, where each midplane is, the size and the shape.
        this._machine = {
                midplanes: midplanes,
                shape: [
                     midplanes.length,
                     midplanes[0].length,
                     midplanes[0][0].length,
                     midplanes[0][0][0].length
                  ],
                mp_loc_to_coords: {}
            };

        console.log( module.id + ": machine shape is " + this._machine.shape );

        // build a map from midplane location to coordinates.
        for ( var a_i = 0 ; a_i < midplanes.length ; ++a_i ) {
            for ( var b_i = 0 ; b_i < midplanes[a_i].length ; ++b_i ) {
                for ( var c_i = 0 ; c_i < midplanes[a_i][b_i].length ; ++c_i ) {
                    for ( var d_i = 0 ; d_i < midplanes[a_i][b_i][c_i].length ; ++d_i ) {
                        this._machine.mp_loc_to_coords[midplanes[a_i][b_i][c_i][d_i]] = [ a_i, b_i, c_i, d_i ];
                    }
                }
            }
        }

        this.reset();
    },


    add: function( location )
    {
        console.log( module.id + ": adding " + location );

        this._updateHardware( location );

        if ( "includes" in this._hw ) {
            this._next_mp_dim = this._calcNextMidplanes();
        } else {
            this._next_mp_dim = {};
        }

        if ( "includes" in this._hw ) {
            console.log( module.id + ": hw.includes is now ", this._hw.includes );
        }
    },

    reset: function()
    {
        this._hw = {};
        this._next_mp_dim = {};
    },

    getHardware: function()
    {
        if ( "nodeBoards" in this._hw ) { // it's a small block.
            return { midplane: this._hw.midplane, nodeBoards: this._hw.nodeBoards };
        }

        if ( "includes" in this._hw ) { // it's a large block.
            return this._calcMidplanes( this._hw.midplane, this._hw.includes );
        }

        return {};
    },

    getNextMidplanes: function()
    {
        return this._next_mp_dim;
    },

    /** returns the size in nodes in each dimension, or null if the size cannot be calculated. */
    getShape: function()
    {
        if ( "includes" in this._hw ) {
            return [
                    _countIncludes( this._hw.includes[l_BlueGene.Dimension.A] ) * l_BlueGene.nodesInMidplane[l_BlueGene.Dimension.A],
                    _countIncludes( this._hw.includes[l_BlueGene.Dimension.B] ) * l_BlueGene.nodesInMidplane[l_BlueGene.Dimension.B],
                    _countIncludes( this._hw.includes[l_BlueGene.Dimension.C] ) * l_BlueGene.nodesInMidplane[l_BlueGene.Dimension.C],
                    _countIncludes( this._hw.includes[l_BlueGene.Dimension.D] ) * l_BlueGene.nodesInMidplane[l_BlueGene.Dimension.D],
                    l_BlueGene.nodesInMidplane[l_BlueGene.Dimension.E]
                ];
        }
        if ( "nodeBoards" in this._hw ) {
            var sbi = l_BlueGene.smallBlockInfo[this._hw.nodeBoards.length];

            if ( ! sbi ) {
                return null;
            }

            return sbi.shape;
        }
        return null;
    },

    getSize: function() {
        // summary: returns the number of nodes in the block.

        if ( "includes" in this._hw ) {
            return (_countIncludes( this._hw.includes[l_BlueGene.Dimension.A] ) * l_BlueGene.nodesInMidplane[l_BlueGene.Dimension.A] *
                    _countIncludes( this._hw.includes[l_BlueGene.Dimension.B] ) * l_BlueGene.nodesInMidplane[l_BlueGene.Dimension.B] *
                    _countIncludes( this._hw.includes[l_BlueGene.Dimension.C] ) * l_BlueGene.nodesInMidplane[l_BlueGene.Dimension.C] *
                    _countIncludes( this._hw.includes[l_BlueGene.Dimension.D] ) * l_BlueGene.nodesInMidplane[l_BlueGene.Dimension.D] *
                    l_BlueGene.nodesInMidplane[l_BlueGene.Dimension.E]);
        }
        if ( "nodeBoards" in this._hw ) {
            return this._hw.nodeBoards.length * 32;
        }
        return 0;
    },

    getConnectivity: function() {
        if ( "includes" in this._hw ) {
            var ret = [
                   l_BlueGene.Connectivity.Mesh,
                   l_BlueGene.Connectivity.Mesh,
                   l_BlueGene.Connectivity.Mesh,
                   l_BlueGene.Connectivity.Mesh,
                   l_BlueGene.Connectivity.Torus
               ];

            d_array.forEach( l_BlueGene.MidplaneDimensions, d_lang.hitch( this, function( dim ) {
                if ( (this._hw.includes[dim].length == 1) || this._hw.includes[dim].length == this._machine.shape[dim] ) {
                    ret[dim] = l_BlueGene.Connectivity.Torus;
                }
            } ) );

            return ret;
        }

        if ( "nodeBoards" in this._hw ) {
            var sbi = l_BlueGene.smallBlockInfo[this._hw.nodeBoards.length];

            if ( ! sbi ) {
                return null;
            }

            return sbi.connectivity;
        }
        return null;
    },

    isValid: function() {
        if ( "includes" in this._hw ) {
            return true;
        }

        if ( "nodeBoards" in this._hw ) {
            var sbi = l_BlueGene.smallBlockInfo[this._hw.nodeBoards.length];

            if ( ! sbi ) {
                return false;
            }

            for ( var i = 0 ; i < sbi.nodeBoards.length ; ++i ) {
                var any_diff = false;

                for ( var i2 = 0 ; i2 < this._hw.nodeBoards.length && ! any_diff ; ++i2 ) {
                    any_diff = (sbi.nodeBoards[i][i2] != this._hw.nodeBoards[i2]);
                }

                if ( ! any_diff ) {
                    return true;
                }
            }
        }

        return false;
    },

    calcCreateBlockInfo: function()
    {
        var create_block_info = {
                description: "Generated via Block Builder"
            };

        if ( "includes" in this._hw ) { // it's a large block.

            create_block_info.midplane = this._hw.midplane;

            create_block_info.midplanes = [
                    _includesToString( this._hw.includes[l_BlueGene.Dimension.A] ),
                    _includesToString( this._hw.includes[l_BlueGene.Dimension.B] ),
                    _includesToString( this._hw.includes[l_BlueGene.Dimension.C] ),
                    _includesToString( this._hw.includes[l_BlueGene.Dimension.D] )
                ];

            var connectivity = this.getConnectivity();

            create_block_info.torus = "";
            if ( connectivity[l_BlueGene.Dimension.A] == l_BlueGene.Connectivity.Torus ) create_block_info.torus += "A";
            if ( connectivity[l_BlueGene.Dimension.B] == l_BlueGene.Connectivity.Torus ) create_block_info.torus += "B";
            if ( connectivity[l_BlueGene.Dimension.C] == l_BlueGene.Connectivity.Torus ) create_block_info.torus += "C";
            if ( connectivity[l_BlueGene.Dimension.D] == l_BlueGene.Connectivity.Torus ) create_block_info.torus += "D";
            create_block_info.torus += "E";

        } else {

            create_block_info.midplane = this._hw.midplane;
            create_block_info.nodeBoards = this._hw.nodeBoards;

        }

        return create_block_info;
    },


    _updateHardware: function( location )
    {
        var mp_loc = location.substr( 0, 6 );

        var mp_coords = this._machine.mp_loc_to_coords[mp_loc];

        var location_is_node_board = (location.length == 10);

        var nb_pos = "";
        if ( location_is_node_board ) {
            nb_pos = location.substr( 7 );
        }

        if ( "includes" in this._hw ) { // it's a large block.

            // check if single midplane and picked the origin midplane

            if ( this._hw.midplane == mp_loc &&
                 this._hw.includes[0].length == 1 &&
                 this._hw.includes[1].length == 1 &&
                 this._hw.includes[2].length == 1 &&
                 this._hw.includes[3].length == 1
               )
            {
                // if pick a node board then switch to a small block.

                if ( location_is_node_board ) {
                    // convert to small block...
                    this._hw = {
                            midplane: mp_loc,
                            nodeBoards: [ nb_pos ]
                        };
                    return;
                }

                // deselect everything.
                this._hw = {};
                return;
            }

            // if pick next midplane in a dimension, then expand that dimension.

            if ( mp_loc in this._next_mp_dim ) {

                // expand shape in the dimension.

                this._hw.includes[this._next_mp_dim[mp_loc]].push( true );

                return;
            }

            // didn't pick next midplane in a dimension.

            // Check if midplane is internal.

            var base_mp_coords = this._machine.mp_loc_to_coords[this._hw.midplane];

            var new_mp_block_coords = [
                             (mp_coords[0] - base_mp_coords[0] + this._machine.shape[0]) % this._machine.shape[0],
                             (mp_coords[1] - base_mp_coords[1] + this._machine.shape[1]) % this._machine.shape[1],
                             (mp_coords[2] - base_mp_coords[2] + this._machine.shape[2]) % this._machine.shape[2],
                             (mp_coords[3] - base_mp_coords[3] + this._machine.shape[3]) % this._machine.shape[3]
                         ];

            if ( new_mp_block_coords[0] < this._hw.includes[0].length &&
                 new_mp_block_coords[1] < this._hw.includes[1].length &&
                 new_mp_block_coords[2] < this._hw.includes[2].length &&
                 new_mp_block_coords[3] < this._hw.includes[3].length )
            {
                // The selected midplane is inside to the current block.

                // if passing through, then set to not passthrough in that dimension, try to set the "next" dimension to passthrough

                if ( ! this._hw.includes[0][new_mp_block_coords[0]] ) {
                    this._hw.includes[0][new_mp_block_coords[0]] = true;

                    if ( _countIncludes( this._hw.includes[1] ) > 2 ) {
                        this._hw.includes[1][new_mp_block_coords[1]] = false;
                        return;
                    }
                    if ( _countIncludes( this._hw.includes[2] ) > 2 ) {
                        this._hw.includes[2][new_mp_block_coords[2]] = false;
                        return;
                    }
                    if ( _countIncludes( this._hw.includes[3] ) > 2 ) {
                        this._hw.includes[3][new_mp_block_coords[3]] = false;
                        return;
                    }

                    return;
                }
                if ( ! this._hw.includes[1][new_mp_block_coords[1]] ) {
                    this._hw.includes[1][new_mp_block_coords[1]] = true;

                    if ( _countIncludes( this._hw.includes[2] ) > 2 ) {
                        this._hw.includes[2][new_mp_block_coords[2]] = false;
                        return;
                    }
                    if ( _countIncludes( this._hw.includes[3] ) > 2 ) {
                        this._hw.includes[3][new_mp_block_coords[3]] = false;
                        return;
                    }

                    return;
                }
                if ( ! this._hw.includes[2][new_mp_block_coords[2]] ) {
                    this._hw.includes[2][new_mp_block_coords[2]] = true;

                    if ( _countIncludes( this._hw.includes[3] ) > 2 ) {
                        this._hw.includes[3][new_mp_block_coords[3]] = false;
                        return;
                    }

                    return;
                }
                if ( ! this._hw.includes[3][new_mp_block_coords[3]] ) {
                    this._hw.includes[3][new_mp_block_coords[3]] = true;
                    return;
                }

                // not passing through, figure out if can pass through.

                // set to pass through only if > 2 included since can't have only 1 included.

                var b_count = _countIncludes( this._hw.includes[1] );
                console.log( module.id + ": countIncludes is " + b_count );

                if ( _countIncludes( this._hw.includes[0] ) > 2 ) {
                    this._hw.includes[0][new_mp_block_coords[0]] = false;
                    return;
                }
                if ( _countIncludes( this._hw.includes[1] ) > 2 ) {
                    this._hw.includes[1][new_mp_block_coords[1]] = false;
                    return;
                }
                if ( _countIncludes( this._hw.includes[2] ) > 2 ) {
                    this._hw.includes[2][new_mp_block_coords[2]] = false;
                    return;
                }
                if ( _countIncludes( this._hw.includes[3] ) > 2 ) {
                    this._hw.includes[3][new_mp_block_coords[3]] = false;
                    return;
                }

                return;
            }

            // didn't pick next midplane in dimension or internal midplane, use the selected midplane as the other corner.

            this._hw.includes = [ [], [], [], [] ];

            for ( var dim = 0 ; dim < 4 ; ++dim ) {
                for ( var i = 0 ; i <= new_mp_block_coords[dim] ; ++i ) { this._hw.includes[dim].push( true ); }
            }

            return;
        }

        // It's not a large block.

        if ( "nodeBoards" in this._hw ) { // it's a small block.
            if ( this._hw.midplane == mp_loc ) { // selected same midplane.

                if ( location_is_node_board ) {
                    if ( d_array.indexOf( this._hw.nodeBoards, nb_pos ) == -1 ) {
                        // New node board!
                        this._hw.nodeBoards.push( nb_pos );
                        this._hw.nodeBoards.sort();
                        return;
                    }

                    // already selected, remove it.

                    if ( this._hw.nodeBoards.length <= 1 ) { // last one?
                        // clear out the hardware.
                        this._hw = {};
                        return;
                    }

                    // remove the selected node board.
                    this._hw.nodeBoards = d_array.filter( this._hw.nodeBoards, function( elem ) { return (elem != nb_pos); } );
                    return;
                }

                // selected midplane, switch to a large block with just the midplane.
                this._hw = { midplanes: [ this._hw.midplane ] };
                return;
            }

            // selected different midplane, switch to a large block with both midplanes.
            this._hw = {
                    midplane: this._hw.midplane,
                    includes: [ [true], [true], [true], [true] ]
                };

            return;
        }

        // it's not a large block or small block.

        this._hw = {
                midplane: mp_loc,
                includes: [ [true], [true], [true], [true] ]
            };
    },

    _nextCoords: function( coords, dim )
    {
        var ret = d_lang.clone( coords );
        ret[dim] = (ret[dim] + 1) % this._machine.shape[dim];
        return ret;
    },

    _midplaneAt: function( coords )
    {
        return this._machine.midplanes
                [coords[l_BlueGene.Dimension.A]]
                [coords[l_BlueGene.Dimension.B]]
                [coords[l_BlueGene.Dimension.C]]
                [coords[l_BlueGene.Dimension.D]]
            ;
    },

    _calcMidplanes: function( corner_mp, includes )
    {
        var ret = { midplanes: [], passthrough: [] };

        var base_coord = this._machine.mp_loc_to_coords[corner_mp];

        for ( var a_i = 0 ; a_i < includes[0].length ; ++a_i ) {
            for ( var b_i = 0 ; b_i < includes[1].length ; ++b_i ) {
                for ( var c_i = 0 ; c_i < includes[2].length ; ++c_i ) {
                    for ( var d_i = 0 ; d_i < includes[3].length ; ++d_i ) {
                        var machine_coord = [
                                (base_coord[l_BlueGene.Dimension.A] + a_i) % this._machine.shape[l_BlueGene.Dimension.A],
                                (base_coord[l_BlueGene.Dimension.B] + b_i) % this._machine.shape[l_BlueGene.Dimension.B],
                                (base_coord[l_BlueGene.Dimension.C] + c_i) % this._machine.shape[l_BlueGene.Dimension.C],
                                (base_coord[l_BlueGene.Dimension.D] + d_i) % this._machine.shape[l_BlueGene.Dimension.D]
                            ];

                        var mp_loc = this._midplaneAt( machine_coord );

                        var includes_count =
                            (includes[l_BlueGene.Dimension.A][a_i] ? 1 : 0) +
                            (includes[l_BlueGene.Dimension.B][b_i] ? 1 : 0) +
                            (includes[l_BlueGene.Dimension.C][c_i] ? 1 : 0) +
                            (includes[l_BlueGene.Dimension.D][d_i] ? 1 : 0)
                            ;

                        if ( includes_count == 4 )
                        {
                            ret.midplanes.push( mp_loc );
                        } else if ( includes_count == 3 )
                        {
                            ret.passthrough.push( mp_loc );
                        }
                    }
                }
            }

        }

        return ret;
    },

    _calcNextMidplanes: function()
    {
        var ret = {};

        // If no midplanes then done.
        if ( this._hw.includes[0].length === 0 ) {
            return ret;
        }

        var cur_mp_pt = this._calcMidplanes( this._hw.midplane, this._hw.includes );

        var cur_mps = {};
        d_array.forEach( cur_mp_pt.midplanes, function( mp ) { cur_mps[mp] = true; } );
        d_array.forEach( cur_mp_pt.passthrough, function( mp ) { cur_mps[mp] = false; } );


        d_array.forEach( l_BlueGene.MidplaneDimensions, d_lang.hitch( this, function ( dimension ) {

            // if the dim already has all midplanes then there's no next midplanes in the dimension.
            if ( this._hw.includes[dimension].length == this._machine.shape[dimension] )  return;

            var temp_includes = d_lang.clone( this._hw.includes );

            temp_includes[dimension].push( true );

            var new_mp_pt = this._calcMidplanes( this._hw.midplane, temp_includes );

            var new_mps = {};
            d_array.forEach( new_mp_pt.midplanes, function( mp ) { new_mps[mp] = true; } );
            d_array.forEach( new_mp_pt.passthrough, function( mp ) { new_mps[mp] = false; } );

            // remove all the current mps from new_mps...

            for ( mp in cur_mps )  delete new_mps[mp];

            // The remaining mps are the next mps in the dimension.
            for ( mp in new_mps )  ret[mp] = dimension;

        } ) );

        return ret;
    }
} );


b_BlockBuilder.countIncludes = _countIncludes;

b_BlockBuilder.includesToString = _includesToString;


return b_BlockBuilder;

} );
