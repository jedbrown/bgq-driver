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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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
#include <vector>
#include <string>
#include "MMCSCommandReply.h"

#ifndef _DBSTATICS_H
#define _DBSTATICS_H

namespace DBStatics {
    enum Status { AVAILABLE, ERROR, MISSING };
    enum Type { ION, CN, IOCARD, COMPUTECARD, SERVICECARD };
    void setLocationStatus(std::vector<std::string>& locations, MMCSCommandReply& reply, Status status, Type type);
};

#endif
