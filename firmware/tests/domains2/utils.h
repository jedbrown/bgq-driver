/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */


#ifndef _DOMAINS_UTILS_H
#define _DOMAINS_UTILS_H

#define MAX_CORES        17
#define THREADS_PER_CORE 4
#define MAX_THREADS      ( MAX_CORES * THREADS_PER_CORE )

#define PERS() ( FIRMWARE()->deprecated.personalityPtr )
#define IS_ROOT_NODE() ( ( PERS()->Network_Config.Acoord == 0 ) && ( PERS()->Network_Config.Bcoord == 0 ) && ( PERS()->Network_Config.Ccoord == 0 ) && ( PERS()->Network_Config.Dcoord == 0 ) && ( PERS()->Network_Config.Ecoord == 0 ) )


#define DEBUG(x)
#define TRACE(x) if ( IS_ROOT_NODE() ) printf x 
#define ERROR(x) printf x 

//#define TRACE(x) if (beQuiet==0) printf x
//#define TRACE(x) printf x

#define TRACE_ENTRY()  TRACE(("(>) %s        [%s:%d]\n", __func__, __FILE__, __LINE__));
#define TRACE_EXIT(rc) TRACE(("(<) %s rc=%d   [%s:%d]\n", __func__, (int)(rc), __FILE__, __LINE__));

extern void barrierThreads( volatile uint64_t* barrier, unsigned numberOfThreads );
extern volatile uint64_t domainIDs[];
extern int64_t testAndSet( volatile int64_t* atomic, int64_t preCondition, int64_t value );
extern void setFirmware(Firmware_Interface_t*);

typedef struct _InterDomainHello_t {
  uint64_t timestamp;
  uint64_t domainId; 
} InterDomainHello_t;

typedef union _InterDomainMessage_t {
  InterDomainHello_t hello;
  unsigned char      _buffer_[128];
} InterDomainMessage_t;

#endif
