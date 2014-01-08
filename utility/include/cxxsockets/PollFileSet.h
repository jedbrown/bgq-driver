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
#ifndef _POLLFILESET_H
#define _POLLFILESET_H

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
        void AddFiles(std::vector<FilePtr>& files, PollType p = RECV);

        //!  \brief Remove a file
        void RemoveFile(FilePtr file);

        //! \brief Plain poll operations. 
        //!
        //! If the vector you pass in is empty, all files
        //! in the set are polled.  If not, then the files passed in are polled
        //! =provided= they are part of this file set.
        //! Note well:  You can use these followed by operations directly on the
        //! files, but you risk a race condition between the poll and the following
        //! operation during which time there is no lock on the file.
        //! Better to use the polled operations on the set defined in subclasses of this.
        //! Protected Polls lock all files and the set lock
        int ProtectedPoll(FileSetPtr fs);
        int ProtectedPoll();

        //! Unprotected polls lock the set lock, but not all of the internal files
        int UnprotectedPoll(FileSetPtr fs);
        int UnprotectedPoll();
    };
};
#endif
