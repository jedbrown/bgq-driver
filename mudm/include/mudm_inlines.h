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

//
// Inline functions for MU Data Mover
//
#ifndef	_MUDM_INLINES_H_ /* Prevent multiple inclusion */
#define	_MUDM_INLINES_H_

#define MUDM_IS_IONODE 0x80000000
#define MUDM_IS_CN     0x00000000
__INLINE__ 
uint32_t mudm_dest32 (        uint32_t             A,
			       uint32_t             B,
			       uint32_t             C,
			       uint32_t             D,
			       uint32_t             E,
                               uint32_t             ionode_indicator
			     )
{
  return ( ( (A & 0x3F) << 24) | ( (B & 0x3F) << 18) | ( (C & 0x3F) << 12) | ( (D & 0x3F) << 6) | (E & 0x3F) | ionode_indicator);
}

     
#endif /* _MUDM_INLINES_H_ */
