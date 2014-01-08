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
    "./AbstractTemplatedContainer",
    "dojo/_base/declare",
    "dojo/_base/lang",
    "dojo/text!./templates/Footer.html",
    "module",

    // Used only in template.
    "./Charts",
    "dijit/layout/ContentPane",
    "dijit/form/Button"
],
function(
        l_AbstractTemplatedContainer,
        d_declare,
        d_lang,
        template,
        module
    )
{


var b_navigator_dijit_Footer = d_declare(
        [ l_AbstractTemplatedContainer ],

{

    templateString : template,


    _dashboard_displayed : true,


    constructor : function()
    {
        this.doLayout = false;
    },


    // override
    startup : function()
    {
        this.inherited( arguments );

        this._toggleVisibilityButton.on( "click", d_lang.hitch( this, this._toggleChartVisibility ) );
        this._refreshButton.on( "click", d_lang.hitch( this, this._refreshClicked ) );
    },


    setChartsVisible : function( displayed )
    {
        this._dashboard_displayed = displayed;

        if ( this._dashboard_displayed ) {

            this._toggleVisibilityButton.set( "label", "Hide" );
            this.charts.show();

        } else {

            this._toggleVisibilityButton.set( "label", "Show Dashboard" );
            this.charts.hide();

        }


        // Force re-layout of root layout dijit.
        var dij = this;
        while ( dij.getParent() )  dij = dij.getParent();
        dij.layout();
    },


    onRefresh : function() {},


    _toggleChartVisibility : function()
    {
        console.log( module.id + ": _toggleChartVisibility! _dashboard_displayed=", this._dashboard_displayed );

        this.setChartsVisible( ! this._dashboard_displayed );
    },

    _refreshClicked : function()
    {
        this.onRefresh();
    }

} );

return b_navigator_dijit_Footer;

} );
