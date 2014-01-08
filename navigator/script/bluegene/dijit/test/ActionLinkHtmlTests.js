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
    "bluegene/dijit/ActionLink",
    "dojo/dom-attr",
    "dojo/dom-class",
    "dojo/dom-construct",
    "dojo/on",
    "dojo/string",
    "dijit/registry",
    "doh/runner"
],
function(
        b_dijit_ActionLink,
        d_attr,
        d_class,
        d_construct,
        d_on,
        d_string,
        j_registry,
        doh
    )
{


var createActionLink = function( args )
    {
        var al = new b_dijit_ActionLink( args );
        al.placeAt( "container1" );
        al.startup();

        return al;
    };


var createAndClick = function( props )
    {
        var click_handled = false;

        var onclick_handler = function()
            {
                click_handled = true;
            };

        var al = createActionLink( props );
        al.on( "click", onclick_handler );

        d_on.emit( al.domNode, "click", { bubbles: true, cancelable: true } );

        return click_handled;
    };


var tests = [


function defaults()
{
    // Can create an ActionLink with no attrs and get default attrs.

    var al = new b_dijit_ActionLink( {} );
    al.placeAt( "container1" );
    al.startup();

    doh.is( "", al.get( "label" ), "label" );
    doh.is( false, al.get( "disabled" ), "disabled" );
    doh.is( "", al.domNode.innerHTML, "innerHTML" );
    doh.t( d_class.contains( al.domNode, "bgActionLink" ), "class" );
},


function no_srcNodeRef_element()
{
    // When create programmatically with no srcNodeRef, it creates a span.

    var al = createActionLink( {} );

    doh.is( "SPAN", al.domNode.tagName, "tagName" );
},


function initialize_label()
{
    // Can initialize with label property. The label is the contents.

    var al = createActionLink( { label: "Click Me!" } );

    doh.is( "Click Me!", al.get( "label" ), "label" );
    doh.is( "Click Me!", al.domNode.innerHTML, "innerHTML" );
},


function initialize_disabled()
{
    // Can initialize with disabled property = true. Then bgActionLink is not set.
    var al = createActionLink( { disabled: true } );

    doh.t( al.get( "disabled" ), "disabled" );
    doh.f( d_class.contains( al.domNode, "bgActionLink" ), "class" );
},


function change_label()
{
    // Can change the label property.
    var al = createActionLink( { label: "Loading...", disabled: true } );

    al.set( "label", "success" );
    doh.is( "success", al.domNode.innerHTML, "set label 1" );

    al.set( "label", "Loading..." );
    doh.is( "Loading...", al.domNode.innerHTML, "set label 2" );
},


function change_disabled()
{
    // Can change the disabled property and goes from clickable to not clickable.
    var al = createActionLink( {} );

    al.set( "disabled", true );
    doh.t( al.get( "disabled" ), "disabled (disabled)" );
    doh.f( d_class.contains( al.domNode, "bgActionLink" ), "class (disabled)" );

    al.set( "disabled", false );
    doh.f( al.get( "disabled" ), "disabled (enabled)" );
    doh.t( d_class.contains( al.domNode, "bgActionLink" ), "class (enabled)" );
},


function click_not_disabled()
{
    // Can click ActionLink and if not disabled calls handler.

    doh.t( createAndClick( {} ), "click handled" );
},


function click_disabled()
{
    // Can click ActionLink and if is disabled then does not call handler.

    doh.f( createAndClick( { disabled: true } ), "click handled (disabled)" );
},


function decl_label_property()
{
    // Can create declaratively with label.

    var al = j_registry.byId( "al1" );

    doh.is( "Test1", al.get( "label" ), "label" );
    doh.is( "Test1", al.domNode.innerHTML, "innerHTML" );
},

function decl_label_contents()
{
    // Can create declaratively, get label from element if not in props.

    var al = j_registry.byId( "al2" );

    doh.is( "Test2", al.get( "label" ), "label" );
    doh.is( "Test2", al.domNode.innerHTML, "innerHTML" );
},

function decl_span()
{
    // Can create declaratively on a span element and domNode is a span.

    var al = j_registry.byId( "al1" );

    doh.is( "SPAN", al.domNode.tagName, "tagName" );
},

function decl_li()
{
    // Can create declaratively on a li element and domNode is a li.

    var al = j_registry.byId( "al3" );

    doh.is( "LI", al.domNode.tagName, "tagName" );
},

function set_style()
{
    var al = new b_dijit_ActionLink(
            { label: ("" + 5), style: "text-align: right;" },
            d_construct.create( "td", {}, "tr1" )
        );
    al.startup();

    doh.is( "text-align: right;", d_string.trim( d_attr.get( al.domNode, "style" ) ) );
    doh.is( "TD", al.domNode.tagName, "tagName" );
},

function set_class()
{
    // Can set the class attribute.

    var al = createActionLink( {} );

    al.set( "class", "bgHardwareNotAvailable" );
    doh.t( d_class.contains( al.domNode, "bgActionLink" ), "setClass1 bgActionLink" );
    doh.t( d_class.contains( al.domNode, "bgHardwareNotAvailable" ), "setClass1 bgHardwareNotAvailable" );

    al.set( "class", "" );
    doh.t( d_class.contains( al.domNode, "bgActionLink" ), "setClass2 bgActionLink" );
    doh.f( d_class.contains( al.domNode, "bgHardwareNotAvailable" ), "setClass2 bgHardwareNotAvailable" );
}

];

return tests;

} );
