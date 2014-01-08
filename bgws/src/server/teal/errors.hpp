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
/* (C) Copyright IBM Corp.  2012, 2011                              */
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

#ifndef BGWS_TEAL_ERRORS_HPP_
#define BGWS_TEAL_ERRORS_HPP_


#include <stdexcept>
#include <string>


namespace bgws {
namespace teal {
namespace errors {


/*! \brief Thrown if an alert with the ID was not found. */
class NotFound : public std::runtime_error
{
public:
    NotFound( const std::string& output );

    /*! \brief The output of the program */
    const std::string& getOutput() const  { return _output; }

    ~NotFound() throw() { /* Nothing to do */ }

private:
    std::string _output;
};


/*! \brief Thrown if the alert is not in the correct state. */
class InvalidState : public std::runtime_error
{
public:
    InvalidState( const std::string& output );

    /*! \brief The output of the program */
    const std::string& getOutput() const  { return _output; }

    ~InvalidState() throw() { /* Nothing to do */ }

private:
    std::string _output;
};


/*! \brief Thrown if the alert is a duplicate so the operation is not valid */
class Duplicate : public std::runtime_error
{
public:
    Duplicate( const std::string& output );

    /*! \brief The output of the program */
    const std::string& getOutput() const  { return _output; }

    ~Duplicate() throw() { /* Nothing to do */ }

private:
    std::string _output;
};


} } } // namespace bgws::teal::errors


#endif
