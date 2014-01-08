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
    "../AbstractTemplatedContainer",
    "../../../BlueGene",
    "../../../dijit/OutputFormat",
    "../../../dijit/OutputText",
    "dojo/dom-construct",
    "dojo/when",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dijit/form/CheckBox",
    "dojo/text!./templates/ConfigureIoRun.html",
    "module",

    // Used only in template.
    "../Table",
    "../../format",
    "../../../dijit/Hideable",
    "dijit/form/Button",
    "dijit/form/Form",
    "dijit/layout/ContentPane"
],
function(
        l_AbstractTemplatedContainer,
        b_BlueGene,
        b_dijit_OutputFormat,
        b_dijit_OutputText,
        d_construct,
        d_when,
        d_array,
        d_declare,
        d_lang,
        j_form_CheckBox,
        template,
        module
    )
{


var b_navigator_dijit_diagnostics_ConfigureIoRun = d_declare(
        [ l_AbstractTemplatedContainer ],

{

    templateString : template,

    _submit_diagnostics_run_fn : null,

    _buckets : null,
    _tests : null,

    _selected_locations : null, // { "<location>" : { "color": "lightblue }, ... } (a set of locations)

    _submit_promise : null,


    constructor : function()
    {
        this._buckets = {};
        this._tests = {};
        this._selected_locations = {};
    },


    setSubmitDiagnosticsRunFn : function( fn )
    {
        this._submit_diagnostics_run_fn = fn;
    },


    setConfig : function( config )
    {
        console.log( module.id + ": setConfig, config=", config );

        this._tests = {};
        this._buckets = {};

        var test_to_buckets = {};

        // set up this._tests, should include only those where target is I/O block.
        for ( test_name in config.tests ) {
            var test = config.tests[test_name];

            if ( ! (test.targetType == "IoBlock" || test.targetType == "ComputeOrIoBlock") ) {
                continue;
            }

            this._tests[test_name] = test;
        }

        // Figure out which buckets have I/O tests, create test_to_buckets.
        for ( bucket_name in config.buckets ) {
            var any_tests_in_bucket = false;

            d_array.forEach( config.buckets[bucket_name].tests, d_lang.hitch( this, function( test_name ) {
                if ( ! this._tests[test_name] ) {
                    console.log( module.id + ": test " + test_name + " in bucket " + bucket_name + " not found, skipping" );
                    return; // skip tests in bucket not in tests (e.g., not compute block tests).
                }

                if ( ! test_to_buckets[test_name] )  test_to_buckets[test_name] = [];
                test_to_buckets[test_name].push( bucket_name );

                any_tests_in_bucket = true;
            } ) );

            if ( any_tests_in_bucket ) {
                // console.log( module.id + ": adding bucket " + bucket_name );
                this._buckets[bucket_name] = config.buckets[bucket_name];
            }
        }


        for ( bucket_name in this._buckets ) {
            var bucket = this._buckets[bucket_name];

            var li_elem = d_construct.create( "li", {}, this._bucketsElem );

            var cb_dij = new j_form_CheckBox( { name: "buckets", value: bucket_name } );

            cb_dij.on( "change", d_lang.hitch( this, this._bucketsChanged ) );

            cb_dij.placeAt( li_elem );
            cb_dij.startup();

            var total_timeout = 0;

            d_array.forEach( bucket.tests, d_lang.hitch( this, function( test_name ) {
                if ( test_name in this._tests ) {
                    total_timeout += this._tests[test_name].timeout;
                }
            } ) );

            d_construct.create( "label", { innerHTML: " " + bucket_name + " (" + total_timeout + " minutes) - " + bucket.description }, li_elem );
        }


        var test_names = [];
        for ( test_name in this._tests ) {
            test_names.push( test_name );
        }
        test_names.sort();

        var tests_data = [];

        d_array.forEach( test_names, d_lang.hitch( this, function( test_name ) {
            var test = this._tests[test_name];

            var test_data = { name : test_name, description: test.description, maxRunTime: test.timeout, buckets: test_to_buckets[test_name] ? test_to_buckets[test_name] : "N/A" };
            tests_data.push( test_data );
        } ) );

        this._testsTable.set( "data", { data: tests_data, idProperty: "name" } );


        var form_obj = {
                buckets: [ "checkup" ]
            };

        this._form.set( "value", form_obj );
    },


    getMachineHighlightData : function()
    {
        var ret = {
                loading: false,
                highlighting: this._selected_locations
            };
        return ret;
    },


    notifyMachineClicked : function( loc )
    {
        if ( loc.search( /^[QR]..-I.$/ ) === -1 ) {
            console.log( module.id + ": [" + this.id + "]  clicked on not an I/O drawer loc:", loc );
            return;
        }

        if ( loc in this._selected_locations )  delete this._selected_locations[loc];
        else this._selected_locations[loc] = { color: "lightblue" };

        console.log( module.id + ": [" + this.id + "]  clicked on", loc, "selected=", this._selected_locations );

        this._checkSubmitButtonStatus();
        this.onMachineHighlightingChanged();
    },


    onMachineHighlightingChanged : function() {},


    // override
    startup : function()
    {
        this.inherited( arguments );


        this._resetButton.on( "click", d_lang.hitch( this, this._resetDrawers ) );


        var layout = { columns: [
                { title: "Name", field: "name", dijitType: b_dijit_OutputText },
                { title: "Description", field: "description", dijitType: b_dijit_OutputText },
                { title: "Max Run Time", field: "maxRunTime", dijitType: b_dijit_OutputText },
                { title: "Buckets", field: "buckets", dijitType: b_dijit_OutputText }
            ] };

        this._testsTable.set( "layout", layout );
        this._testsTable.footerDijit.set( "value", { runTime: 0 } );
        this._testsTable.watch( "value", d_lang.hitch( this, this._testsChanged ) );


        this._submitButton.on( "click", d_lang.hitch( this, this._submit ) );
        this._dismissButton.on( "click", d_lang.hitch( this, this._dismissClicked ) );


        var monitor_update_command_dijs = [
                this._optionsStopOnError, this._optionsSaveAllOutput, this._optionsKillJobs,
                this._optionsDeleteBlock, this._optionsDisruptMasterClock, this._optionsDisruptIoRack,
                this._optionsInsertRas
            ];

        d_array.forEach( monitor_update_command_dijs, d_lang.hitch( this, function( j ) { j.on( "change", d_lang.hitch( this, this._updateCommandText ) ); } ) );
    },


    // override
    destroy : function()
    {
        if ( this._submit_promise ) {
            console.log( module.id + ": canceling operation in progress because destroyed." );
            this._submit_promise.cancel();
            this._submit_promise = null;
        }

        this.inherited( arguments );
    },


    _resetDrawers : function()
    {
        this._selected_locations = {};

        this._checkSubmitButtonStatus();
        this.onMachineHighlightingChanged();
    },


    _bucketsChanged : function()
    {
        // console.log( module.id + ": buckets changed",  this._form.get( "value" ).buckets );

        var form_obj = this._form.get( "value" );

        var new_buckets = form_obj.buckets;

        var tests = {};

        // Set the tests, the tests are all the ones that are in the buckets.

        d_array.forEach( new_buckets, d_lang.hitch( this, function( bucket_name ) {
            var bucket_tests = this._buckets[bucket_name].tests;
            d_array.forEach( bucket_tests, d_lang.hitch( this, function( test_name ) {
                if ( test_name in this._tests )  tests[test_name] = true;
            } ) );
        } ) );

        form_obj.tests = [];

        for ( test_name in tests ) {
            form_obj.tests.push( test_name );
        }

        this._form.set( "value", form_obj );

        this._updateRunTime();
    },


    _testsChanged: function()
    {
        this._updateRunTime();
        this._checkSubmitButtonStatus();
    },


    _updateRunTime : function()
    {
        var total_run_time = 0;

        var form_obj = this._form.get( "value" );

        var test_names = form_obj.tests;

        d_array.forEach( test_names, d_lang.hitch( this, function( test_name ) {
            total_run_time += this._tests[test_name].timeout;
        } ) );

        this._testsTable.footerDijit.set( "value", { runTime: total_run_time } );
    },


    _checkSubmitButtonStatus : function()
    {
        this._submitButton.set( "disabled", ! this._checkCanSubmit() );
        this._updateCommandText();
    },


    _checkCanSubmit : function()
    {
        var any_selected = false;
        for ( loc in this._selected_locations ) {
            any_selected = true;
            break;
        }

        if ( any_selected ) {

            this._drawersErrorInd.hide();

        } else {

            this._drawersErrorInd.show();
            return false;

        }


        if ( this._submit_promise != null )  return false; // can't submit if already submitted a diagnostics run.


        var form_obj = this._form.get( "value" );

        if ( ! ("tests" in form_obj) )  return false;
        if ( form_obj.tests.length === 0 )  return false;

        return true;
    },


    _submit: function( e )
    {
        e.preventDefault();

        var form_obj = this._form.get( "value" );

        console.log( module.id + ": submit form_obj=", form_obj );

        if ( ! this._checkCanSubmit() ) {
            console.log( "Cannot submit now for some reason," );
            return;
        }

        var new_diags_run_post_obj = this._calcNewRunPostData();

        this._submit_promise = this._submit_diagnostics_run_fn( new_diags_run_post_obj );

        this._checkSubmitButtonStatus();

        this._resultHideable.set( "visiblity", "hidden" );
        this._submittingHideable.set( "visibility", "visible" );

        d_when(
                this._submit_promise,
                d_lang.hitch( this, this._diagnosticsSubmitComplete ),
                d_lang.hitch( this, this._diagnosticsSubmitFailed )
            );
    },


    _calcNewRunPostData : function()
    {
        var form_obj = this._form.get( "value" );

        var new_diags_run_post_obj = {};

        new_diags_run_post_obj.io = [];

        for ( loc in this._selected_locations ) {
            new_diags_run_post_obj.io.push( loc );
        }
        new_diags_run_post_obj.io.sort();

        new_diags_run_post_obj.tests = form_obj.tests;

        d_array.forEach( form_obj.runOptions, function( run_option ) { new_diags_run_post_obj[run_option] = true; } );

        console.log( module.id + ": submit submit_obj=", new_diags_run_post_obj );

        return new_diags_run_post_obj;
    },


    _diagnosticsSubmitComplete : function( args )
    {
        this._submit_promise = null;
        this._checkSubmitButtonStatus();

        this._submittingHideable.set( "visibility", "hidden" );

        var run_id = args.runId;

        console.log( module.id + ": started diagnostics with run id=", run_id );

        this._failedHideable.set( "visibility", "hidden" );

        this._startedRunIdText.set( "value", run_id );
        this._startedHideable.set( "visibility", "visible" );

        this._resultHideable.set( "visibility", "visible" );
    },


    _diagnosticsSubmitFailed : function( error )
    {
        this._submit_promise = null;
        this._checkSubmitButtonStatus();

        this._submittingHideable.set( "visibility", "hidden" );

        var err_text = error.message;

        console.log( module.id + ": failed to started diagnostics with error text=", err_text );

        this._startedHideable.set( "visibility", "hidden" );

        this._failedText.set( "value", err_text );
        this._failedHideable.set( "visibility", "visible" );

        this._resultHideable.set( "visibility", "visible" );
    },


    _dismissClicked : function()
    {
        this._resultHideable.set( "visibility", "hidden" );
    },


    _calcCommandText : function()
    {
        if ( ! this._checkCanSubmit() ) {
            return null;
        }

        var post_data = this._calcNewRunPostData();

        var cmd_str = b_BlueGene.calcDiagnosticsCommand( post_data );

        return cmd_str;
    },


    _updateCommandText : function()
    {
        var new_command_text = this._calcCommandText();

        this._commandText.set( "value", new_command_text );
    },
    

    _b_dijit_OutputText : b_dijit_OutputText

} );

return b_navigator_dijit_diagnostics_ConfigureIoRun;

} );
