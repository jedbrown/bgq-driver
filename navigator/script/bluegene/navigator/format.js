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
    "./xlate",
    "../Bgws",
    "../BlueGene",
    "../utility/xlate",
    "dojo/number",
    "dojo/_base/array",
    "dojo/_base/lang",
    "dojo/date/locale",
    "dojox/color"
],
function(
        b_xlate,
        b_Bgws,
        b_BlueGene,
        b_utility_xlate,
        d_number,
        d_array,
        d_lang,
        d_date_locale,
        x_color
    )
{


var _MIN_TEMP = 20;
var _MAX_TEMP = 90;


var _colorsForTemp = function( t )
{
    var h;

    if ( t <= _MIN_TEMP )  h = 240;
    else if ( t >= _MAX_TEMP )  h = 0;
    else {
        h = Math.floor( 240 - (((t - _MIN_TEMP) / (_MAX_TEMP - _MIN_TEMP)) * 240) );
    }

    var c = x_color.fromHsv( h, 255, 255 );

    var css = c.toCss();

    return { bg: css, fg: (h >= 40 && h <= 200 ? "black" : "white") };
};


var _service_action_status_xlator = b_utility_xlate.xlator( b_xlate.serviceActionStatus );


/** @name bluegene^navigator^format
 *  @namespace
 */

var b_navigator_format =

/** @lends bluegene^navigator^format */
{


    alerts : {

        locationType : b_utility_xlate.xlator( b_xlate.alerts.locationType ),
        severity : b_utility_xlate.xlator( b_xlate.alerts.severity ),
        state: b_utility_xlate.xlator( b_xlate.alerts.state ),
        urgency : b_utility_xlate.xlator( b_xlate.alerts.urgency ),

        duplicate: function( val )
        {
            if ( ! val )  return val;
            return val.substr( val.lastIndexOf( "/" ) + 1 );
        }
    },


    /** Convert the block status code to its long string for display. */
    blockStatus: b_utility_xlate.xlator( b_xlate.blockStatus ),


    /** Format diagnostics run blocks to the targets string. */
    diagnosticsRunTargets: function( run_blocks )
    {
        var ret = "";
        d_array.forEach( run_blocks, function( block ) {
            if ( ret.length != 0 ) ret += ",";
            ret += ("location" in block ? block.location : block.blockId);
        } );
        return ret;
    },

    diagnostics: {

        calcRunLogFile : function( log_directory )
        {
            if ( ! log_directory )  return null;
            return (log_directory + b_BlueGene.diagnostics.LOG_FILE_NAME);
        },

        hardwareStatus: b_utility_xlate.xlator( b_xlate.diagnostics.hardwareStatus )

    },


    hardwareReplaceType : b_utility_xlate.xlator( b_xlate.hardwareReplaceType ),

    /** Format a BG hardware status string. */
    hardwareStatus: b_utility_xlate.xlator( b_xlate.hardwareStatus ),


    /** Calculate class to put on hardware element given hardware status value. */
    hardwareStatusCalcClass: function( hw_status_str )
    {
        if ( hw_status_str == null )  return "";
        if ( hw_status_str == b_BlueGene.hardwareStatus.AVAILABLE )  return "";
        return "bgHardwareNotAvailable";
    },


    /** Convert the job status code to its long string for display. */
    jobStatus: b_utility_xlate.xlator( b_xlate.jobStatus ),


    jobText: function( job_count ) {
        if ( job_count == 1 ) {  return "job is"; }
        return "jobs are";
    },


    isAre : function( count ) {
        if ( count == 1 )  return "is";
        return "are";
    },


    alertPlural : function( count ) {
        if ( count == 1 )  return "alert";
        return "alerts";
    },


    percent: function( n )
    {
        return d_number.format( n, { type: "percent" } );
    },


    serviceActionStatus: _service_action_status_xlator,

    serviceActionStatusAttention : function( vals )
    {
        var ret = _service_action_status_xlator( vals[0] );
        if ( vals[1] ) {
            ret += " <img src='resources/script/bg_navigator/dojo/resources/blank.gif' title='" + vals[1] + "' style='width: 18px; height: 18px; background: url(resources/script/bg_navigator/dojox/image/resources/images/warning.png) -141px -141px;'></img>";
        }
        return ret;
    },


    /** Format a BGWS timestamp. */
    timestamp: function( timestamp_str )
    {
        var d = b_Bgws.parseTimestamp( timestamp_str );

        if ( ! d )  return;

        return d_date_locale.format( d );
    },


    /** Format a BGWS timestamp, in a long format that shows the partial seconds. */
    timestampLong: function( timestamp_str )
    {
        if ( timestamp_str == null )  return null;

        // replace 'T' with ' '

        var ts2 = timestamp_str.substr( 0, 10 ) + " " + timestamp_str.substr( 11 );

        return ts2;
    },


    colorsForTemp: _colorsForTemp,


    colorizeTemp: function( t )
    {
        var colors = _colorsForTemp( t );

        return ("<span style='background-color: " + colors.bg + "; color: " + colors.fg + ";'>" + t + "</span>");
    },


    coolantDouble : function( val )
    {
        return d_number.format( val, { pattern: "##0.00" } );
    },

    colorizeCoolantTemp : function( t )
    {
        var colors = _colorsForTemp( t );

        return ("<span style='background-color: " + colors.bg + "; color: " + colors.fg + ";'>" + d_number.format( t, { pattern: "##0.00" } ) + "</span>");
    },


    emptyQuotes : function( val )
    {
        if ( val === "" )  return "''";
        return val;
    }


};


var b_navigator = d_lang.getObject( "bluegene.navigator", true );
b_navigator.format = b_navigator_format;


return b_navigator_format;

} );
