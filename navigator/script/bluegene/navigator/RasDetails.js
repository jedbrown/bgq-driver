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
    "./topic",
    "../Bgws",
    "dojo/_base/declare",
    "dojo/_base/lang"
],
function(
        l_topic,
        b_Bgws,
        d_declare,
        d_lang
    )
{


var b_navigator_RasDetails = d_declare( [],

/** @lends bluegene^navigator^RasDetails# */
{

    _bgws : null,
    _ras : null,

    _ras_details_dij : null,


    /** @constructs */
    constructor : function( bgws, ras )
    {
        this._bgws = bgws;
        this._ras = ras;

        l_topic.subscribe( l_topic.ras, d_lang.hitch( this, this._eventSelected ) );
    },


    notifyLoggedIn : function( has_authority )
    {
        if ( has_authority ) {

            this._ras_details_dij = this._ras.getDetailsDijit();

            this._ras_details_dij.setFetchDetailsFn( d_lang.hitch( this, this._fetchDetails ) );

        } else {

            this._ras_details_dij = null;

        }
    },


    _fetchDetails : function( rec_id )
    {
        return this._bgws.fetch( b_Bgws.resourcePaths.ras + "/" + rec_id );
    },


    _eventSelected : function( id )
    {
        if ( this._ras_details_dij ) {
            this._ras_details_dij.setRecId( id );
        }
    }

} );


return b_navigator_RasDetails;

} );
