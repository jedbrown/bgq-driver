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

#include "BlockHelper.h"
#include "BlockControllerBase.h"

BlockHelper::BlockHelper(BlockControllerBase* b) {
    BlockPtr p(b);
    _base = p;
    _base->setHelper(this);
}

BlockHelper::BlockHelper(BlockPtr b) {
    _base = b;
    _base->setHelper(this);
}

void BlockHelper::setBase(BlockPtr b) {
    _base->_machineXML = 0;
    _base = b;
}

PthreadMutex& BlockHelper::getMutex() { return getBase()->_mutex; }
