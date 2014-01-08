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

// All comments and documentation in bgq_util/include/BgNDReduce.h

#include "BgNDReduce.h"

#include <stack>
#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "assert.h"
#include "math.h"   // isnan()

using namespace std;

int BgNDReduce::x86endian = 0;

static uint32_t u_endian32(int flip, uint32_t x) {
  if (!flip) return x;
  uint8_t *b = ((uint8_t *)(&x));
  uint8_t n[4];
  n[0] = b[3];
  n[1] = b[2];
  n[2] = b[1];
  n[3] = b[0];
  uint32_t *r = (uint32_t*)n;
  return *r;
}

#if 0
static int32_t s_endian32(int flip, int32_t x) {
  if (!flip) return x;
  uint8_t *b = ((uint8_t *)(&x));
  uint8_t n[4];
  n[0] = b[3];
  n[1] = b[2];
  n[2] = b[1];
  n[3] = b[0];
  return *((int32_t*)n);
}
#endif

uint64_t BgNDReduce::vectorReduce(
    int numElements,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    char *Tree,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *vResult,
    uint64_t *vExceptions
    ) {
  uint64_t r = 0;
  for (int e=0; e<numElements; e++) {
    // *((uint32_t *)vResult + e*elementWords)= __LINE__; // Unexpected get 0 back.
    vExceptions[e] = scalarReduce(e*elementWords, elementWords, opCode, numParticipants, Tree, vContribution, vFlagsContribution, vFlagsResult, (uint32_t *)vResult + e*elementWords);
    r |= vExceptions[e];
    //    *((uint32_t *)vResult + e*elementWords)= __LINE__; // as expected.
  }
  return r;
}

uint64_t BgNDReduce::scalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    char *Tree,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
    ) {
  uint64_t rc = 0;
  switch(opCode) {
  case andColOpcode:
  case  orColOpcode:
  case xorColOpcode:
    rc = logicOpScalarReduce(offset,elementWords,opCode,numParticipants,vContribution,vFlagsContribution, vFlagsResult, Result);
    break;
  case u_minColOpcode:
  case u_maxColOpcode:
  case   minColOpcode:
  case   maxColOpcode:
    rc = minORmaxScalarReduce(offset,elementWords,opCode,numParticipants,vContribution,vFlagsContribution, vFlagsResult, Result);
    break;
  case u_addColOpcode:
    rc =    u_addScalarReduce(offset,elementWords,opCode,numParticipants,vContribution,vFlagsContribution, vFlagsResult, Result);
    break;
  case   addColOpcode:
    rc =      addScalarReduce(offset,elementWords,opCode,numParticipants, Tree, vContribution,vFlagsContribution, vFlagsResult, Result);
    break;
  case fp_minColOpcode:
  case fp_maxColOpcode:
  case fp_addColOpcode:
    rc =    fp_ScalarReduce(offset,elementWords,opCode,numParticipants,vContribution,vFlagsContribution, vFlagsResult, Result);
    break;
  default:
    return invalidOpCodeBGNdReduce;
    break;
  }

  switch(opCode) {
  case u_minColOpcode:
  case u_maxColOpcode:
  case u_addColOpcode:
    rc |= verify_uint_ScalarReduce(offset,elementWords,opCode,numParticipants,vContribution,vFlagsContribution, vFlagsResult, Result);
    break;
  case   minColOpcode:
  case   maxColOpcode:
  case   addColOpcode:
    rc |= verify_int_ScalarReduce(offset,elementWords,opCode,numParticipants,vContribution,vFlagsContribution, vFlagsResult, Result);
    break;
  default:
    break;
  }

  return rc;
}

uint64_t BgNDReduce::logicOpScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
    ) {
  for (int e = 0; e<elementWords; e++) {
    uint32_t *u = (uint32_t *)Result + e;
    *u = (opCode==andColOpcode) ? 0xFFFFFFFF : 0;
    for (int p = 0; p<numParticipants; p++) {
      uint32_t c = *((uint32_t*)(vContribution[p]) + offset + e);
      if        (opCode==andColOpcode) {
	*u &= c;
      } else if (opCode== orColOpcode) {
	*u |= c;
      } else if (opCode==xorColOpcode) {
	*u ^= c;
      } else {
	return invalidOpCodeBGNdReduce;
      }
    }
  }
  //  assert(0); // This point in the code is definitely reached.
  //  *((uint32_t *)Result)= 789; // unexpectedly does not work
  return 0ull;
}


