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
    "dojo/text!./templates/Environmentals.html",

    // Used only in template.

    "./environmentals/BulkPower",
    "./environmentals/Coolant",
    "./environmentals/Fan",
    "./environmentals/HealthCheck",
    "./environmentals/IoCard",
    "./environmentals/LinkChip",
    "./environmentals/Node",
    "./environmentals/NodeCard",
    "./environmentals/Optical",
    "./environmentals/ServiceCard",
    "./environmentals/ServiceCardTemp",

    "dijit/layout/TabContainer"
],
function(
        l_AbstractTemplatedContainer,
        d_declare,
        template
    )
{

var b_navigator_dijit_Environmentals = d_declare(
        [ l_AbstractTemplatedContainer ],

{

    templateString : template

} );

return b_navigator_dijit_Environmentals;

} );
