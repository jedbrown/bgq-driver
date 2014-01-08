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

#ifndef DB_DOMAININFO_H_
#define DB_DOMAININFO_H_

#include <sys/types.h>

namespace BGQDB {

/*!
 * \brief
 */

struct DomainInfo
{
    static const size_t ImageSize = 1025;
    static const size_t DomainOptionsSize = 513;
    static const size_t IdSize = 33;
    int startCore;
    int endCore;
    long long int startAddr;
    long long int endAddr;
    char imageSet[ImageSize];
    char options[DomainOptionsSize];
    char id[IdSize];
    long long int custAddr;
};

} // BGQDB

#endif
