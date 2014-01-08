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

// These verify*() routines:
// - test and are part of $BGQHOME/bgq/bgq_util/include/BgNDReduce_C.h
// - are a port of the tests from void oceanGen::testBgNDReduce() in $BGQHOME/bgq/hwsim_unitsim/src/fusion/msg_unit/mut/oceanGen.C

// TODO - MAX_ELEMENTS, MAX_PARTICIPANTS, MAX_WORDS should be passed in a as argumnets.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

#include <BgRandom.h>
#include <BgEndian.h>
#include <BgNDReduce_C.h>
#include <PRINT_ASSERT.h>

static uint64_t minmaxgoofy64(int opcode, uint64_t ua, uint64_t ub) {
  int64_t sa = *(int64_t*)&ua;
  int64_t sb = *(int64_t*)&ub;
  if        (opcode==   minColOpcode) {
    int64_t m = min(sa,sb);
    return *(uint64_t*)&m;
  } else if (opcode==   maxColOpcode) {
    int64_t m = max(sa,sb);
    return *(uint64_t*)&m;
  } else if (opcode== u_minColOpcode) {
    return min(ua,ub);
  } else if (opcode== u_maxColOpcode) {
    return max(ua,ub);
  } else {
    assert(0);
  }
  return 0;
}

void verifyLogicalOps2wordElement(MTRandomArg *R) {
    int results_verified = 0;
    // Using uint64_t to verify 2-word element.
#define MAX_ELEMENTS 5
#define MAX_PARTICIPANTS 5
    const int elementWords = 2;
    uint64_t c[MAX_PARTICIPANTS][MAX_ELEMENTS];
    void *pc[MAX_PARTICIPANTS];
    uint64_t vResult[MAX_ELEMENTS];
    uint64_t vException[MAX_ELEMENTS];

    int numParticipants, numElements, p, e, opcode;
    for (numParticipants=0; numParticipants<=MAX_PARTICIPANTS; numParticipants++) {
      for (numElements=0; numElements<=MAX_ELEMENTS; numElements++) {
        for (p=0; p<numParticipants; p++) {
          pc[p] = &(c[p][0]);
          for (e=0; e<numElements; e++) {
            c[p][e] = BgRandom64(R); // TODO g_oceanMem->m_rnd64();
          }
        }
        for (opcode=andColOpcode; opcode <= xorColOpcode; opcode++) {
          uint64_t r = BgNDReduce_vectorReduce(numElements,elementWords,opcode,numParticipants,NULL,pc,NULL,0,vResult,vException);
          assert(r==0); // TODO oceanASSERT1(r==0,r);
          for (e=0; e<numElements; e++) {
            uint64_t correct = opcode==andColOpcode ? ~0ull : 0;
            assert(vException[e]==0); // TODO oceanASSERT3(vException[e]==0,e,numParticipants,numElements);
            for (p=0; p<numParticipants; p++) {
              if        (opcode==andColOpcode) {
                correct &= c[p][e];
              } else if (opcode== orColOpcode) {
                correct |= c[p][e];
              } else if (opcode==xorColOpcode) {
                correct ^= c[p][e];
              }
            }
            assert(correct==vResult[e]); // TODO oceanASSERT8(correct==vResult[e],correct,vResult[e],e,c[0][e],c[1][e],c[2][e],numParticipants,opcode);
            ++results_verified;
          }
        }
      }
    }
    printf("%s results_verified=%d\n", __func__,results_verified);
}

void verifyLogicalOpsVariousWordLengths(MTRandomArg *R) {
    int results_verified = 0;
    // Verifies various word lengths
#define MAX_WORDS 5
#define MAX_ELEMENTS 5
#define MAX_PARTICIPANTS 5
    void *pc[MAX_PARTICIPANTS];
    uint64_t vException[MAX_ELEMENTS];

    int numParticipants; for (numParticipants=0; numParticipants<=MAX_PARTICIPANTS; numParticipants++) {
      int numElements; for (numElements=0; numElements<=MAX_ELEMENTS; numElements++) {
	int elementWords; for (elementWords=0; elementWords<=MAX_WORDS; elementWords++) {
          uint32_t c[MAX_PARTICIPANTS][MAX_ELEMENTS][elementWords]; // Must use elementWords, not MAX_WORDS!
          uint32_t vResult[MAX_ELEMENTS][elementWords];             // Must use elementWords, not MAX_WORDS!
          int p; for (p=0; p<numParticipants; p++) {
            pc[p] = &(c[p][0]);
            int e; for (e=0; e<numElements; e++) {
              int w; for (w=0; w<elementWords; w++) {
		// WARNING! gcc -Wall -Werror does not pick up:  c[p][e][w] = BgRandom32(&R);
                c[p][e][w] = BgRandom32(R); // g_oceanMem->m_rnd32();
              }
            }
	  }
          int opcode; for (opcode=andColOpcode; opcode <= xorColOpcode; opcode++) {
            uint32_t r = BgNDReduce_vectorReduce(numElements,elementWords,opcode,numParticipants,NULL,pc,NULL,0,vResult[0],vException);
            assert(r==0); // oceanASSERT1(r==0,r);
            int e; for (e=0; e<numElements; e++) {
              assert(vException[e]==0); // oceanASSERT(vException[e]==0);
              int w; for (w=0; w<elementWords; w++) {
                uint32_t correct = opcode==andColOpcode ? ~0ul : 0;
                int p; for (p=0; p<numParticipants; p++) {
        	  if        (opcode==andColOpcode) {
                    correct &= c[p][e][w];
        	  } else if (opcode== orColOpcode) {
                    correct |= c[p][e][w];
        	  } else if (opcode==xorColOpcode) {
                    correct ^= c[p][e][w];
        	  }
                }
                assert(correct==vResult[e][w]); // oceanASSERT11(correct==vResult[e][w],correct,vResult[e][w],e,w,c[0][e][w],c[1][e][w],c[2][e][w],numParticipants,numElements,elementWords,opcode);
                ++results_verified;
              }
            }
          } // opcode

	} // elementsWords
      } // numElements
    } // numParticipants
    printf("%s results_verified=%d\n", __func__,results_verified);
}


