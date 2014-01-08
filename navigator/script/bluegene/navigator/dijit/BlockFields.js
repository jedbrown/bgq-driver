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
    "../format",
    "../../BlueGene",
    "dojo/dom-class",
    "dojo/number",
    "dojo/_base/declare",
    "dojo/text!./templates/BlockFields.html",
    "module",

    // Used in template.
    "../../dijit/Hideable",
    "../../dijit/OutputText",
    "dijit/layout/ContentPane"
],
function(
        l_AbstractTemplatedContainer,
        ll_format,
        b_BlueGene,
        d_class,
        d_number,
        d_declare,
        template,
        module
    )
{

var b_navigator_dijit_BlockFields = d_declare(
        [ l_AbstractTemplatedContainer ],

{

    templateString : template,


    _setValueAttr : function( block_obj )
    {
        console.log( module.id + ": displaying", block_obj );

        if ( ! block_obj )  return;

        var b = block_obj;

        var block_type_io = ("ioNodeCount" in b);

        this._id.set( "value", b.id );
        this._status.set( "value", b.status );
        this._statusChanged.set( "value", b.statusChangeTime );
        this._createdDij.set( "value", b.createTime );
        this._description.set( "value", b.description );
        this._owner.set( "value", b.owner === "" ? "''" : b.owner );

        if ( "user" in b ) {
            this._user.set( "value", b.user );
            d_class.toggle( this._user.domNode, "bgNotAvailable", false );
        } else {
            this._user.set( "value", null );
            d_class.toggle( this._user.domNode, "bgNotAvailable", true );
        }


        if ( "computeNodeCount" in b ) {
            this._size.set( "value", b.computeNodeCount );
            this._sizeItem.show();
        } else {
            this._sizeItem.hide();
        }

        if ( "shape" in b ) {
            this._shape.set( "value", b.shape );
            this._shapeItem.show();
        } else {
            this._shapeItem.hide();
        }

        if ( "torus" in b ) {
            this._torus.set( "value", b.torus );
            this._torusItem.show();
        } else {
            this._torusItem.hide();
        }


        if ( "ioNodeCount" in b ) {
            this._ioNodes.set( "value", b.ioNodeCount );
            this._ioNodesItem.show();
        } else {
            this._ioNodesItem.hide();
        }

        if ( "locations" in b ) {
            this._locations.set( "value", b.locations );
            this._locationsItem.show();
        } else {
            this._locationsItem.hide();
        }


        if ( "bootOptions" in b ) {
            this._bootOptionsItem.show();
            this._bootOptions.set( "value", b.bootOptions );
        } else {
            this._bootOptionsItem.hide();
        }

        if ( "errorMessage" in b ) {
            this._errorMessage.set( "value", b.errorMessage );
            this._errorMessageItem.show();
        } else {
            this._errorMessageItem.hide();
        }

        if ( ("microloaderImage" in b) && b.microloaderImage !== b_BlueGene.DEFAULT_MICROLOADER_IMAGE ) {
            this._microloaderImage.set( "value", b.microloaderImage );
            this._microloaderImageItem.show();
        } else {
            this._microloaderImageItem.hide();
        }

        if ( "options" in b ) {
            this._options.set( "value", b.options );
            this._optionsItem.show();
        } else {
            this._optionsItem.hide();
        }


        var DEFAULT_NODE_CONFIGURATION_NAME = (block_type_io ? b_BlueGene.DEFAULT_IO_NODE_CONFIGURATION_NAME : b_BlueGene.DEFAULT_CNK_NODE_CONFIGURATION_NAME);

        if ( ("nodeConfiguration" in b) && (b.nodeConfiguration !== DEFAULT_NODE_CONFIGURATION_NAME) ) {
            this._nodeConfiguration.set( "value", b.nodeConfiguration );
            this._nodeConfigurationItem.show();
        } else {
            this._nodeConfigurationItem.hide();
        }

    },
    
    
    _d_number : d_number,
    _ll_format : ll_format

} );

return b_navigator_dijit_BlockFields;

} );
