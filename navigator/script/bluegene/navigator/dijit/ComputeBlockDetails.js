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
    "../../Bgws",
    "../../BlueGene",
    "../../TimeInterval",
    "dojo/when",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/text!./templates/ComputeBlockDetails.html",
    "module",

    // Used only in template.
    "./BlockFields",
    "../format",
    "../../dijit/OutputFormat",
    "dojo/number",
    "dijit/layout/ContentPane",
    "dijit/layout/StackContainer",
    "dijit/layout/TabContainer",
    "dojox/grid/EnhancedGrid"
],
function(
        l_AbstractTemplatedContainer,
        l_MonitorActiveMixin,
        b_Bgws,
        b_BlueGene,
        b_TimeInterval,
        d_when,
        d_array,
        d_declare,
        d_lang,
        template,
        module
    )
{


var b_navigator_dijit_ComputeBlockDetails = d_declare(
        [ l_AbstractTemplatedContainer, l_MonitorActiveMixin ],

{

    templateString : template,

    _block_fetch_fn : null,
    _jobs_store : null,

    _block_id : null,
    _machine_highlighting : null,

    _jobs_initialized : false,
    _dirty : true,

    _fetch_details_deferred : null,


    constructor : function()
    {
        this._machine_highlighting = {};
    },


    // override
    destroy : function()
    {
        if ( this._fetch_details_deferred ) {
            this._fetch_details_deferred.cancel();
            this._fetch_details_deferred = null;
        }

        this.inherited( arguments );
    },


    setBlockFetchFn : function( fn )
    {
        this._block_fetch_fn = fn;
    },


    setJobsStore : function( store )
    {
        this._jobs_store = store;
    },


    setBlockId : function( block_id )
    {
        this._block_id = block_id;

        this._dirty = true;
        this._machine_highlighting = {};
        this.onMachineHighlightingChanged();

        this._refresh();
    },


    getMachineHighlighting : function()
    {
        var ret = {
                loading: this._fetch_details_deferred,
                highlighting: this._machine_highlighting
            };
        return ret;
    },


    onMachineHighlightingChanged : function()  {},


    notifyRefresh : function()
    {
        this._refresh();
    },


    _notifyActive : function()
    {
        this._refresh();
    },


    _refresh : function()
    {
        if ( ! this._isActive() /*MonitorActiveMixin*/ ) {
            // Not visible.
            this._dirty = true;
            return;
        }

        if ( ! this._dirty ) {
            return;
        }

        if ( this._fetch_details_deferred ) {
            // Fetch in progress, cancel it.
            this._fetch_details_deferred.cancel();
            this._fetch_details_deferred = null;
        }

        if ( this._block_id === null ) {
            this._dirty = false;
            this._detailsStack.selectChild( this._detailsNonePane );
            return;
        }

        this._fetch_details_deferred = this._block_fetch_fn( this._block_id );
        this._detailsLoadingFmt.set( "value", { name: this._block_id } );
        this._detailsStack.selectChild( this._detailsLoadingPane );

        this.onMachineHighlightingChanged();

        d_when(
                this._fetch_details_deferred,
                d_lang.hitch( this, this._gotBlockDetails ),
                d_lang.hitch( this, this._fetchDetailsFailed )
            );
    },


    _gotBlockDetails : function( response )
    {
        console.log( module.id + ": got block details", response );

        this._fetch_details_deferred = null;
        this._dirty = false;

        var block_obj = response;

        this._detailsLoadedPane.set( "value", block_obj );
        this._updateMachineHighlighting( block_obj );

        this._detailsStack.selectChild( this._detailsLoadedPane );


        var start_ts = b_Bgws.parseTimestamp( response.createTime );
        var end_ts = new Date( (new Date()).getTime() + (24*60*60*1000) );

        var interval = new b_TimeInterval( { start: start_ts, end: end_ts } );

        var jobs_query = { block : this._block_id, startTime: interval.toIso() };

        if ( ! this._jobs_initialized ) {
            this._jobsGrid.setStore( this._jobs_store, jobs_query, null );
            this._jobs_initialized = true;
        } else {
            this._jobsGrid.setQuery( jobs_query, null );
        }


        return response;
    },

    _updateMachineHighlighting : function( block_obj )
    {
        this._machine_highlighting = {};

        if ( block_obj ) {
            if ( "midplanes" in block_obj ) {
                d_array.forEach(
                        block_obj.midplanes,
                        d_lang.hitch( this, function( mp_location ) {
                            this._machine_highlighting[mp_location] = { color: "lightblue" };
                        } )
                    );
            }
            if ( "passthrough" in block_obj ) {
                d_array.forEach(
                        block_obj.passthrough,
                        d_lang.hitch( this, function( mp_location ) {
                            this._machine_highlighting[mp_location] = { color: "orange", nodeBoards: {} };
                        } )
                    );
            }
            if ( "midplane" in block_obj && "nodeBoards" in block_obj ) {
                this._machine_highlighting[block_obj.midplane] = { nodeBoards: {} };
                d_array.forEach( block_obj.nodeBoards,
                        d_lang.hitch( this, function( nb_pos ) {
                            this._machine_highlighting[block_obj.midplane].nodeBoards[nb_pos] = "lightblue";
                        } )
                    );
            }
        }

        this.onMachineHighlightingChanged();
    },


    _fetchDetailsFailed : function( error )
    {
        var io_args = this._fetch_details_deferred.ioArgs;
        var block_id = this._block_id;

        this._fetch_details_deferred = null;
        this._block_id = null;
        this._dirty = false;

        var err_text = b_Bgws.calculateErrorMessage( error, io_args );

        console.log( module.id + ": Failed to get details for '" + block_id + "', " + err_text );

        this._detailsErrorFmt.set( "value", { name: block_id, msg: err_text } );
        this._detailsStack.selectChild( this._detailsLoadingErrorPane );
    }

} );

return b_navigator_dijit_ComputeBlockDetails;

} );
