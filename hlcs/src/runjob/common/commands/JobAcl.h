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
#ifndef RUNJOB_COMMANDS_JOB_ACL_H
#define RUNJOB_COMMANDS_JOB_ACL_H
/*!
 * \file runjob/commands/JobAcl.h
 * \brief runjob::commands::request::JobAcl and runjob::commands::response::JobAcl definition and implementation.
 * \ingroup command_protocol
 */

#include <db/include/api/job/types.h>

#include <hlcs/include/runjob/commands/Request.h>
#include <hlcs/include/runjob/commands/Response.h>

#include <hlcs/include/security/Action.h>
#include <hlcs/include/security/Authority.h>

#include <boost/serialization/vector.hpp>

#include <string>
#include <vector>

namespace runjob {
namespace commands {
namespace request {

/*!
 * \brief Request message for job_status command.
 * \ingroup command_protocol
 */
class JobAcl : public Request
{
public:
    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<JobAcl> Ptr;

    /*!
     * \brief mode types.
     */
    enum Mode {
        Grant,
        List,
        Revoke,
        Invalid,
    };
    
    /*!
     * \brief Covert an Mode type into a string.
     */
    static const std::string& toString(
            Mode a
            )
    {
        static const std::string strings[] = {
            "Grant",
            "List",
            "Revoke",
            "INVALID"
        };

        if ( a < Invalid ) {
            return strings[a];
        } else {
            return strings[Invalid];
        }
    }

public:
    /*!
     * \brief ctor.
     */
    JobAcl() :
        Request( Message::Tag::JobAcl ),
        _id( 0 ),
        _mode( List ),
        _user(),
        _action( hlcs::security::Action::Invalid )
    {

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

public:
    BGQDB::job::Id _id;                     //!<
    Mode _mode;                             //!<
    std::string _user;                      //!< 
    hlcs::security::Action::Type _action;   //!<

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */)
    {
        ar & boost::serialization::base_object<Request>(*this);
        ar & _id;
        ar & _mode;
        ar & _user;
        ar & _action;
    }
};

} // request

namespace response {

/*!
 * \brief Response message for job_status command.
 * \ingroup command_protocol
 */
class JobAcl : public Response
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<JobAcl> Ptr;

    /*!
     * \brief
     */
    class Authority
    {
    public:
        /*!
         * \brief ctor.
         */
        Authority() :
            _user(),
            _action( hlcs::security::Action::Invalid ),
            _source( hlcs::security::Authority::Source::Invalid )
        {

        }

        std::string _user;  //!<
        hlcs::security::Action::Type _action;   //!<
        hlcs::security::Authority::Source::Type _source;   //!<
    
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int /* version */)
        {
            ar & _user;
            ar & _action;
            ar & _source;
        }
    };

    /*!
     * \brief
     */
    typedef std::vector<Authority> Authorities;

public:
    /*!
     * brief ctor
     */
    JobAcl() :
        Response( Message::Tag::JobAcl ),
        _owner(),
        _authorities()
    {

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

public:
    std::string _owner;
    Authorities _authorities;

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */)
    {
        ar & boost::serialization::base_object<Response>(*this);
        ar & _owner;
        ar & _authorities;
    }
};

} // response

} // commands
} // runjob

BOOST_CLASS_VERSION( runjob::commands::request::JobAcl, runjob::commands::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::JobAcl, runjob::commands::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::JobAcl::Authority, runjob::commands::ProtocolVersion )

#endif
