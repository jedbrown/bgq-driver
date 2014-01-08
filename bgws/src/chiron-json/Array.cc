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

#include "Array.hpp"

#include "functions.hpp"
#include "Object.hpp"
#include "Value.hpp"
#include "value_types.hpp"


namespace json {


ArrayValuePtr Array::create( const Array& a )
{
    return ArrayValuePtr( new ArrayValue( a ) );
}


Object& Array::addObject()
{
    ObjectValuePtr obj_val_ptr( Object::create() );
    add( ValuePtr( obj_val_ptr ) );
    return obj_val_ptr->get();
}


Array& Array::addArray()
{
    ArrayValuePtr arr_val_ptr( create() );
    add( ValuePtr( arr_val_ptr ) );
    return arr_val_ptr->get();
}


}
