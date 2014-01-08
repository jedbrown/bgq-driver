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
#ifndef RUNJOB_COMMANDS_LOG_LEVEL_H
#define RUNJOB_COMMANDS_LOG_LEVEL_H
/*!
 * \file runjob/commands/LogLevel.h
 * \brief runjob::commands::request::LogLevel and runjob::commands::response::LogLevel definition and implementation.
 * \ingroup command_protocol
 */

#include <hlcs/include/runjob/commands/Request.h>
#include <hlcs/include/runjob/commands/Response.h>

#include <utility/include/LoggingProgramOptions.h>

#include <boost/serialization/vector.hpp>

namespace runjob {
namespace commands {

namespace request {

/*!
 * \brief Request message for log_level command.
 * \ingroup command_protocol
 */
class LogLevel : public Request
{
public:
    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<LogLevel> Ptr;

public:
    /*!
     * \brief ctor.
     */
    LogLevel() :
        Request(Message::Tag::LogLevel),
        _args()
    {

    }

    /*!
     * \brief Set args.
     */
    void setArgs(
            const bgq::utility::LoggingProgramOptions::Strings& args    //!< [in]
            )
    {
        _args = args;
    }

    /*!
     * \brief Get args.
     */
    const bgq::utility::LoggingProgramOptions::Strings& getArgs() const {
        return _args;
    }

    /*!
     * \copydoc runjob::commands::Message::serialize
     */
    void doSerialize(
            std::ostream& os
            )
    {
        boost::archive::text_oarchive ar(os);
        ar & *this;
    }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */)
    {
        ar & boost::serialization::base_object<Request>(*this);
        ar & _args;
    }

private:
    bgq::utility::LoggingProgramOptions::Strings _args;
};

} // request

namespace response {

/*!
 * \brief Response message for log_level command.
 * \ingroup command_protocol
 */
class LogLevel : public Response
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<LogLevel> Ptr;

    /*!
     * \brief Encapsulates a logger's name and level.
     */
    class Logger
    {
    public:
        std::string _name;  //!< logger name.
        std::string _level; //!< logger level.

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int /* version */)
        {
            ar & _name;
            ar & _level;
        }
    };

    /*!
     * \brief Connection container.
     */
    typedef std::vector<Logger> Loggers;

public:
    /*!
     * brief Ctor.
     */
    LogLevel() :
        Response(Message::Tag::LogLevel)
    {

    }

    /*!
     * \brief Add a connection to the container.
     */
    void addLogger(
            const Logger& logger    //!< [in]
            )
    {
        _loggers.push_back( logger );
    }

    /*!
     * \brief Get the list of loggers..
     */
    const Loggers& getLoggers() const { return _loggers; }

    /*!
     * \copydoc runjob::commands::Message::serialize
     */
    void doSerialize(
            std::ostream& os
            )
    {
        boost::archive::text_oarchive ar(os);
        ar & *this;
    }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */)
    {
        ar & boost::serialization::base_object<Response>(*this);
        ar & _loggers;
    }

private:
    Loggers _loggers;
};

} // response

} // commands
} // runjob

BOOST_CLASS_VERSION( runjob::commands::request::LogLevel, runjob::commands::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::LogLevel, runjob::commands::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::LogLevel::Logger, runjob::commands::ProtocolVersion )

#endif