int BgNDReduce::getFlip(void) {
  int flip = !isBGQendian();
  if        (x86endian ==  0) {
    // No change
  } else if (x86endian == 32) {
    flip = !flip;
  } else {
    return illegalx86endianBGNdReduce;
  }
  return flip;
}


uint64_t BgNDReduce::minORmaxScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
    ) {
  if (!numParticipants || !elementWords) {
    return 0ull;
  }

  int winParticipant = 0;

  if (opCode != u_minColOpcode && opCode != u_maxColOpcode && opCode != minColOpcode && opCode != maxColOpcode) {
    return invalidOpCodeBGNdReduce;
  }

  int potentialWinner[numParticipants];
  for (int p = 0; p<numParticipants; p++) {
    potentialWinner[p] = 1;
  }

  for (int e = 0; e<elementWords; e++) {
    uint32_t winValue = u_endian32(getFlip(),*((uint32_t*)(vContribution[winParticipant]) + offset + e));
    // Start at p=0 so that also 0 is a potentialWinner
    for (int p = 0; p<numParticipants; p++) {
      if (!potentialWinner[p]) continue;
      uint32_t pValue = u_endian32(getFlip(),*((uint32_t*)(vContribution[p]) + offset + e));
      if (pValue==winValue) {
        potentialWinner[p]=1;
      } else {
        int newWinner = 0;
        // For signed, only observe the sign of the first word.
        // The remaining words are treated as unsigned.
        // This also works if first tied winning word(s) is negative, since -1==FF > -128==80
        if        (e==0 && opCode == minColOpcode) {
          newWinner = *(int32_t*)&pValue < *(int32_t*)&winValue;
        } else if (e==0 && opCode == maxColOpcode) {
          newWinner = *(int32_t*)&pValue > *(int32_t*)&winValue;
        } else if (opCode == u_minColOpcode || opCode == minColOpcode) {
          newWinner = pValue < winValue;
        } else if (opCode == u_maxColOpcode || opCode == maxColOpcode) {
          newWinner = pValue > winValue;
        }
        if (newWinner) {
          winValue = pValue;
          winParticipant = p;
          potentialWinner[p]=1;
	  for (int i=0; i<p; i++) {
            // Cancel all previous potential winners
            potentialWinner[i]=0;
	  }
        } else {
          potentialWinner[p]=0;
        }
      }
    }
    uint32_t *u = (uint32_t *)Result + e;
    *u = u_endian32(getFlip(),winValue);
  }

  return 0ull;
}


uint64_t BgNDReduce::u_addScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
    ) {
  if (!numParticipants || !elementWords) {
    return 0ull;
  }

  if (opCode != u_addColOpcode) {
    return invalidOpCodeBGNdReduce;
  }

  uint32_t carry = 0;
  for (int e = 0; e<elementWords; e++) {
    uint64_t sum = carry;
    for (int p = 0; p<numParticipants; p++) {
	int eIndex = vFlagsContribution && (vFlagsContribution[p] & NdOrderBGNdReduce ) ? e : elementWords - 1 - e;
      uint32_t pValue = u_endian32(getFlip(),*((uint32_t*)(vContribution[p]) + offset + eIndex));
	sum += pValue;
    }
    int eResultIndex = vFlagsResult & NdOrderBGNdReduce ? e : elementWords - 1 - e;
    uint32_t *u = (uint32_t *)Result + eResultIndex;
    *u = u_endian32(getFlip(), 0x00000000FFFFFFFFull & sum);
    carry = sum >> 32;
  }

  if (carry) {
    return u_addOverflowBGNdReduce;
  }

  return 0ull;
}


// inCarry[], outCarry[], signedOverflow[] are indexed by adder. There is 1 less adder than participants.

