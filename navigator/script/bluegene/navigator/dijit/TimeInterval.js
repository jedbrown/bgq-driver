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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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
    "../../Duration",
    "../../TimeInterval",
    "../../utility/utility",
    "../../dijit/ActionLink", // used in template
    "../../dijit/Hideable", // used in template
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/date/locale",
    "dijit/_TemplatedMixin",
    "dijit/_Widget",
    "dijit/_WidgetsInTemplateMixin",
    "dijit/form/DateTextBox", // used in template
    "dijit/form/NumberTextBox", // used in template
    "dijit/form/Select", // used in template
    "dijit/form/TimeTextBox",
    "dojo/text!./templates/TimeInterval.html"
],
function(
        b_Duration,
        b_TimeInterval,
        b_utility,
        b_dijit_ActionLink,
        b_dijit_Hideable,
        d_declare,
        d_lang,
        d_date_locale,
        j__TemplatedMixin,
        j__Widget,
        j__WidgetsInTemplateMixin,
        j_form_DateTextBox,
        j_form_NumberTextBox,
        j_form_Select,
        j_form_TimeTextBox,
        template
    )
{


var _PLACEHOLDER_DATE = new Date( (new Date()).getFullYear(), 0, 1, 0, 0, 0 );

var _IntervalType = {
        RELATIVE: "R",
        ABSOLUTE: "A",
        RECORD_ID: "I"
    };

var _UnitType = {
        YEARS: "Y",
        MONTHS: "M",
        DAYS: "D",
        HOURS: "h",
        MINUTES: "m",
        SECONDS: "s"
    };

var _UNITS_OPTIONS = [
        { value: _UnitType.MONTHS, label: "months" },
        { value: _UnitType.DAYS, label: "days" },
        { value: _UnitType.HOURS, label: "hours" },
        { value: _UnitType.MINUTES, label: "minutes" }
    ];


var b_navigator_dijit_TimeInterval = d_declare(
        [ j__Widget, j__TemplatedMixin, j__WidgetsInTemplateMixin ],

/** @lends bluegene^navigator^dijit^TimeInterval# */
{

    name: "",
    label: "Entries",
    value: "",
    ras: false,

    templateString : template,

    _initial_value: null,
    _interval_type: null,


    /** @constructs */
    constructor : function( params, srcNodeRef )
    {
        this._interval_type = _IntervalType.RELATIVE;
    },


    // override
    postCreate : function()
    {
        this.inherited( arguments );

        this._unit_dij.set( "options", _UNITS_OPTIONS );
        this._unit_dij.set( "value", _UnitType.DAYS );

        this._start_date_dij.set( "placeHolder", d_date_locale.format( _PLACEHOLDER_DATE, { fullYear: true, selector: "date" } ) );
        this._start_time_dij.set( "placeHolder", d_date_locale.format( _PLACEHOLDER_DATE, { selector: "time" } ) );

        this._end_date_dij.set( "placeHolder", d_date_locale.format( _PLACEHOLDER_DATE, { fullYear: true, selector: "date" } ) );
        this._end_time_dij.set( "placeHolder", d_date_locale.format( _PLACEHOLDER_DATE, { selector: "time" } ) );

        this._rel_toggle_button.on( "click", d_lang.hitch( this, this._toggle ) );
        this._abs_toggle_button.on( "click", d_lang.hitch( this, this._toggle ) );
        this._recid_toggle_button.on( "click", d_lang.hitch( this, this._toggle ) );

        if ( this.value !== "" ) {
            this._setValueAttr( this.value );
        }
    },


    _getValueAttr : function()
    {
        if ( this._interval_type === _IntervalType.RECORD_ID ) {
            return this._calcRecordIdInterval();
        }

        var interval;

        if ( this._interval_type === _IntervalType.ABSOLUTE ) {
            interval = this._calcAbsoluteInterval();
        } else {
            interval = this._calcRelativeInterval();
        }

        if ( ! interval )  return "";

        return interval.toIso();
    },


    _setValueAttr : function( val )
    {
        this.value = val;

        if ( (! val) || val === "" ) {
            // reset the value.

            if ( this._interval_type === _IntervalType.ABSOLUTE ) {
                this._toggle();
            }

            this._count_dij.set( "value", null );

            this._start_date_dij.set( "value", null );
            this._start_time_dij.set( "value", null );

            this._end_date_dij.set( "value", null );
            this._end_time_dij.set( "value", null );

            return;
        }

        var interval = b_TimeInterval.parseIso( val );

        if ( interval.hasDuration() ) { // it's a relative interval.

            if ( this._interval_type === _IntervalType.ABSOLUTE ) {
                this._toggle();
            }

            var duration = interval.getDuration();

            var val = 1;
            var unit_type = _UnitType.DAYS;

            if ( duration.getYears() != 0 ) {
                val = duration.getYears();
                unit_type = _UnitType.YEARS;
            } else if ( duration.getMonths() != 0 ) {
                val = duration.getMonths();
                unit_type = _UnitType.MONTHS;
            } else if ( duration.getDays() != 0 ) {
                val = duration.getDays();
                unit_type = _UnitType.DAYS;
            } else if ( duration.getHours() != 0 ) {
                val = duration.getHours();
                unit_type = _UnitType.HOURS;
            } else if ( duration.getMinutes() != 0 ) {
                val = duration.getMinutes();
                unit_type = _UnitType.MINUTES;
            } else if ( duration.getSeconds() != 0 ) {
                val = duration.getSeconds();
                unit_type = _UnitType.SECONDS;
            }

            this._count_dij.set( "value", val );
            this._unit_dij.set( "value", unit_type );

        } else { // it's an abs interval.

            if ( this._interval_type === _IntervalType.RELATIVE ) {
                this._toggle();
            }

            this._rel_hideable.hide();
            this._abs_hideable.show();

            this._start_date_dij.set( "value", interval.getStartDate() );
            this._start_time_dij.set( "value", interval.getStartDate() );

            this._end_date_dij.set( "value", interval.getEndDate() );
            this._end_time_dij.set( "value", interval.getEndDate() );

        }
    },


    _calcAbsoluteInterval: function()
    {
        if ( ! (this._start_date_dij.get( "value" ) && this._start_time_dij.get( "value" ) && this._end_date_dij.get( "value" ) && this._end_time_dij.get( "value" )) ) {
            return;
        }

        var start_date = b_utility.combineDates( this._start_date_dij.get( "value" ), this._start_time_dij.get( "value" ) );
        var end_date = b_utility.combineDates( this._end_date_dij.get( "value" ), this._end_time_dij.get( "value" ) );

        var interval = new b_TimeInterval( { start: start_date, end: end_date } );
        return interval;
    },


    _calcRelativeInterval: function()
    {
        if ( ! this._count_dij.get( "value" ) )  return;

        var count = this._count_dij.get( "value" );
        var unit = this._unit_dij.get( "value" );

        var duration_arg_obj = (
                unit == _UnitType.YEARS ?  { years: count } :
                unit == _UnitType.MONTHS ? { months: count } :
                unit == _UnitType.DAYS ? { days: count } :
                unit == _UnitType.HOURS ? { hours: count } :
                unit == _UnitType.MINUTES ?  { minutes: count } :
                unit == _UnitType.SECONDS ?  { seconds: count } :
                { days: count } // default to days.
            );

        var interval = new b_TimeInterval( { duration: new b_Duration( duration_arg_obj ) } );
        return interval;
    },


    _calcRecordIdInterval: function()
    {
        return (this._recid_start_dij.get( "value" ) + "-" + this._recid_end_dij.get( "value"));
    },


    _toggle : function()
    {
        if ( this._interval_type === _IntervalType.RELATIVE ) {

            this._interval_type = _IntervalType.ABSOLUTE;

            this._rel_hideable.hide();
            this._recid_hideable.hide();

            this._abs_hideable.show();

            return;
        }

        if ( this._interval_type === _IntervalType.ABSOLUTE ) {

            if ( this.ras ) {

                this._interval_type = _IntervalType.RECORD_ID;

                this._rel_hideable.hide();
                this._abs_hideable.hide();

                this._recid_hideable.show();

                return;
            }

            this._interval_type = _IntervalType.RELATIVE;

            this._abs_hideable.hide();
            this._recid_hideable.hide();

            this._rel_hideable.show();

            return;
        }


        if ( this._interval_type === _IntervalType.RECORD_ID ) {

            this._interval_type = _IntervalType.RELATIVE;

            this._recid_hideable.hide();
            this._abs_hideable.hide();

            this._rel_hideable.show();

            return;

        }
    }

} );

return b_navigator_dijit_TimeInterval;

} );
