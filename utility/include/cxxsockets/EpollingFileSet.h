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
#ifndef _EPOLLINGFILESET_H
#define _EPOLLINGFILESET_H

#include <sys/epoll.h>

namespace CxxSockets {

    class EpollingFileSet : public FileSet {
        friend class EpollingTCPSocketSet;
    protected:
        int _epfd;
        int _timeout;
        std::vector<struct epoll_event> _events;
        virtual void pAddFile(FilePtr file, PollType p = RECV);
    private:
        int pEpoll(unsigned int timeout);
    public:
        EpollingFileSet(int count = 25) {  _epfd = epoll_create(count); }
        EpollingFileSet(std::vector<FilePtr>& files) {  
            _epfd = epoll_create(files.size()); 
            AddFiles(files);
        }

        ~EpollingFileSet() { ::close(_epfd); }
        //! \brief Perform a poll on the file
        int Epoll(unsigned int timeout);
        int Epoll(unsigned int timeout, FileSetPtr fs);

        //! \brief Add a file to the set
        //! 
        //! \param file File to add
        //! \param p
        virtual void AddFile(FilePtr file, PollType p = RECV);

        //! \brief Add several files to the set        
        //! 
        //! \param files vector of files to add
        //! \param p
        void AddFiles(std::vector<FilePtr>& files, PollType p = RECV);

        //! \brief Remove a file from the set
        //! 
        //! \param file File to remove from the set
        virtual void RemoveFile(FilePtr file);

        //! \brief Plain poll operations. 
        //!
        //! Note well:  You can use these followed by operations directly on the
        //! files, but you risk a race condition between the poll and the following
        //! operation during which time there is no lock on the file.
        int ProtectedPoll();
        int UnprotectedPoll();
    };
}
#endif