uint64_t BgNDReduce::treeAddScalarReduce(
    int numParticipants,
    char *Tree,
    uint32_t *inCarry,
    uint32_t *outCarry,
    uint32_t *signedOverflow,
    uint32_t *vContribution,
    uint32_t &Result
    ) {
  if (!numParticipants) {
    return 0ull;
  }
  char defaultTree[10*numParticipants];
  if (!Tree) {
    Tree = defaultTree;
    char *t = defaultTree;
    // Example: "0S 1S + 2S + 3S + 4S + 5S + 6S + 7S + 8S + 9S + 10S + 11S +";
    t += sprintf(t,"0S ");
    for (int p = 1; p<numParticipants; p++) {
      t += sprintf(t,"%dS +",p);
    }
    //    printf("Tree:%s:Tree\n",Tree);
  }

  int whichCarry = 0;

  int done[numParticipants];
  for (int p = 0; p<numParticipants; p++) {
    done[p] = 0;
  }
  stack <uint32_t> st;

  char *pTree = Tree;
  int participant = -1;
  while (*pTree) {
    if (isdigit(*pTree)) {
	char *next;
	participant = strtol(pTree,&next,10);
	if (next<=pTree) {
	  return strtolTroubleBGNdReduce;
	}
	if (participant >= numParticipants) {
	  return illegalParticipantBGNdReduce;
	}
	pTree = next;
    } else if (   isspace(*pTree)
               || *pTree=='(' || *pTree==')' || *pTree=='[' || *pTree==']'
               || *pTree=='{' || *pTree=='}' ) {
	++pTree;
    } else if (*pTree=='S') {
	// push the paricipant just mentioned onto the stack
	if (participant <0) {
	  return noParticipantBGNdReduce;
	}
	if (done[participant]) {
	  return doubleParticipantBGNdReduce;
	}
	st.push(vContribution[participant]);
	done[participant] = 1;
        participant = -1;
	++pTree;
    } else if (*pTree=='+') {
	if (participant >=0) {
	  return unexpectedParticipantBGNdReduce;
	}
	if (st.size() < 2) {
	  return lt2OnStackBGNdReduce;
	}
	uint32_t a = st.top();
	st.pop();
	uint32_t b = st.top();
	st.pop();
	// http://en.wikipedia.org/wiki/Two's_complement
	// An overflow condition exists when a carry (an extra 1) is generated into but not out of the far left bit (the MSB), or out of but not into the MSB. As mentioned above, the sign of the number is encoded in the MSB of the result.
	uint64_t sum = (uint64_t)a + (uint64_t)b + (uint64_t)inCarry[whichCarry];
	uint32_t sum32 = sum & 0xFFFFFFFFu;
	outCarry[whichCarry] = sum >> 32;
	uint32_t carryIntoMSB = (a&0x80000000u) ^ (b&0x80000000u) ^ (sum&0x80000000u);
	signedOverflow[whichCarry] = (carryIntoMSB && !outCarry[whichCarry]) || (!carryIntoMSB && outCarry[whichCarry]);
	st.push(sum32);
	++whichCarry;
	++pTree;
    } else {
      return illegalCharBGNdReduce;
    }
  }
  if (whichCarry != numParticipants-1) {
    return whichCarryTroubleBGNdReduce;
  }
  if (st.size() < 1) {
    return nothingOnStackBGNdReduce;
  }
  if (st.size() > 1) {
    return tooMuchOnStackBGNdReduce;
  }
  for (int p = 0; p<numParticipants; p++) {
    if (done[p] == 0) {
      return unusedParticipantBGNdReduce;
    }
  }
  Result = st.top();
  st.pop();

  return 0ull;
}