void verifyMinOrMax(MTRandomArg *R) {
    int results_verified = 0;
    // Using uint64_t to verify 2-word element.
#define MAX_ELEMENTS 5
#define MAX_PARTICIPANTS 5
    const int elementWords = 2;
    uint64_t c[MAX_PARTICIPANTS][MAX_ELEMENTS];
    void *pc[MAX_PARTICIPANTS];
    uint64_t vResult[MAX_ELEMENTS];
    uint64_t vException[MAX_ELEMENTS];

    int numParticipants; for (numParticipants=1; numParticipants<=MAX_PARTICIPANTS; numParticipants++) {
      int numElements; for (numElements=1; numElements<=MAX_ELEMENTS; numElements++) {
	//      int numElements=1; {
        int p; for (p=0; p<numParticipants; p++) {
          pc[p] = &(c[p][0]);
          int e; for (e=0; e<numElements; e++) {
            // c[p][e] = endian64(g_oceanMem->m_rnd64());//(uint64_t)p<<32; //g_oceanMem->m_rnd64();
            c[p][e] = nonBG_uint64_EndianFlip(BgRandom64(R)); //(uint64_t)p<<32; //g_oceanMem->m_rnd64();
          }
        }
        int opcode; for (opcode=u_minColOpcode; opcode <= maxColOpcode; opcode++) {
          if (opcode == 7) continue;
          if (opcode == addColOpcode) continue;
          uint64_t r = BgNDReduce_vectorReduce(numElements,elementWords,opcode,numParticipants,NULL,pc,NULL,0,vResult,vException);
          assert(r==0); // oceanASSERT4(r==0,r,numParticipants,numElements,opcode);
          int e; for (e=0; e<numElements; e++) {
            assert(vException[e]==0); // oceanASSERT3(vException[e]==0,e,numParticipants,numElements);
            uint64_t correct = nonBG_uint64_EndianFlip(c[0][e]);
            int p; for (p=1; p<numParticipants; p++) {
               correct = minmaxgoofy64(opcode,correct,nonBG_uint64_EndianFlip(c[p][e]));
            }
            assert(correct==nonBG_uint64_EndianFlip(vResult[e])); // oceanASSERT9(correct==nonBG_uint64_EndianFlip(vResult[e]),correct,nonBG_uint64_EndianFlip(vResult[e]),e,c[0][e],c[1][e],c[2][e],numParticipants,numElements,opcode);
            ++results_verified;
            if (correct==nonBG_uint64_EndianFlip(vResult[e])) {
              //oceanINFO9(1,correct,vResult[e],e,c[0][e],c[1][e],c[2][e],numParticipants,numElements,opcode);
	    }
          }
        }
      }
    }
    printf("%s results_verified=%d\n", __func__,results_verified);
} // verifyMinOrMax()

void verify_u_add(MTRandomArg *R) {
    int results_verified = 0;
    // Using uint64_t to verify 2-word element.
#define MAX_ELEMENTS 5
#define MAX_PARTICIPANTS 5
    const int elementWords = 2;
    uint64_t c[MAX_PARTICIPANTS][MAX_ELEMENTS];
    void *pc[MAX_PARTICIPANTS];
    uint64_t vResult[MAX_ELEMENTS];
    uint64_t vException[MAX_ELEMENTS];

    int allow_overflow = u_addOverflowBGNdReduce; // 0;

    int numParticipants; for (numParticipants=1; numParticipants<=MAX_PARTICIPANTS; numParticipants++) {
      int numElements; for (numElements=1; numElements<=MAX_ELEMENTS; numElements++) {
        int p; for (p=0; p<numParticipants; p++) {
          pc[p] = &(c[p][0]);
          int e; for (e=0; e<numElements; e++) {
	    if (allow_overflow) {
    	      c[p][e] = nonBG_uint64_EndianFlip(BgRandom64(R)); // nonBG_uint64_EndianFlip(g_oceanMem->m_rnd64());//(uint64_t)p<<32; //g_oceanMem->m_rnd64();
	    } else {
  	      static uint64_t counter = 0;
              c[p][e] = nonBG_uint64_EndianFlip(++counter);
	    }
          }
        }
        int opcode; for (opcode=u_addColOpcode; opcode <= u_addColOpcode; opcode++) {
          uint64_t r = BgNDReduce_vectorReduce(numElements,elementWords,opcode,numParticipants,NULL,pc,NULL,0,vResult,vException);
	  uint64_t overall_exception = 0;
          int e; for (e=0; e<numElements; e++) {
	    uint64_t exception = 0;
            uint64_t correct = nonBG_uint64_EndianFlip(c[0][e]);
            int p; for (p=1; p<numParticipants; p++) {
	      uint64_t old = correct;
              correct += nonBG_uint64_EndianFlip(c[p][e]);
	      if (correct < old) {
		exception = u_addOverflowBGNdReduce;
	      }
            }
	    overall_exception |= exception;
            assert(vException[e]==exception); // oceanASSERT3(vException[e]==exception,e,numParticipants,numElements);
            assert(correct==nonBG_uint64_EndianFlip(vResult[e])); // oceanASSERT9(correct==nonBG_uint64_EndianFlip(vResult[e]),correct,nonBG_uint64_EndianFlip(vResult[e]),e,c[0][e],c[1][e],c[2][e],numParticipants,numElements,opcode);
            ++results_verified;
            if (correct==nonBG_uint64_EndianFlip(vResult[e])) {
              //oceanINFO9(1,correct,vResult[e],e,c[0][e],c[1][e],c[2][e],numParticipants,numElements,opcode);
	    }
          }
          assert(r==overall_exception); // oceanASSERT4(r==overall_exception,r,numParticipants,numElements,opcode);
        }
      }
    }
    printf("%s results_verified=%d\n", __func__,results_verified);
} // verify_u_add()


