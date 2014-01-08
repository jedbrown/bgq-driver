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


#ifndef BGWS_BLUE_GENE_SERVICE_ACTIONS_ERRORS_HPP_
#define BGWS_BLUE_GENE_SERVICE_ACTIONS_ERRORS_HPP_


#include <stdexcept>
#include <string>


namespace bgws {
namespace blue_gene {
namespace service_actions {


class InvalidLocationError : public std::runtime_error
{
public:
    InvalidLocationError();
};


class HardwareDoesntExistError : public std::runtime_error
{
public:
    HardwareDoesntExistError();
};


class NoIdProvidedError : public std::runtime_error
{
public:
    NoIdProvidedError( const std::string& error_msg );
    const std::string& getErrorMessage() const  { return _error_msg; }

    ~NoIdProvidedError() throw()  { /* Nothing to do. */ }

private:
    std::string _error_msg;
};


    /*! Thrown when can't start service action because there's a conflict. */
class ConflictError : public std::runtime_error
{
public:
    ConflictError();
};


} } } // namespace bgws::blue_gene::service_actions


#endif