uint64_t BgNDReduce::addScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    char *Tree,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
    ) {
  if (!numParticipants || !elementWords) {
    return 0ull;
  }

  if (opCode != addColOpcode) {
    return invalidOpCodeBGNdReduce;
  }

  // All words have to go through tree, since overflow is detected on each adder, so each adder has to see all previous words.
  uint32_t inCarry[numParticipants-1];
  uint32_t outCarry[numParticipants-1];
  uint32_t signedOverflow[numParticipants-1];

  for (int p =0; p<numParticipants-1; p++) {
    inCarry[p] = 0;
  }

  uint64_t rc = 0;
  uint32_t sum32;

  uint32_t iC[numParticipants];
  for (int e = 0; e<elementWords; e++) {
    for (int p = 0; p<numParticipants; p++) {
      int eIndex = vFlagsContribution && (vFlagsContribution[p] & NdOrderBGNdReduce ) ? e : elementWords - 1 - e;
      iC[p] = u_endian32(getFlip(),*((uint32_t*)(vContribution[p]) + offset + eIndex));
    }
    rc |= treeAddScalarReduce(numParticipants, Tree, inCarry, outCarry, signedOverflow, iC, sum32);
  
    int eResultIndex = vFlagsResult & NdOrderBGNdReduce ? e : elementWords - 1 - e;
    uint32_t *u = (uint32_t *)Result + eResultIndex;
    *u = u_endian32(getFlip(), sum32);

    for (int p =0; p<numParticipants-1; p++) {
      inCarry[p] = outCarry[p];
    }

  }

  for (int i =0; i<numParticipants-1; i++) {
    if (signedOverflow[i]) {
      rc |= addOverflowBGNdReduce;
    }
  }

  return rc;
}


// Do NOT pass in Nan? Or is Nan==Nan never true?
uint64_t BgNDReduce::native_fp_minScalarReduce(
    int numParticipants,
    double *vContribution,
    double &Result
    ) {

  Result = vContribution[0];

  for (int p = 1; p<numParticipants; p++) {
    if (   Result == vContribution[p]
	   && *(uint64_t*)&Result != *(uint64_t*)&vContribution[p]) {
      assert(Result == 0 && vContribution[p]==0);
      // IEEE says +0 == -0,so min(-0,+0) is free to return either.
      // BGQ says min(-0,+0) is -0, which is allowed by IEEE.
      fp_set_sem(1,0,0,(uint64_t&)Result);
    } else {
      Result = min(Result, vContribution[p]);
    }
  }

  return 0ull;
}


// Do NOT pass in Nan? Or is Nan==Nan never true?
uint64_t BgNDReduce::native_fp_maxScalarReduce(
    int numParticipants,
    double *vContribution,
    double &Result
    ) {

  Result = vContribution[0];

  for (int p = 1; p<numParticipants; p++) {
    if (   Result == vContribution[p]
	   && *(uint64_t*)&Result != *(uint64_t*)&vContribution[p]) {
      assert(Result == 0 && vContribution[p]==0);
      // IEEE says +0 == -0,so max(-0,+0) is free to return either.
      // BGQ says max(-0,+0) is +0, which is allowed by IEEE.
      fp_set_sem(0,0,0,(uint64_t&)Result);
    } else {
      Result = max(Result, vContribution[p]);
    }
  }

  return 0ull;
}


