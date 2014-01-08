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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


class LockFile
{
public:
    std::ofstream *pout;
    std::string fname;
    char _pid[64];
    bool _fileExists;

    LockFile(const char* server_name):pout(NULL), _fileExists(false) {
        std::string sname(server_name);
        init(sname);
    }

    LockFile(std::string server_name):pout(NULL), _fileExists(false) {
        init(server_name);
    }

    void setpid() {
        pout = new std::ofstream(fname.c_str());
        *pout << getpid() << "\n";
        pout->flush();
        pout->close(); 
    }

    void init(std::string server_name) {
        fname += "/tmp/" + server_name + "-lock";
        struct stat lstat;
        if (!stat(fname.c_str(), &lstat)){
            _fileExists=true;
            std::ifstream in(fname.c_str());
            in.getline(_pid,64);
            return;
        }
        setpid();
    }

    ~LockFile() {
        if (pout!=0) {
            delete pout;
            ::remove(fname.c_str());
        }
    }
};


#endif
