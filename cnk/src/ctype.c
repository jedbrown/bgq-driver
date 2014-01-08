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
/* (C) Copyright IBM Corp.  2007, 2012                              */
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


#include <hwi/include/common/compiler_support.h>

__BEGIN_DECLS

#include <hwi/include/common/bgq_alignment.h>
//#include <common/linkage.h>

#include "ctype.h"


unsigned short _bp_ctype[128] ALIGN_L1D_CACHE =
{
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_BLANK | C_CNTRL | C_SPACE),
    (C_ASCII | C_CNTRL | C_SPACE),
    (C_ASCII | C_CNTRL | C_SPACE),
    (C_ASCII | C_CNTRL | C_SPACE),
    (C_ASCII | C_CNTRL | C_SPACE),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_CNTRL),
    (C_ASCII | C_BLANK | C_PRINT | C_SPACE),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ALNUM | C_ASCII | C_DIGIT | C_GRAPH | C_PRINT | C_XDIGIT),
    (C_ALNUM | C_ASCII | C_DIGIT | C_GRAPH | C_PRINT | C_XDIGIT),
    (C_ALNUM | C_ASCII | C_DIGIT | C_GRAPH | C_PRINT | C_XDIGIT),
    (C_ALNUM | C_ASCII | C_DIGIT | C_GRAPH | C_PRINT | C_XDIGIT),
    (C_ALNUM | C_ASCII | C_DIGIT | C_GRAPH | C_PRINT | C_XDIGIT),
    (C_ALNUM | C_ASCII | C_DIGIT | C_GRAPH | C_PRINT | C_XDIGIT),
    (C_ALNUM | C_ASCII | C_DIGIT | C_GRAPH | C_PRINT | C_XDIGIT),
    (C_ALNUM | C_ASCII | C_DIGIT | C_GRAPH | C_PRINT | C_XDIGIT),
    (C_ALNUM | C_ASCII | C_DIGIT | C_GRAPH | C_PRINT | C_XDIGIT),
    (C_ALNUM | C_ASCII | C_DIGIT | C_GRAPH | C_PRINT | C_XDIGIT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER | C_XDIGIT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER | C_XDIGIT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER | C_XDIGIT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER | C_XDIGIT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER | C_XDIGIT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER | C_XDIGIT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_PRINT | C_UPPER),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT | C_XDIGIT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT | C_XDIGIT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT | C_XDIGIT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT | C_XDIGIT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT | C_XDIGIT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT | C_XDIGIT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ALPHA | C_ALNUM | C_ASCII | C_GRAPH | C_LOWER | C_PRINT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_GRAPH | C_PRINT | C_PUNCT),
    (C_ASCII | C_CNTRL)
};


__END_DECLS

