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
    "../../format",
    "../../../dijit/ActionLink",
    "../../../dijit/OutputFormat",
    "dojo/when",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dijit/_WidgetBase",
    "dijit/_TemplatedMixin",
    "dijit/_WidgetsInTemplateMixin",
    "dojo/text!./templates/SummaryRunningCell.html",
    "module",

    // Used in template
    "../Table",
    "../../../dijit/Hideable",
    "dijit/form/Button"
],
function(
        b_navigator_format,
        b_dijit_ActionLink,
        b_dijit_OutputFormat,
        d_when,
        d_declare,
        d_lang,
        j__WidgetBase,
        j__TemplatedMixin,
        j__WidgetsInTemplateMixin,
        template,
        module
    )
{


var calcBlockIdProperties = function( o )
    {
        var label = (o.location ? o.location : o.blockId);
        return { label : label };
    };




var b_navigator_dijit_diagnostics_SummaryRunningCell = d_declare(
        [ j__WidgetBase, j__TemplatedMixin, j__WidgetsInTemplateMixin ],

{

    templateString : template,

    value: null,

    _cancel_diagnostics_run_fn : null,

    _run_id : null,


    setCancelDiagnosticsRunFn : function( new_fn )
    {
        this._cancel_diagnostics_run_fn = new_fn;
    },


    onBlockSelected : function( obj )  {},


    // override
    startup : function()
    {
        this.inherited( arguments );

        var create_fn = d_lang.hitch( this, function( dij, o )
            {
                dij.on( "click", d_lang.hitch( this, this._blockClicked, o ) );
            } );

        var layout = { columns: [
                { dijitType: b_dijit_ActionLink, calcProperties: calcBlockIdProperties, createCb: create_fn },
                { dijitType: b_dijit_OutputFormat, dijitProperties : { format: "${testsAnalyzed} of ${testsToAnalyze} Completed" } }
            ] };

        this._blocksTable.set( "layout", layout );
    },


    _blockClicked : function( o )
    {
        console.log( module.id + ": block selected o=", o, "runId=", this._run_id );

        var sel_obj = {
                runId : this._run_id
            };

        if ( "location" in o ) {
            if ( "blockId" in o ) {
                sel._obj.blockId = o.blockId;
            } else {
                sel_obj.blockId = "_DIAGS_" + o.location;
            }
            sel_obj.location = o.location;
        } else {
            sel_obj.blockId = o.blockId;
        }

        this.onBlockSelected( sel_obj );
    },


    _setValueAttr : function( new_value )
    {
        console.log( module.id + ": [" + this.id + "] set value to ", new_value );

        this._run_id = new_value.runId;


        this._startTimetext.set( "value", new_value.start );

        if ( "user" in new_value ) {
            this._username.set( "value", new_value.user );
        } else {
            this._userHideable.hide();
        }


        if ( "cancelable" in new_value ) {
            if ( new_value.cancelable === "canceled" ) {
                this._canceledText.show();
                this._cancelButton.set( "disabled", true );
            } else {
                this._cancelButton.on( "click", d_lang.hitch( this, this._cancelClicked ) );
            }
        } else {
            this._cancelButtonContainer.hide();
        }

        this._blocksTable.set( "data", { data: new_value.blocks, idProperty: "blockId" } );
    },


    _cancelClicked : function()
    {
        console.log( module.id + ": [" + this.id + "] Cancel!" );

        this._cancelButton.set( "disabled", true );
        this._errorHideable.hide();
        this._cancelingText.show();

        if ( ! this._cancel_diagnostics_run_fn ) {
            return;
        }

        d_when(
                this._cancel_diagnostics_run_fn( this._run_id ),
                d_lang.hitch( this, this._cancelledRun ),
                d_lang.hitch( this, this._cancelRunFailed )
            );
    },


    _cancelledRun : function( res_obj )
    {
        console.log( module.id + ": [" + this.id + "] run canceled id=", this._run_id, "res=", res_obj );

        this._cancelingText.hide();
        this._canceledText.show();
    },


    _cancelRunFailed : function( error )
    {
        console.log( module.id + ": failed to cancel run id=", this._run_id, "error=", error );

        this._cancelingText.hide();
        this._errorText.set( "value", "Error canceling the run, " + error.message );
        this._errorHideable.show();
        this._cancelButton.set( "disabled", false );
    },
    
    
    _b_navigator_format : b_navigator_format

} );


return b_navigator_dijit_diagnostics_SummaryRunningCell;

} );
