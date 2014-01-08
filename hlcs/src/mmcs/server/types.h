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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#ifndef MMCS_SERVER_TYPES_H_
#define MMCS_SERVER_TYPES_H_

#include "common/fwd.h"

#include <boost/shared_ptr.hpp>

namespace mmcs {
namespace server {

class BCIconInfo;
class BCLinkchipInfo;
class BCNodecardInfo;
class BCNodeInfo;
class BCTargetInfo;
class BlockControllerBase;
class CNBlockController;
class DBBlockController;
class IOBlockController;

typedef boost::shared_ptr<BlockControllerBase> BlockPtr;
typedef boost::shared_ptr<CNBlockController> CNBlockPtr;
typedef boost::shared_ptr<DBBlockController> DBBlockPtr;
typedef boost::shared_ptr<IOBlockController> IOBlockPtr;


class DBBlockController;

} } // namespace mmcs::server

#endif
