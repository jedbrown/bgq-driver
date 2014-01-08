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
    "../../BlueGene",
    "dojo/dom-construct",
    "dojo/when",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojox/charting/Chart",
    "dojox/charting/action2d/MoveSlice",
    "dojox/charting/action2d/Tooltip",
    "dojox/charting/axis2d/Default",
    "dojox/charting/plot2d/Pie",
    "dojox/charting/plot2d/StackedColumns",
    "dojox/charting/themes/Julie",
    "dojox/charting/widget/Legend",
    "dojox/gfx/utils",
    "dojo/text!./templates/PerformanceMonitoring.html",
    "module",

    // Used only in template.
    "./TimeInterval",
    "../../dijit/Hideable",
    "../../dijit/OutputText",
    "dijit/TooltipDialog",
    "dijit/layout/ContentPane",
    "dijit/form/Button",
    "dijit/form/DropDownButton",
    "dijit/form/Form",
    "dijit/form/Select",
    "dijit/form/TextBox"
],
function(
        l_AbstractTemplatedContainer,
        l_MonitorActiveMixin,
        ll_FilterOptions,
        b_BlueGene,
        d_construct,
        d_when,
        d_array,
        d_declare,
        d_lang,
        x_charting_Chart,
        x_charting_action2d_MoveSlice,
        x_charting_action2d_Tooltip,
        x_charting_axis2d_Default,
        x_charting_plot2d_Pie,
        x_charting_plot2d_StackedColumns,
        x_charting_themes_Julie,
        x_charting_widget_Legend,
        x_gfx_utils,
        template,
        module
    )
{


var b_navigator_dijit_PerformanceMonitoring = d_declare(
        [ l_AbstractTemplatedContainer, l_MonitorActiveMixin ],

{

    templateString : template,

    _PAGE_SIZE: 20,

    _fetch_performance_data_fn : null,

    _initial_filter : null,

    _dirty : true,
    _fetch_def : null,

    _form_obj : null,
    _filter_obj : null,

    _chart : null,
    _legend : null,

    _page_number: 1,

    _history : null,


    constructor : function()
    {
        this._history = [];
    },


    setFetchPerformanceDataFn : function( fn )
    {
        this._fetch_performance_data_fn = fn;
        this._dirty = true;
    },


    notifyRefresh : function()
    {
        if ( this._isActive() /*MonitorActiveMixin*/ ) {
            this._load();
            return;
        }

        this._dirty = true;
    },


    // override
    startup : function()
    {
        this.inherited( arguments );

        this._foInterval.set( "value", "P7D" );

        this._updateFilter();
        this._setCurrentPageNumber( 1 );

        this._prevPageButton.on( "click", d_lang.hitch( this, this._previousPage ) );
        this._nextPageButton.on( "click", d_lang.hitch( this, this._nextPage ) );

        this._foForm.on( "submit", d_lang.hitch( this, function( e ) {
                e.preventDefault();
                if ( ! this._foForm.isValid() )  return;
                this._applyFilter();
            } ) );

        this._backButton.set( "disabled", (this._history.length === 0) );
        this._backButton.on( "click", d_lang.hitch( this, this._back ) );

        this._foDetail.on( "change", d_lang.hitch( this, this._filterChanged ) );
        this._foBlock.on( "change", d_lang.hitch( this, this._filterChanged ) );

        this._initial_filter = this._foForm.get( "value" );
        this._foResetButton.on( "click", d_lang.hitch( this, function() { this._foForm.set( "value", this._initial_filter ); } ) );
    },


    // override
    destroy : function()
    {
        if ( this._fetch_def ) {
            this._fetch_def.cancel();
            this._fetch_def = null;
        }

        this.inherited( arguments );
    },


    // override MonitorActiveMixin
    _notifyActive : function()
    {
        console.log( module.id + ": active" );

        if ( ! this._dirty )  return;

        if ( this._fetch_def != null )  return; // Still loading.

        this._load();
    },


    _updateFilter : function()
    {
        this._form_obj = this._foForm.get( "value" );

        this._filter_obj = ll_FilterOptions.formToQueryHelper( this._form_obj, [ "detail", "block", "interval" ] );

        if ( this._form_obj.grouping && this._form_obj.grouping !== "N" )  this._filter_obj.grouping = this._form_obj.grouping;

        console.log( module.id + ": _updateFilter form=", this._form_obj, "filter=", this._filter_obj );
    },


    _previousPage : function()
    {
        console.log( module.id + ": previous page" );

        if ( this._page_number == 1 )  return;

        this._setCurrentPageNumber( this._page_number - 1 );

        this._load();
    },


    _nextPage : function()
    {
        console.log( module.id + ": next page" );

        if ( this._page_number == this._page_count )  return;

        this._setCurrentPageNumber( this._page_number + 1 );

        this._load();
    },


    _applyFilter : function()
    {
        console.log( module.id + ": apply filter" );

        this._addToHistory();

        this._setCurrentPageNumber( 1 );
        this._formToFilter();

        this._foDropDown.closeDropDown();
    },


    _filterChanged : function()
    {
        var form_obj = this._foForm.get( "value" );

        var disable_grouping = (form_obj.detail === "" && form_obj.block === "");
        this._foGrouping.set( "disabled", disable_grouping );
    },


    _load : function()
    {
        if ( ! this._fetch_performance_data_fn )  return; // Not initialized.

        // first cancel current request if in progress.
        if ( this._fetch_def != null ) {
            console.log( module + ": canceling current request." );
            this._fetch_def.cancel();
            this._fetch_def = null;
        }

        var range_start = (this._page_number - 1) * this._PAGE_SIZE;
        var range_end = range_start + this._PAGE_SIZE - 1;

        var range_str = "items=" + range_start + "-" + range_end;

        var headers = { Range: range_str };

        this._fetch_def = this._fetch_performance_data_fn( { headers: headers, query: this._filter_obj } );

        this._errorHideable.hide();
        this._loadingInd.show();

        this._updatePagingButtonState();

        d_when(
                this._fetch_def.response,
                d_lang.hitch( this, this._gotData )
            );
    },


    _gotData : function( response )
    {
        this._loadingInd.hide();

        var content_range = response.getHeader( "Content-Range" );

        var data = response.data;

        console.log( module.id + ": _gotData. data=", data, "content_range=", content_range );

        this._fetch_def = null;
        this._dirty = false;

        this._updatePagingButtonState();

        if ( this._chart != null ) {
            this._chart.destroy();
            this._chart = null;
        }

        if ( this._legend != null ) {
            this._legend.destroy();
            this._legend = null;
        }

        if ( (this._filter_obj.detail || this._filter_obj.block) && this._filter_obj.grouping ) {

            this._gotGroupedData( data, content_range );

        } else if ( this._filter_obj.detail || this._filter_obj.block ) {

            this._gotSingleData( data );

        } else {

            this._gotMultiDetailData( data, content_range );

        }

    },


    _gotGroupedData : function( data, content_range )
    {
        // If no data, then display error message.

        if ( data.length == 0 ) {
            var error_msg = "No data available for";
            if ( this._filter_obj.detail )  error_msg += " detail " + this._filter_obj.detail;
            if ( this._filter_obj.block )  error_msg += " block " + this._filter_obj.block;

            this._errorMessage.set( "value", error_msg );
            this._errorHideable.show();
            return;
        }


        // Create the chart.

        var title = (this._filter_obj.grouping == "D" ? "Daily" : this._filter_obj.grouping == "W" ? "Weekly" : "Monthly") + " Boot Block";
        if ( this._filter_obj.block ) title += " for " + this._filter_obj.block;
        if ( this._filter_obj.detail ) title += " detail " + this._filter_obj.detail;

        this._chart = new x_charting_Chart( this._perfChart, { title: title } );

        this._chart.setTheme( x_charting_themes_Julie );

        this._chart.addPlot( "default",
                {
                    type: "StackedColumns",
                    gap: 5,
                    minBarSize: 3,
                    maxBarSize: 20
                }
            );

        this._chart.addAxis(
                "y",
                {
                    vertical: true,
                    includeZero: true,
                    title: "Time(seconds)"
                }
            );


        var x_axis_labels = [];
        var series = {};

        d_array.forEach( b_BlueGene.performanceMonitoring.bootSteps, function( boot_step ) {
            series[boot_step] = [];
        } );

        d_array.forEach( data, function( group_obj, i ) {

            x_axis_labels.push( { value: (i + 1), text: group_obj.group } );

            d_array.forEach( b_BlueGene.performanceMonitoring.bootSteps, function( boot_step ) {
                if ( boot_step in group_obj.durations )  series[boot_step].push( group_obj.durations[boot_step] );
                else series[boot_step].push( 0 );
            } );

        } );


        console.log( module.id + ": _gotData. x_axis_labels=", x_axis_labels, " series=", series );

        this._chart.addAxis( "x",
                {
                    labels: x_axis_labels,
                    title: (this._filter_obj.grouping == "D" ? "Day" : this._filter_obj.grouping == "W" ? "Week" : "Month"),
                    titleOrientation: "away",
                    natural: true
                }
            );

        d_array.forEach( b_BlueGene.performanceMonitoring.bootSteps, d_lang.hitch( this, function( boot_step ) {
            this._chart.addSeries( boot_step, series[boot_step] );
        } ) );

        new x_charting_action2d_Tooltip( this._chart, "default", { text: function( i ) { return (i.run.name + ": " + i.run.data[i.index]); } } );

        this._chart.render();

        // If click on a bar, show the pie chart for that bar.
        this._chart.connectToPlot( "default", this, d_lang.hitch(
                    this,
                    function( data, e )
                    {
                        if ( e.type != "onclick" )   return;

                        var interval_str = data[e.index].interval;
                        console.log( module.id + ": group plot clicked e=", e, "interval_str=", interval_str );

                        this._addToHistory(); // add current form (before changing) to history.

                        this._foInterval.set( "value", interval_str );
                        this._foGrouping.set( "value", "N" );

                        this._formToFilter();
                    },
                    data
                )
            );

        d_construct.create( "div", { id: (this.id + "-chart-legend") }, this._contentPane.domNode );

        this._legend = new x_charting_widget_Legend(
                { chart: this._chart,
                  horizontal: false,
                  outline: true
                },
                (this.id + "-chart-legend")
            );


        this._updatePaging( content_range );
    },


    _gotSingleData : function( data )
    {
        // If no data, then display error message.

        var any_data = false;
        for ( i in data ) {
            any_data = true;
            break;
        }

        if ( ! any_data ) {
            var error_msg = "No data available for";
            if ( this._filter_obj.detail )  error_msg += " detail " + this._filter_obj.detail;
            if ( this._filter_obj.block )  error_msg += " block " + this._filter_obj.block;

            this._errorMessage.set( "value", error_msg );
            this._errorHideable.show();

            this._prevPageButton.set( "disabled", true );
            this._nextPageButton.set( "disabled", true );

            return;
        }


        // Create the chart.

        var title = "Boot Block";
        if ( this._filter_obj.detail )  title += " detail " + this._filter_obj.detail;
        if ( this._filter_obj.block )  title += " for " + this._filter_obj.block;

        this._chart = new x_charting_Chart( this._perfChart, { title: title } );

        this._chart.setTheme( x_charting_themes_Julie );

        var pie_plot_args =  {
                type: "Pie",
                radius: 200
            };

        pie_plot_args.labelWiring = "ccc";
        pie_plot_args.labelStyle = "columns";

        this._chart.addPlot( "default",
                pie_plot_args
            );


        var series = [];

        d_array.forEach( b_BlueGene.performanceMonitoring.bootSteps, function( boot_step ) {
            series.push( { text: boot_step, y: ((boot_step in data) ? data[boot_step] : 0.0) } );
        } );

        this._chart.addSeries( "A", series );

        new x_charting_action2d_MoveSlice( this._chart, "default" );
        new x_charting_action2d_Tooltip( this._chart, "default", { text: function( i ) {
                return (i.run.data[i.index].text + ": " + i.run.data[i.index].y);
            } } );

        this._chart.render();

        this._pagingContainer.hide();
    },


    _gotMultiDetailData : function( data, content_range )
    {
        // If no data, then display error message.

        if ( data.length == 0 ) {
            var error_msg = "No data available.";

            this._errorMessage.set( "value", error_msg );
            this._errorHideable.show();

            this._prevPageButton.set( "disabled", true );
            this._nextPageButton.set( "disabled", true );

            return;
        }


        // Create the chart.

        this._chart = new x_charting_Chart( this._perfChart, { title: "Boot Block" } );

        this._chart.setTheme( x_charting_themes_Julie );

        this._chart.addPlot( "default",
                {
                    type: "StackedColumns",
                    gap: 5,
                    minBarSize: 3,
                    maxBarSize: 20
                }
            );

        this._chart.addAxis(
                "y",
                {
                    vertical: true,
                    includeZero: true,
                    title: "Time(seconds)"
                }
            );


        var x_axis_labels = [];
        var series = {};

        d_array.forEach( b_BlueGene.performanceMonitoring.bootSteps, function( boot_step ) {
            series[boot_step] = [];
        } );

        d_array.forEach( data, function( detail_obj, i ) {

            x_axis_labels.push( { value: (i + 1), text: detail_obj.detail } );

            d_array.forEach( b_BlueGene.performanceMonitoring.bootSteps, function( boot_step ) {
                if ( boot_step in detail_obj.durations )  series[boot_step].push( detail_obj.durations[boot_step] );
                else series[boot_step].push( 0 );
            } );

        } );


        console.log( module.id + ": _gotData. x_axis_labels=", x_axis_labels, " series=", series );


        this._chart.addAxis( "x",
                {
                    labels: x_axis_labels,
                    title: "Detail",
                    titleOrientation: "away",
                    natural: true
                }
            );

        d_array.forEach( b_BlueGene.performanceMonitoring.bootSteps, d_lang.hitch( this, function( boot_step ) {
            this._chart.addSeries( boot_step, series[boot_step] );
        } ) );

        new x_charting_action2d_Tooltip( this._chart, "default", { text: function( i ) { return (i.run.name + ": " + i.run.data[i.index]); } } );

        this._chart.render();

        // If click on a bar, show the pie chart for that bar.
        this._chart.connectToPlot( "default", this, d_lang.hitch(
                    this,
                    function( data, e )
                    {
                        if ( e.type != "onclick" )   return;
                        var detail = data[e.index].detail;
                        console.log( module.id + ": plot clicked e=", e, " detail=", detail );

                        this._addToHistory(); // add current form (before setting detail) to history.

                        this._foDetail.set( "value", detail );

                        this._formToFilter();
                    },
                    data
                )
            );

        d_construct.create( "div", { id: (this.id + "-chart-legend") }, this._contentPane.domNode );

        this._legend = new x_charting_widget_Legend(
                { chart: this._chart,
                  horizontal: false,
                  outline: true
                },
                (this.id + "-chart-legend")
            );

        this._updatePaging( content_range );
    },


    _updatePaging : function( content_range )
    {
        if ( content_range ) {
            // Got partial response, update paging stuff.

            // Parse the content range, should be like items x-y/t

            var re = /items\s+\d+-\d+\/(\d+)/;

            var match = re.exec( content_range );

            var total_count_str = match[1];

            var total_count = Number( total_count_str );

            this._page_count = Math.ceil( total_count / this._PAGE_SIZE );

            console.log( module.id + ": total_count=", total_count, " page_count=", this._page_count );

        } else {
            // Got full response, update paging stuff.

            this._page_count = 1;

        }

        this._pageCountText.set( "value", "" + this._page_count );

        this._updatePagingButtonState();

        this._pagingContainer.show();
    },


    _setCurrentPageNumber : function( page_number )
    {
        this._page_number = page_number;

        this._currentPageText.set( "value", "" + this._page_number );
        this._updatePagingButtonState();
    },


    _updatePagingButtonState : function()
    {
        // If loading, disable all buttons.
        if ( this._fetch_def ) {
            this._prevPageButton.set( "disabled", true );
            this._nextPageButton.set( "disabled", true );
            return;
        }

        this._prevPageButton.set( "disabled", (this._page_number <= 1) );
        this._nextPageButton.set( "disabled", (this._page_number >= this._page_count) );
    },

    _addToHistory : function()
    {
        var hist_obj = {
                form: this._form_obj,
                page_number: this._page_number
            };

        console.log( module.id + ": adding to history, hist_obj=", hist_obj );

        this._history.push( hist_obj );
        this._backButton.set( "disabled", (this._history.length == 0) );
    },


    _back : function()
    {
        if ( this._history.length == 0 )  return;

        var history_obj = this._history.pop();
        this._backButton.set( "disabled", (this._history.length == 0) );

        console.log( module.id + ": back, history_obj=", history_obj );

        this._form_obj = history_obj.form;
        this._foForm.set( "value", this._form_obj );

        this._setCurrentPageNumber( history_obj.page_number );

        this._formToFilter();
    },


    _formToFilter : function()
    {
        this._updateFilter();
        this._load();
    }


} );

return b_navigator_dijit_PerformanceMonitoring;

} );
