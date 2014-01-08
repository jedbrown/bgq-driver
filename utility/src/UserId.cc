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

#include <UserId.h>

#include <Log.h>

#include <grp.h>
#include <pwd.h>
#include <unistd.h>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/scoped_array.hpp>

#include <cerrno>
#include <sstream>
#include <stdexcept>

LOG_DECLARE_FILE( "utility" );

namespace bgq {
namespace utility {

UserId::UserId(
        const uid_t uid
        ) :
    _name(),
    _uid(uid),
    _groups()
{
    // get username
    LOG_DEBUG_MSG("Getting username for uid " << _uid);
    const long usernameLength = sysconf(_SC_GETPW_R_SIZE_MAX);
    const boost::scoped_array<char> buf( new char[usernameLength] );
    struct passwd mypwent;
    memset(&mypwent, 0, sizeof(mypwent));
    struct passwd* mypwent_p = NULL;
    errno = 0;
    const int result = getpwuid_r(_uid, &mypwent, buf.get(), static_cast<size_t>(usernameLength), &mypwent_p);
    if ( !result && mypwent_p ) {
        _name = mypwent_p->pw_name;
        LOG_DEBUG_MSG("username " << _name);
    } else {
        char strbuf[1024];
        std::ostringstream errmsg;
        errmsg << "Failed to get user for uid " << uid << ": ";
        if (result == 0 || result == ENOENT || result == ESRCH || result == EBADF || result == EPERM) {
            errmsg << "user not found";
        } else {
            errmsg << "result=" << result << " (" << strerror_r(result, strbuf, sizeof(buf)) << ")";
        }
        LOG_WARN_MSG( errmsg.str() );
        throw std::runtime_error(errmsg.str());
    }

    // get secondary groups
    this->setGroupList(mypwent_p->pw_gid);
}

UserId::UserId(
        const std::string& user,
        const bool allowRemoteUser
        ) :
    _name(user),
    _uid(),
    _groups()
{
    // get uid
    LOG_DEBUG_MSG("Getting uid for username " << _name);
    long usernameLength = sysconf(_SC_GETPW_R_SIZE_MAX);
    boost::scoped_array<char> buf(new char[usernameLength]);
    struct passwd mypwent;
    memset(&mypwent, 0, sizeof(mypwent));
    struct passwd* mypwent_p = NULL;
    const int result = getpwnam_r(_name.c_str(), &mypwent, buf.get(), static_cast<size_t>(usernameLength), &mypwent_p);
    if ( !result && mypwent_p ) {
        _uid = mypwent_p->pw_uid;
        LOG_DEBUG_MSG("uid " << _uid);
    } else if (allowRemoteUser) {
        // If remote user indicator then only user field needs to be set
        return;
    } else {
        char strbuf[1024];
        std::ostringstream errmsg;
        errmsg << "Failed to get uid for user '" << _name << "', ";
        if (result == 0 || result == ENOENT || result == ESRCH || result == EBADF || result == EPERM) {
            errmsg << "user not found";
        } else {
            errmsg << "result=" << result << " (" << strerror_r(result, strbuf, sizeof(buf)) << ")";
        }
        LOG_WARN_MSG( errmsg.str() );
        throw std::runtime_error(errmsg.str());
    }

    // get secondary groups
    this->setGroupList(mypwent_p->pw_gid);
}

UserId::UserId(
        const std::vector<char>& buf
        )
{
    const std::string buf_str( buf.begin(), buf.end() );
    std::istringstream is( buf_str );
    try {
        boost::archive::text_iarchive ar( is );
        ar & *this;
    } catch ( const std::exception& e ) {
        LOG_ERROR_MSG( "could not deserialize buffer: " << e.what() );
        LOG_ERROR_MSG( "buffer: " << buf_str );
        throw;
    }
}

bool
UserId::isMember(
        const std::string& group
        ) const
{
    BOOST_FOREACH( const Group& g, _groups ) {
        LOG_DEBUG_MSG("Comparing " << group << " to " << g.second);
        if (g.second == group) {
            return true;
        }
    }
    LOG_DEBUG_MSG(_name << " is not a member of group " << group);

    return false;
}

void
UserId::setGroupList(
        const gid_t gid
        )
{
    LOG_DEBUG_MSG( "Getting secondary group list for '" << _name << "' gid '" << gid << "'" );
    // get supplementary group list
    boost::scoped_array<gid_t> grouplist;
    int group_count = 0;
    if (getgrouplist(_name.c_str(), gid, NULL, &group_count) < 0) {
        grouplist.reset(new gid_t[group_count]);
        getgrouplist(_name.c_str(), gid, grouplist.get(), &group_count);
    }
    LOG_TRACE_MSG("Secondary group list " << group_count);

    // add each group
    for ( int i = 0; i < group_count; ++i ) {
        // get gid
        const gid_t gid = grouplist[i];

        // allocate storage for group name
        long group_buffer_length = sysconf(_SC_GETPW_R_SIZE_MAX);
        LOG_TRACE_MSG( "Group buffer length " << group_buffer_length );
        boost::scoped_array<char> buf(new char[group_buffer_length]);

        // create variables for getgrgid_r
        struct group mygroup;
        memset( &mygroup, 0, sizeof(mygroup) );
        struct group* group_p = NULL;
        errno = 0;
        int rc = 0;

        // as long as rc == ERANGE, try to increase the buffer size
        while (
            ( rc = getgrgid_r(gid, &mygroup, buf.get(), static_cast<size_t>(group_buffer_length), &group_p) )
            ==
            ERANGE
            )
        {
            group_buffer_length *= 2;
            LOG_TRACE_MSG( "Increasing group buffer length to " << group_buffer_length );
            buf.reset( new char[group_buffer_length] );
        }

        if (rc == 0 && group_p) {
            // add to list
            BOOST_ASSERT( mygroup.gr_name );
            _groups.push_front(GroupList::value_type(gid, mygroup.gr_name));
            LOG_DEBUG_MSG("Added group " << mygroup.gr_name << " with " << gid);
        } else if ( !rc ) {
            LOG_DEBUG_MSG("Could not find group name for gid " << gid );
            // add entry with an empty group name
            _groups.push_front(
                    GroupList::value_type( gid, Group::second_type() )
                    );
        } else {
            LOG_WARN_MSG("Could not find group name for gid " << gid << ": " << strerror(rc));
        }
    }

    // ensure group list matches expected count
    if ( static_cast<int>(_groups.size()) != group_count ) {
        LOG_WARN_MSG( "Group list size (" << _groups.size() << ") does not equal expected size (" << group_count << ")" );
    }

    // primary gid needs to be at the front
    const GroupList::iterator primary = std::find_if(
            _groups.begin(),
            _groups.end(),
            boost::bind(
                std::equal_to<gid_t>(),
                gid,
                boost::bind(
                    &Group::first,
                    _1
                    )
                )
            );
    if ( primary == _groups.end() ) {
        LOG_WARN_MSG( "Could not find primary gid " << gid << " in secondary group list" );
    } else if ( primary != _groups.begin() ) {
        _groups.push_front( *primary );
        _groups.erase( primary );
        LOG_DEBUG_MSG( "moved gid " << gid << " (" << _groups.begin()->second << ") to front" );
    }
}

std::string
UserId::serialize()
{
    std::ostringstream os;
    boost::archive::text_oarchive ar(os);
    ar & *this;
    return os.str();
}

} // utility
} // bgq
