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

#ifndef __PRINT_ASSERT_H__
#define __PRINT_ASSERT_H__

#include <stdio.h>
#include <assert.h>

#define PRINT_INFO1(A)   printf("%s:%d: %s=%lld=x%0llX ""\n",__FILE__,__LINE__,#A,(long long)(A),(long long unsigned)(A))

#define PRINT_INFO2(A,B)   printf("%s:%d: "#A"=%lld=x%0llX "#B"=%lld=x%0llX ""\n",__FILE__,__LINE__,(long long)(A),(long long unsigned)(A),(long long)(B),(long long unsigned)(B))

#define PRINT_INFO3(A,B,C) printf("%s:%d: "#A"=%lld=x%0llX "#B"=%lld=x%0llX "#C"=%lld=x%0llX ""\n",__FILE__,__LINE__,(long long)(A),(long long unsigned)(A),(long long)(B),(long long unsigned)(B),(long long)(C),(long long unsigned)(C))

#define PRINT_INFO4(A,B,C,D) printf("%s:%d: "#A"=%lld=x%0llX "#B"=%lld=x%0llX "#C"=%lld=x%0llX "#D"=%lld=x%0llX ""\n",__FILE__,__LINE__,(long long)(A),(long long unsigned)(A),(long long)(B),(long long unsigned)(B),(long long)(C),(long long unsigned)(C),(long long)(D),(long long unsigned)(D))

#define PRINT_INFO5(A,B,C,D,E) printf("%s:%d: "#A"=%lld=x%0llX "#B"=%lld=x%0llX "#C"=%lld=x%0llX "#D"=%lld=x%0llX "#E"=%lld=x%0llX ""\n",__FILE__,__LINE__,(long long)(A),(long long unsigned)(A),(long long)(B),(long long unsigned)(B),(long long)(C),(long long unsigned)(C),(long long)(D),(long long unsigned)(D),(long long)(E),(long long unsigned)(E))

#define PRINT_INFO6(A,B,C,D,E,F) printf("%s:%d: "#A"=%lld=x%0llX "#B"=%lld=x%0llX "#C"=%lld=x%0llX "#D"=%lld=x%0llX "#E"=%lld=x%0llX "#F"=%lld=x%0llX ""\n",__FILE__,__LINE__,(long long)(A),(long long unsigned)(A),(long long)(B),(long long unsigned)(B),(long long)(C),(long long unsigned)(C),(long long)(D),(long long unsigned)(D),(long long)(E),(long long unsigned)(E),(long long)(F),(long long unsigned)(F))

#define PRINT_INFO7(A,B,C,D,E,F,G) printf("%s:%d: "#A"=%lld=x%0llX "#B"=%lld=x%0llX "#C"=%lld=x%0llX "#D"=%lld=x%0llX "#E"=%lld=x%0llX "#F"=%lld=x%0llX "#G"=%lld=x%0llX ""\n",__FILE__,__LINE__,(long long)(A),(long long unsigned)(A),(long long)(B),(long long unsigned)(B),(long long)(C),(long long unsigned)(C),(long long)(D),(long long unsigned)(D),(long long)(E),(long long unsigned)(E),(long long)(F),(long long unsigned)(F),(long long)(G),(long long unsigned)(G))

#define PRINT_INFO8(A,B,C,D,E,F,G,H) printf("%s:%d: "#A"=%lld=x%0llX "#B"=%lld=x%0llX "#C"=%lld=x%0llX "#D"=%lld=x%0llX "#E"=%lld=x%0llX "#F"=%lld=x%0llX "#G"=%lld=x%0llX "#H"=%lld=x%0llX ""\n",__FILE__,__LINE__,(long long)(A),(long long unsigned)(A),(long long)(B),(long long unsigned)(B),(long long)(C),(long long unsigned)(C),(long long)(D),(long long unsigned)(D),(long long)(E),(long long unsigned)(E),(long long)(F),(long long unsigned)(F),(long long)(G),(long long unsigned)(G),(long long)(H),(long long unsigned)(H))

