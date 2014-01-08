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

#ifndef JSON_OBJECT_HPP_
#define JSON_OBJECT_HPP_


/*! \file
 *  \brief class Object.
 */


#include "fwd.hpp"
#include "exception.hpp"

#include <boost/unordered_map.hpp>

#include <string>


namespace json {


/*!
 *  \brief Represents a JSON object.
 */
class Object : public boost::unordered_map<std::string,ValuePtr>
{
public:

    /*! \brief Create an ObjectValuePtr. */
    static ObjectValuePtr create( const Object& other = Object() );


    /*! \brief Set member <code>name</code> to the value. */
    void set( const std::string& name, ValuePtr value_ptr );

    template <class T> void set( const std::string& name, const T& val ) {
        set( name, ValueCreator<T>::create( val ) );
    }

    template <class T> void set( const std::string& name, const T* val ) {
        set( name, ValueCreator<T*>::create( val ) );
    }


    /*! \brief Set member <code>name</code> to a null. */
    void setNull( const std::string& name );

    /*! \brief Set member <code>name</code> to an empty object. */
    Object& createObject( const std::string& name );

    /*! \brief Set member <code>name</code> to an empty array. */
    Array& createArray( const std::string& name );

    /*! \brief Get the member <code>name</code>. */
    ValuePtr get( const std::string& name );

    /*! \brief Get the member <code>name</code>, const version. */
    const ValuePtr& get( const std::string& name ) const;

    /*! \brief Returns true if <code>name</code> is a member. */
    bool contains( const std::string& name ) const;

    /*! \brief Returns true if the <code>name</code> member is a string. */
    bool isString( const std::string& name ) const;

    /*! \brief Returns true if the <code>name</code> member is a number. */
    bool isNumber( const std::string& name ) const;

    /*! \brief Returns true if the <code>name</code> member is an object. */
    bool isObject( const std::string& name ) const;

    /*! \brief Returns true if the <code>name</code> member is an array. */
    bool isArray( const std::string& name ) const;

    /*! \brief Returns true if the <code>name</code> member is a Boolean. */
    bool isBool( const std::string& name ) const;

    /*! \brief Returns true if the <code>name</code> member is null. */
    bool isNull( const std::string& name ) const;


    /*! \brief Check if member is numeric type. */
    template <class T> bool is( const std::string& name ) const {
        ValuePtr val_ptr(get( name ));
        if ( ! val_ptr )  return false;
        return json::is<T>( *val_ptr );
    }


    /*! \brief Return the string member <code>name</code>.
     *
     *  \throw MemberNotFound if the object doesn't have a member with the name.
     *  \throw WrongType if the member isn't a String.
     */
    const std::string& getString( const std::string& name ) const;

    /*! \brief Return the number member <code>name</code>.
     *
     *  \throw MemberNotFound if the object doesn't have a member with the name.
     *  \throw WrongType if the member isn't a Number.
     */
    double getDouble( const std::string& name ) const;

    /*! \brief Return the object member <code>name</code>.
     *
     *  \throw MemberNotFound if the object doesn't have a member with the name.
     *  \throw WrongType if the member isn't a Object.
     */
    const Object& getObject( const std::string& name ) const;

    /*! \brief Return the object member <code>name</code>.
     *
     *  \throw MemberNotFound if the object doesn't have a member with the name.
     *  \throw WrongType if the member isn't a Object.
     */
    Object& getObject( const std::string& name );

    /*! \brief Return the array member <code>name</code>.
     *
     *  \throw MemberNotFound if the object doesn't have a member with the name.
     *  \throw WrongType if the member isn't a Array.
     */
    const Array& getArray( const std::string& name ) const;

    /*! \brief Return the array member <code>name</code>.
     *
     *  \throw MemberNotFound if the object doesn't have a member with the name.
     *  \throw WrongType if the member isn't a Array.
     */
    Array& getArray( const std::string& name );

    /*! \brief Return the Boolean member <code>name</code>.
     *
     *  \throw MemberNotFound if the object doesn't have a member with the name.
     *  \throw WrongType if the member isn't a Bool.
     */
    bool getBool( const std::string& name ) const;


    /*! \brief Get member as numeric type. */
    template <class T> T as( const std::string& name ) const {
        ValuePtr val_ptr(get( name ));
        if ( ! val_ptr )  BOOST_THROW_EXCEPTION( MemberNotFound( name ) );
        return json::as<T>( *val_ptr );
    }


private:

    const ValuePtr& _get( const std::string& name, bool throw_if_not_found ) const;
};


} // namespace json


#endif
