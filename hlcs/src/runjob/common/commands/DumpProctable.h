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
#ifndef RUNJOB_COMMANDS_DUMP_PROCTABLE_STATUS_H
#define RUNJOB_COMMANDS_DUMP_PROCTABLE_STATUS_H
/*!
 * \file runjob/commands/DumpProctable.h
 * \brief runjob::commands::request::DumpProctable and runjob::commands::response::DumpProctable definition and implementation.
 * \ingroup command_protocol
 */

#include "common/tool/Proctable.h"

#include "common/Uci.h"

#include <db/include/api/job/types.h>

#include <hlcs/include/runjob/commands/Request.h>
#include <hlcs/include/runjob/commands/Response.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

#include <cstdint>
#include <map>
#include <string>
#include <vector>


namespace runjob {
namespace commands {
namespace request {

/*!
 * \brief Request message for job_status command.
 * \ingroup command_protocol
 */
class DumpProctable : public Request
{
public:
    /*!
     * \brief Protocol version.
     */
    static const unsigned ProtocolVersion = 1;

    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<DumpProctable> Ptr;

    /*!
     * \brief
     */
    typedef std::vector<unsigned> Ranks;

public:
    /*!
     * \brief ctor.
     */
    DumpProctable() :
        Request( Message::Tag::DumpProctable ),
        _job( 0 ),
        _ranks()
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

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */)
    {
        ar & boost::serialization::base_object<Request>(*this);
        ar & _job;
        ar & _ranks;
    }

public:
    BGQDB::job::Id _job;
    Ranks _ranks;
};

} // request

namespace response {

/*!
 * \brief Response message for job_status command.
 * \ingroup command_protocol
 *
 * A node's Universal Component Identifier (UCI) is used rather than location strings
 * to reduce memory usage for large job sizes.
 */
class DumpProctable : public Response
{
public:
    /*!
     * \brief Protocol version.
     */
    static const unsigned ProtocolVersion = 4;

    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<DumpProctable> Ptr;

    /*!
     * \brief I/O uci to IP address mapping.
     */
    typedef std::map<Uci, std::string> Io;

public:
    /*!
     * brief ctor
     */
    DumpProctable() :
        Response( Message::Tag::DumpProctable ),
        _block(),
        _corner(),
        _shape(),
        _np( 0 ),
        _ranksPerNode( 0 ),
        _mapping(),
        _io(),
        _proctable()
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

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */)
    {
        ar & boost::serialization::base_object<Response>(*this);
        ar & _block;
        ar & _corner;
        ar & _shape;
        ar & _np;
        ar & _ranksPerNode;
        ar & _mapping;
        ar & _io;
        ar & _proctable;
    }

public:
    std::string _block;
    std::string _corner;
    std::string _shape;
    unsigned _np;
    unsigned _ranksPerNode;
    std::string _mapping;
    Io _io;
    tool::Proctable _proctable;
};

} // response
} // commands
} // runjob

BOOST_CLASS_VERSION( runjob::commands::request::DumpProctable, runjob::commands::request::DumpProctable::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::DumpProctable, runjob::commands::response::DumpProctable::ProtocolVersion )

#endif