#define PRINT_INFO9(A,B,C,D,E,F,G,H,I) printf("%s:%d: "#A"=%lld=x%0llX "#B"=%lld=x%0llX "#C"=%lld=x%0llX "#D"=%lld=x%0llX "#E"=%lld=x%0llX "#F"=%lld=x%0llX "#G"=%lld=x%0llX "#H"=%lld=x%0llX "#I"=%lld=x%0llX ""\n",__FILE__,__LINE__,(long long)(A),(long long unsigned)(A),(long long)(B),(long long unsigned)(B),(long long)(C),(long long unsigned)(C),(long long)(D),(long long unsigned)(D),(long long)(E),(long long unsigned)(E),(long long)(F),(long long unsigned)(F),(long long)(G),(long long unsigned)(G),(long long)(H),(long long unsigned)(H),(long long)(I),(long long unsigned)(I))

#define PRINT_INFO10(A,B,C,D,E,F,G,H,I,J) printf("%s:%d: "#A"=%lld=x%0llX "#B"=%lld=x%0llX "#C"=%lld=x%0llX "#D"=%lld=x%0llX "#E"=%lld=x%0llX "#F"=%lld=x%0llX "#G"=%lld=x%0llX "#H"=%lld=x%0llX "#I"=%lld=x%0llX "#J"=%lld=x%0llX ""\n",__FILE__,__LINE__,(long long)(A),(long long unsigned)(A),(long long)(B),(long long unsigned)(B),(long long)(C),(long long unsigned)(C),(long long)(D),(long long unsigned)(D),(long long)(E),(long long unsigned)(E),(long long)(F),(long long unsigned)(F),(long long)(G),(long long unsigned)(G),(long long)(H),(long long unsigned)(H),(long long)(I),(long long unsigned)(I),(long long)(J),(long long unsigned)(J))

#define PRINT_INFO11(A,B,C,D,E,F,G,H,I,J,K  ) printf("%s:%d: "#A"=%lld=x%0llX "#B"=%lld=x%0llX "#C"=%lld=x%0llX "#D"=%lld=x%0llX "#E"=%lld=x%0llX "#F"=%lld=x%0llX "#G"=%lld=x%0llX "#H"=%lld=x%0llX "#I"=%lld=x%0llX "#J"=%lld=x%0llX "#K"=%lld=x%0llX ""\n",__FILE__,__LINE__,(long long)(A),(long long unsigned)(A),(long long)(B),(long long unsigned)(B),(long long)(C),(long long unsigned)(C),(long long)(D),(long long unsigned)(D),(long long)(E),(long long unsigned)(E),(long long)(F),(long long unsigned)(F),(long long)(G),(long long unsigned)(G),(long long)(H),(long long unsigned)(H),(long long)(I),(long long unsigned)(I),(long long)(J),(long long unsigned)(J),(long long)(K),(long long unsigned)(K))

#define PRINT_INFO12(A,B,C,D,E,F,G,H,I,J,K,M) printf("%s:%d: "#A"=%lld=x%0llX "#B"=%lld=x%0llX "#C"=%lld=x%0llX "#D"=%lld=x%0llX "#E"=%lld=x%0llX "#F"=%lld=x%0llX "#G"=%lld=x%0llX "#H"=%lld=x%0llX "#I"=%lld=x%0llX "#J"=%lld=x%0llX "#K"=%lld=x%0llX "#M"=%lld=x%0llX ""\n",__FILE__,__LINE__,(long long)(A),(long long unsigned)(A),(long long)(B),(long long unsigned)(B),(long long)(C),(long long unsigned)(C),(long long)(D),(long long unsigned)(D),(long long)(E),(long long unsigned)(E),(long long)(F),(long long unsigned)(F),(long long)(G),(long long unsigned)(G),(long long)(H),(long long unsigned)(H),(long long)(I),(long long unsigned)(I),(long long)(J),(long long unsigned)(J),(long long)(K),(long long unsigned)(K),(long long)(M),(long long unsigned)(M))

