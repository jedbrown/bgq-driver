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

#ifndef CXXDB_TRANSACTION_H_
#define CXXDB_TRANSACTION_H_

#include "fwd.h"

namespace cxxdb {

/*! \brief A guard class for starting and ending a transaction on a Connection.
 *
 *  When a Transaction is created it calls startTransaction() on the
 *  connection and when it goes away it calls endTransaction().
 *
 */
class Transaction
{
public:

    Transaction(
            Connection& conn
        );

    ~Transaction();


private:

    Connection& _conn;
};

} // namespace cxxdb

#endif
