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
#ifndef RUNJOB_SAMPLES_FIND_A_NODE_NODE_H
#define RUNJOB_SAMPLES_FIND_A_NODE_NODE_H

#include "Status.h"

#include <db/include/api/job/types.h>

#include <string>

#include <sys/types.h>

namespace find_a_node {

class Node
{
public:
    explicit Node(
            const std::string& location
            );

    Status status() const { return _status; }
    const std::string& location() const { return _location; }
    pid_t pid() const { return _pid; }

    void reset( Status status );
    void setPid( pid_t pid );

private:
    std::string _location;
    Status _status;
    pid_t _pid;
};

} // find_a_node

#endif