// 20100813 - Use of L causes: error: '#' is not followed by a macro parameter
//#define PRINT_INFO12(A,B,C,D,E,F,G,H,I,J,K,L) printf("%s:%d: "#A"=%lld=x%0llX "#B"=%lld=x%0llX "#C"=%lld=x%0llX "#D"=%lld=x%0llX "#E"=%lld=x%0llX "#F"=%lld=x%0llX "#G"=%lld=x%0llX "#H"=%lld=x%0llX "#I"=%lld=x%0llX "#J"=%lld=x%0llX "#K"=%lld=x%0llX "#L"=%lld=x%0llX ""\n",__FILE__,__LINE__,(long long)(A),(long long unsigned)(A),(long long)(B),(long long unsigned)(B),(long long)(C),(long long unsigned)(C),(long long)(D),(long long unsigned)(D),(long long)(E),(long long unsigned)(E),(long long)(F),(long long unsigned)(F),(long long)(G),(long long unsigned)(G),(long long)(H),(long long unsigned)(H),(long long)(I),(long long unsigned)(I),(long long)(J),(long long unsigned)(J),(long long)(K),(long long unsigned)(K),(long long)(L),(long long unsigned)(L))

#define PRINT_INFO13(A,B,C,D,E,F,G,H,I,J,K,M,N) printf("%s:%d: "#A"=%lld=x%0llX "#B"=%lld=x%0llX "#C"=%lld=x%0llX "#D"=%lld=x%0llX "#E"=%lld=x%0llX "#F"=%lld=x%0llX "#G"=%lld=x%0llX "#H"=%lld=x%0llX "#I"=%lld=x%0llX "#J"=%lld=x%0llX "#K"=%lld=x%0llX "#M"=%lld=x%0llX "#N"=%lld=x%0llX ""\n",__FILE__,__LINE__,(long long)(A),(long long unsigned)(A),(long long)(B),(long long unsigned)(B),(long long)(C),(long long unsigned)(C),(long long)(D),(long long unsigned)(D),(long long)(E),(long long unsigned)(E),(long long)(F),(long long unsigned)(F),(long long)(G),(long long unsigned)(G),(long long)(H),(long long unsigned)(H),(long long)(I),(long long unsigned)(I),(long long)(J),(long long unsigned)(J),(long long)(K),(long long unsigned)(K),(long long)(M),(long long unsigned)(M),(long long)(N),(long long unsigned)(N))

#define PRINT_INFO14(A,B,C,D,E,F,G,H,I,J,K,M,N,O) printf("%s:%d: "#A"=%lld=x%0llX "#B"=%lld=x%0llX "#C"=%lld=x%0llX "#D"=%lld=x%0llX "#E"=%lld=x%0llX "#F"=%lld=x%0llX "#G"=%lld=x%0llX "#H"=%lld=x%0llX "#I"=%lld=x%0llX "#J"=%lld=x%0llX "#K"=%lld=x%0llX "#M"=%lld=x%0llX "#N"=%lld=x%0llX "#O"=%lld=x%0llX ""\n",__FILE__,__LINE__,(long long)(A),(long long unsigned)(A),(long long)(B),(long long unsigned)(B),(long long)(C),(long long unsigned)(C),(long long)(D),(long long unsigned)(D),(long long)(E),(long long unsigned)(E),(long long)(F),(long long unsigned)(F),(long long)(G),(long long unsigned)(G),(long long)(H),(long long unsigned)(H),(long long)(I),(long long unsigned)(I),(long long)(J),(long long unsigned)(J),(long long)(K),(long long unsigned)(K),(long long)(M),(long long unsigned)(M),(long long)(N),(long long unsigned)(N),(long long)(O),(long long unsigned)(O))

#define PRINT_INFO15(A,B,C,D,E,F,G,H,I,J,K,M,N,O,P) printf("%s:%d: "#A"=%lld=x%0llX "#B"=%lld=x%0llX "#C"=%lld=x%0llX "#D"=%lld=x%0llX "#E"=%lld=x%0llX "#F"=%lld=x%0llX "#G"=%lld=x%0llX "#H"=%lld=x%0llX "#I"=%lld=x%0llX "#J"=%lld=x%0llX "#K"=%lld=x%0llX "#M"=%lld=x%0llX "#N"=%lld=x%0llX "#O"=%lld=x%0llX "#P"=%lld=x%0llX ""\n",__FILE__,__LINE__,(long long)(A),(long long unsigned)(A),(long long)(B),(long long unsigned)(B),(long long)(C),(long long unsigned)(C),(long long)(D),(long long unsigned)(D),(long long)(E),(long long unsigned)(E),(long long)(F),(long long unsigned)(F),(long long)(G),(long long unsigned)(G),(long long)(H),(long long unsigned)(H),(long long)(I),(long long unsigned)(I),(long long)(J),(long long unsigned)(J),(long long)(K),(long long unsigned)(K),(long long)(M),(long long unsigned)(M),(long long)(N),(long long unsigned)(N),(long long)(O),(long long unsigned)(O),(long long)(P),(long long unsigned)(P))

