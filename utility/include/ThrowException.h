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
#ifndef BGQ_UTILITY_THROW_EXCEPTION_H
#define BGQ_UTILITY_THROW_EXCEPTION_H
/*!
 * \file utility/include/ThrowException.h
 * \brief UTILITY_THROW_EXCEPTION macro definition.
 */

#include <sstream>

/*!
 * \brief throw an exception of the given type with the msg as the what parameter.
 */
#define UTILITY_THROW_EXCEPTION(type, msg) { std::ostringstream os; os << msg; throw type( os.str() ); }

#endif
