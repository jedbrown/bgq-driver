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
    "../format",
    "dojo/number",
    "dojo/_base/declare",
    "dojo/text!./templates/JobFields.html",
    "module",

    // Used in template.
    "../../dijit/Hideable",
    "../../dijit/OutputText",
    "dijit/layout/ContentPane"
],
function(
        l_AbstractTemplatedContainer,
        ll_format,
        d_number,
        d_declare,
        template,
        module
    )
{


var _setOptional = function( o, fieldname, output_dij, hideable_dij )
    {
        if ( ! (fieldname in o) ) {
            hideable_dij.hide();
            return;
        }

        output_dij.set( "value", o[fieldname] );
        hideable_dij.show();
    };


var b_navigator_dijit_JobFields = d_declare(
        [ l_AbstractTemplatedContainer ],

{

    templateString : template,


    _setValueAttr : function( job_obj )
    {
        var o = job_obj;

        this._id.set( "value", o.id );
        this._executable.set( "value", o.executable );
        this._user.set( "value", o.username );
        this._block.set( "value", o.block );
        this._idir.set( "value", o.workingDirectory );
        this._args.set( "value", o.arguments );
        this._env.set( "value", o.environment );
        this._startedDij.set( "value", o.startTime );
        _setOptional( o, "endTime", this._ended, this._endedItem );
        this._status.set( "value", o.status );
        _setOptional( o, "statusChangeTime", this._statusChanged, this._statusChangedItem );
        this._nodesUsed.set( "value", o.nodesUsed );
        this._shape.set( "value", o.shape );
        _setOptional( o, "corner", this._corner, this._cornerItem );
        this._PPN.set( "value", o.processesPerNode );
        this._NP.set( "value", o.np );
        _setOptional( o, "mapping", this._mapping, this._mappingItem );
        _setOptional( o, "schedulerData", this._schedData, this._schedDataItem );
        this._client.set( "value", o.client );
        _setOptional( o, "exitStatus", this._exitStatus, this._exitStatusItem );
        _setOptional( o, "errorText", this._errText, this._errTextItem );
    },
    
    
    _d_number : d_number,
    _ll_format : ll_format

} );

return b_navigator_dijit_JobFields;

} );
