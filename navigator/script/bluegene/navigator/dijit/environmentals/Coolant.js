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
    "./AbstractQueryResultsDijit",
    "../../format",
    "dojo/_base/declare",
    "dojo/text!./templates/Coolant.html"
],
function(
        l_AbstractQueryResultsDijit,
        b_navigator_format,
        d_declare,
        template
    )
{


var _FLOW_RATE_FORMATTER_SI = function( val )
    {
        var si_val = val * 3.78541178;
        return b_navigator_format.coolantDouble( si_val );
    };


var _PRESSURE_FORMATTER_SI = function( val )
    {
        var si_val = val * 6.89475729;
        return b_navigator_format.coolantDouble( si_val );
    };


var _UNITS_SYSTEM_DATA = {
            "US": {
                    flowRate : { unitsText : "gal/min", formatter: b_navigator_format.coolantDouble },
                    pressure: { unitsText : "PSI", formatter: b_navigator_format.coolantDouble }
                },

            "SI": {
                    flowRate : { unitsText : "L/min", formatter: _FLOW_RATE_FORMATTER_SI },
                    pressure: { unitsText : "kPa", formatter: _PRESSURE_FORMATTER_SI }
                }
        };


var b_navigator_dijit_environmentals_Coolant = d_declare(
        [ l_AbstractQueryResultsDijit ],

{

    templateString : template,


    measurementSystem : "US",


    // override
    _getExtraFieldNames: function()
    {
        return [
                "inletFlowRate",
                "outletFlowRate",
                "coolantPressure",
                "diffPressure",
                "inletCoolantTemp",
                "outletCoolantTemp",
                "dewpointTemp",
                "ambientTemp",
                "ambientHumidity",
                "systemPower",
                "shutoffCause"
            ];
    },


    _setMeasurementSystemAttr : function( new_system )
    {
        if( ! new_system )  return;

        new_system = new_system.toUpperCase();

        if ( ! (new_system in _UNITS_SYSTEM_DATA ) ) {
            console.log( module.id + ": Tried to use invalid measurement system '" + new_system + "', ignoring." );
            return;
        }

        this._set( "measurementSystem", new_system );

        var msi = _UNITS_SYSTEM_DATA[this.measurementSystem];

        var structure = [ {
                rows: [
                        { field: "location", name: "Location", width: "100px" },
                        { field: "time", formatter: b_navigator_format.timestamp, name: "Time", width: "110px", cellStyles: "text-align: right;" },
                        { field: "inletFlowRate", formatter: msi.flowRate.formatter, name: ("Inlet Flow Rate (" + msi.flowRate.unitsText + ")"), width: "60px", cellStyles: "text-align: right;" },
                        { field: "outletFlowRate", formatter: msi.flowRate.formatter, name: ("Outlet Flow Rate (" + msi.flowRate.unitsText + ")"), width: "60px", cellStyles: "text-align: right;" },
                        { field: "coolantPressure", formatter: msi.pressure.formatter, name: "Inlet Pressure (" + msi.pressure.unitsText + ")", width: "100px", cellStyles: "text-align: right;" },
                        { field: "diffPressure", formatter: msi.pressure.formatter, name: "Differential Pressure (" + msi.pressure.unitsText + ")", width: "140px", cellStyles: "text-align: right;" },
                        { field: "inletCoolantTemp", formatter: b_navigator_format.colorizeCoolantTemp, name: "Inlet Temp (&deg;C)", width: "60px", cellStyles: "text-align: right;" },
                        { field: "outletCoolantTemp", formatter: b_navigator_format.colorizeCoolantTemp, name: "Outlet Temp (&deg;C)", width: "60px", cellStyles: "text-align: right;" },
                        { field: "dewpointTemp", formatter: b_navigator_format.coolantDouble, name: "Dewpoint (&deg;C)", width: "60px", cellStyles: "text-align: right;" },
                        { field: "ambientTemp", formatter: b_navigator_format.colorizeCoolantTemp, name: "Ambient Temp (&deg;C)", width: "60px", cellStyles: "text-align: right;" },
                        { field: "ambientHumidity", formatter: b_navigator_format.coolantDouble, name: "Ambient Humidity (%)", width: "60px", cellStyles: "text-align: right;" },
                        { field: "systemPower", formatter: b_navigator_format.coolantDouble, name: "System Power (KW)", width: "60px", cellStyles: "text-align: right;" },
                        { field: "shutoffCause", name: "Shutoff Cause", width: "70px", cellStyles: "text-align: center;" }
                    ]
            } ];

        this._grid.set( "structure", structure );
    }

} );

return b_navigator_dijit_environmentals_Coolant;

} );
