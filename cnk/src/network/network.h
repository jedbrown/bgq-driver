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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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

#ifndef _CNK_NETWORK_H
#define _CNK_NETWORK_H

//! Format of a generated IPv4 address for torus network.

struct TorusIPAddress
{
   uint32_t octet0 : 8;      //!< Always 10 for the torus network.
   uint32_t type   : 2;      //!< Node type -- 0 for I/O node or 1 for compute node.
   uint32_t port   : 2;      //!< I/O link port -- 0 for port 10, 1 for port 6, or 2 for port 7.
   uint32_t aCoord : 4;      //!< A coordinate.
   uint32_t bCoord : 4;      //!< B coordinate.
   uint32_t cCoord : 4;      //!< C coordinate.
   uint32_t dCoord : 4;      //!< D coordinate.
   uint32_t eCoord : 4;      //!< E coordinate.
};

//! \brief  Initialize network interfaces on the node.
//! \return 0 when successful, errno when unsuccessful.

int network_init(void);

//! \brief  Poll for data on network interfaces on the node.
//! \return 0 when successful, errno when unsuccessful.

int network_poll(void);

//! \brief  Terminate network interfaces on the node.
//! \return 0 when successful, errno when unsuccessful.

int network_term(void);

#endif // _CNK_NETWORK_H

