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
/*!
 * \file security/Object.h
 */

#ifndef BGQ_HLCS_SECURITY_OBJECT_H_
#define BGQ_HLCS_SECURITY_OBJECT_H_

#include <hlcs/include/security/Action.h>

#include <ostream>

namespace hlcs {
namespace security {

/*!
 * \brief an object to enforce a security policy on.
 */
class Object
{
public:
    /*!
     * \brief types of objects.
     */
    enum Type {
        Job,       //!< job object
        Block,     //!< block object
        Hardware,  //!< hardware object
        Invalid    //!< invalid object
    };

    /*!
     * \brief Validate an action.
     *
     * \throws std::logic_error if the action is not valid for this object.
     *
     * All actions on Block objects are supported.
     * Read and Execute are supported for Job and Hardware objects.
     * Any other combination is not valid.
     */
    void validate(
            Action::Type action
            ) const;

    /*!
     * \brief ctor.
     */
    Object(
            Type type,              //!< [in]
            const std::string& name //!< [in]
          );

    /*!
     * \brief Get object type.
     */
    Type type() const { return _type; }

    /*!
     * \brief Get object name.
     */
    const std::string& name() const { return _name; }

private:
    Type _type;
    std::string _name;
};

/*!
 * \brief Object insertion operator.
 */
std::ostream&
operator<<(std::ostream&, Object::Type);

/*!
 * \brief Object extraction operator.
 */
std::istream&
operator>>(std::istream&, Object::Type&);

} // security
} // hlcs

#endif
