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
 * \file PollFileSet.h
 */
#ifndef POLLFILESET_H
#define POLLFILESET_H

#include <poll.h>

namespace CxxSockets {

//! \brief Set of files to poll
class PollingFileSet : public FileSet {
protected:
    //! \brief vector of polling objects.  Vectors are
    //! guaranteed to be compatable with C-style arrays.
    std::vector<pollfd> _pollinfo;
    int _timeout;
    void pAddFile(FilePtr file, PollType p = RECV); 
public:
    PollingFileSet(int timeout = 0) : _timeout(timeout) { _pollinfo.clear(); }
    //! \brief Perform a poll() on the set
    //!
    //! \param timeout Amount of time to wait on the poll
    int Poll(unsigned int timeout);
    int Poll(unsigned int timeout, FileSetPtr fs);

    //!  \brief Add a file to the set.  
    void AddFile(FilePtr file, PollType p = RECV); 

    //!  \brief Add several files to the set
    void AddFiles(std::vector<FilePtr>& files);

    //!  \brief Remove a file
    void RemoveFile(FilePtr file);
};

}
#endif
