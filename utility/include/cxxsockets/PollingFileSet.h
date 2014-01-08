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
 * \file utility/include/cxxsockets/PollingFileSet.h
 */
#ifndef CXXSOCKET_POLLING_FILESET_H
#define CXXSOCKET_POLLING_FILESET_H

#include <utility/include/cxxsockets/FileSet.h>
#include <utility/include/cxxsockets/types.h>

#include <poll.h>

namespace CxxSockets {

//! \brief Which side to poll, or poll for errors
enum PollType { RECV, SEND, ERROR };

//! \brief Set of files to poll
class PollingFileSet : public FileSet
{
protected:
    //! \brief vector of polling objects.  Vectors are
    //! guaranteed to be compatable with C-style arrays.
    std::vector<pollfd> _pollinfo;
    void pAddFile(FilePtr file, PollType p = RECV); 
public:
    PollingFileSet();

    //! \brief Perform a poll() on the set
    //!
    //! \param timeout Amount of time to wait on the poll
    int Poll(unsigned int timeout);

    //!  \brief Add a file to the set.  
    void AddFile(FilePtr file, PollType p = RECV); 
};

}
#endif
