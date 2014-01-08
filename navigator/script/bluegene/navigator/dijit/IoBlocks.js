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
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/text!./templates/IoBlocks.html",
    "module",

    // Used only in template.
    "./IoBlockDetails",

    "../format",
    "dojo/number",
    "dijit/layout/BorderContainer",
    "dijit/layout/ContentPane",
    "dojox/grid/EnhancedGrid"
],
function(
        l_AbstractTemplatedContainer,
        l_MonitorActiveMixin,
        d_declare,
        d_lang,
        template,
        module
    )
{


var b_navigator_dijit_IoBlocks = d_declare(
        [ l_AbstractTemplatedContainer, l_MonitorActiveMixin ],

{

    templateString : template,

    _store : null,

    _initialized : false,
    _dirty : true,


    getDetailsDijit : function()
    {
        return this._blockDetails;
    },


    setStore : function( new_store )
    {
        this._store = new_store;
        this._refresh();
    },


    notifyRefresh : function()
    {
        this._refresh();
    },


    onBlockSelected : function( block_id ) {},


    // override
    startup : function()
    {
        this.inherited( arguments );

        this._grid.on( "rowClick", d_lang.hitch( this, this._selectBlock ) );
    },


    // override MonitorActiveMixin
    _notifyActive : function()
    {
        console.log( module.id + ": notify active." );

        this.inherited( arguments );
        this._loadIfReady();
    },


    _refresh : function()
    {
        this._dirty = true;
        this._loadIfReady();
    },


    _loadIfReady : function()
    {
        if ( ! this._store )  return; // no store.
        if ( ! this._isActive() /*MonitorActiveMixin*/ )  return; // Not displayed.
        if ( ! this._dirty )  return; // Clean.

        if ( this._initialized ) {
            this._grid.render();
        } else {
            this._grid.setStore( this._store, { "type": "io" } /* query */ , null );
            this._initialized = true;
        }

        this._dirty = false;
    },


    _selectBlock : function( ev )
    {
        if ( ev.rowIndex < 0 ) {
            // Nothing to do, selected invalid row.
            return;
        }

        var item = this._grid.getItem( ev.rowIndex );

        if ( item == null ) {
            console.log( module.id + ": no item" );
            return;
        }

        var block_id = this._store.getValue( item, "id", "" );

        console.log( module.id + ": I/O block '" + block_id + "' selected" );

        this.onBlockSelected( block_id );
    }

} );

return b_navigator_dijit_IoBlocks;

} );
