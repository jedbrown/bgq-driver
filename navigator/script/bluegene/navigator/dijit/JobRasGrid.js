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
    "dojo/text!./templates/JobRasGrid.html",
    "module",

    // Used only in template.
    "../format",
    "dojox/grid/EnhancedGrid"
],
function(
        l_AbstractTemplatedContainer,
        l_MonitorActiveMixin,
        d_declare,
        template,
        module
    )
{

var b_navigator_dijit_JobRasGrid = d_declare(
        [ l_AbstractTemplatedContainer, l_MonitorActiveMixin ],
{

    templateString : template,


    setStore : function( store, query, queryOptions )
    {
        this._grid.setStore( store, query, queryOptions );
    }

} );


return b_navigator_dijit_JobRasGrid;

} );