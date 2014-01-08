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
    "./Duration",
    "dojo/_base/declare"
],
function(
        b_Duration,
        d_declare
    )
{


var _pad2 = function( n ) {
        return (n < 10 ? "0" + n : "" + n);
    };


var _dateToIso = function( date )
    {
        return ("" +
                date.getFullYear() +
                _pad2( date.getMonth() + 1 ) +
                _pad2( date.getDate() ) +
                "T" +
                _pad2( date.getHours() ) +
                _pad2( date.getMinutes() ) +
                _pad2( date.getSeconds() ));
    };


var _parseIsoTimestamp = function( s )
    {
        var re = /(\d{4})(\d{2})(\d{2})T(\d{2})(\d{2})(\d{2})/;

        var m = re.exec( s );

        var year = m[1];
        var month = m[2];
        var day = m[3];
        var hour = m[4];
        var minute = m[5];
        var second = m[6];

        return new Date( year, month - 1, day, hour, minute, second );
    };


var b_TimeInterval = d_declare( null,

/** @lends bluegene^TimeInterval */
{
    _start_date: null,
    _end_date: null,
    _duration: null,


    /** @constructs */
    constructor: function( obj )
    {
        this._start_date = ("start" in obj ? obj.start : null);
        this._end_date = ("end" in obj ? obj.end : null);
        this._duration = ("duration" in obj ? obj.duration : null);
    },

    toIso: function()
    {
        if ( this._start_date !== null && this._end_date !== null ) {
            return (_dateToIso( this._start_date ) + "/" + _dateToIso( this._end_date ));
        }

        if ( this._start_date !== null && this._duration !== null ) {
            return (_dateToIso( this._start_date ) + "/" + this._duration.toIso());
        }

        if ( this._duration !== null && this._end_date !== null ) {
            return (this._duration.toIso() + "/" + _dateToIso( this._end_date ));
        }

        if ( this._duration !== null ) {
            return this._duration.toIso();
        }
    },

    hasStartDate : function()  { return (this._start_date !== null); },
    hasEndDate : function()  { return (this._end_date !== null); },
    hasDuration : function()  { return (this._duration !== null); },

    getStartDate : function()  { return this._start_date; },
    getEndDate : function()  { return this._end_date; },
    getDuration : function()  { return this._duration; }

} );


b_TimeInterval.dateToIso = _dateToIso;

b_TimeInterval.parseIsoTimestamp = _parseIsoTimestamp;


b_TimeInterval.parseIso = function( s )
{
    var re = /(.*)\/(.*)/;

    var match = re.exec( s );

    var obj = {};

    if ( match ) {

        var start_str = match[1];

        if ( start_str[0] == "P" ) {
            obj.duration = b_Duration.parseIso( start_str );
        } else {
            obj.start = _parseIsoTimestamp( start_str );
        }

        var end_str = match[2];

        if ( end_str[0] == "P" ) {
            obj.duration = b_Duration.parseIso( end_str );
        } else {
            obj.end = _parseIsoTimestamp( end_str );
        }

    } else { // no /, it must be a duration.

        obj.duration = b_Duration.parseIso( s );

    }

    return new b_TimeInterval( obj );

};


return b_TimeInterval;

} );