void verify_add(MTRandomArg *R) {
    int results_verified = 0;
    // Using int64_t to verify 2-word element.
#undef MAX_ELEMENTS
#define MAX_ELEMENTS 55
#undef MAX_PARTICIPANTS
#define MAX_PARTICIPANTS 12
    const int elementWords = 2;
    int64_t c[MAX_PARTICIPANTS][MAX_ELEMENTS];
    void *pc[MAX_PARTICIPANTS];
    int64_t vResult[MAX_ELEMENTS];
    uint64_t vException[MAX_ELEMENTS];

    int allow_overflow = 1;//addOverflowBGNdReduce;

    //    for (int numParticipants=2; numParticipants<=2; numParticipants++) {
    int numParticipants; for (numParticipants=MAX_PARTICIPANTS; numParticipants<=MAX_PARTICIPANTS; numParticipants++) {
    //    for (int numParticipants=1; numParticipants<=MAX_PARTICIPANTS; numParticipants++) {
      int numElements; for (numElements=1; numElements<=MAX_ELEMENTS; numElements++) {
        int p; for (p=0; p<numParticipants; p++) {
          pc[p] = &(c[p][0]);
          int e; for (e=0; e<numElements; e++) {
	    if (allow_overflow) {
	      //	      uint64_t u = g_oceanMem->m_rnd64(0x3FFFFFFFFFFFFFFFull,0xFFFFFFFFFFFFFFFFull);   //(uint64_t)p<<32; 
	      //	      uint64_t u = g_oceanMem->m_rnd64(0x8000000000000000ull,0xA000000000000000ull);   //(uint64_t)p<<32; 
	      uint64_t u = BgRandom64(R); //g_oceanMem->m_rnd64();   //(uint64_t)p<<32; 
	      //    	      c[p][e] = nonBG_int64_EndianFlip(*(int64_t*)&u);
	      //42    	      c[p][e] = nonBG_int64_EndianFlip((*(int64_t*)&u )/2);
	      c[p][e] = nonBG_int64_EndianFlip((*(int64_t*)&u )/2);
	    } else {
  	      static int64_t counter = 0;
              c[p][e] = nonBG_int64_EndianFlip(++counter);
	    }
          }
        }
        int opcode; for (opcode=addColOpcode; opcode <= addColOpcode; opcode++) {
	  char *Tree = NULL;
	  if (numParticipants== 12) {
	    //            Tree = "[ { (0S 1S +) (2S 3S +) +}  { (4S 5S +) (6S 7S +) +}  +] [{(8S 9S +) (10S 11S +) +] +}";
  	    //	  char Tree[] = "0S 1S + 2S + 3S + 4S + 5S + 6S + 7S + 8S + 9S + 10S + 11S +";
	    //	  char Tree[] = "0S 1S + 2S +";
	  }
          uint64_t r = BgNDReduce_vectorReduce(numElements,elementWords,opcode,numParticipants,Tree,pc,NULL,0,vResult,vException);
	  uint64_t overall_exception = 0;
          int e; for (e=0; e<numElements; e++) {
	    uint64_t exception = 0;
            int64_t correct = 0;
	    int  p; for (p=0; p<numParticipants; p++) {
	    //            for (int p=numParticipants-1; p>=0; p--) {
              int64_t old = correct;
              correct += nonBG_int64_EndianFlip(c[p][e]);
	      if (old>0 && nonBG_int64_EndianFlip(c[p][e])>0 && (correct<old || correct<nonBG_int64_EndianFlip(c[p][e])) ) {
		exception |= addOverflowBGNdReduce;
	      }
	      if (old<0 && nonBG_int64_EndianFlip(c[p][e])<0 && (correct>old || correct>nonBG_int64_EndianFlip(c[p][e])) ) {
		//		exception |= addUnderflowBGNdReduce;
		exception |= addOverflowBGNdReduce;
	      }
            }
	    overall_exception |= vException[e]; // exception;
	    assert(vException[e]==exception); //oceanASSERT5(vException[e]==exception,vException[e],exception,e,numParticipants,numElements);
            //oceanASSERT5(vException[e]==0,vException[e],exception,e,numParticipants,numElements);
            assert(correct==nonBG_int64_EndianFlip(vResult[e])); //oceanASSERT9(correct==nonBG_int64_EndianFlip(vResult[e]),correct,nonBG_int64_EndianFlip(vResult[e]),e,c[0][e],c[1][e],c[2][e],numParticipants,numElements,opcode);
            ++results_verified;
            if (correct==nonBG_int64_EndianFlip(vResult[e])) {
              //oceanINFO9(1,correct,vResult[e],e,c[0][e],c[1][e],c[2][e],numParticipants,numElements,opcode);
	    }
          }
          assert(r==overall_exception); //oceanASSERT5(r==overall_exception,r,overall_exception,numParticipants,numElements,opcode);
        }
      }
    }
    printf("%s results_verified=%d\n", __func__,results_verified);
} // verify_add()



