/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (c) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/*                                                                  */
/* US Government Users Restricted Rights - Use, duplication or      */
/* disclosure restricted by GSA ADP Schedule Contract with IBM      */
/* Corporation.                                                     */
/*                                                                  */
/* This software is available to you under either the GNU General   */
/* Public License (GPL) version 2 or the Eclipse Public License     */
/* (EPL) at your discretion.                                        */
/*                                                                  */


#ifndef	_MUDM_CONNECTION_H_ /* Prevent multiple inclusion */
#define	_MUDM_CONNECTION_H_

int connect_reply(struct my_context * mcontext, MUHWI_PacketHeader_t * hdr, uint32_t bytes);
int connect_request(struct my_context * mcontext, MUHWI_PacketHeader_t * hdr, uint32_t bytes);
int disconnect_request(struct my_context * mcontext, MUHWI_PacketHeader_t * hdr, uint32_t bytes);

int32_t flush_iolink_connection_list(struct my_context * mcontext,struct mudm_connection ** conn_activelist);

#endif
