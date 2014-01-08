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
    "dojo/_base/declare"
],
function(
        d_declare
    )
{


var b_Duration = d_declare( null,

/** @lends bluegene^Duration# */
{
    _years: 0,
    _months: 0,
    _days: 0,
    _hours: 0,
    _minutes: 0,
    _seconds: 0,

    /** @constructs */
    constructor: function( args )
    {
        this._years = ("years" in args ? args.years : 0);
        this._months = ("months" in args ? args.months : 0);
        this._days = ("days" in args ? args.days : 0);
        this._hours = ("hours" in args ? args.hours : 0);
        this._minutes = ("minutes" in args ? args.minutes : 0);
        this._seconds = ("seconds" in args ? args.seconds : 0);
    },

    toIso: function()
    {
        var ret = "P";
        if ( this._years !== 0 )  ret += ("" + this._years + "Y");
        if ( this._months !== 0 )  ret += ("" + this._months + "M");
        if ( this._days !== 0 )  ret += ("" + this._days + "D");

        if ( this._hours !== 0 || this._minutes !== 0 || this._seconds !== 0 )  ret += "T";

        if ( this._hours !== 0 )  ret += ("" + this._hours + "H");
        if ( this._minutes !== 0 )  ret += ("" + this._minutes + "M");
        if ( this._seconds !== 0 )  ret += ("" + this._seconds + "S");
        return ret;
    },

    getYears : function()  { return this._years; },
    getMonths : function()  { return this._months; },
    getDays : function()  { return this._days; },
    getHours : function()  { return this._hours; },
    getMinutes : function()  { return this._minutes; },
    getSeconds : function()  { return this._seconds; }

} );


b_Duration.parseIso = function( s )
{
    var re = /P((\d+)Y)?((\d+)M)?((\d+)D)?(T((\d+)H)?((\d+)M)?((\d+)S)?)?/;

    var match = re.exec( s );

    var years = 0;
    var months = 0;
    var days = 0;
    var hours = 0;
    var minutes = 0;
    var seconds = 0;

    if ( match[2] )  years = Number( match[2] );
    if ( match[4] )  months = Number( match[4] );
    if ( match[6] )  days = Number( match[6] );
    if ( match[9] )  hours = Number( match[9] );
    if ( match[11] )  minutes = Number( match[11] );
    if ( match[13] )  seconds = Number( match[13] );

    return new b_Duration( {
            years: years, months: months, days: days,
            hours: hours, minutes: minutes, seconds: seconds
        } );
};

return b_Duration;

} );