void verify_fp_min_or_fp_max(void) {
    int results_verified = 0;
#undef MAX_ELEMENTS
#define MAX_ELEMENTS 5
#undef MAX_PARTICIPANTS
#define MAX_PARTICIPANTS 5
    const int elementWords = 2;
    double c[MAX_PARTICIPANTS][MAX_ELEMENTS];
    void *pc[MAX_PARTICIPANTS];
    double vResult[MAX_ELEMENTS];
    uint64_t vException[MAX_ELEMENTS];

    int numParticipants; for (numParticipants=1; numParticipants<=MAX_PARTICIPANTS; numParticipants++) {
      int numElements; for (numElements=1; numElements<=MAX_ELEMENTS; numElements++) {
        int p; for (p=0; p<numParticipants; p++) {
          pc[p] = &(c[p][0]);
          int e; for (e=0; e<numElements; e++) {
	    //            c[p][e] = nonBG_double_EndianFlip((double) (g_oceanMem->m_rnd64()) );
            c[p][e] = nonBG_double_EndianFlip(-(double)p);
          }
        }
	int opcode; for (opcode=fp_minColOpcode; opcode <= fp_maxColOpcode; opcode++) {
          uint64_t r = BgNDReduce_vectorReduce(numElements,elementWords,opcode,numParticipants,NULL,pc,NULL,0,vResult,vException);
          assert(r==0); //oceanASSERT4(r==0,r,numParticipants,numElements,opcode);
          int e; for (e=0; e<numElements; e++) {
            assert(vException[e]==0); //oceanASSERT3(vException[e]==0,e,numParticipants,numElements);
            double correct = nonBG_double_EndianFlip(c[0][e]);
            int p; for (p=1; p<numParticipants; p++) {
	      if (opcode==fp_minColOpcode) {
               correct = min(correct,nonBG_double_EndianFlip(c[p][e]));
	      } else {
               correct = max(correct,nonBG_double_EndianFlip(c[p][e]));
	      }
            }
#if 0
	    double v = nonBG_double_EndianFlip(vResult[e]);
	    double c0 =  nonBG_double_EndianFlip(c[0][e]);
	    double c1 =  nonBG_double_EndianFlip(c[1][e]);
	    double c2 =  nonBG_double_EndianFlip(c[2][e]);
#endif
	    //    printf("BgNDReduce_x86endian=%d\n",BgNDReduce_x86endian);
    //    printf("correct=%ll0X vResult[e]=%ll0X v=%ll0X c0=%ll0X c1=%ll0X c2=%ll0X e=%d numParticipants=%d numElements=%d opcode=%d \n",correct,vResult[e], v,c0,c1,c2, e,numParticipants,numElements,opcode);
            assert(correct==nonBG_double_EndianFlip(vResult[e])); //oceanASSERT5cS(correct==nonBG_double_EndianFlip(vResult[e]),1,e,numParticipants,numElements,opcode,,oAFdouble(correct)+" "oAFdouble(v)+" "oAFdouble(c0)+" "oAFdouble(c1)+" "oAFdouble(c2) );
            ++results_verified;
            //oceanASSERT5(correct==nonBG_double_EndianFlip(vResult[e]),*(uint64_t*)&correct,e,numParticipants,numElements,opcode);
	    //            oceanASSERT9(correct==nonBG_double_EndianFlip(vResult[e]),*(uint64_t*)&correct,*(uint64_t*)&nonBG_uint64_EndianFlip(vResult[e]),e,*(uint64_t*)&c[0][e],*(uint64_t*)&c[1][e],*(uint64_t*)&c[2][e],numParticipants,numElements,opcode);
            if (correct==nonBG_double_EndianFlip(vResult[e])) {
              //oceanINFO9(1,correct,vResult[e],e,c[0][e],c[1][e],c[2][e],numParticipants,numElements,opcode);
	    }
          }
        }
      }
    }
    printf("%s results_verified=%d\n", __func__,results_verified);
} // verify_fp_min_or_fp_max()


