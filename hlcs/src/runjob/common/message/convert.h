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
#ifndef RUNJOB_COMMON_MESSAGE_CONVERT_H
#define RUNJOB_COMMON_MESSAGE_CONVERT_H

#include "common/message/ExitJob.h"
#include "common/message/InsertJob.h"
#include "common/message/InsertRas.h"
#include "common/message/KillJob.h"
#include "common/message/Header.h"
#include "common/message/PerfCounters.h"
#include "common/message/Proctable.h"
#include "common/message/Result.h"
#include "common/message/StartJob.h"
#include "common/message/StartTool.h"
#include "common/message/StdIo.h"

#include "common/logging.h"

#include <boost/archive/binary_iarchive.hpp>

#include <boost/asio.hpp>
#include <boost/assert.hpp>
#include <boost/make_shared.hpp>

namespace runjob {
namespace message {

/*!
 ::Ptr \brief deserialize a message from a buffer into a abstract Message object.
 *
 ::Ptr \throws boost::archive::archive_exception
 */
inline Message::Ptr
convert(
        const Header& header,               //!< [in]
        boost::asio::streambuf& buffer      //!< [in]
       )
{
    Message::Ptr result;
    const size_t bufferSize( buffer.size() );
    std::istream is( &buffer );

    try {
        boost::archive::binary_iarchive ar(is);
        const Message::Type type = static_cast<Message::Type>(header._type);
        if (type == Message::InsertJob) {
            const InsertJob::Ptr derived(boost::make_shared<InsertJob>());
            ar & *derived;
            result = InsertJob::Ptr(derived);
        } else if (type == Message::InsertRas) {
            const InsertRas::Ptr derived(boost::make_shared<InsertRas>());
            ar & *derived;
            result = InsertRas::Ptr(derived);
        } else if (type == Message::ExitJob) {
            const ExitJob::Ptr derived(boost::make_shared<ExitJob>());
            ar & *derived;
            result = ExitJob::Ptr(derived);
        } else if (type == Message::KillJob) {
            const KillJob::Ptr derived(boost::make_shared<KillJob>());
            ar & *derived;
            result = KillJob::Ptr(derived);
        } else if (type == Message::PerfCounters) {
            const PerfCounters::Ptr derived(boost::make_shared<PerfCounters>());
            ar & *derived;
            result = PerfCounters::Ptr(derived);
        } else if (type == Message::Proctable) {
            const Proctable::Ptr derived(boost::make_shared<Proctable>());
            ar & *derived;
            result = Proctable::Ptr(derived);
        } else if (type == Message::Result) {
            const Result::Ptr derived(boost::make_shared<Result>());
            ar & *derived;
            result = Result::Ptr(derived);
        } else if (type == Message::StartJob) {
            const StartJob::Ptr derived(boost::make_shared<StartJob>());
            derived->setType( Message::StartJob );
            ar & *derived;
            result = StartJob::Ptr(derived);
        } else if (type == Message::StartTool) {
            const StartTool::Ptr derived(boost::make_shared<StartTool>());
            derived->setType( Message::StartTool );
            ar & *derived;
            result = StartTool::Ptr(derived);
        } else if (type == Message::StdError) {
            const StdIo::Ptr derived(boost::make_shared<StdIo>());
            derived->setType( Message::StdError );
            ar & *derived;
            result = StdIo::Ptr(derived);
        } else if (type == Message::StdIn) {
            const StdIo::Ptr derived(boost::make_shared<StdIo>());
            derived->setType( Message::StdIn );
            ar & *derived;
            result = StdIo::Ptr(derived);
        } else if (type == Message::StdOut) {
            const StdIo::Ptr derived(boost::make_shared<StdIo>());
            derived->setType( Message::StdOut );
            ar & *derived;
            result = StdIo::Ptr(derived);
        } else {
            BOOST_ASSERT(!"shouldn't get here");
        }
    } catch ( const std::exception& e ) {
        const log4cxx::LoggerPtr log_logger_ = runjob::getLogger();
        LOG_ERROR_MSG( "could not convert " << header._length << " bytes of " << Message::toString(static_cast<Message::Type>(header._type)) << " message: " << e.what() );
        LOG_ERROR_MSG( "buffer has " << buffer.size() << " bytes of original " << bufferSize << " remaining" );
        LOG_ERROR_MSG( "stream eof(" << is.eof() << ") fail(" << is.fail() << ") bad (" << is.bad() << ")" );

        result.reset();
    }

    return result;
}

} // message
} // runjob

#endif
