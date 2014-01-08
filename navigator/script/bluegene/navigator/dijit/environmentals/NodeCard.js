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
    "dojo/_base/declare",
    "dojo/text!./templates/NodeCard.html"
],
function(
        l_AbstractQueryResultsDijit,
        d_declare,
        template
    )
{

var b_navigator_dijit_environmentals_NodeCard = d_declare(
        [ l_AbstractQueryResultsDijit ],

{

    templateString : template,


    // override
    _getExtraFieldNames: function()
    {
        return [
                "v08",
                "v14",
                "v25",
                "v33",
                "v120p",
                "v15",
                "v09",
                "v10",
                "v33p",
                "v12a",
                "v12b",
                "v18",
                "v25p",
                "v12p",
                "v18p",
                "temp0",
                "temp1"
            ];
    }


} );

return b_navigator_dijit_environmentals_NodeCard;

} );