#define DELAYED_assert(A)     do {++fired_PRINT_ASSERT;} while(0)
#define IMMEDIATE_assert(A)  assert(A)
#define MAYBE_DELAY_assert(A) IMMEDIATE_assert(A)

// PRINT_ASSERT(cond) prints out the cond because otherwise silent if DELAYED_assert(A)
#define PRINT_ASSERT(  cond)                           do {if (!(cond)) {PRINT_INFO1( cond);                      MAYBE_DELAY_assert(cond);} } while (0)
#define PRINT_ASSERT1( cond,A)                         do {if (!(cond)) {PRINT_INFO1( A);                         MAYBE_DELAY_assert(cond);} } while (0)
#define PRINT_ASSERT2( cond,A,B)                       do {if (!(cond)) {PRINT_INFO2( A,B);                       MAYBE_DELAY_assert(cond);} } while (0)
#define PRINT_ASSERT3( cond,A,B,C)                     do {if (!(cond)) {PRINT_INFO3( A,B,C);                     MAYBE_DELAY_assert(cond);} } while (0)
#define PRINT_ASSERT4( cond,A,B,C,D)                   do {if (!(cond)) {PRINT_INFO4( A,B,C,D);                   MAYBE_DELAY_assert(cond);} } while (0)
#define PRINT_ASSERT5( cond,A,B,C,D,E)                 do {if (!(cond)) {PRINT_INFO5( A,B,C,D,E);                 MAYBE_DELAY_assert(cond);} } while (0)
#define PRINT_ASSERT6( cond,A,B,C,D,E,F)               do {if (!(cond)) {PRINT_INFO6( A,B,C,D,E,F);               MAYBE_DELAY_assert(cond);} } while (0)
#define PRINT_ASSERT7( cond,A,B,C,D,E,F,G)             do {if (!(cond)) {PRINT_INFO7( A,B,C,D,E,F,G);             MAYBE_DELAY_assert(cond);} } while (0)
#define PRINT_ASSERT8( cond,A,B,C,D,E,F,G,H)           do {if (!(cond)) {PRINT_INFO8( A,B,C,D,E,F,G,H);           MAYBE_DELAY_assert(cond);} } while (0)
#define PRINT_ASSERT9( cond,A,B,C,D,E,F,G,H,I)         do {if (!(cond)) {PRINT_INFO9( A,B,C,D,E,F,G,H,I);         MAYBE_DELAY_assert(cond);} } while (0)
#define PRINT_ASSERT10(cond,A,B,C,D,E,F,G,H,I,J)       do {if (!(cond)) {PRINT_INFO10(A,B,C,D,E,F,G,H,I,J);       MAYBE_DELAY_assert(cond);} } while (0)
#define PRINT_ASSERT11(cond,A,B,C,D,E,F,G,H,I,J,K)     do {if (!(cond)) {PRINT_INFO11(A,B,C,D,E,F,G,H,I,J,K);     MAYBE_DELAY_assert(cond);} } while (0)
#define PRINT_ASSERT12(cond,A,B,C,D,E,F,G,H,I,J,K,L)   do {if (!(cond)) {PRINT_INFO12(A,B,C,D,E,F,G,H,I,J,K,L);   MAYBE_DELAY_assert(cond);} } while (0)
#define PRINT_ASSERT13(cond,A,B,C,D,E,F,G,H,I,J,K,L,N) do {if (!(cond)) {PRINT_INFO13(A,B,C,D,E,F,G,H,I,J,K,L,N); MAYBE_DELAY_assert(cond);} } while (0)


#endif //  __PRINT_ASSERT_H__
