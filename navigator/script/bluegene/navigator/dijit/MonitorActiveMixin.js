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
    "dojo/aspect",
    "dojo/topic",
    "dojo/_base/declare",
    "dojo/_base/lang"
],
function(
        d_aspect,
        d_topic,
        d_declare,
        d_lang
    )
{


var b_navigator_dijit_MonitorActiveMixin = d_declare(
        [],
{
    __widgets : null,
    __all_active_count : 0,
    __active_count : 0,


    constructor : function()
    {
        this.__widgets = {};

        d_aspect.after( this, "startup", d_lang.hitch( this, this._startup ) );
    },


    _startup : function()
    {
        //console.log( module.id + ": _startup! id=", this.id );

        for ( var dij = this ; dij !== null ; dij = dij.getParent() ) {
            // If dij's parent is a Layout container where can select child, then monitor for changes in selected child.

            if ( ! dij.getParent() )  break; // No parent so not included in a stack-style dijit.

            if ( ! dij.getParent().selectChild )  continue; // Ignore this ancestor since parent isn't a stack-style dijit.

            var container_dij = dij.getParent();
            d_topic.subscribe( (container_dij.id + "-selectChild"), d_lang.hitch( this, this.__tabChanged, container_dij ) );

            // console.log( module.id + ": " + this.id + " subscribing to '" + (container_dij.id + "-selectChild") + "'" );

            var is_active = (container_dij.selectedChildWidget === dij);

            this.__widgets[container_dij.id] = { widget: dij, active: is_active };
            ++this.__all_active_count;

            if ( is_active ) {
                ++this.__active_count;
            }
        }

        //console.log( module.id + ": startup. " + this.id + " widgets=", this.__widgets, "all_active_count=", this.__all_active_count, "active_count=", this.__active_count );
    },


    // Call this to discover if is active or not.
    _isActive : function()  { return (this.__active_count === this.__all_active_count); },


    // Override this function to do something when becomes active, does nothing by default.
    _notifyActive : function() {},

    // Override this function to do something when becomes inactive, does nothing by default.
    _notifyInactive : function() {},


    __tabChanged : function( container_dij, widget )
    {
        if ( ! (container_dij.id in this.__widgets) ) {
            // console.log( module.id + ": " + this.id + " unexpected container dijit notification..." );
            return;
        }

        var originally_active = this._isActive();

        if ( this.__widgets[container_dij.id].widget === widget ) {
            if ( ! this.__widgets[container_dij.id].active )  ++this.__active_count;
            this.__widgets[container_dij.id].active = true;
        } else {
            if ( this.__widgets[container_dij.id].active )  --this.__active_count;
            this.__widgets[container_dij.id].active = false;
        }

        // console.log( module.id + ": " + this.id + " widget=", widget.id, "container_dij=", container_dij.id, " count=", this.__active_count, "/", this.__all_active_count );

        if ( ! this._isActive() ) {
            if ( originally_active ) {
                //console.log( module.id + ": " + this.id + " now INactive. count=", this.__active_count, "/", this.__all_active_count );
                this._notifyInactive();
            }
            return;
        }

        // console.log( module.id + ": " + this.id + " calling this._notifyActive." );

        //console.log( module.id + ": " + this.id + " now ACTIVE. count=", this.__active_count, "/", this.__all_active_count );
        this._notifyActive();
    }

} );

return b_navigator_dijit_MonitorActiveMixin;

} );