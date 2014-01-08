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
    "dojo/_base/declare",
    "dojo/text!./templates/RasFields.html",
    "module",

    // Used in template.
    "../../dijit/Hideable",
    "../../dijit/OutputText",
    "dijit/layout/ContentPane"
],
function(
        l_AbstractTemplatedContainer,
        ll_format,
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


var b_navigator_dijit_RasFields = d_declare(
        [ l_AbstractTemplatedContainer ],

{

    templateString : template,


    _setValueAttr : function( ras_obj )
    {
        var o = ras_obj;

        _setOptional( o, "block", this._blockId, this._blockIdItem );
        _setOptional( o, "category", this._category, this._categoryItem );
        _setOptional( o, "component", this._component, this._componentItem );
        _setOptional( o, "controlAction", this._controlAction, this._controlActionItem );
        _setOptional( o, "count", this._count, this._countItem );
        _setOptional( o, "cpu", this._cpu, this._cpuItem );
        _setOptional( o, "description", this._description, this._descriptionItem );
        _setOptional( o, "ecid", this._ecid, this._ecidItem );
        this._eventTime.set( "value", o.eventTime );
        this._idField.set( "value", o.id );
        _setOptional( o, "jobid", this._jobId, this._jobIdItem );
        _setOptional( o, "location", this._location, this._locationItem );
        _setOptional( o, "message", this._message, this._messageItem );
        _setOptional( o, "msg_id", this._messageId, this._messageIdItem );
        _setOptional( o, "rawData", this._rawData, this._rawDataItem );
        _setOptional( o, "relevantDiags", this._relevantDiags, this._relevantDiagsItem );
        _setOptional( o, "serialnumber", this._serialnumber, this._serialnumberItem );
        _setOptional( o, "severity", this._severity, this._severityItem );
        _setOptional( o, "svcAction", this._svcAction, this._svcActionItem );
        _setOptional( o, "thresholdCount", this._thresholdCount, this._thresholdCountItem );
        _setOptional( o, "thresholdPeriod", this._thresholdPeriod, this._thresholdPeriodItem );
    },


    _ll_format : ll_format

} );

return b_navigator_dijit_RasFields;

} );
