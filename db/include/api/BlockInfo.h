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

#ifndef DB_BLOCKINFO_H_
#define DB_BLOCKINFO_H_

#include "DomainInfo.h"

#include <vector>

namespace BGQDB {

/*!
 * \brief
 */
struct BlockInfo
{
    static const size_t MicroLoaderSize = 257;
    static const size_t ComputeNodeSize = 1025;
    static const size_t OptionsSize     = 257;

    char uloaderImg[MicroLoaderSize];
    char nodeConfig[33];
    std::vector<DomainInfo> domains;
    char bootOptions[OptionsSize];
    char mode[2];
    char options[17];
    char status[2];
    int  cnodes;
    int  ionodes;
};

} // BGQDB

#endif
