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

#ifndef CXXDB_PARAMETERS_H_
#define CXXDB_PARAMETERS_H_

#include "fwd.h"
#include "Parameter.h"

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <map>
#include <string>
#include <vector>

namespace cxxdb {

class Parameter;
class StatementHandle;


/*! \brief Parameters in a Statement.
 *
 *  \section Usage
 *
 *  Any Parameter can be referenced by index or name.
 *  Indexes start at 1.
 *
 *  stmt_ptr->parameters()[1].set( value );
 *
 *  or
 *
 *  stmt_ptr->parameters()["id"].cast( myid );
 *
 *
 *  \section naming Naming parameters
 *
 *  A name can be assigned to a parameter
 *  or to a set of parameters.
 *  If a name isn't assigned to a parameter,
 *  the parameter can be accessed by index.
 *
 *  Give a name to a parameter by passing the
 *  name in to the Statement::prepare() method.
 *  The first name is assigned to the first index
 *  and so on.
 *  If there are fewer names then parameters,
 *  the remaining parameters are left unnamed.
 *  An empty name leaves the parameter unnamed.
 *
 *  If two parameters are given the same name,
 *  then assigning a value to the name assigns the same
 *  value to both parameters.
 *  Also, if two parameters have the same name
 *  then the index cannot be used for either parameter.
 *
 */
class Parameters : boost::noncopyable
{
public:

    /*! \brief Constructor. */
    Parameters(
            StatementHandle& stmt_handle, //!< [ref]
            const ParameterNames& parameter_names //!< \sa \ref naming
        );

    /*! \brief Access the parameter by index.
     *
     *  Note that the first parameter is 1 and not 0.
     *
     *  \throws std::logic_error The index is out of range.
     */
    Parameter& operator[]( unsigned param_index );

    /*! \brief Access the parameter by name.
     *
     *  \throws std::logic_error The name is not known.
     */
    Parameter& operator[]( const std::string& name );


private:

    typedef std::map<std::string,Parameter::Ptr> _NameToParameter;
    typedef std::vector<Parameter::Ptr> _Parameter_ptrs;


    _NameToParameter _name_to_parameter;
    _Parameter_ptrs _param_ptrs;
};

} // namespace cxxdb


#endif