void verify_native_fp_minScalarReduce(void) {
    int results_verified = 0;
    int thisCase;
    for (thisCase = 0; thisCase<=9; thisCase++) {
      double f0[] = {1, 2}; // worked
      double f1[] = {1, 1}; // worked
      double f2[] = {1, -1}; // worked
      double f3[] = {1, -0.5}; // worked
      double f4[] = {1, 2,4,8,16,32,64,128,256,512,1024,2048,4096}; // worked

      uint64_t u0[] = {1,1}; // The smallest possoble denorm
      uint64_t u1[] = {1,0x8000000000000001ull}; // The smallest possoble denorms
      uint64_t u2[] = {1,0x8000000000000002ull}; // The smallest possoble denorms
      uint64_t u3[2]; fp_set_sem(1u,2u,31u,&u3[0]); fp_set_sem(0u,2u,30u,&u3[1]);
      uint64_t u4[2]; fp_set_sem(0u,0x7ffu,0u,&u4[0]); fp_set_sem(0u,0x628u,0xbe6d8u,&u4[1]);

      int p = 1; // '= 1' needed to avoid AIX g++ warning.
      switch (thisCase) {
      case 0: p = sizeof(f0)/sizeof(f0[0]); break;
      case 1: p = sizeof(f1)/sizeof(f1[0]); break;
      case 2: p = sizeof(f2)/sizeof(f2[0]); break;
      case 3: p = sizeof(f3)/sizeof(f3[0]); break;
      case 4: p = sizeof(f4)/sizeof(f4[0]); break;
      case 5: p = sizeof(u0)/sizeof(u0[0]); break;
      case 6: p = sizeof(u1)/sizeof(u1[0]); break;
      case 7: p = sizeof(u2)/sizeof(u2[0]); break;
      case 8: p = sizeof(u3)/sizeof(u3[0]); break;
      case 9: p = sizeof(u4)/sizeof(u4[0]); break;
      default: assert(0); break;
      }

      double d[p];
      switch (thisCase) {
      case 0: memcpy(d,f0,p*sizeof(double)); break;
      case 1: memcpy(d,f1,p*sizeof(double)); break;
      case 2: memcpy(d,f2,p*sizeof(double)); break;
      case 3: memcpy(d,f3,p*sizeof(double)); break;
      case 4: memcpy(d,f4,p*sizeof(double)); break;
      case 5: memcpy(d,u0,p*sizeof(double)); break;
      case 6: memcpy(d,u1,p*sizeof(double)); break;
      case 7: memcpy(d,u2,p*sizeof(double)); break;
      case 8: memcpy(d,u3,p*sizeof(double)); break;
      case 9: memcpy(d,u4,p*sizeof(double)); break;
      default: assert(0); break;
      }
      
      { // verify_native_fp_minScalarReduce()
        double s;
        uint64_t rc;
        double v = d[0];
        int i;
        for (i = 0; i<p; i++) {
          v = min(v,d[i]);
        }
        rc = BgNDReduce_native_fp_minScalarReduce(p,d,&s);
        assert(!rc); //oceanASSERT1(!rc,rc);
        assert(v==s); //oceanASSERT5cS(v==s,1,2,3,4,5,,oAFdouble(s)+" "oAFdouble(v)+" "oAFdouble(d[0])+" "oAFdouble(d[1]) );
        ++results_verified;
      }

      { // verify_fp_add_individual_cases - this used to have have own function.
        double BgND;
        uint64_t rc;
        double nativeCPP = 0;
        int i; for (i = 0; i<p; i++) {
          nativeCPP += d[i];
        }
	// PRINT_INFO3(thisCase,*(long long*)&d[0],*(long long*)&d[1]);
        rc = BgNDReduce_native_fp_addScalarReduce(p,d,&BgND);
        assert(!rc); //oceanASSERT1(!rc,rc);
        assert(nativeCPP==BgND); //oceanASSERT5cS(nativeCPP==BgND,1,2,3,4,5,,oAFdouble(nativeCPP)+" "oAFdouble(BgND)+" "oAFdouble(d[0])+" "oAFdouble(d[1]) );
        ++results_verified;
      }
    }
    printf("%s results_verified=%d\n", __func__,results_verified);
} // verify_native_fp_minScalarReduce()


