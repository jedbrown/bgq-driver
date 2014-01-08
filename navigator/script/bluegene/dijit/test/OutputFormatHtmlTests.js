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
    "bluegene/dijit/OutputFormat",
    "dojo/dom-attr",
    "dojo/dom-construct",
    "dojo/string",
    "dijit/registry",
    "doh/runner"
],
function(
        b_dijit_OutputFormat,
        d_attr,
        d_construct,
        d_string,
        j_registry,
        doh
    )
{


var _ct = function( obj )
    {
        var of = new b_dijit_OutputFormat( obj );
        of.placeAt( "container1" );
        of.startup();
        return of;
    };


var tests = [


function defaults()
{
    var of = _ct( {} );

    doh.is( "", of.domNode.innerHTML, "innerHTML" );
    doh.is( null, of.get( "format" ), "format" );
    doh.is( null, of.get( "value" ), "value" );
},


function no_value_no_nullText()
{
    // If no value then just empty I guess.

    var of = _ct(
            {
                format: "Text '${abc}'"
            }
        );

    doh.is( "Text '${abc}'", of.get( "format" ), "format" );
    doh.is( null, of.get( "value" ), "value" );
    doh.is( "", of.domNode.innerHTML, "innerHTML" );
},


function nullText()
{
    // nullText gets displayed if the value is null, so if no value initially then that's what's displayed.
    var of = _ct(
            {
                format: "Text '${abc}'",
                nullText:  "Null Text"
            }
        );

    doh.is( "Null Text", of.get( "nullText" ), "nullText" );
    doh.is( "Null Text", of.domNode.innerHTML, "innerHTML" );
},


function simple_format_value()
{
    // If have construct with format and value that value is displayed.
    var of = _ct(
            {
                format: "Text '${abc}'",
                value: { abc: "def" }
            }
        );

    doh.is( { abc: "def" }, of.get( "value" ), "value" );
    doh.is( "Text 'def'", of.domNode.innerHTML, "innerHTML" );
},


function set_value()
{
    // Can set value and the displayed text is updated.

    var of = _ct(
            {
                format: "Text '${abc}'",
                nullText: "Loading..."
            }
        );

    doh.is( "Loading...", of.domNode.innerHTML, "innerHTML nullInitial" );
    of.set( "value", { abc : "pki" } );
    doh.is( "Text 'pki'", of.domNode.innerHTML, "innerHTML pki");
    of.set( "value", { abc : "pki2" } );
    doh.is( "Text 'pki2'", of.domNode.innerHTML, "innerHTML pki2");
    of.set( "value", null );
    doh.is( "Loading...", of.domNode.innerHTML, "innerHTML null2");
    of.set( "value", { abc : "pki3" } );
    doh.is( "Text 'pki3'", of.domNode.innerHTML, "innerHTML pki3");
},


function html_escaped()
{
    // if the format string has HTML it gets escaped in the output.

    var of = _ct(
            {
                format: "Text &${abc}&",
                value: { abc: "ghi" }
            }
        );

    doh.is( "Text &amp;ghi&amp;", of.domNode.innerHTML, "innerHTML" );
},


function no_srcNodeRef_element()
{
    // When create programmatically with no srcNodeRef, it creates a span.

    var ot = _ct( {} );

    doh.is( "SPAN", ot.domNode.tagName, "tagName" );
},


function defaults_declarative()
{
    // Declared with no properties, all the defaults.

    var of = j_registry.byId( "of1" );
    doh.is( null, of.get( "value" ), "value" );
    doh.is( "", of.domNode.innerHTML, "innerHTML" );
    doh.is( null, of.get( "nullText" ), "nullText" );
},

function as_span()
{
    // The domNode should be whatever the node is. If is span then it's a span.

    doh.is( "SPAN", j_registry.byId( "of1" ).domNode.tagName );
},

function as_li()
{
    // The domNode should be whatever the node is. If is <li> then it's a li.

    doh.is( "LI", j_registry.byId( "of2" ).domNode.tagName );
},

function create_with_generated_srcNode()
{
    // Can also construct element and pass as srcNode.

    var of = new b_dijit_OutputFormat(
            {
                format: "Text ${field1}",
                nullText: "Loading..."
            },
            d_construct.create( "td", {}, "t1tr" )
        );
    of.startup();

    doh.is( "TD", of.domNode.tagName );
},

function format_from_contents()
{
    // Set the contents of the element to the null text.

    var ot = j_registry.byId( "of3" );

    doh.is( "Formatted ${field1}", ot.get( "format" ), "format" );
    doh.is( "Loading...", ot.domNode.innerHTML, "innerHTML" );
    ot.set( "value", { field1: "Field1Value1" } );
    doh.is( "Formatted Field1Value1", ot.domNode.innerHTML, "innerHTML" );
},

function format_with_format_fn()
{
    // Format string allows a :formatter for a field.

    var ot = j_registry.byId( "of4" );

    doh.is( "10,000 of 12,345", ot.domNode.innerHTML );
},

function decl_style()
{
    // If declarative has style set then the style is on the domNode.

    var of = j_registry.byId( "of5" );

    doh.is( "font-size: 12pt; font-weight: bold;", d_string.trim( d_attr.get( of.domNode, "style" ) ), "initial" );
    of.set( "value", { blockType: "Midplane", blockOrLocation: "R00-M0" } );
    doh.is( "font-size: 12pt; font-weight: bold;", d_string.trim( d_attr.get( of.domNode, "style" ) ), "set1" );
    of.set( "value", null );
    doh.is( "font-size: 12pt; font-weight: bold;", d_string.trim( d_attr.get( of.domNode, "style" ) ), "set2" );
}


];

return tests;

} );