uint64_t BgNDReduce::fp_ScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
    ) {
  if (elementWords != 2) {
    return fpWordsNot2BGNdReduce;
  }

  if (!numParticipants) {
    return 0ull;
  }

  // On bgq, order within double is [msw][lsw], on x86 the order is [lsw][msw]
  int msw = isBGQendian() ? 0 : 1;
  int lsw = isBGQendian() ? 1 : 0;

  double dC[numParticipants];
  double dR;
  uint32_t *internal_r32 = (uint32_t *)(void *)&dR;
  uint32_t *external_r32 = (uint32_t *)Result;

  int non0 = 0;
  double mySum = 0;
  for (int p = 0; p<numParticipants; p++) {
    uint32_t *c = (uint32_t *)(void *)&(dC[p]);
    c[msw] = u_endian32(getFlip(), *( (uint32_t*)(vContribution[p]) + offset    ) );
    c[lsw] = u_endian32(getFlip(), *( (uint32_t*)(vContribution[p]) + offset + 1) );
    
    if (fp_is_Nan(*(uint64_t*)(void*)c )) {
      //    if (fp_is_Nan(*(uint64_t*)(void*)&dC[p])) {
      // isnan(dC[p]) - does not work, it thinks +,-Inf is Nan
      // dR = dC[p]; and using internal_r32 does not work since it does not preserve exact Nan value
      // Below is when returned first Nan seen
      //      external_r32[msw] = *( (uint32_t*)(vContribution[p]) + offset    );
      //      external_r32[lsw] = *( (uint32_t*)(vContribution[p]) + offset + 1);
      external_r32[0] = 0x7ff80000u;
      external_r32[1] = 0x00000000u;
      return NaNinputBGNdReduce;
    }

    if (dC[p] != 0) {
      ++non0;
    }
    mySum += dC[p];
  }

  uint64_t rc;

  switch(opCode) {
  case fp_minColOpcode:
    rc = native_fp_minScalarReduce(numParticipants,dC,dR);
    break;
  case fp_maxColOpcode:
    rc = native_fp_maxScalarReduce(numParticipants,dC,dR);
    break;
  case fp_addColOpcode:
    rc = native_fp_addScalarReduce(numParticipants,dC,dR);
    if (rc == 0 && non0 == 2) {
      // Avoid rc!=0 to avoid +inf-inf gives NAN.
      // Only expect exact answer for 2 non-0 contributions.
      if (   dR    == mySum
          || dR    == nextafter(mySum,dR)
	  || mySum == nextafter(dR,mySum) ) {
        rc = 0;
      } else {
        rc = verificationFailBGNdReduce;
      }
      //      printf("dR=%016llx cpp_mySum=%016llx rc=%016llx\n", *(long long unsigned int*)(void*)&dR, *(long long unsigned int*)(void*)&mySum, *(long long unsigned int*)(void*)&rc);
    }
    //    printf("dR=%016llx cpp_mySum=%016llx rc=%016llx non0=%d\n", *(long long unsigned int*)(void*)&dR, *(long long unsigned int*)(void*)&mySum, *(long long unsigned int*)(void*)&rc, non0);
    break;
  default:
    return invalidOpCodeBGNdReduce;
    break;
  }

  external_r32[0] = u_endian32(getFlip(),internal_r32[msw]);
  external_r32[1] = u_endian32(getFlip(),internal_r32[lsw]);

  return rc;

}


uint64_t BgNDReduce::verify_uint_ScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
    ) {
  if (elementWords != 2) {
    return 0;
  }
  if (!numParticipants) {
    return 0ull;
  }

  // On bgq, order within double is [msw][lsw], on x86 the order is [lsw][msw]
  int msw = isBGQendian() ? 0 : 1;
  int lsw = isBGQendian() ? 1 : 0;

  uint64_t dC[numParticipants];
  volatile uint64_t dR;
  uint32_t *internal_r32 = (uint32_t *)(void *)&dR;
  uint32_t *external_r32 = (uint32_t *)Result;

  uint64_t myReduce = 0;
  for (int p = 0; p<numParticipants; p++) {
    uint32_t *c = (uint32_t *)(void *)&(dC[p]);
    if ( opCode==u_addColOpcode && vFlagsContribution && (vFlagsContribution[p] & NdOrderBGNdReduce )  ) {
      // In the network, first lsw, then msw.
      c[lsw] = u_endian32(getFlip(), *( (uint32_t*)(vContribution[p]) + offset    ) );
      c[msw] = u_endian32(getFlip(), *( (uint32_t*)(vContribution[p]) + offset + 1) );
    } else {
      c[msw] = u_endian32(getFlip(), *( (uint32_t*)(vContribution[p]) + offset    ) );
      c[lsw] = u_endian32(getFlip(), *( (uint32_t*)(vContribution[p]) + offset + 1) );
    }
    if (p==0) {
      myReduce = dC[p];
    } else {
      switch(opCode) {
      case u_minColOpcode:
        myReduce = min(myReduce,dC[p]);
        break;
      case u_maxColOpcode:
        myReduce = max(myReduce,dC[p]);
        break;
      case u_addColOpcode:
        myReduce += dC[p];
        break;
      default:
        return invalidOpCodeBGNdReduce;
        break;
      }
    }
  }

#if 0
  // Even with dR declared volatile, the following did not work.
  internal_r32[msw] = u_endian32(getFlip(),external_r32[0]);
  internal_r32[lsw] = u_endian32(getFlip(),external_r32[1]);
#else
  {
    uint32_t u0 = u_endian32(getFlip(),external_r32[0]) ;
    uint32_t u1 = u_endian32(getFlip(),external_r32[1]);
    // 20100717 - vFlagsContribution seems wrong in line below:        if ( opCode==u_addColOpcode && vFlagsContribution && (vFlagsResult & NdOrderBGNdReduce )  ) {
    if ( opCode==u_addColOpcode && (vFlagsResult & NdOrderBGNdReduce )  ) {
      // In the network, first lsw, then msw.
      memcpy( &internal_r32[lsw] , &u0, 4);
      memcpy( &internal_r32[msw] , &u1, 4);
    } else {
      memcpy( &internal_r32[msw] , &u0, 4);
      memcpy( &internal_r32[lsw] , &u1, 4);
    }
  }
#endif

  if ( dR != myReduce) {
    //    printf("numParticipants=%d opCode=%d dR=%016llx myReduce=%016llx dC[0]=%016llx\n",numParticipants, opCode, *(long long unsigned int*)(void*)&dR, *(long long unsigned int*)(void*)&myReduce, *(long long unsigned int*)(void*)&dC[0]);
    return verificationFailBGNdReduce;
  }



  return 0;
}


