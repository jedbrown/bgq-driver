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

#ifndef _DBBASIC_H
#define _DBBASIC_H

// a= instance of dbobj; i=index in the query; v= attribute (defines)
#define SET_COLUMN(a,v) a._columns+=(1ULL<<v);                	// a.col[v]=i;
								// a= instance of dbobj; n=max number of attributes N_<class-bgp prefix>_ATTR
#define SET_ALL_COLUMN(a) a._columns=0x7fffffffffffffffULL;     // for (int i=0; i<n; i++) a.col[i]=i+1;
#define UNSET_ALL_COLUMN(a) a._columns=0x0ULL;
#define UNSET_COLUMN(a,v) a._columns-=(1ULL<<v);

#endif
