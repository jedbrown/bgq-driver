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

#ifndef BGQDB_JOB_EXCEPTION_H_
#define BGQDB_JOB_EXCEPTION_H_

#include <stdexcept>

namespace BGQDB {
namespace job {

/*! \brief Exceptions thrown by job functions. */
namespace exception {

/*! \brief Thrown when try to insert a job with an invalid block ID. */
class BlockNotFound : public std::runtime_error
{
public:

    explicit BlockNotFound( const std::string& message );

};


/*! \brief Thrown when try to insert a job and the block is not Initialized. */
class BlockNotInitialized : public std::runtime_error
{
public:

    explicit BlockNotInitialized( const std::string& message );

};

/*! \brief Thrown when try to insert a job and the block is Initialized, but has a block action pending. */
class BlockActionNotEmpty : public std::runtime_error
{
public:

    explicit BlockActionNotEmpty( const std::string& message );

};

/*! \brief Thrown when try to insert a job another user already has a sub-node job running. */
class SubNodeJobUserConflict : public std::runtime_error
{
public:

    explicit SubNodeJobUserConflict( const std::string& message );

};


/*! \brief Thrown when try to update or remove a job that doesn't exist. */
class JobNotFound : public std::runtime_error
{
public:

    explicit JobNotFound( const std::string& message );

};


/*! \brief Thrown when try to update with an invalid status. */
class InvalidStatus: public std::runtime_error
{
public:

    explicit InvalidStatus( const std::string& message );

};


} } } // namespace BGQDB::job::exception


#endif
