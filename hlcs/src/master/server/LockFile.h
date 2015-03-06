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


#ifndef MASTER_LOCK_FILE_H_
#define MASTER_LOCK_FILE_H_


#include <fstream>
#include <string>
#include <stdexcept>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


class LockFile
{
public:
    const std::string _fname;
    char _pid[64];
    bool _fileExists;

    LockFile(
            const char* server_name
            ) : 
        _fname( std::string("/tmp/") + server_name + "-lock"),
        _fileExists(false)
    {
        struct stat lstat;
        if (!stat(_fname.c_str(), &lstat)) {
            _fileExists=true;
            std::ifstream in(_fname.c_str());
            in.getline(_pid, 64);
            return;
        }

        this->setpid();
    }

    void setpid() {
        std::ofstream out( _fname.c_str() );
        out << getpid() << "\n";
        if ( !out ) {
            throw std::runtime_error( std::string("Could not write pid to ") + _fname );
        }
    }

    ~LockFile() {
        if ( !_fileExists ) {
            ::remove(_fname.c_str());
        }
    }
};


#endif
