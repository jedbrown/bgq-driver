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
    "dojo/dom-class",
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
    "dojox/charting/plot2d/Lines",
    "dojox/charting/themes/Julie",
    "dojo/text!./templates/UtilizationChart.html",
    "module"
],
function(
        ll_ChartsCommon,
        d_class,
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
        x_charting_plot2d_Lines,
        x_charting_themes_Julie,
        template,
        module
    )
{



var b_navigator_dijit_UtilizationChart = d_declare(
        [ j__WidgetBase, j__TemplatedMixin ],

{

    templateString : template,

    _fetch_data_fn : null,

    _chart: null,

    _promise: null,


    setFetchDataFn : function( fn )
    {
        this._fetch_data_fn = fn;
        this._refresh();
    },


    notifyRefresh : function()
    {
        this._refresh();
    },


    // override
    startup : function()
    {
        this.inherited( arguments );

        this._chart = new x_charting_Chart(
                this._chartElem,
                { title: "Utilization", titleFont: "bold normal normal 8pt Arial", titleGap: 5 }
            );

        this._chart.setTheme( x_charting_themes_Julie );

        this._chart.addPlot( "default",
                {
                    type: "Lines",
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

        this._chart.addSeries( "default", [ 0,0,0,0,0 ] );

        new x_charting_action2d_Tooltip( this._chart, "default",
                {
                    text : function( i ) {
                        return Math.floor( i.run.data[i.index] );
                    }
                }
            );

        this._chart.render();
    },


    // override
    destroy : function()
    {
        if ( this._promise ) {
            this._promise.cancel();
            this._promise = null;
        }

        this.inherited( arguments );
    },


    _refresh: function()
    {
        if ( ! this._fetch_data_fn )  return;

        if ( this._promise ) {
            console.log( module.id + ": still loading..." );
            return;
        }

        this._promise = this._fetch_data_fn();

        d_when(
                this._promise,
                d_lang.hitch( this, this._gotChartData )
            );
    },


    _gotChartData: function( result )
    {
        this._promise = null;

        this._chart.removeAxis( "x" );

        var labels = [];
        var series = [];

        d_array.forEach( result, function( d, i ) {
            labels.push( { value: (i + 1), text: ll_ChartsCommon.formatDate( d.date ) } );
            series.push( d.utilization );
        } );

        this._chart.addAxis(
                "x",
                {
                    minorTicks: false,
                    labels: labels
                }
            );

        this._chart.updateSeries( "default", series );

        this._chart.render();
    }

} );

return b_navigator_dijit_UtilizationChart;

} );
