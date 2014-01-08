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

typedef struct {
  uint64_t dcrAddress;
  void (*decoder)( ostringstream&, uint64_t );
} Decoder;

extern "C" void   walkDetails( Decoder* decoders, size_t numDecoders, const vector<uint64_t>& mbox, RasEvent& rev, string name );
extern "C" void   decode_INTERNAL_ERROR( ostringstream& str, uint64_t status );
extern "C" void   decode_INTERNAL_ERROR_core( ostringstream& str, uint64_t status, unsigned core );

//#define TRACE(x) printf x
#define TRACE(x)

