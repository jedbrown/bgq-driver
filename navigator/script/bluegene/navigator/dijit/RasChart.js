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
    "../ChartsCommon",
    "../../TimeInterval",
    "dojo/when",
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dijit/_WidgetBase",
    "dijit/_TemplatedMixin",
    "dojox/gfx/utils",
    "dojox/charting/Chart",
    "dojox/charting/action2d/Tooltip",
    "dojox/charting/axis2d/Default",
    "dojox/charting/plot2d/StackedAreas",
    "dojox/charting/themes/Julie",
    "dojox/charting/widget/Legend",
    "dojo/text!./templates/RasChart.html",
    "module"
],
function(
        ll_ChartsCommon,
        b_TimeInterval,
        d_when,
        d_array,
        d_declare,
        d_lang,
        j__WidgetBase,
        j__TemplatedMixin,
        x_gfx_utils,
        x_charting_Chart,
        x_charting_action2d_Tooltip,
        x_charting_axis2d_Default,
        x_charting_plot2d_StackedAreas,
        x_charting_themes_Julie,
        x_charting_widget_Legend,
        template,
        module
    )
{


var b_navigator_dijit_RasChart = d_declare(
        [ j__WidgetBase, j__TemplatedMixin ],

{

    templateString : template,


    _fetch_ras_data_fn : null,

    _chart : null,

    _promise: null,


    setFetchRasDataFn : function( fn )
    {
        this._fetch_ras_data_fn = fn;
        this._refresh();
    },


    onSelected : function( obj ) {},


    notifyRefresh : function()
    {
        this._refresh();
    },


    // override.
    startup : function()
    {
        this.inherited( arguments );

        console.log( module.id + ": startup" );

        this._chart = new x_charting_Chart( this._chartElem, { title: "RAS Events", titleFont: "bold normal normal 8pt Arial", titleGap: 5 } );

        this._chart.setTheme( x_charting_themes_Julie );

        this._chart.addPlot( "default",
                {
                    type: "StackedAreas",
                    markers: true,
                    tension: "S"
                }
            );

        this._chart.addAxis(
                "x",
                {
                    minorTicks: false,
                    labels: [ { value: 1, text: '1/1' },
                              { value: 2, text: '1/2' },
                              { value: 3, text: '1/3' },
                              { value: 4, text: '1/4' },
                              { value: 5, text: '1/5' }
                            ]
                }
            );

        this._chart.addAxis(
                "y",
                {
                    vertical: true,
                    fixUpper: "major",
                    includeZero: true,
                    minorTicks: false
                }
            );

        this._chart.addSeries( "Fatal", [ 0,0,0,0,0 ] );
        this._chart.addSeries( "Warn", [ 0,0,0,0,0 ] );
        this._chart.addSeries( "Info", [ 0,0,0,0,0 ] );

        new x_charting_action2d_Tooltip( this._chart, "default",
                {
                    text : function( i ) {
                        return (i.run.name + ": " + i.run.data[i.index]);
                    }
                }
            );

        this._chart.render();

        // I had a check for is not IE (! dojo.isIE) to prevent creating legend if on IE for some reason.
        // I've tried it now and seems to work in IE8, so I'm removing the check and it will be created for IE.
        new x_charting_widget_Legend(
                { chart: this._chart, horizontal: false },
                this._legendElem
            );

        this._chart.connectToPlot( "default", this, this._event );
    },


    // override
    destroy : function()
    {
        console.log( module.id + ": destroy" );

        if ( this._promise ) {
            this._promise.cancel();
            this._promise = null;
        }

        this.inherited( arguments );
    },


    _refresh : function()
    {
        if ( ! this._fetch_ras_data_fn )  return;

        if ( this._promise ) {
            console.log( module.id + ": still loading..." );
            return;
        }

        this._promise = this._fetch_ras_data_fn();

        d_when(
                this._promise,
                d_lang.hitch( this, this._gotChartData )
            );
    },


    _gotChartData: function( ras_data )
    {
        this._promise = null;

        var labels = [];
        var series = {
                f: [],
                w: [],
                i: []
        };

        d_array.forEach( ras_data, function( d, i ) {
            labels.push( { value: (i + 1), text: ll_ChartsCommon.formatDate( d.date ) } );
            series.f.push( d.fatal );
            series.w.push( d.warn );
            series.i.push( d.info );
        } );


        this._chart.removeAxis( "x" );

        this._chart.addAxis(
                "x",
                {
                    minorTicks: false,
                    labels: labels
                }
            );

        this._chart.updateSeries( "Fatal", series.f );
        this._chart.updateSeries( "Warn", series.w );
        this._chart.updateSeries( "Info", series.i );

        this._chart.render();

        this._today_start_ms = ll_ChartsCommon.parseDate( ras_data[ras_data.length-1].date ).getTime();
    },


    _event : function( e )
    {
        if ( e.type !== "onclick" )  return;

        console.log( module.id + ": click on, e=", e );

        var days_back = (5 - e.x);

        var that_day_start_ms = this._today_start_ms - (days_back * 24 * 60 * 60 * 1000);

        var time_start = new Date( that_day_start_ms );
        var time_end = new Date( that_day_start_ms + (24 * 60 * 60 * 1000) );

        var interval = new b_TimeInterval( { start: time_start, end: time_end } );

        var severities = [];
        if ( e.run.name === "Fatal" )  severities.push( "F" );
        else if ( e.run.name === "Warn" )  severities.push( "W" );
        else if ( e.run.name === "Info" )  severities.push( "I" );

        var pub_obj = {
                interval : interval,
                severities : severities
            };


        this.onSelected( pub_obj );
    }

} );

return b_navigator_dijit_RasChart;

} );
