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
    "../FilterOptions",
    "../xlate",
    "../../Bgws",
    "../../BlueGene",
    "dojo/number",
    "dojo/when",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/text!./templates/Blocks.html",
    "module",

    // Used only in template.
    "./ComputeBlockDetails",
    "./MultiSelectCodes",
    "../format",
    "../../dijit/Hideable",
    "dijit/TooltipDialog",
    "dijit/form/Button",
    "dijit/form/DropDownButton",
    "dijit/form/Form",
    "dijit/layout/BorderContainer",
    "dijit/layout/ContentPane",
    "dojox/grid/EnhancedGrid"
],
function(
        l_AbstractTemplatedContainer,
        l_MonitorActiveMixin,
        ll_FilterOptions,
        ll_xlate,
        b_Bgws,
        b_BlueGene,
        d_number,
        d_when,
        d_declare,
        d_lang,
        template,
        module
    )
{


var _formToQuery = function( form_obj )
    {
        var query_obj = ll_FilterOptions.formToQueryHelper( form_obj, [ "status" ] );

        query_obj.type = "compute";

        return query_obj;
    };


var b_navigator_dijit_Blocks = d_declare(
        [ l_AbstractTemplatedContainer, l_MonitorActiveMixin ],

{

    templateString : template,

    _store : null,
    _delete_block_fn : null,

    _filter_options : null,

    _initialized : false,
    _dirty : true,

    _delete_deferred : null,
    _delete_block_id : null,


    // override
    startup : function()
    {
        this.inherited( arguments );


        this._filter_options = new ll_FilterOptions( {
                name : "Blocks",
                grid : this._grid,
                form : this._filterOptionsForm,
                dropdown : this._filterOptionsDropDownButton,
                resetButton: this._filterOptionsResetButton,
                formToQuery : _formToQuery,
                onChange: d_lang.hitch( this, this._checkUpdateDeleteButtonState )
            } );

        this._createButton.on( "click", d_lang.hitch( this, this._createClicked ) );
        this._deleteButton.on( "click", d_lang.hitch( this, this._deleteOrCancel ) );
        this._grid.on( "rowClick", d_lang.hitch( this, this._selectBlock ) );
    },


    // override
    destroy : function()
    {
        if ( this._delete_deferred ) {
            console.log( module.id + ": canceling delete operation because destroyed." );
            this._delete_deferred.cancel();
            this._delete_deferred = null;
        }

        this.inherited( arguments );
    },


    getDetailsDijit : function()
    {
        return this._details;
    },


    setStore : function( new_store )
    {
        this._store = new_store;
        this._initialized = false;
        this._refresh();
    },


    setUserCanCreateBlocks : function( new_value )
    {
        if ( new_value ) {
            this._createButtonContainer.show();
        } else {
            console.log( module.id + ": user doesn't have block create so hiding the block create button." );
            this._createButtonContainer.hide();
        }
    },


    setDeleteBlockFn : function( delete_block_fn )
    {
        this._delete_block_fn = delete_block_fn;
    },


    onBlockSelected : function( block_id )  {},
    onCreateBlock : function() {},


    notifyRefresh : function()
    {
        this._refresh();
    },


    // override MonitorActiveMixin
    _notifyActive : function()
    {
        this._loadIfReady();
    },


    _refresh : function()
    {
        this._dirty = true;

        this._grid.selection.clear();
        this._checkUpdateDeleteButtonState();

        this._loadIfReady();
    },


    _loadIfReady : function()
    {
        if ( ! this._store )  return;
        if ( ! this._dirty )  return;
        if ( ! this._isActive() /*MonitorActiveMixin*/ )  return;

        if ( this._initialized ) {
            this._grid.selection.clear();
            this._checkUpdateDeleteButtonState();
            this._grid.render();
        } else {
            this._grid.setStore( this._store, this._filter_options.calcQuery(), null );
            this._checkUpdateDeleteButtonState();
            this._initialized = true;
        }

        this._dirty = false;
    },


    _selectBlock : function()
    {
        var item = this._grid.selection.getFirstSelected();

        if ( item == null ) {
            console.log( module.id + ": selectBlock with no item" );
            return;
        }

        var block_name = this._store.getValue( item, "id", "" );

        this.onBlockSelected( block_name );

        this._checkUpdateDeleteButtonState();
    },


    _createClicked : function()
    {
        this.onCreateBlock();
    },


    _checkUpdateDeleteButtonState : function()
    {
        // If deleting a block, then must be active.
        // If not deleting a block, only active if block is selected and selected block is Free.

        if ( this._delete_deferred ) {
            // Already an operation in progress.
            this._deleteButton.set( "label", "Cancel delete " + this._delete_block_id );
            this._deleteButton.set( "disabled", false );
            return;
        }

        // Not deleting a block.

        this._deleteButton.set( "label", "Delete" );

        var item = this._grid.selection.getFirstSelected();

        if ( ! item ) {
            // Nothing selected, return.
            this._deleteButton.set( "disabled", true );
            return;
        }

        var block_status = this._store.getValue( item, "status", "Unknown" );

        if ( block_status !== b_BlueGene.blockStatus.FREE ) {
            this._deleteButton.set( "disabled", true );
            return;
        }

        // Selected block is Free.
        this._deleteButton.set( "disabled", false );
    },


    _deleteOrCancel : function()
    {
        if ( this._delete_deferred ) {
            console.log( module.id + ": canceling delete of " + this._delete_block_id );
            this._delete_deferred.cancel();
            this._delete_deferred = null;
            return;
        }


        var item = this._grid.selection.getFirstSelected();

        if ( item == null ) {
            console.log( module.id + ": delete with no item" );
            return;
        }

        var block_id = this._store.getValue( item, "id", "" );
        var block_status = this._store.getValue( item, "status", "Unknown" );

        if ( block_status != b_BlueGene.blockStatus.FREE )  {
            console.log( module.id + ": delete block is not FREE" );
            return;
        }

        var res = confirm( "Press OK to delete " + block_id + "." );

        if ( ! res ) {
            return;
        }

        this._delete_block_id = block_id;

        this._delete_deferred = this._delete_block_fn( block_id );

        this._checkUpdateDeleteButtonState();

        d_when(
                this._delete_deferred,
                d_lang.hitch( this, this._blockDeleted ),
                d_lang.hitch( this, this._deleteFailed )
            );
    },


    _blockDeleted: function()
    {
        console.log( module.id + ": deleted block " + this._delete_block_id );

        this._delete_deferred = null;
        this._delete_block_id = null;

        this._refresh();
    },


    _deleteFailed: function( err )
    {
        var io_args = this._delete_deferred.ioArgs;
        var block_id = this._delete_block_id;

        this._delete_deferred = null;
        this._delete_block_id = null;


        var err_text = b_Bgws.calculateErrorMessage( err.response, io_args );

        console.log( module.id + ": failed to delete " + block_id + ", " + err_text );

        this._checkUpdateDeleteButtonState();

        alert( "Failed to delete " + block_id + ", error is '" + err_text + "'" );
    },

    _d_number : d_number,
    _ll_xlate : ll_xlate

} );

return b_navigator_dijit_Blocks;

} );
