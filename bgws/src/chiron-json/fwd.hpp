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

#ifndef JSON_FWD_HPP_
#define JSON_FWD_HPP_

/*! \file
 *  \brief Forward declarations.
 */


#include <boost/shared_ptr.hpp>


namespace json {


class Value;

/*! \brief Shared pointer to a value. */
typedef boost::shared_ptr<Value> ValuePtr;

/*! \brief Shared pointer to a value, const. */
typedef boost::shared_ptr<const Value> ConstValuePtr;


class Object;

class ObjectValue;

/*! \brief Shared pointer to an object value. */
typedef boost::shared_ptr<ObjectValue> ObjectValuePtr;

class Array;

class ArrayValue;

/*! \brief Shared pointer to an array value. */
typedef boost::shared_ptr<ArrayValue> ArrayValuePtr;


template <class T> class ValueCreator;


/*! \brief Checks if value can be converted to the type. */
template <class T> inline bool is( const Value& v );


/*! \brief Returns the number value converted to the type.
 *
 * \throw WrongType the value can't be converted to the type.
 */
template <class T> inline T as( const Value& v );


} // namespace json

#endif
