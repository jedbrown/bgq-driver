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


#ifndef BGWS_COMMON_REFRESH_BGWS_SERVER_CONFIGURATION_HPP_
#define BGWS_COMMON_REFRESH_BGWS_SERVER_CONFIGURATION_HPP_


#include "chiron-json/fwd.hpp"

#include <boost/filesystem.hpp>

#include <iosfwd>
#include <stdexcept>
#include <string>


namespace bgws {
namespace common {


class RefreshBgwsServerConfiguration
{
public:

    enum class Type {
        RereadCurrent,
        ReadDefault,
        ReadNew
    };


    class PathNotComplete : public std::invalid_argument
    {
        public:
            PathNotComplete( const boost::filesystem::path& path );
            const boost::filesystem::path& getPath() const  { return _path; }
            ~PathNotComplete() throw()  { /* Nothing to do */ }
        private:
            boost::filesystem::path _path;
    };

    class NotObjectError : public std::runtime_error
    {
        public:
            NotObjectError();
    };


    class NoOperationError : public std::runtime_error
    {
        public:
            NoOperationError();
    };


    class OperationNotStringError : public std::runtime_error
    {
        public:
            OperationNotStringError();
    };

    class UnexpectedOperationError : public std::runtime_error
    {
        public:
            UnexpectedOperationError( const std::string& operation );
            const std::string& getOperation() const  { return _operation; }
            ~UnexpectedOperationError() throw()  { /* Nothing to do */ }
        private:
            std::string _operation;
    };


    static const std::string OPERATION_NAME;

    static const RefreshBgwsServerConfiguration RereadCurrent;
    static const RefreshBgwsServerConfiguration ReadDefault;

    static RefreshBgwsServerConfiguration ReadNew( const boost::filesystem::path& path );


    RefreshBgwsServerConfiguration(
            const json::Value& json_value
        );


    Type getType() const  { return _type; }
    const boost::filesystem::path& getFilename() const;

    json::ValuePtr toJson() const;


private:

    RefreshBgwsServerConfiguration(
            Type type,
            const boost::filesystem::path& path = boost::filesystem::path()
        );


    Type _type;
    boost::filesystem::path _path;
};


std::ostream& operator<<( std::ostream& os, RefreshBgwsServerConfiguration::Type type );


} } // namespace bgws::common

#endif