#if 0 // since fopen() and thus BgNDReduce_read12fromNuts() does not exist.
void verify_read12fromNuts(void) {
  if (1) {
    int numParticipants = 12;
    int maxWords = 10000;
    uint32_t u[numParticipants][maxWords];
    uint32_t *pc[numParticipants];
    uint32_t nutsBgNDReduce[maxWords];
    uint32_t vhdl[maxWords];

    int p; for (p=0; p<numParticipants; p++) {
       pc[p] = &(u[p][0]);
    }
    typedef struct {char *file; int opcode; int elementWords;} nuts_case_t;
    nuts_case_t nuts_case[] = {
      { "../../nuts.SUM/20090529fp_add_fails", fp_addColOpcode, 2 },  // 0
      { "../../nuts.SUM/20090605fp_min_fails", fp_minColOpcode, 2 },  // 1
      { "../../nuts.SUM/20090605fp_max_fails", fp_maxColOpcode, 2 },  // 2
      { "../../nuts.SUM/20090605min_fails",       minColOpcode, 2 },  // 3
      { "../../nuts.SUM/20090605u_min_fails",   u_minColOpcode, 8 },  // 4
      { "../../nuts.SUM/20090622fp_add_fails", fp_addColOpcode, 2 },  // 5
      { "../../nuts.SUM/20090623fp_add_create_nan", fp_addColOpcode, 2 },  // 6
    };
    // From      { "../../nuts.SUM/20090529fp_add_fails", fp_addColOpcode, 2 },  // 0
    // deleted lines below since vhdl now returns particular Nan.
    //          j=4 answer_uints=  0xfffe386f  rec_uints=  0xfff6386f 
    //inputs= 0x00000000 0x00000000 0x00000000 0x00000000 0x00000000 0x00000000
    //inputs= 0x00000000 0x00000000 0x00000000 0x00000000 0x00000000 0xfff6386f
    //          j=5 answer_uints=  0xb25c76ca  rec_uints=  0x92c60baa 
    //inputs= 0x00000000 0x00000000 0x00000000 0x00000000 0x00000000 0x00000000
    //inputs= 0x00000000 0x00000000 0x00000000 0x00000000 0x00000000 0x92c60baa

    int num_nuts_case = sizeof(nuts_case)/sizeof(nuts_case[0]);
    int case_index; for (    case_index = 0; case_index < num_nuts_case; case_index++) {

      int numWords = BgNDReduce_read12fromNuts(nuts_case[case_index].file, maxWords, pc,  nutsBgNDReduce,  vhdl);
      //oceanINFO1(1,numWords);
   
      assert(numWords>0 && numWords%nuts_case[case_index].elementWords==0); //oceanASSERT(numWords>0 && numWords%nuts_case[case_index].elementWords==0);
   
      int numElements = numWords/nuts_case[case_index].elementWords;
      uint32_t vResult[maxWords];
      uint64_t vException[maxWords];
   
BgNDReduce_x86endian = 32;
      uint64_t allexceptions = BgNDReduce_vectorReduce(numElements,nuts_case[case_index].elementWords,nuts_case[case_index].opcode,numParticipants,NULL,(void**)pc,NULL,0,vResult,vException);
BgNDReduce_x86endian = 0;
//oceanINFO1(1,allexceptions);
   
    
      // check the data 
      int j; for (j=0;j<  numWords;j++) {
   
        //      if ( answer_uints[j] != received_uints[j] ) {
        if ( 1 ) {
   
	  //   	sum() << getName() << (nutsBgNDReduce[j]!=vhdl[j]||vhdl[j]!=vResult[j] ? " Error " : "       " )
	  /*
   	sum() << getName() << (vhdl[j]!=vResult[j] ? " Error " : "       " )
                << " j=" << dec << j
   	      << " nutsBgNDReduce= " << hex <<  nutsBgNDReduce[j]
   	      << " vhdl= "    <<  vhdl[j]
   	      << " vResult= " << hex <<  vResult[j]
   	      << " vException= " << hex <<  vException[j/nuts_case[case_index].elementWords];
	  */
          char p_inputs[256];
   	sprintf(p_inputs," inputs= 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
   		pc[0][j],pc[1][j],pc[2][j],pc[3][j],
   		pc[4][j],pc[5][j],pc[6][j],pc[7][j],
   		pc[8][j],pc[9][j],pc[10][j],pc[11][j]);
	/*
   	sum() << p_inputs << endl;
	*/
   	
        }
         
      }
    }

  }
#endif


void verify_fp_add_loops(void) {
    int results_verified = 0;
#define MAX_ELEMENTS 5
#define MAX_PARTICIPANTS 5
    const int elementWords = 2;
    double c[MAX_PARTICIPANTS][MAX_ELEMENTS];
    void *pc[MAX_PARTICIPANTS];
    double vResult[MAX_ELEMENTS];
    uint64_t vException[MAX_ELEMENTS];

    int numParticipants; for (numParticipants=1; numParticipants<=MAX_PARTICIPANTS; numParticipants++) {
      int numElements; for (numElements=1; numElements<=MAX_ELEMENTS; numElements++) {
        int p; for (p=0; p<numParticipants; p++) {
          pc[p] = &(c[p][0]);
          int e; for (e=0; e<numElements; e++) {
	    //            c[p][e] = nonBG_double_EndianFlip((double) (g_oceanMem->m_rnd64()) );
            c[p][e] = nonBG_double_EndianFlip(-(double)p);
          }
        }
	int opcode; for (opcode=fp_addColOpcode; opcode <= fp_addColOpcode; opcode++) {
          uint64_t r = BgNDReduce_vectorReduce(numElements,elementWords,opcode,numParticipants,NULL,pc,NULL,0,vResult,vException);
          assert(r==0); //oceanASSERT4(r==0,r,numParticipants,numElements,opcode);
          int e; for (e=0; e<numElements; e++) {
            assert(vException[e]==0); //oceanASSERT3(vException[e]==0,e,numParticipants,numElements);
            double correct = nonBG_double_EndianFlip(c[0][e]);
            int p; for (p=1; p<numParticipants; p++) {
               correct += nonBG_double_EndianFlip(c[p][e]);
            }
#if 0
	    double v = nonBG_double_EndianFlip(vResult[e]);
	    double c0 =  nonBG_double_EndianFlip(c[0][e]);
	    double c1 =  nonBG_double_EndianFlip(c[1][e]);
	    double c2 =  nonBG_double_EndianFlip(c[2][e]);
#endif
	    //    printf("correct=%ll0X vResult[e]=%ll0X v=%ll0X c0=%ll0X c1=%ll0X c2=%ll0X e=%d numParticipants=%d numElements=%d opcode=%d \n",correct,vResult[e], v,c0,c1,c2, e,numParticipants,numElements,opcode);
            assert(correct==nonBG_double_EndianFlip(vResult[e])); //oceanASSERT5cS(correct==nonBG_double_EndianFlip(vResult[e]),1,e,numParticipants,numElements,opcode,,oAFdouble(correct)+" "oAFdouble(v)+" "oAFdouble(c0)+" "oAFdouble(c1)+" "oAFdouble(c2) );
            ++results_verified;
            //oceanASSERT5(correct==nonBG_double_EndianFlip(vResult[e]),*(uint64_t*)&correct,e,numParticipants,numElements,opcode);
	    //            oceanASSERT9(correct==nonBG_double_EndianFlip(vResult[e]),*(uint64_t*)&correct,*(uint64_t*)&nonBG_uint64_EndianFlip(vResult[e]),e,*(uint64_t*)&c[0][e],*(uint64_t*)&c[1][e],*(uint64_t*)&c[2][e],numParticipants,numElements,opcode);
            if (correct==nonBG_double_EndianFlip(vResult[e])) {
              //oceanINFO9(1,correct,vResult[e],e,c[0][e],c[1][e],c[2][e],numParticipants,numElements,opcode);
	    }
          }
        }
      }
    }
    printf("%s results_verified=%d\n", __func__,results_verified);
} // verify_fp_add_loops()


typedef struct {
  int RootNode;
  int NumNodes;
} linearClass_willParticipate_parms_t;

int linearClass_willParticipate(void *void_parms,int node, int link) {
  linearClass_willParticipate_parms_t *parms = (linearClass_willParticipate_parms_t*)void_parms;
  // class looks like 0 > 1 > 2 > . . . > linearClass_RootNode < . . . < (linearClass_NumNodes-1)
  // On node x, using link x+1
  assert(parms->NumNodes>0);
  assert(parms->RootNode>=0 && parms->RootNode<parms->NumNodes);
  assert(node>=0 && node<parms->NumNodes);
  assert(link>=0 && link<=11);
  int r;
  int belowLink = (node + parms->RootNode)%11;
  int aboveLink = (node + 2*parms->RootNode)%11;
  if (aboveLink==belowLink) {
    aboveLink = (aboveLink+1)%11;
  }
  assert(belowLink>=0 && belowLink<=10);
  assert(aboveLink>=0 && belowLink<=10);
  assert(belowLink!=aboveLink);
  if (link==11) {
    // every node has local contribution
    r = node;
  } else if (node>0 && node<=parms->RootNode && link==belowLink) {
    // get contribution from below.
    r = node-1;
  } else if (node<parms->NumNodes-1 && node>=parms->RootNode && link==aboveLink) {
    // get contribution from above.
    r = node+1;
  } else {
    r = -1;
  }
  //  PRINT_INFO3(node,link,r);
  return r;
}

// TODO - add min_words and min,max_opcode and check fp-add using node#.
 int verify_allNode(int debugWhat, MTRandomArg *R, int numParticipants, int rootNode, int (*willParticipate)(void *parms, int node, int link), void *willParticipate_parms, int max_words, int max_elements) {

   int what = debugWhat%100;
   char *whatText = (char*)"ERROR: Forgot to set in verify_allNODE() code";
   if (what==0 || what==1) {
     max_elements = 1; // override
   }
    int results_verified = 0;
    // Verifies various word lengths
    void *pc[numParticipants];
    void *pNodeResults[numParticipants];
    uint64_t *pNodeExceptions[numParticipants];
    uint64_t vException[max_elements];

      int numElements; for (numElements=0; numElements<=max_elements; numElements++) {
	int elementWords; for (elementWords=0; elementWords<=max_words; elementWords++) {
          uint32_t c[numParticipants][max_elements][elementWords];
          uint32_t nodeResults[numParticipants][max_elements][elementWords];
          uint64_t nodeExceptions[numParticipants][max_elements];
          uint32_t vResult[max_elements][elementWords];
          int p; for (p=0; p<numParticipants; p++) {
            pc[p] = &(c[p][0]);
            pNodeResults[p] = &(nodeResults[p][0]);
            pNodeExceptions[p] = &(nodeExceptions[p][0]);
            int e; for (e=0; e<numElements; e++) {
              int w; for (w=0; w<elementWords; w++) {
		// WARNING! gcc -Wall -Werror does not pick up:  c[p][e][w] = BgRandom32(&R);
                c[p][e][w] = BgRandom32(R); // g_oceanMem->m_rnd32();
              }
            }
	  }
          int opcode; for (opcode=orColOpcode; opcode <= orColOpcode; opcode++) {
	    // PRINT_INFO5(vResult, vResult[0], elementWords,numElements,opcode);

	    uint64_t r = 0xDEADBEEFDEADBEEFull;
	    if (what == 0) {
	      assert(max_elements == 1);
              whatText =  (char*)"allNodeScalar";
              r = BgNDReduce_allNodeScalarReduce(rootNode,
						        willParticipate, willParticipate_parms, numParticipants-1,pNodeExceptions,pNodeResults,0/*offset*/,elementWords,opcode,NULL,pc,NULL,0,vResult[0]);
	      vException[0] = r;
	    } else if (what == 1) {
	      assert(max_elements == 1);
              whatText =  (char*)"allNodeFlatScalar";
              r = BgNDReduce_allNodeFlatScalarReduce(rootNode,
						 willParticipate, willParticipate_parms, numParticipants-1,nodeExceptions[0],nodeResults,/*no offset*/ elementWords,opcode,NULL,c/*not pc*/,NULL,0,vResult[0]);
	      vException[0] = r;
	    } else if (what == 2) {
              // Verify that this testbench is ok. 5 participants, 5 elements, 5 words has worked.
	      whatText = (char*)"single-node Vector";
              r = BgNDReduce_vectorReduce(numElements,elementWords,opcode,numParticipants,NULL,pc,NULL,0,vResult,vException);
	    } else if (what == 3) {
	      whatText = (char*)"allNodeVector";
               r = BgNDReduce_allNodeVectorReduce(rootNode,
						  willParticipate, willParticipate_parms, numParticipants-1,pNodeExceptions,pNodeResults,numElements,elementWords,opcode,NULL,pc,NULL,0,vResult[0], vException);
	    } else {
	      assert(0);
	    }

            assert(r==0); // oceanASSERT1(r==0,r);
            int e; for (e=0; e<numElements; e++) {
              assert(vException[e]==0); // oceanASSERT(vException[e]==0);
              int w; for (w=0; w<elementWords; w++) {
                uint32_t correct = opcode==andColOpcode ? ~0ul : 0;
                int p; for (p=0; p<numParticipants; p++) {
        	  if        (opcode==andColOpcode) {
                    correct &= c[p][e][w];
        	  } else if (opcode== orColOpcode) {
                    correct |= c[p][e][w];
        	  } else if (opcode==xorColOpcode) {
                    correct ^= c[p][e][w];
        	  }
                }
		PRINT_ASSERT6(correct==vResult[e][w],correct,vResult[e][w],&vResult[e][w],e,w,numElements);
		// PRINT_INFO6(correct,vResult[e][w],&vResult[e][w],e,w,numElements);
                assert(correct==vResult[e][w]); // oceanASSERT11(correct==vResult[e][w],correct,vResult[e][w],e,w,c[0][e][w],c[1][e][w],c[2][e][w],numParticipants,numElements,elementWords,opcode);
                ++results_verified;
              }
            }
          } // opcode

	} // elementsWords
      } // numElements

      if (debugWhat>100) {
        printf(" with %s in %s results_verified=%d\n", whatText, __func__,results_verified);
      }

  return results_verified;
} // verify_allNode()



 int subComm_verify_allNode(int numNodesInSubComm, int *IsNodeInSubComm, int debugWhat, MTRandomArg *R, int numParticipants, int rootNode, int (*willParticipate)(void *parms, int node, int link), void *willParticipate_parms, int max_words, int max_elements) {

   int what = debugWhat%100;
   char *whatText = (char*)"ERROR: Forgot to set in verify_allNODE() code";
   if (what==0 || what==1) {
     max_elements = 1; // override
   }
    int results_verified = 0;
    // Verifies various word lengths
    void *pc[numParticipants];
    void *pNodeResults[numParticipants];
    uint64_t *pNodeExceptions[numParticipants];
    uint64_t vException[max_elements];

      int numElements; for (numElements=0; numElements<=max_elements; numElements++) {
	int elementWords; for (elementWords=0; elementWords<=max_words; elementWords++) {
          uint32_t c[numParticipants][max_elements][elementWords];
          uint32_t nodeResults[numParticipants][max_elements][elementWords];
          uint64_t nodeExceptions[numParticipants][max_elements];
          uint32_t vResult[max_elements][elementWords];
          int p; for (p=0; p<numParticipants; p++) {
            pc[p] = &(c[p][0]);
            pNodeResults[p] = &(nodeResults[p][0]);
            pNodeExceptions[p] = &(nodeExceptions[p][0]);
            int e; for (e=0; e<numElements; e++) {
              int w; for (w=0; w<elementWords; w++) {
		// WARNING! gcc -Wall -Werror does not pick up:  c[p][e][w] = BgRandom32(&R);
                c[p][e][w] = BgRandom32(R); // g_oceanMem->m_rnd32();
              }
            }
	  }
          int opcode; for (opcode=orColOpcode; opcode <= orColOpcode; opcode++) {
	    // PRINT_INFO5(vResult, vResult[0], elementWords,numElements,opcode);

	    uint64_t r = 0xDEADBEEFDEADBEEFull;
	    if (what == 0) {
	      assert(max_elements == 1);
              whatText =  (char*)"subCommScalar";
              r = BgNDReduce_subCommScalarReduce(numNodesInSubComm, IsNodeInSubComm, rootNode,
						        willParticipate, willParticipate_parms, numParticipants-1,pNodeExceptions,pNodeResults,0/*offset*/,elementWords,opcode,NULL,pc,NULL,0,vResult[0]);
	      vException[0] = r;
	    } else if (what == 1) {
	      assert(max_elements == 1);
              whatText =  (char*)"subCommFlatScalar";
              r = BgNDReduce_subCommFlatScalarReduce(numNodesInSubComm, IsNodeInSubComm, rootNode,
						 willParticipate, willParticipate_parms, numParticipants-1,nodeExceptions[0],nodeResults,/*no offset*/ elementWords,opcode,NULL,c/*not pc*/,NULL,0,vResult[0]);
	      vException[0] = r;
	    } else if (what == 2) {
	      assert(0); // Not yet ported to subComm.
              // Verify that this testbench is ok. 5 participants, 5 elements, 5 words has worked.
	      whatText = (char*)"single-node Vector";
              r = BgNDReduce_vectorReduce(numElements,elementWords,opcode,numParticipants,NULL,pc,NULL,0,vResult,vException);
	    } else if (what == 3) {
              // Not yet ported to subComm.
	      whatText = (char*)"allNodeVector";
               r = BgNDReduce_allNodeVectorReduce(rootNode,
						  willParticipate, willParticipate_parms, numParticipants-1,pNodeExceptions,pNodeResults,numElements,elementWords,opcode,NULL,pc,NULL,0,vResult[0], vException);
	    } else {
	      assert(0);
	    }

            assert(r==0); // oceanASSERT1(r==0,r);
            int e; for (e=0; e<numElements; e++) {
              assert(vException[e]==0); // oceanASSERT(vException[e]==0);
              int w; for (w=0; w<elementWords; w++) {
                uint32_t correct = opcode==andColOpcode ? ~0ul : 0;
                int p; for (p=0; p<numParticipants; p++) {
		  if (!IsNodeInSubComm[p]) continue;
        	  if        (opcode==andColOpcode) {
                    correct &= c[p][e][w];
        	  } else if (opcode== orColOpcode) {
                    correct |= c[p][e][w];
        	  } else if (opcode==xorColOpcode) {
                    correct ^= c[p][e][w];
        	  }
                }
		PRINT_ASSERT6(correct==vResult[e][w],correct,vResult[e][w],&vResult[e][w],e,w,numElements);
		// PRINT_INFO6(correct,vResult[e][w],&vResult[e][w],e,w,numElements);
                assert(correct==vResult[e][w]); // oceanASSERT11(correct==vResult[e][w],correct,vResult[e][w],e,w,c[0][e][w],c[1][e][w],c[2][e][w],numParticipants,numElements,elementWords,opcode);
                ++results_verified;
              }
            }
          } // opcode

	} // elementsWords
      } // numElements

      if (debugWhat>100) {
        printf(" with %s in %s results_verified=%d\n", whatText, __func__,results_verified);
      }

  return results_verified;
} // subComm_verify_allNode()



 void linearClass_verify_allNode(int what, MTRandomArg *R) {
    int results_verified = 0;
    // Verifies various word lengths
    linearClass_willParticipate_parms_t parms;
#undef MAX_PARTICIPANTS
#define MAX_PARTICIPANTS 5
    for (parms.NumNodes=1; parms.NumNodes<=MAX_PARTICIPANTS; parms.NumNodes++) {
      for (parms.RootNode=0; parms.RootNode<parms.NumNodes; parms.RootNode++) {
	results_verified += verify_allNode(what, R, parms.NumNodes, parms.RootNode, linearClass_willParticipate, &parms, 5, 5);
      }
    }
    printf(" %s results_verified=%d\n", __func__,results_verified);
} // linearClass_verify_verify_allNode()


