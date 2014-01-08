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
    "bluegene/dijit/OutputText",
    "dojo/dom-attr",
    "dojo/dom-class",
    "dojo/dom-construct",
    "dojo/string",
    "dijit/registry",
    "doh/runner"
],
function(
        b_dijit_OutputText,
        d_attr,
        d_class,
        d_construct,
        d_string,
        j_registry,
        doh
    )
{

var _ct = function( args )
    {
        var ot = new b_dijit_OutputText( args );
        ot.placeAt( "container1" );
        ot.startup();
        return ot;
    };


var tests = [


function defaults()
{
    var ot = _ct( {} );

    doh.is( null, ot.get( "value" ), "value" );
    doh.is( "", ot.domNode.innerHTML, "innerHTML" );
    doh.is( true, ot.get( "escapeHtml" ), "escapeHtml" );
    doh.is( null, ot.get( "nullText" ), "nullText" );
    doh.is( false, ot.get( "nullTextHtml" ), "nullTextHtml");
},

function set_value_string_no_html()
{
    // set( "value", text ) sets the text for the element.

    var ot = _ct( {} );
    ot.set( "value", "Hello" );
    doh.is( "Hello", ot.domNode.innerHTML, "innerHTML" );
    doh.is( "Hello", ot.get( "value" ), "value" );
},

function set_value_string_html_esc()
{
    // Set value to a string with HTML chars. The HTML chars are escaped by default.

    var ot = _ct( {} );

    doh.is( true, ot.get( "escapeHtml" ), "escapeHtml" );
    ot.set( "value", "Hello & Goodbye" );
    doh.is( "Hello &amp; Goodbye", ot.domNode.innerHTML, "innerHTML" );
    doh.is( "Hello & Goodbye", ot.get( "value" ), "value" );
},

function set_value_multi()
{
    // Can set the value to one value and then set to another value and only the latest value is displayed.

    var ot = _ct( {} );
    ot.set( "value", "test1" );
    doh.is( "test1", ot.domNode.innerHTML, "innerHTML" );
    ot.get( "value", "test1" );

    ot.set( "value", "test2" );
    doh.is( "test2", ot.domNode.innerHTML, "innerHTML2" );
    ot.get( "value", "test2" );
},

function set_value_string_html_noesc()
{
    // Set value to a string with HTML chars on when escapeHTML is set to false. The HTML chars are not escaped.

    var ot = _ct( { escapeHtml: false } );

    doh.is( false, ot.get( "escapeHtml" ), "escapeHtml" );

    ot.set( "value", "Hello <br> Goodbye" );
    doh.is( "Hello <br> Goodbye", ot.domNode.innerHTML, "innerHTML" );
    doh.is( "Hello <br> Goodbye", ot.get( "value" ), "value" );
},

function null_text()
{
    // If has nullText and no value then that's the initial value.

    var ot = _ct( { nullText: 'NullText' } );

    doh.is( null, ot.get( "value" ), "value" );
    doh.t( ot.get( "nullText" ) === "NullText", "nullText" );
    doh.t( ot.domNode.innerHTML === "NullText", "innerHTML" );
},

function set_value_null_default_nullText()
{
    var ot = _ct( {} );

    ot.set( "value", "testText" );
    doh.is( "testText", ot.domNode.innerHTML, "innerHTML" );
    ot.set( "value", null );
    doh.is( "", ot.domNode.innerHTML, "innerHTML empty" );
},

function set_value_null_with_nullText()
{
    var ot = _ct( { nullText: 'Loading...' } );

    ot.set( "value", "testText" );
    doh.is( "testText", ot.domNode.innerHTML, "innerHTML" );
    ot.set( "value", null );
    doh.is( "Loading...", ot.domNode.innerHTML, "innerHTML nullText" );
},

function null_text_and_value()
{
    // If has nullText and value then displays the value.

    var ot = _ct( { nullText: 'NullText', value: 'testValue' } );
    doh.is( "testValue", ot.get( "value" ), "value" );
    doh.is( "NullText", ot.get( "nullText" ), "nullText" );
    doh.is( "testValue", ot.domNode.innerHTML, "innerHTML" );
},

function set_null_no_nullText()
{
    // Set value to null and no nullText then displays nothing.

    var ot = _ct( { value: 'testValue9' } );
    doh.is( "testValue9", ot.get( "value" ), "value" );
    doh.is( null, ot.get( "nullText" ), "nullText" );
    ot.set( "value", null );
    doh.is( "", ot.domNode.innerHTML, "innerHTML" );
},

function set_value_number()
{
    // set value to a number. The displayed value is the number as text.

    var ot = _ct( {} );
    ot.set( "value", 12345 );
    doh.is( "12345", ot.domNode.innerHTML );
    ot.get( "value", 12345 );
},

function value_array()
{
    // Set value to an array with default formatter gets formatted with ,s between the elements.

    var ot = _ct( {} );
    ot.set( "value", [ 'R00-M0', 'R00-M1' ] );
    doh.is( "R00-M0, R00-M1", ot.domNode.innerHTML, "innerHTML" );
    doh.is( 2, ot.get( "value" ).length, "value" );
},

function formatter_string()
{
    // Set a formatter and set a value displays formatted value.

    var f = function( value )
        {
            if ( value === 'A' )  return "Allocated";
            return "unexpected";
        };

    var ot = _ct( { formatter: f } );
    ot.set( "value", 'A' );
    doh.is( "Allocated", ot.domNode.innerHTML, "innerHTML" );
    ot.get( "value", 'A' );
},

function no_escape_with_formatter()
{
    // If escapeHtml false and have formatter then can set value to HTML.

    var f = function( value )
        {
            if ( value === null )  return "";
            return "<span style='font-weight: bold;'>" + value.text + "</span>";
        };

    var ot = _ct( { escapeHtml: false, formatter: f } );
    doh.is( false, ot.get( "escapeHtml" ), "escapeHtml" );
    ot.set( "value", { text: "Here's text" } );
    doh.is( ("<span style=\"font-weight: bold;\">Here's text</span>"), ot.domNode.innerHTML, "innerHTML" );
},

function formatter_no_nullText()
{
    // If nullText is not set then the formatter gets called with null to get the null text.

    var f = function( value )
        {
            if ( value === null )  return "Null Text";
            return (value + "text");
        };

    var ot = _ct( { formatter: f } );
    doh.is( "Null Text", ot.domNode.innerHTML );
},

function formatter_nullText_simple()
{
    // If nullText is set then the formatter does not get called with null to get the null text.

    var f = function( value )
        {
            if ( value === null )  return "Formatted Null Text";
            return (value + "text");
        };

    var ot = _ct( { formatter: f, nullText: "Null Text Property Value" } );
    doh.is( "Null Text Property Value", ot.domNode.innerHTML );
},

function formatter_nullText_multi()
{
    // Same as previous but set the value to null.

    var f = function( value )
    {
        if ( value === null )  return "Formatted Null Text";
        return (value + "text");
    };

    var ot = _ct( { formatter: f, nullText: "Null Text Property Value", value: "Initial Value" } );
    ot.set( "value", null );
    doh.is( "Null Text Property Value", ot.domNode.innerHTML );
},


function classFn_simple()
{
    // Can set the classFn property to something and then when set value it gets called.

    var _fn1 = function( value )
        {
            if ( value === null )  return "";
            return "class1";
        };

    var ot = _ct( { classFn: _fn1 } );

    ot.set( "value", "test1" );
    doh.t( d_class.contains( ot.domNode, "class1" ) );
},

function classFn_multi()
{
    // If set the classFn can set value multiple times and the old classes are removed and new classes added.

    var _fn1 = function( value )
        {
            if ( value === 'E' )  return "bgHardwareError";
            return "";
        };

    var ot = _ct( { classFn: _fn1 } );

    doh.f( d_class.contains( ot.domNode, "bgHardwareError" ), "not contain class 1" );
    ot.set( "value", "E" );
    doh.t( d_class.contains( ot.domNode, "bgHardwareError" ), "contain class 2" );
    ot.set( "value", "A" );
    doh.f( d_class.contains( ot.domNode, "bgHardwareError" ), "not contain class 3" );
},

function classFn_called_with_null()
{
    // If have a class function it will get called with null if there's no initial value.

    var class_fn = function( value )
        {
            if ( value === null )  return "nullClass";
            return ("class" + value);
        };

    var ot = _ct( { classFn: class_fn } );

    doh.t( d_class.contains( ot.domNode, "nullClass" ), "class not set, class_fn wasn't called with null!" );
},


function nullText_html()
{
    // Can set nullTextHtml to true to cause null text to not be escaped.
    var ot = _ct( { nullText: "<span style='font-weight: bold;'>Loading...</span>", nullTextHtml: true } );

    doh.t( ot.get( "nullTextHtml" ), "nullTextHtml" );
    doh.is( "<span style=\"font-weight: bold;\">Loading...</span>", ot.domNode.innerHTML, "innerHTML" );
},

function value_classFn()
{
    // If initialize with both value and classFn the classFn is applied.

    var class_fn = function( value ) { return (value + "Class"); };

    var ot = _ct( { classFn: class_fn, value: "A" } );
    doh.t( d_class.contains( ot.domNode, "AClass" ), "contains AClass" );
},


function no_srcNodeRef_element()
{
    // When create programmatically with no srcNodeRef, it creates a span.

    var ot = _ct( { value: "Test value" } );

    doh.is( "SPAN", ot.domNode.tagName, "tagName" );
},

function defaults_declarative()
{
    // Declared with no properties, all the defaults.

    var ot = j_registry.byId( "ot1" );
    doh.is( null, ot.get( "value" ), "value" );
    doh.is( "", ot.domNode.innerHTML, "innerHTML" );
    doh.is( true, ot.get( "escapeHtml" ), "escapeHtml" );
    doh.is( null, ot.get( "nullText" ), "nullText" );
    doh.is( false, ot.get( "nullTextHtml" ), "nullTextHtml");
},

function as_span()
{
    // The domNode should be whatever the node is. If is span then it's a span.

    doh.is( "SPAN", j_registry.byId( "ot1" ).domNode.tagName);
},

function as_li()
{
    // The domNode should be whatever the node is. If is <li> then it's a li.

    doh.is( "LI", j_registry.byId( "ot2" ).domNode.tagName );
},


function create_with_generated_srcNode()
{
    // Can also construct element and pass as srcNode.

    var ot = new b_dijit_OutputText(
            { nullText: "Loading..." },
            d_construct.create( "td", {}, "t1tr" )
        );
    ot.startup();

    doh.is( "TD", ot.domNode.tagName );
},

function create_with_id_on_dijit()
{
    // Try setting id on the dijit rather than the srcNode.


    (new b_dijit_OutputText(
            {
                id: "testOTNumber1",
                nullText: "Loading..."
            },
            d_construct.create( "td", {}, "t1tr" )
        )).startup();

    doh.is( "TD", j_registry.byId( "testOTNumber1" ).domNode.tagName );
},

function null_text_from_contents()
{
    // Set the contents of the element to the null text.

    var ot = j_registry.byId( "ot3" );

    doh.is( "Loading...", ot.domNode.innerHTML, "innerHTML" );
    doh.is( "Loading...", ot.get( "nullText" ), "nullText" );
    doh.t( ot.get( "nullTextHtml" ), "nullTextHtml" );
},

function null_text_and_contents()
{
    // If nullText is set then the element's contents are not used.

    var ot = j_registry.byId( "ot4" );

    doh.is( "nullTextValue", ot.domNode.innerHTML, "innerHTML" );
    doh.is( "nullTextValue", ot.get( "nullText" ), "nullText" );
    doh.f( ot.get( "nullTextHtml" ), "nullTextHtml" );
},

function decl_element_empty_formatter()
{
    // If the element is empty the formatter is called with null to get the null text.

    doh.is( "FormattedNullValue", j_registry.byId( "ot5" ).domNode.innerHTML );
},

function decl_style()
{
    // If style set on element doesn't get changed when put dijit on it, nor when set value.

    var ot = j_registry.byId( "ot6" );

    doh.is( "font-weight: bold;", d_string.trim( d_attr.get( ot.domNode, "style" ) ), "initial" );
    ot.set( "value", "BGQSN1" );
    doh.is( "font-weight: bold;", d_string.trim( d_attr.get( ot.domNode, "style" ) ), "set1" );
    ot.set( "value", null );
    doh.is( "font-weight: bold;", d_string.trim( d_attr.get( ot.domNode, "style" ) ), "set2" );
}


];

return tests;

} );