uint64_t BgNDReduce::verify_int_ScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
    ) {
  if (elementWords != 2) {
    return 0;
  }
  if (!numParticipants) {
    return 0ull;
  }

  // On bgq, order within double is [msw][lsw], on x86 the order is [lsw][msw]
  int msw = isBGQendian() ? 0 : 1;
  int lsw = isBGQendian() ? 1 : 0;

  int64_t dC[numParticipants];
  volatile int64_t dR;
  uint32_t *internal_r32 = (uint32_t *)(void *)&dR;
  uint32_t *external_r32 = (uint32_t *)Result;

  int64_t myReduce = 0;
  for (int p = 0; p<numParticipants; p++) {
    uint32_t *c = (uint32_t *)(void *)&(dC[p]);
    if ( opCode==addColOpcode && vFlagsContribution && (vFlagsContribution[p] & NdOrderBGNdReduce )  ) {
      // In the network, first lsw, then msw.
      c[lsw] = u_endian32(getFlip(), *( (uint32_t*)(vContribution[p]) + offset    ) );
      c[msw] = u_endian32(getFlip(), *( (uint32_t*)(vContribution[p]) + offset + 1) );
    } else {
      c[msw] = u_endian32(getFlip(), *( (uint32_t*)(vContribution[p]) + offset    ) );
      c[lsw] = u_endian32(getFlip(), *( (uint32_t*)(vContribution[p]) + offset + 1) );
    }
    if (p==0) {
      myReduce = dC[p];
    } else {
      switch(opCode) {
      case minColOpcode:
        myReduce = min(myReduce,dC[p]);
        break;
      case maxColOpcode:
        myReduce = max(myReduce,dC[p]);
        break;
      case addColOpcode:
        myReduce += dC[p];
        break;
      default:
        return invalidOpCodeBGNdReduce;
        break;
      }
    }
  }

#if 0
  // Even with dR declared volatile, the following did not work.
  internal_r32[msw] = u_endian32(getFlip(),external_r32[0]);
  internal_r32[lsw] = u_endian32(getFlip(),external_r32[1]);
#else
  {
    uint32_t u0 = u_endian32(getFlip(),external_r32[0]) ;
    uint32_t u1 = u_endian32(getFlip(),external_r32[1]);
// 20100717 - vFlagsContribution seems wrong in line below:            if ( opCode==addColOpcode && vFlagsContribution && (vFlagsResult & NdOrderBGNdReduce )  ) {
    if ( opCode==addColOpcode && (vFlagsResult & NdOrderBGNdReduce )  ) {
      // In the network, first lsw, then msw.
      memcpy( &internal_r32[lsw] , &u0, 4);
      memcpy( &internal_r32[msw] , &u1, 4);
    } else {
      memcpy( &internal_r32[msw] , &u0, 4);
      memcpy( &internal_r32[lsw] , &u1, 4);
    }
  }
#endif

  if ( dR != myReduce) {
    //    printf("numParticipants=%d opCode=%d dR=%016llx myReduce=%016llx dC[0]=%016llx\n",numParticipants, opCode, *(long long unsigned int*)(void*)&dR, *(long long unsigned int*)(void*)&myReduce, *(long long unsigned int*)(void*)&dC[0]);
    return verificationFailBGNdReduce;
  }



  return 0;
}


