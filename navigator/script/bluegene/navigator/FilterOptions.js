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
    "dojo/_base/array",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "module"
],
function(
        d_array,
        d_declare,
        d_lang,
        module
    )
{


var b_navigator_FilterOptions = d_declare( [],

/** @lends bluegene^navigator^FilterOptions# */
{
    _name : null, /* string */
    _grid : null, /* EnhancedGrid */
    _form : null, /* dijit.form.Form */
    _dropdown : null, /* dijit.form.DropDownButton */
    _form_to_query : null, /* function */
    _on_change : null, /* function */

    _reset_button : null, /* dijit.form.Button */

    _initial_form_value : null,


    /**
     * args contains:
     *   name : A name
     *   grid : EnhancedGrid
     *   form : dijit.form.Form
     *   dropdown : dijit.form.DropDownButton
     *   formToQuery : function, takes a form obj and returns a query obj. Optional, returns the form obj by default.
     *   onChange : function, called when the filter is set on the grid. Optional, does nothing by default.
     *
     * @constructs
     */
    constructor : function( args )
    {
        this._name = args.name;
        this._grid = args.grid;
        this._form = args.form;
        this._dropdown = args.dropdown;
        this._form_to_query = args.formToQuery || (function( form_obj ) { return form_obj; });
        this._on_change = args.onChange || (function() {});
        this._reset_button = args.resetButton;

        this._form.on( "submit", d_lang.hitch( this, this._onSubmit ) );

        if ( this._reset_button ) {
            this._reset_button.on( "click", d_lang.hitch( this, this._handleReset ) );
        }

        this._initial_form_value = this._form.get( "value" );
    },


    calcQuery : function( form_obj_out /* optional */ )
    {
        form_obj_out = form_obj_out || {};
        d_lang.mixin( form_obj_out, this._form.get( "value" ) );
        var query_obj = this._form_to_query( form_obj_out );
        return query_obj;
    },


    apply : function()
    {
        var form_obj = {};
        var query_obj = this.calcQuery( form_obj );

        console.log( module.id + ": apply " + this._name + ". form=", form_obj, "query=", query_obj );

        this._grid.selection.clear();
        this._grid.setQuery( query_obj, null );

        this._on_change();
    },


    _onSubmit : function( e )
    {
        e.preventDefault();

        if ( ! this._form.isValid() )  return;

        this.apply();

        this._dropdown.toggleDropDown();
    },


    _handleReset : function( e )
    {
        console.log( module.id + ": reset " + this._name + " to ", this._initial_form_value );

        this._form.set( "value", this._initial_form_value );
    }

} );


b_navigator_FilterOptions.formToQueryHelper = function( form_obj, names )
    {
        var ret = {};

        d_array.forEach( names, function( name ) {
            if ( typeof form_obj[name] === "string" ) {
                if ( form_obj[name] === "" )  return;
                ret[name] = form_obj[name];
                return;
            }

            // It's not a string, assume it's a number for now.
            if ( isNaN( form_obj[name] ) )  return;

            ret[name] = form_obj[name];
        } );

        return ret;
    };


return b_navigator_FilterOptions;

} );
