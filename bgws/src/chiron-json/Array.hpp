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

#ifndef JSON_ARRAY_HPP_
#define JSON_ARRAY_HPP_


#include "fwd.hpp"

#include <vector>


namespace json {


/*! \brief Represents a JSON array. */
class Array : public std::vector<ValuePtr>
{
public:

    static ArrayValuePtr create( const Array& a = Array() );


    /*! \brief Add the value to the array. */
    void add( ValuePtr value_ptr )  { push_back( value_ptr ); }

    /*! \brief Add the converted value to the array. */
    template <class T> void add( const T& val ) {
        add( ValueCreator<T>::create( val ) );
    }

    /*! \brief Add the converted value to the array. */
    template <class T> void add( const T* val ) {
        add( ValueCreator<T*>::create( val ) );
    }

    /*! \brief Add an empty object and return it. */
    Object& addObject();

    /*! \brief Add an empty array and return it. */
    Array& addArray();
};


} // namespace json

#endif