uint64_t BgNDReduce::native_fp_addScalarReduce(
    int numParticipants,
    double *vContribution,
    double &Result) {

  if (numParticipants>15) {
    // 15 is the max for which can avoid overflow.
    return fp_addParticipantsGT15BGNdReduce;
  }

  uint64_t *cU64 = (uint64_t*)(void *)vContribution;
  int pInf = 0;
  int nInf = 0;

  for (int p = 0; p<numParticipants; p++) {
    if (isnan(vContribution[p])) {
      Result = vContribution[p];
      return NaNinputBGNdReduce;
    }

    if (isinf(vContribution[p])) {
      if (signbit(vContribution[p])) {
	++nInf;
      } else {
	++pInf;
      }
    }
  }

  // IEEE says +Inf + -Inf is Nan
  if (nInf && pInf) {
    Result = nan("0"); // 20090623 - This gives the expected 0x7ff80000 00000000
    assert(isnan(Result));
    return NaNcreatedBGNdReduce;
  }

  if (nInf) {
    Result = -INFINITY;
    assert(isinf(Result) &&  signbit(Result));
    return 0; // Q: Should input Inf be flagged as exception?
  } else if (pInf) {
    Result =  INFINITY;
    assert(isinf(Result) && !signbit(Result));
    return 0; // Q: Should input Inf be flagged as exception?
  }


  int maxE = 0;
  uint64_t mantU64[numParticipants];
  int effectiveE[numParticipants]; // Denorms have an effective shift of E=1.
  for (int p = 0; p<numParticipants; p++) {
    effectiveE[p] = max((uint64_t)1,fp_exp(cU64[p]) );
    maxE = max( maxE, effectiveE[p] );
    mantU64[p] = fp_mant(cU64[p]);
    if ( fp_exp(cU64[p]) ) {
      // Only if exp>0 is there an implicit 1.
      // Using fp_set_exp() to set explicit 1 in mantisa.
      fp_set_exp(1,mantU64[p]);
    }
  }

  int firstSumMantBit = 5;
  int shiftLeftExtra = 11-firstSumMantBit; // 6
  int64_t sum = 0;
  int shift = 100;
  for (int p = 0; p<numParticipants; p++) {
    // +shift is to the right
    // -shift is to the left
    // Max normal without shift would have MSB at position 11.
    // but avoiding overflow allows MSB at position 5, so shift left extra 6 bits.
    shift = maxE - effectiveE[p] - shiftLeftExtra;
    assert(shift >= -shiftLeftExtra);
    if (abs(shift) >52) {
      // 20090606 saw shift==1057 produce result 0x0000000000080000
      // 20090622 saw shift==64 produce crap.
      //    if (abs(shift) >=64) { // would pobably be good enough.
      mantU64[p] = 0;
    } else if (shift >= 0) {
      mantU64[p] = mantU64[p] >>  shift;
    } else {
      mantU64[p] = mantU64[p] << -shift;
    }
    if (signbit(vContribution[p])) {
      sum -= mantU64[p];
    } else {
      sum += mantU64[p];
    }
    //    printf("NORMAL p=%d sum=%016llx shift=%d maxE=%d mantU64[p]=%016llx\n",p, *(long long unsigned int*)(void*)&sum, shift, maxE, *(long long unsigned int*)(void*)&(mantU64[p]));
  }
  // WARNING! Below shift is for the the last participant.

  uint64_t absSum = (sum >= 0) ? sum : -sum;    /*abs(sum);*/       // aix had a headache with the abs function
  int first = firstBit(absSum);
  if (first == -1) {
    // If no bit found, then the answer is 0.
    Result = 0;
    return 0;
  }

  int newSign = sum<0 ? 1 : 0;

  // Max normal started at bit firstSumMantBit, so if at firstSumMantBit then keep same exp.
  int newExp = maxE + (firstSumMantBit-first); // Initial guess.

  if (newExp>=2047) {
    if (newSign) {
      Result = -INFINITY;
      assert(isinf(Result) &&  signbit(Result));
    } else {
      Result =  INFINITY;
      assert(isinf(Result) && !signbit(Result));
    }
    return 0; // Q: Should output Inf be flagged as over,undeflow exception?
  }

  int oshift = 100;
  uint64_t newMant;
  // A normalized fp would have implicit firstBit == 11
  if (newExp>0) {
    // Normal result
    //    printf("NORMAL newExp=%d first=%d absSUM=%016llx shift=%d maxE=%d\n",newExp, first, (long long unsigned)absSum, shift, maxE);
    if (11-first>=0) {
      // shift absSum to the right
      newMant = absSum >> (11-first);
    } else {
      // shift absSum to the left
      newMant = absSum << (first-11);
    }
    assert(fp_exp(newMant)==1);
    assert(firstBit(newMant)==11);
    fp_set_exp(0,newMant); // Delete the implicit 1 bit.
  } else {
    // Denormal result
    // Have to shift RIGHT shiftLeftExtra due to move from 11 to firstSumMantBit
    // If maxE was >1, then have to shift LEFT (maxE-1) such that new effective exp is 1.
    // +shift is to the right
    // -shift is to the left
    oshift = (shiftLeftExtra + 1 - maxE);
    //    printf("DENORM newExp=%d first=%d absSUM=%016llx shift=%d  maxE=%d\n",newExp, first, (long long unsigned)absSum, shift, maxE);
    if (oshift >= 0) {
      newMant = absSum >>  oshift;
    } else {
      newMant = absSum << -oshift;
    }
    //    printf("2DENORM newExp=%d first=%d newMant=%016llx shift=%d maxE=%d\n",newExp, first, (long long unsigned)newMant, shift, maxE);
    assert(fp_exp(newMant)==0);
    assert(newMant==0 || firstBit(newMant)>11);
    newExp = 0;
  }
  //  printf("ALL newExp=%d first=%d newMant=%016llx shift=%d maxE=%d\n",newExp, first, (long long unsigned)newMant, shift, maxE);
  fflush(NULL);

  uint64_t *uR = (uint64_t*)(void *)&Result;

  fp_set_sign(newSign, *uR);
  fp_set_exp( newExp,  *uR);
  fp_set_mant(newMant, *uR);

  return 0;
}



