/* ------------------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                                      */
/* Blue Gene/Q 5765-PER 5765-PRP                                             */
/* © Copyright IBM Corp. 2012 All Rights Reserved                            */
/* US Government Users Restricted Rights - Use, duplication or disclosure    */
/*   restricted by GSA ADP Schedule Contract with IBM Corp.                  */
/*                                                                           */
/* This software is available to you under the Eclipse Public License (EPL). */
/* ------------------------------------------------------------------------- */

#ifndef __test_h__
#define __test_h__

#include <pami.h>

/* Each test will implement this function in a separate object file */
void test_fn (int argc, char * argv[],
              pami_client_t client,
              pami_context_t context[]);

#endif /* __test_h__ */

