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
    "../../BlueGene",
    "../../BlockBuilder",
    "../../dijit/Hideable",
    "dojo/dom-prop",
    "dojo/number",
    "dojo/when",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/text!./templates/BlockBuilder.html",
    "module",

    // Used only in template.
    "../../dijit/OutputText",
    "dijit/form/Button",
    "dijit/form/Form",
    "dijit/form/ValidationTextBox",
    "dijit/layout/ContentPane"
],
function(
        l_AbstractTemplatedContainer,
        b_BlueGene,
        b_BlockBuilder,
        b_dijit_Hideable,
        d_prop,
        d_number,
        d_when,
        d_array,
        d_declare,
        d_lang,
        template,
        module
    )
{


var b_navigator_dijit_BlockBuilder = d_declare(
        [ l_AbstractTemplatedContainer ],

{

    templateString : template,

    _create_block_fn : null,

    _block_builder : null,

    _deferred : null,


    setMidplanes : function( midplanes )
    {
        this._block_builder = new b_BlockBuilder( midplanes );
    },


    setCreateBlockFn : function( create_block_fn )
    {
        this._create_block_fn = create_block_fn;
    },


    notifyLocationSelected : function( location )
    {
        this._block_builder.add( location );

        this._updateMidplanes();

        this._checkState();
    },


    onHighlightChanged : function()  {},
    onBlockCreated : function() {},


    getMachineHighlightData : function()
    {
        var bb_hw = this._block_builder.getHardware();

        var highlight_info = {};

        if ( "midplanes" in bb_hw ) {
            d_array.forEach(
                    bb_hw.midplanes,
                    function( mp_location ) {
                        highlight_info[mp_location] = { color: "lightblue" };
                    }
                );
        }
        if ( "passthrough" in bb_hw ) {
            d_array.forEach(
                    bb_hw.passthrough,
                    function( mp_location ) {
                        highlight_info[mp_location] = { color: "orange", nodeBoards: {} };
                    }
                );
        }
        if ( "midplane" in bb_hw && "nodeBoards" in bb_hw ) {
            highlight_info[bb_hw.midplane] = { nodeBoards: {} };
            d_array.forEach( bb_hw.nodeBoards,
                    function( nb_pos ) {
                        highlight_info[bb_hw.midplane].nodeBoards[nb_pos] = "lightblue";
                    }
                );
        }

        var next_mp_dims = this._block_builder.getNextMidplanes();

        if ( next_mp_dims != null ) {
            for ( mp_loc in next_mp_dims ) {
                var dim_info = undefined;
                if ( next_mp_dims[mp_loc] == b_BlueGene.Dimension.A ) {
                    dim_info = { text: "A+", color: "blue" };
                } else if ( next_mp_dims[mp_loc] == b_BlueGene.Dimension.B ) {
                    dim_info = { text: "B+", color: "blue" };
                } else if ( next_mp_dims[mp_loc] == b_BlueGene.Dimension.C ) {
                    dim_info = { text: "C+", color: "blue" };
                } else if ( next_mp_dims[mp_loc] == b_BlueGene.Dimension.D ) {
                    dim_info = { text: "D+", color: "blue" };
                }

                highlight_info[mp_loc] = { overlay: dim_info };
            }
        }

        return highlight_info;
    },


    // override.
    startup : function()
    {
        this.inherited( arguments );

        this._idInput.on(
                "change",
                d_lang.hitch( this, this._notifyIdChange )
            );

        this._resetButton.on(
                "click",
                d_lang.hitch( this, this._reset )
            );

        this._form.on(
                "submit",
                d_lang.hitch( this, this._submit )
            );

        this._resultDismissButton.on(
                "click",
                d_lang.hitch( this, this._dismissResult )
            );

    },


    // override
    destroy : function()
    {
        if ( this._deferred ) {
            console.log( module.id + ": destroyed when operation in progress. Canceling the operation." );
            this._deferred.cancel();
            this._deferred = null;
        }

        this.inherited( arguments );
    },


    _notifyIdChange : function( new_value )
    {
        this._checkState();
    },


    _updateMidplanes : function()
    {
        this._sizeText.set( "value", this._block_builder.isValid() ? this._block_builder.getSize() : null );
        this._shapeText.set( "value", this._block_builder.isValid() ? this._block_builder.getShape() : null );

        var connectivity = [
                    b_BlueGene.Connectivity.Mesh,
                    b_BlueGene.Connectivity.Mesh,
                    b_BlueGene.Connectivity.Mesh,
                    b_BlueGene.Connectivity.Mesh,
                    b_BlueGene.Connectivity.Mesh
                ];

        if ( this._block_builder.isValid() ) {
            connectivity = this._block_builder.getConnectivity();
        }

        d_prop.set( this._connectivityA, "checked", (connectivity[b_BlueGene.Dimension.A] == b_BlueGene.Connectivity.Torus) );
        d_prop.set( this._connectivityB, "checked", (connectivity[b_BlueGene.Dimension.B] == b_BlueGene.Connectivity.Torus) );
        d_prop.set( this._connectivityC, "checked", (connectivity[b_BlueGene.Dimension.C] == b_BlueGene.Connectivity.Torus) );
        d_prop.set( this._connectivityD, "checked", (connectivity[b_BlueGene.Dimension.D] == b_BlueGene.Connectivity.Torus) );
        d_prop.set( this._connectivityE, "checked", true );

        this.onHighlightChanged();
    },


    _checkState : function()
    {
        this._machineErrorIndicator.set( "visibility",
                this._block_builder.isValid() ? b_dijit_Hideable.Visibility.HIDDEN : b_dijit_Hideable.Visibility.VISIBLE
            );

        if ( this._canCreateBlock() ) {
            this._createButton.set( "disabled", false );
        } else {
            this._createButton.set( "disabled", true );
        }
    },


    _canCreateBlock : function()
    {
        if ( this._deferred )  return false; // already creating a block.

        var id_value = this._idInput.get( "value" );

        if ( ! (id_value.length > 0 && id_value.length <= 32 && id_value.match( /^[a-zA-Z0-9-_]*$/ ) != null) ) {
            return false; // the form isn't valid.
        }

        if ( ! this._block_builder.isValid() )  return false; // the selected hardware isn't valid.

        return true;
    },


    _reset : function()
    {
        this._block_builder.reset();

        this._updateMidplanes();

        this._checkState();
    },


    _submit : function( e )
    {
        e.preventDefault();

        if ( ! this._canCreateBlock() ) {
            return;
        }

        var input_form = this._form.get( "value" );

        console.log( module.id + ": create block input=", input_form );

        var create_block_info = this._block_builder.calcCreateBlockInfo();
        create_block_info.id = input_form.id;

        this._deferred = this._create_block_fn( create_block_info );

        this._resultContainer.hide();
        this._processingContainer.show();
        this._checkState();

        d_when(
                this._deferred,
                d_lang.hitch( this, this._blockCreated ),
                d_lang.hitch( this, this._createBlockFailed )
            );
    },


    _blockCreated: function( response )
    {
        this._deferred = null;

        console.log( module.id + ": block created" );

        this._processingContainer.hide();
        this._resultErrorContainer.hide();
        this._resultCreatedContainer.show();
        this._resultContainer.show();
        this._checkState();

        this.onBlockCreated();

        return response;
    },


    _createBlockFailed: function( error )
    {
        this._deferred = null;

        console.log( module.id + ": create block failed! ", error );

        this._processingContainer.hide();
        this._resultCreatedContainer.hide();
        this._resultErrorText.set( "value", error.message );
        this._resultErrorContainer.show();
        this._resultContainer.show();

        this._checkState();
    },


    _dismissResult: function()
    {
        this._resultContainer.hide();
    },
    
    _d_number : d_number,

    _formatShape : function( shape )
    {
        return ("" + shape[0] + " x " + shape[1] + " x " + shape[2] + " x " + shape[3] + " x " + shape[4]);
    }

} );


return b_navigator_dijit_BlockBuilder;

} );