int BgNDReduce::read12fromNuts(char *filename, int max, uint32_t *(d[12]), uint32_t answer[], uint32_t rec[]) {
  // file expected to contain 1 or more instances of the following 3 lines. Value of j is ignored.
  // 'Error' words (if any) must be deleted.
  //   j=3 answer_uints=  0xb25c76ca  rec_uints=  0xb25c76ca 
  //   inputs= 0x00000000 0x00000000 0x00000000 0x00000000 0x00000000 0x00000000
  //   inputs= 0x00000000 0x00000000 0x00000000 0x00000000 0x00000000 0xb25c76ca
  FILE *fp = fopen(filename,"r");
  if (!fp) return -1;

  int i;
  for (i=0; i<max; i++) {
    int j;
    int r;
    r = fscanf(fp," j=%d answer_uints= %x  rec_uints=  %x ", &j, &(answer[i]), &(rec[i]));
    //    printf(" j=%d answer_uints= %x  rec_uints=  %x \n", j, answer[i], rec[i]);
    //    if (r!=3) return -1000000 -i*1000 - r*100; // break;
    if (r!=3) break;
    r = fscanf(fp," inputs= %x %x %x %x %x %x", d[0]+i, d[1]+i, d[2]+i, d[3]+i, d[ 4]+i, d[ 5]+i);
    //    printf("inputs= %x %x %x %x %x %x\n", *(d[0]+i), *(d[1]+i), *(d[2]+i), *(d[3]+i), *(d[ 4]+i), *(d[ 5]+i));
    //    if (r!=6) return -100000 -i*1000 - r*10; // break;
    if (r!=6) break;
    r = fscanf(fp," inputs= %x %x %x %x %x %x", d[6]+i, d[7]+i, d[8]+i, d[9]+i, d[10]+i, d[11]+i);
    //    printf("inputs= %x %x %x %x %x %x\n", *(d[6]+i), *(d[7]+i), *(d[8]+i), *(d[9]+i), *(d[10]+i), *(d[11]+i));
    //    if (r!=6) return -10000 -i*1000 - r; // break;
    if (r!=6) break;
  }
  fclose(fp);
  return i;
}
