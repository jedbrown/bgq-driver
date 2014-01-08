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
 * \file utility/include/Inotify.h
 */


#ifndef BGQ_UTILITY_INOTIFY_H_
#define BGQ_UTILITY_INOTIFY_H_


#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <string>
#include <vector>

#include <sys/inotify.h>


namespace bgq {
namespace utility {


/*! \brief An wrapper using asio around Linux's inotify API.
 *
 *  inotify: http://www.kernel.org/doc/man-pages/online/pages/man7/inotify.7.html
 *
 *  asio: http://www.boost.org/doc/libs/1_39_0/doc/html/boost_asio.html
 *
 *  Create your Inotify with your io_service,
 *  then call watch() with the path and mask / flags
 *  (keep a copy of the returned Watch otherwise it'll stop watching),
 *  then call async_read() and handle the events in your handler.
 */
class Inotify : public boost::asio::posix::stream_descriptor
{
private:
    class WatchImpl;

public:

    typedef boost::shared_ptr<WatchImpl> Watch; //!< Represents a watch on a path.
    typedef boost::weak_ptr<WatchImpl> WatchRef; //!< Weak reference to a Watch.


    /*! \brief inotify event, see the inotify man page for field descriptions. */
    struct Event {
        WatchRef watch_ref;
        uint32_t mask;
        uint32_t cookie;
        std::string name;
    };


    typedef std::vector<Event> Events; //!< Container for inotify events.

    typedef boost::function <void ( const boost::system::error_code& error )> ReadHandler; //!< Read callback.


    /*! \brief Format an inotify mask, useful for logging.
     */
    static std::string formatMask( uint32_t mask );


    /*! \brief Constructor
     */
    Inotify(
            boost::asio::io_service& io //!< [ref]
        );

    /*! \brief Start watching a file or directory.
     *
     *  Inotify will stop watching the path when the Watch is destroyed.
     *
     *  \pre The file or directory exists.
     *
     *  \throws boost::system::system_error if adding the watch failed.
     */
    Watch watch(
            const boost::filesystem::path& path, //!< [in] The file or directory
            uint32_t mask //!< [in] See inotify
        );

    /*! \brief Synchronous read of inotify events.
     */
    void read(
            Events& events_out, //!< [out] Will be filled in with Event s if no error.
            boost::system::error_code& err_out //!< [out] Filled in with error info.
        );

    /*! \brief Start asynchronous read for events.
     */
    void async_read(
            Events& events_out, //!< [ref] Will be filled in with Event s by the time the handler is called if no error.
            ReadHandler handler //!< [in] Called when the read completes or error.
        );


private:

    typedef std::vector< uint8_t > Buffer;

    typedef std::map< int, WatchRef > Watches;


    /*! \brief Ignore this internal class. */
    class WatchImpl {
    public:
        explicit WatchImpl(
                int id,
                uint32_t mask,
                Inotify& inotify
            );

        uint32_t getMask() const  { return _mask; }

        void invalidate()  { _id = -1; }

        ~WatchImpl();

    private:
        int _id;
        uint32_t _mask;
        Inotify& _inotify;
    };


    Watches _watches;


    void _readEvents( Events& events_out, boost::system::error_code& err_out );

    void _parseEvents( const Buffer& buf, Events& events_out );

    bool _parseEvent(
            const struct inotify_event* event_struct,
            Event& event_out,
            const struct inotify_event*& next_event_struct_out
        );

    void _readHandler(
            const boost::system::error_code& error,
            std::size_t bytes_transferred,
            Events& events_out,
            ReadHandler read_handler
        );

    void _endWatch( int id );
        // Called by WatchImpl's destructor.
};


} // namespace bgq::utility
} // namespace bgq


#endif
