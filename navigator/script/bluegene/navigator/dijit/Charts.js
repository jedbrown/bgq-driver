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
    "./HideableMixin",
    "dojo/_base/declare",
    "dijit/_WidgetBase",
    "dijit/_TemplatedMixin",
    "dijit/_WidgetsInTemplateMixin",
    "dojo/text!./templates/Charts.html",

    // Used in template
    "./JobsChart",
    "./RasChart",
    "./UtilizationChart"
],
function(
        l_HideableMixin,
        d_declare,
        j__WidgetBase,
        j__TemplatedMixin,
        j__WidgetsInTemplateMixin,
        template
    )
{

var b_navigator_dijit_Charts = d_declare(
        [ j__WidgetBase, j__TemplatedMixin, j__WidgetsInTemplateMixin, l_HideableMixin ],

{

    templateString : template

} );

return b_navigator_dijit_Charts;

} );
