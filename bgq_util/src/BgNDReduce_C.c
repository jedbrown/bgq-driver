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

// All comments and documentation in bgq_util/include/BgNDReduce_C.h
// Except for *multiNode* routines, this is the later C port of the original C++ BgNDReduce.cc

#include "BgEndian.h"
#include "BgNDReduce_C.h"
#include "PRINT_ASSERT.h"

#include <ctype.h> // isdigit() isspace()
//#include <fwext_lib.h>
extern long          fwext_strtol(const char* s, char** endp, int base);
extern unsigned long fwext_strtoul(const char* s, char** endp, int base);
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "assert.h"
#include "math.h"   // isnan() and isinf( ) no longer used because not in early q firmware.

__BEGIN_DECLS

int BgNDReduce_x86endian = 0;

uint64_t BgNDReduce_vectorReduce(
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
  //  printf("BgNDReduce_vectorReduce Tree=%p\n",Tree);
  uint64_t r = 0;
  for (int e=0; e<numElements; e++) {
    // *((uint32_t *)vResult + e*elementWords)= __LINE__; // Unexpected get 0 back.
    vExceptions[e] = BgNDReduce_scalarReduce(e*elementWords, elementWords, opCode, numParticipants, Tree, vContribution, vFlagsContribution, vFlagsResult, (uint32_t *)vResult + e*elementWords);
    r |= vExceptions[e];
    //    *((uint32_t *)vResult + e*elementWords)= __LINE__; // as expected.
  }
  return r;
}

uint64_t BgNDReduce_scalarReduce(
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
  //    printf("BgNDReduce_scalarReduce Tree=%p\n",Tree);
  uint64_t rc = 0;
  switch(opCode) {
  case andColOpcode:
  case  orColOpcode:
  case xorColOpcode:
    rc = BgNDReduce_logicOpScalarReduce(offset,elementWords,opCode,numParticipants,vContribution,vFlagsContribution, vFlagsResult, Result);
    break;
  case u_minColOpcode:
  case u_maxColOpcode:
  case   minColOpcode:
  case   maxColOpcode:
    rc = BgNDReduce_minORmaxScalarReduce(offset,elementWords,opCode,numParticipants,vContribution,vFlagsContribution, vFlagsResult, Result);
    break;
  case u_addColOpcode:
    rc =    BgNDReduce_u_addScalarReduce(offset,elementWords,opCode,numParticipants,vContribution,vFlagsContribution, vFlagsResult, Result);
    break;
  case   addColOpcode:
    rc =      BgNDReduce_addScalarReduce(offset,elementWords,opCode,numParticipants, Tree, vContribution,vFlagsContribution, vFlagsResult, Result);
    break;
  case fp_minColOpcode:
  case fp_maxColOpcode:
  case fp_addColOpcode:
    //    printf("Tree=%p\n",Tree);
    rc =    BgNDReduce_fp_ScalarReduce(offset,elementWords,opCode,numParticipants,vContribution,vFlagsContribution, vFlagsResult, Result);
    break;
  default:
    return invalidOpCodeBGNdReduce;
    break;
  }

  switch(opCode) {
  case u_minColOpcode:
  case u_maxColOpcode:
  case u_addColOpcode:
    rc |= BgNDReduce_verify_uint_ScalarReduce(offset,elementWords,opCode,numParticipants,vContribution,vFlagsContribution, vFlagsResult, Result);
    break;
  case   minColOpcode:
  case   maxColOpcode:
  case   addColOpcode:
    rc |= BgNDReduce_verify_int_ScalarReduce(offset,elementWords,opCode,numParticipants,vContribution,vFlagsContribution, vFlagsResult, Result);
    break;
  default:
    break;
  }

  return rc;
}

uint64_t BgNDReduce_logicOpScalarReduce(
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


int BgNDReduce_getFlip(void) {
  int flip = !isBGendian();
  if        (BgNDReduce_x86endian ==  0) {
    // No change
  } else if (BgNDReduce_x86endian == 32) {
    flip = !flip;
  } else {
    return illegalx86endianBGNdReduce;
  }
  return flip;
}


uint64_t BgNDReduce_minORmaxScalarReduce(
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
    uint32_t winValue = maybe_uint32_EndianFlip(BgNDReduce_getFlip(),*((uint32_t*)(vContribution[winParticipant]) + offset + e));
    // Start at p=0 so that also 0 is a potentialWinner
    for (int p = 0; p<numParticipants; p++) {
      if (!potentialWinner[p]) continue;
      uint32_t pValue = maybe_uint32_EndianFlip(BgNDReduce_getFlip(),*((uint32_t*)(vContribution[p]) + offset + e));
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
    *u = maybe_uint32_EndianFlip(BgNDReduce_getFlip(),winValue);
  }

  return 0ull;
}


uint64_t BgNDReduce_u_addScalarReduce(
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
      uint32_t pValue = maybe_uint32_EndianFlip(BgNDReduce_getFlip(),*((uint32_t*)(vContribution[p]) + offset + eIndex));
	sum += pValue;
    }
    int eResultIndex = vFlagsResult & NdOrderBGNdReduce ? e : elementWords - 1 - e;
    uint32_t *u = (uint32_t *)Result + eResultIndex;
    *u = maybe_uint32_EndianFlip(BgNDReduce_getFlip(), 0x00000000FFFFFFFFull & sum);
    carry = sum >> 32;
  }

  if (carry) {
    return u_addOverflowBGNdReduce;
  }

  return 0ull;
}


// inCarry[], outCarry[], signedOverflow[] are indexed by adder. There is 1 less adder than participants.

uint64_t BgNDReduce_treeAddScalarReduce(
    int numParticipants,
    char *Tree,
    uint32_t *inCarry,
    uint32_t *outCarry,
    uint32_t *signedOverflow,
    uint32_t *vContribution,
    uint32_t *Result
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
  //  stack <uint32_t> st;
  int      stMAX_ELEMENTS = numParticipants;
  uint32_t stARRAY[stMAX_ELEMENTS];
  int      stINDEX = -1;
#define stackPUSH(A,MAX_E,INDEX,VALUE)    do { assert(INDEX+1<MAX_E && INDEX>=-1); A[++INDEX]=VALUE;} while (0)
#define stackPOP( A,MAX_E,INDEX,VARIABLE) do { assert(INDEX  <MAX_E && INDEX>= 0); VARIABLE=A[INDEX--];} while (0)
#define stPUSH(VALUE)    stackPUSH(stARRAY,stMAX_ELEMENTS,stINDEX,VALUE   )
#define stPOP( VARIABLE) stackPOP( stARRAY,stMAX_ELEMENTS,stINDEX,VARIABLE)
#define stSIZE() (assert(stINDEX  <stMAX_ELEMENTS && stINDEX>= -1), stINDEX+1)
#define stTOP()  (assert(stINDEX  <stMAX_ELEMENTS && stINDEX>=  0), stARRAY[stINDEX])

  char *pTree = Tree;
  int participant = -1;
  while (*pTree) {
    if (isdigit(*pTree)) {
	char *next;
	participant = strtol(pTree,&next,10);
	//	participant = fwext_strtoul(pTree,&next,10);
	if (next<=pTree) {
	  return strtolTroubleBGNdReduce;
	}
	if (participant >= numParticipants) {
	  return illegalParticipantBGNdReduce;
	}
	pTree = next;
    } else if (   *pTree==' ' // isspace(*pTree)
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
	//	st.push(vContribution[participant]);
	stPUSH(vContribution[participant]);
	done[participant] = 1;
        participant = -1;
	++pTree;
    } else if (*pTree=='+') {
	if (participant >=0) {
	  return unexpectedParticipantBGNdReduce;
	}
	//	if (st.size() < 2) {
	if (stSIZE() < 2) {
	  return lt2OnStackBGNdReduce;
	}
	//	uint32_t a = st.top();
	//	st.pop();
	uint32_t a; stPOP(a);
	//	uint32_t b = st.top();
	//	st.pop();
	uint32_t b; stPOP(b);
	// http://en.wikipedia.org/wiki/Two's_complement
	// An overflow condition exists when a carry (an extra 1) is generated into but not out of the far left bit (the MSB), or out of but not into the MSB. As mentioned above, the sign of the number is encoded in the MSB of the result.
	uint64_t sum = (uint64_t)a + (uint64_t)b + (uint64_t)inCarry[whichCarry];
	uint32_t sum32 = sum & 0xFFFFFFFFu;
	outCarry[whichCarry] = sum >> 32;
	uint32_t carryIntoMSB = (a&0x80000000u) ^ (b&0x80000000u) ^ (sum&0x80000000u);
	signedOverflow[whichCarry] = (carryIntoMSB && !outCarry[whichCarry]) || (!carryIntoMSB && outCarry[whichCarry]);
	//	st.push(sum32);
	stPUSH(sum32);
	++whichCarry;
	++pTree;
    } else {
      return illegalCharBGNdReduce;
    }
  }
  if (whichCarry != numParticipants-1) {
    return whichCarryTroubleBGNdReduce;
  }
  //  if (st.size() < 1) {
  if (stSIZE() < 1) {
    return nothingOnStackBGNdReduce;
  }
  //  if (st.size() > 1) {
  if (stSIZE() > 1) {
    return tooMuchOnStackBGNdReduce;
  }
  for (int p = 0; p<numParticipants; p++) {
    if (done[p] == 0) {
      return unusedParticipantBGNdReduce;
    }
  }
  //  *Result = st.top();
  //  st.pop();
  stPOP(*Result);

  return 0ull;
}


uint64_t BgNDReduce_addScalarReduce(
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
      iC[p] = maybe_uint32_EndianFlip(BgNDReduce_getFlip(),*((uint32_t*)(vContribution[p]) + offset + eIndex));
    }
    rc |= BgNDReduce_treeAddScalarReduce(numParticipants, Tree, inCarry, outCarry, signedOverflow, iC, &sum32);
  
    int eResultIndex = vFlagsResult & NdOrderBGNdReduce ? e : elementWords - 1 - e;
    uint32_t *u = (uint32_t *)Result + eResultIndex;
    *u = maybe_uint32_EndianFlip(BgNDReduce_getFlip(), sum32);

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
uint64_t BgNDReduce_native_fp_minScalarReduce(
    int numParticipants,
    double *vContribution,
    double *Result
    ) {

  *Result = vContribution[0];

  for (int p = 1; p<numParticipants; p++) {
    if (   *Result == vContribution[p]
	   && *(uint64_t*)Result != *(uint64_t*)&vContribution[p]) {
      // If the assert below fails, then use union for: *(uint64_t*)Result != *(uint64_t*)&vContribution[p]
      assert(*Result == 0 && vContribution[p]==0);
      // IEEE says +0 == -0,so min(-0,+0) is free to return either.
      // BGQ says min(-0,+0) is -0, which is allowed by IEEE.
      fp_set_sem(1,0,0,(uint64_t*)Result);
    } else {
#define min(a,b) (((a)<(b))?(a):(b))
      *Result = min(*Result, vContribution[p]);
    }
  }

  return 0ull;
}


// Do NOT pass in Nan? Or is Nan==Nan never true?
uint64_t BgNDReduce_native_fp_maxScalarReduce(
    int numParticipants,
    double *vContribution,
    double *Result
    ) {

  *Result = vContribution[0];

  for (int p = 1; p<numParticipants; p++) {
    if (   *Result == vContribution[p]
	   && *(uint64_t*)Result != *(uint64_t*)&vContribution[p]) {
      // If the assert below fails, then use union for: *(uint64_t*)Result != *(uint64_t*)&vContribution[p]
      assert(*Result == 0 && vContribution[p]==0);
      // IEEE says +0 == -0,so max(-0,+0) is free to return either.
      // BGQ says max(-0,+0) is +0, which is allowed by IEEE.
      fp_set_sem(0,0,0,(uint64_t*)Result);
    } else {
      *Result = max(*Result, vContribution[p]);
    }
  }

  return 0ull;
}


uint64_t BgNDReduce_fp_ScalarReduce(
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
  int msw = isBGendian() ? 0 : 1;
  int lsw = isBGendian() ? 1 : 0;

  union {double dC; uint32_t u32C[2]; uint64_t u64;} u[numParticipants];
  double dR;
  uint32_t *internal_r32 = (uint32_t *)(void *)&dR; // WARNING! Dangerous cast. Consider using union.
  uint32_t *external_r32 = (uint32_t *)Result;

  int non0 = 0;
  double mySum = 0;
  for (int p = 0; p<numParticipants; p++) {
    u[p].u32C[msw] = maybe_uint32_EndianFlip(BgNDReduce_getFlip(), *( (uint32_t*)(vContribution[p]) + offset    ) );
    u[p].u32C[lsw] = maybe_uint32_EndianFlip(BgNDReduce_getFlip(), *( (uint32_t*)(vContribution[p]) + offset + 1) );
    
    if (fp_is_Nan(u[p].u64)) {
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

    if (u[p].dC != 0) {
      ++non0;
    }
    mySum += u[p].dC;
  }

  uint64_t rc;

  switch(opCode) {
  case fp_minColOpcode:
    rc = BgNDReduce_native_fp_minScalarReduce(numParticipants,&u[0].dC,&dR);
    break;
  case fp_maxColOpcode:
    rc = BgNDReduce_native_fp_maxScalarReduce(numParticipants,&u[0].dC,&dR);
    break;
  case fp_addColOpcode:
    rc = BgNDReduce_native_fp_addScalarReduce(numParticipants,&u[0].dC,&dR);
    if (rc == 0 && non0 == 2) {
      // Avoid rc!=0 to avoid +inf-inf gives NAN.
      // Only expect exact answer for 2 non-0 contributions.
      // WARNING! CANNOT SIMPLY IGNORE LAST BIT, SINCE CARY CAN CHANGE MANY BITS!
      if (   dR    == mySum
	      || dR    == nextafter(mySum,dR)
		|| mySum == nextafter(dR,mySum)  ) {
        rc = 0;
      } else {
	//	    printf("dR=%016llx cpp_mySum=%016llx rc=%016llx non0=%d\n", *(long long unsigned int*)(void*)&dR, *(long long unsigned int*)(void*)&mySum, *(long long unsigned int*)(void*)&rc, non0);
        rc = verificationFailBGNdReduce;
      }
      //      printf("dR=%016llx cpp_mySum=%016llx rc=%016llx\n", *(long long unsigned int*)(void*)&dR, *(long long unsigned int*)(void*)&mySum, *(long long unsigned int*)(void*)&rc);
    }
    break;
  default:
    return invalidOpCodeBGNdReduce;
    break;
  }

  external_r32[0] = maybe_uint32_EndianFlip(BgNDReduce_getFlip(),internal_r32[msw]);
  external_r32[1] = maybe_uint32_EndianFlip(BgNDReduce_getFlip(),internal_r32[lsw]);

  return rc;

}


uint64_t BgNDReduce_verify_uint_ScalarReduce(
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
  int msw = isBGendian() ? 0 : 1;
  int lsw = isBGendian() ? 1 : 0;

  uint64_t dC[numParticipants];
  volatile uint64_t dR;
  uint32_t *internal_r32 = (uint32_t *)(void *)&dR;  // WARNING! Maybe dangerous cast. Consider using union.
  uint32_t *external_r32 = (uint32_t *)Result;

  uint64_t myReduce = 0;
  for (int p = 0; p<numParticipants; p++) {
    uint32_t *c = (uint32_t *)(void *)&(dC[p]);
    if ( opCode==u_addColOpcode && vFlagsContribution && (vFlagsContribution[p] & NdOrderBGNdReduce )  ) {
      //printf("%d\n",__LINE__);
      // In the network, first lsw, then msw.
      c[lsw] = maybe_uint32_EndianFlip(BgNDReduce_getFlip(), *( (uint32_t*)(vContribution[p]) + offset    ) );
      c[msw] = maybe_uint32_EndianFlip(BgNDReduce_getFlip(), *( (uint32_t*)(vContribution[p]) + offset + 1) );
    } else {
      //printf("%d\n",__LINE__);
      c[msw] = maybe_uint32_EndianFlip(BgNDReduce_getFlip(), *( (uint32_t*)(vContribution[p]) + offset    ) );
      c[lsw] = maybe_uint32_EndianFlip(BgNDReduce_getFlip(), *( (uint32_t*)(vContribution[p]) + offset + 1) );
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
  internal_r32[msw] = maybe_uint32_EndianFlip(BgNDReduce_getFlip(),external_r32[0]);
  internal_r32[lsw] = maybe_uint32_EndianFlip(BgNDReduce_getFlip(),external_r32[1]);
#else
  {
    uint32_t u0 = maybe_uint32_EndianFlip(BgNDReduce_getFlip(),external_r32[0]) ;
    uint32_t u1 = maybe_uint32_EndianFlip(BgNDReduce_getFlip(),external_r32[1]);
    // 20100717 - vFlagsContribution seems wrong in line below:    if ( opCode==u_addColOpcode && vFlagsContribution && (vFlagsResult & NdOrderBGNdReduce )  ) {
    if ( opCode==u_addColOpcode && (vFlagsResult & NdOrderBGNdReduce )  ) {
      //printf("%d\n",__LINE__);
      // In the network, first lsw, then msw.
      memcpy( &internal_r32[lsw] , &u0, 4);
      memcpy( &internal_r32[msw] , &u1, 4);
    } else {
      //printf("%d\n",__LINE__);
      memcpy( &internal_r32[msw] , &u0, 4);
      memcpy( &internal_r32[lsw] , &u1, 4);
    }
  }
#endif

  //  printf("BgNDReduce_x86endian=%d BgNDReduce_getFlip()=%d msw=%d lsw=%d vFlagsContribution=%llx 0=%llx 1=%llx\n",BgNDReduce_x86endian,BgNDReduce_getFlip(),msw,lsw, (unsigned long long)vFlagsContribution, (long long)(vFlagsContribution ? vFlagsContribution[0]:-1), (long long)(vFlagsContribution ? vFlagsContribution[1]:-2));
  //  printf("%s: numParticipants=%d opCode=%d dR=%016llx myReduce=%016llx dC[0]=%016llx dC[1]=%016llx\n",__func__,numParticipants, opCode, *(long long unsigned int*)(void*)&dR, *(long long unsigned int*)(void*)&myReduce, *(long long unsigned int*)(void*)&dC[0], *(long long unsigned int*)(void*)&dC[1]);
  if ( dR != myReduce) {
    //    printf("%s: numParticipants=%d opCode=%d dR=%016llx myReduce=%016llx dC[0]=%016llx\n",__func__,numParticipants, opCode, *(long long unsigned int*)(void*)&dR, *(long long unsigned int*)(void*)&myReduce, *(long long unsigned int*)(void*)&dC[0]);
    return verificationFailBGNdReduce;
  }



  return 0;
}


uint64_t BgNDReduce_verify_int_ScalarReduce(
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
  int msw = isBGendian() ? 0 : 1;
  int lsw = isBGendian() ? 1 : 0;

  int64_t dC[numParticipants];
  volatile int64_t dR;
  uint32_t *internal_r32 = (uint32_t *)(void *)&dR; // WARNING! Maybe dangerous cast. Consider using union.
  uint32_t *external_r32 = (uint32_t *)Result;

  int64_t myReduce = 0;
  for (int p = 0; p<numParticipants; p++) {
    uint32_t *c = (uint32_t *)(void *)&(dC[p]);
    if ( opCode==addColOpcode && vFlagsContribution && (vFlagsContribution[p] & NdOrderBGNdReduce )  ) {
      // In the network, first lsw, then msw.
      c[lsw] = maybe_uint32_EndianFlip(BgNDReduce_getFlip(), *( (uint32_t*)(vContribution[p]) + offset    ) );
      c[msw] = maybe_uint32_EndianFlip(BgNDReduce_getFlip(), *( (uint32_t*)(vContribution[p]) + offset + 1) );
    } else {
      c[msw] = maybe_uint32_EndianFlip(BgNDReduce_getFlip(), *( (uint32_t*)(vContribution[p]) + offset    ) );
      c[lsw] = maybe_uint32_EndianFlip(BgNDReduce_getFlip(), *( (uint32_t*)(vContribution[p]) + offset + 1) );
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
  internal_r32[msw] = maybe_uint32_EndianFlip(BgNDReduce_getFlip(),external_r32[0]);
  internal_r32[lsw] = maybe_uint32_EndianFlip(BgNDReduce_getFlip(),external_r32[1]);
#else
  {
    uint32_t u0 = maybe_uint32_EndianFlip(BgNDReduce_getFlip(),external_r32[0]) ;
    uint32_t u1 = maybe_uint32_EndianFlip(BgNDReduce_getFlip(),external_r32[1]);
    // 20100717 - vFlagsContribution seems wrong in line below:    if ( opCode==addColOpcode && vFlagsContribution && (vFlagsResult & NdOrderBGNdReduce )  ) {
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

  //  printf("%s: numParticipants=%d opCode=%d dR=%016llx myReduce=%016llx dC[0]=%016llx\n",__func__,numParticipants, opCode, *(long long unsigned int*)(void*)&dR, *(long long unsigned int*)(void*)&myReduce, *(long long unsigned int*)(void*)&dC[0]);
  if ( dR != myReduce) {
    //printf("%s: numParticipants=%d opCode=%d dR=%016llx myReduce=%016llx dC[0]=%016llx\n",__func__,numParticipants, opCode, *(long long unsigned int*)(void*)&dR, *(long long unsigned int*)(void*)&myReduce, *(long long unsigned int*)(void*)&dC[0]);
    return verificationFailBGNdReduce;
  }



  return 0;
}


uint64_t BgNDReduce_native_fp_addScalarReduce(
    int numParticipants,
    double *vContribution,
    double *Result) {

  if (numParticipants>15) {
    // 15 is the max for which can avoid overflow.
    return fp_addParticipantsGT15BGNdReduce;
  }

  uint64_t *cU64 = (uint64_t*)(void *)vContribution; // WARNING! Dangerous cast. Consider using union.
  int pInf = 0;
  int nInf = 0;

  for (int p = 0; p<numParticipants; p++) {
    if (double_is_Nan(vContribution[p])) {
      *Result = vContribution[p];
      return NaNinputBGNdReduce;
    }

    if (double_is_Inf(vContribution[p])) {
      if (signbit(vContribution[p])) {
	++nInf;
      } else {
	++pInf;
      }
    }
  }

  // IEEE says +Inf + -Inf is Nan
  if (nInf && pInf) {
    *Result = nan("0"); // 20090623 - This gives the expected 0x7ff80000 00000000
    assert(double_is_Nan(*Result));
    return NaNcreatedBGNdReduce;
  }

  if (nInf) {
    //    assert(isinf(-INFINITY) &&  signbit(-INFINITY));
    *Result = -INFINITY;
    union {double d; uint64_t u64;} u;
    u.d = *Result;
    PRINT_ASSERT1(fp_is_Inf(u.u64) &&  fp_is_Neg(u.u64),u.u64);
    //    assert(isinf(*Result) &&  signbit(*Result)); // This would fail due to stale data in memory.
    return 0; // Q: Should input Inf raise an exception?
  } else if (pInf) {
    *Result =  INFINITY;
    union {double d; uint64_t u64;} u;
    u.d = *Result;
    PRINT_ASSERT1(fp_is_Inf(u.u64) &&  fp_is_Pos(u.u64),u.u64);
    return 0; // Q: Should input Inf raise an exception?

    // 20100506 troubles with cross-compiler for A2. Seen running on mambo. 20100710 - solved troubles here and elsewhere by going to union.
    // Here and elsewhere, when using old type-casting instead of curent union:
    // - would get from compiler: BgNDReduce_C.c:837: warning: dereferencing type-punned pointer will break strict-aliasing rules
    // - due to stale value, 'if (!cond)' in PRINT_ASSERT() fires due to stale *Result, but assert() gets ok *Resut and thu does not fail. So see PRINT_ASSERT() printout, but the applicaion would not end at that point.
    // For the bad type cast technique, with stale value, declaring 'volatile double *Result', here and in *.h does not help.

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
      fp_set_exp(1,&mantU64[p]);
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
    *Result = 0;
    return 0;
  }

  int newSign = sum<0 ? 1 : 0;

  // Max normal started at bit firstSumMantBit, so if at firstSumMantBit then keep same exp.
  int newExp = maxE + (firstSumMantBit-first); // Initial guess.

  if (newExp>=2047) {
    if (newSign) {
      *Result = -INFINITY;
      assert(double_is_Inf(*Result) &&  signbit(*Result));
    } else {
      *Result =  INFINITY;
      assert(double_is_Inf(*Result) && !signbit(*Result));
    }
    return 0; // Q: Should output Inf raise an over,undeflow exception?
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
    fp_set_exp(0,&newMant); // Delete the implicit 1 bit.
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
  //  fflush(NULL);

  uint64_t *uR = (uint64_t*)(void *)Result; // WARNING! Dangerous cast. Consider using union.

  fp_set_sign(newSign, uR);
  fp_set_exp( newExp,  uR);
  fp_set_mant(newMant, uR);

  return 0;
}



// BEGIN code new to BgNDReduce_C.h, not ported from C++ BgNDReduce.h --------------------------

// Only because memcmp is not (yet) provided.
int Bg_memcmp(void *va, void *vb, int n) {
  char *a = (char*)va;
  char *b = (char*)vb;
  int i;
  for (i=0; i<n; i++) {
    if (a[i]!=b[i]) return 1;
  }
  return 0;
}

  uint64_t BgNDReduce_multiNodeScalarReduce(
    int thisNode,
    int (*willParticipate)(void *parms, int node, int link),
    void *willParticipate_parms,
    int maxParticipant,
    uint64_t **vNodeExceptions,
    void **vNodeResult,

    int participationGeneration,
    int *nodeParticipation,
    int *totalParticipation,

    int offset,
    int elementWords,
    int32_t opCode,
    char *Tree,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
    ) {

    if (maxParticipant<0) {
      return 0;
    }
    assert(thisNode>=0 && thisNode<=maxParticipant);

    uint64_t globalException = 0;

    //    assert(opCode != addColOpcode); // TODO add needs all 12, with 0 contrib where needed, so that over/underflows exceptions occur correctly. Note that result is correct, regardless of order.

    assert(nodeParticipation[thisNode]==participationGeneration);
    ++nodeParticipation[thisNode];
    ++*totalParticipation;
    PRINT_ASSERT3(*totalParticipation<=(participationGeneration+1)*(maxParticipant+1),*totalParticipation,maxParticipant,participationGeneration);
    //    PRINT_INFO2(*totalParticipation,maxParticipant);

    int numParticipants = 0;
#define MAX_PARTICIPANTS_AT_A_NODE 12

    uint32_t localContributions[MAX_PARTICIPANTS_AT_A_NODE][elementWords];
    void *plocalContributions[MAX_PARTICIPANTS_AT_A_NODE];

    uint64_t localFlags[MAX_PARTICIPANTS_AT_A_NODE];

    int link;
    for (link=0; link<MAX_PARTICIPANTS_AT_A_NODE; link++) {
      int otherParticipant = (*willParticipate)(willParticipate_parms,thisNode, link);
      if (otherParticipant >= 0) {
        plocalContributions[numParticipants] = &(localContributions[numParticipants]);
	if (link<11) {
          localFlags[numParticipants] = 0;
	  // contribution from a neighbor node.
	  uint64_t otherException = BgNDReduce_multiNodeScalarReduce(
            otherParticipant,
            willParticipate,
            willParticipate_parms,
            maxParticipant,
            vNodeExceptions,
            vNodeResult,

            participationGeneration,
            nodeParticipation,
            totalParticipation,
            
            offset,
            elementWords,
            opCode,
            Tree,
            vContribution,
            vFlagsContribution,
            localFlags[numParticipants],
            localContributions[numParticipants]
            );
          globalException |= otherException;
          if (vNodeExceptions) {
            // Historical ugliness that offset is in uint32_t units, but here want by element.
            assert(offset%elementWords == 0);
	    // PRINT_INFO2(otherException,otherException);
	    // vNodeExceptions[] contains the local exceptions that happened at that node.
	    // otherExceptions contains all the exceptions that happened at that node or its descndants.
            uint64_t otherLocal = *(vNodeExceptions[otherParticipant] + offset/elementWords); 
	    assert(otherLocal == (otherLocal & otherException)); 
	  }
	  if (vNodeResult) {
	    // Ensure that what I got is indeed in vNodeResult
  	    int cmp = Bg_memcmp(localContributions[numParticipants],
                             (uint32_t*)(vNodeResult[otherParticipant]) + offset,4*elementWords);
	    // PRINT_INFO2(cmp,cmp);
	    assert(cmp==0);
	  }
	} else {
	  // local contribution.
	  assert(link==11);
	  assert(otherParticipant==thisNode);
          localFlags[numParticipants] = vFlagsContribution ? vFlagsContribution[thisNode] : 0;
	  memcpy(localContributions[numParticipants],(uint32_t*)(vContribution[thisNode]) + offset,4*elementWords);
	}
        ++numParticipants;
      }
    }

    assert(numParticipants>0);

    uint64_t localException = BgNDReduce_scalarReduce(
						      0, // no offset, since this is localContribution!
      elementWords,
      opCode,
      numParticipants,
      opCode == addColOpcode ? Tree : NULL,
      plocalContributions,
      localFlags,
      vFlagsResult,
      Result
    );
    // PRINT_INFO2(Result,*(uint32_t*)Result);

    if (vNodeExceptions) {
      // Historical ugliness that offset is in uint32_t units, but here want by element.
      assert(offset%elementWords == 0);
      *(vNodeExceptions[thisNode] + offset/elementWords) = localException;
    }
    if (vNodeResult) {
      memcpy((uint32_t*)(vNodeResult[thisNode]) + offset,Result,4*elementWords);
    }

    globalException |= localException;
    return globalException;
  }


  uint64_t BgNDReduce_multiNodeFlatScalarReduce(
    int thisNode,
    int (*willParticipate)(void *parms, int node, int link),
    void *willParticipate_parms,
    int maxParticipant,
    uint64_t *vFlatNodeExceptions,
    void *vFlatNodeResult,

    int participationGeneration,
    int *nodeParticipation,
    int *totalParticipation,

    int elementWords,
    int32_t opCode,
    char *Tree,
    void *vFlatContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
    ) {

    void *vContribution[maxParticipant+1];
    void *pNodeResults[maxParticipant+1];
    uint64_t *pNodeExceptions[maxParticipant+1];
    int i;
    for (i=0; i<= maxParticipant; i++) {
      // (uint8_t *) use below for ptr arithmetic might not be needed.
      vContribution[i] = (uint8_t *)vFlatContribution + i*(4*elementWords);
      pNodeResults[i] = (uint8_t *)vFlatNodeResult + i*(4*elementWords);
      pNodeExceptions[i] = vFlatNodeExceptions ? (uint64_t *)vFlatNodeExceptions + i : NULL;
    }

    uint64_t rc = BgNDReduce_multiNodeScalarReduce(
      thisNode,
      willParticipate,
      willParticipate_parms,
      maxParticipant,
      vFlatNodeExceptions ? pNodeExceptions : NULL,
      pNodeResults,
    
      participationGeneration,
      nodeParticipation,
      totalParticipation,
    
      0, // offset
      elementWords,
      opCode,
      Tree,
      vContribution,
      vFlagsContribution,
      vFlagsResult,
      Result
      );

    // BELOW A Check on above pointer math. The result must be the same as for this node.
    int pcmp = Bg_memcmp(pNodeResults[thisNode],Result,4*elementWords);
    PRINT_ASSERT(pcmp==0);

    return rc;
  }

  uint64_t BgNDReduce_allNodeScalarReduce(
    int rootNode,
    int (*willParticipate)(void *parms, int node, int link),
    void *willParticipate_parms,
    int maxParticipant,
    uint64_t **vNodeExceptions,
    void **vNodeResult,

    int offset,
    int elementWords,
    int32_t opCode,
    char *Tree,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
    ) {

    if (maxParticipant<0) {
      return 0;
    }
    PRINT_ASSERT2(rootNode>=0 && rootNode<=maxParticipant,rootNode,maxParticipant);

    int participationGeneration = 42; // Better verification using non-0 42. 0 would also work.
    int totalParticipation = participationGeneration*(maxParticipant+1);
    int i;
    int nodeParticipation[maxParticipant+1];
    for (i=0; i<=maxParticipant; i++) {
      nodeParticipation[i]=participationGeneration;
    }

    uint64_t r = BgNDReduce_multiNodeScalarReduce(
    rootNode,
    willParticipate,
    willParticipate_parms,
    maxParticipant,
    vNodeExceptions,
    vNodeResult,
    participationGeneration,
    nodeParticipation,
    &totalParticipation,

    offset,
    elementWords,
    opCode,
    Tree,
    vContribution,
    vFlagsContribution,
    vFlagsResult,
    Result
    );

    PRINT_ASSERT3(totalParticipation == (participationGeneration+1)*(maxParticipant+1),totalParticipation,maxParticipant,participationGeneration);
    for (i=0; i<=maxParticipant; i++) {
      assert(nodeParticipation[i]==(participationGeneration+1));
    }
    return r;
  }

// BgNDReduce_subCommScalarReduce() is based on BgNDReduce_allNodeScalarReduce()
// TODO Implement BgNDReduce_allNodeScalarReduce() using BgNDReduce_subCommScalarReduce(), just needs a vector with all elements true!
  uint64_t BgNDReduce_subCommScalarReduce(
    int numNodesInSubComm,
    int *IsNodeInSubComm,
    int rootNode,
    int (*willParticipate)(void *parms, int node, int link),
    void *willParticipate_parms,
    int maxParticipant,
    uint64_t **vNodeExceptions,
    void **vNodeResult,

    int offset,
    int elementWords,
    int32_t opCode,
    char *Tree,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
    ) {

    if (maxParticipant<0) {
      return 0;
    }
    PRINT_ASSERT2(rootNode>=0 && rootNode<=maxParticipant,rootNode,maxParticipant);

    int checkNumNodesInSubComm = 0;

    int participationGeneration = 42; // Better verification using non-0 42. 0 would also work.
    int totalParticipation = participationGeneration*(numNodesInSubComm);
    int i;
    int nodeParticipation[maxParticipant+1];
    for (i=0; i<=maxParticipant; i++) {
      nodeParticipation[i]=participationGeneration;
      if (IsNodeInSubComm[i]) {
	++checkNumNodesInSubComm;
      }
    }
    PRINT_ASSERT2(checkNumNodesInSubComm==numNodesInSubComm, checkNumNodesInSubComm, numNodesInSubComm);

    uint64_t r = BgNDReduce_multiNodeScalarReduce(
    rootNode,
    willParticipate,
    willParticipate_parms,
    maxParticipant,
    vNodeExceptions,
    vNodeResult,
    participationGeneration,
    nodeParticipation,
    &totalParticipation,

    offset,
    elementWords,
    opCode,
    Tree,
    vContribution,
    vFlagsContribution,
    vFlagsResult,
    Result
    );

    PRINT_ASSERT4(totalParticipation == (participationGeneration+1)*(numNodesInSubComm),totalParticipation,maxParticipant,participationGeneration,numNodesInSubComm);
    for (i=0; i<=maxParticipant; i++) {
      if (IsNodeInSubComm[i]) {
        assert(nodeParticipation[i]==(participationGeneration+1));
      } else {
        assert(nodeParticipation[i]==(participationGeneration  ));
      }
    }
    return r;
  }

  uint64_t BgNDReduce_allNodeFlatScalarReduce(
    int rootNode,
    int (*willParticipate)(void *parms, int node, int link),
    void *willParticipate_parms,
    int maxParticipant,
    uint64_t *vFlatNodeExceptions,
    void *vFlatNodeResult,

    int elementWords,
    int32_t opCode,
    char *Tree,
    void *vFlatContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
    ) {

    void *vContribution[maxParticipant+1];
    void *pNodeResults[maxParticipant+1];
    uint64_t *pNodeExceptions[maxParticipant+1];
    int i;
    for (i=0; i<= maxParticipant; i++) {
      // (uint8_t *) use below for ptr arithmetic might not be needed.
      vContribution[i] = (uint8_t *)vFlatContribution + i*(4*elementWords);
      pNodeResults[i] = (uint8_t *)vFlatNodeResult + i*(4*elementWords);
      pNodeExceptions[i] = vFlatNodeExceptions ? (uint64_t *)vFlatNodeExceptions + i : NULL;
    }

    uint64_t rc = BgNDReduce_allNodeScalarReduce(
      rootNode,
      willParticipate,
      willParticipate_parms,
      maxParticipant,
      vFlatNodeExceptions ? pNodeExceptions : NULL,
      pNodeResults,
    
      0, // offset,
      elementWords,
      opCode,
      Tree,
      vContribution,
      vFlagsContribution,
      vFlagsResult,
      Result
      );

    // BELOW A Check on above pointer math. The result must be the same as for this node.
    int pcmp = Bg_memcmp(pNodeResults[rootNode],Result,4*elementWords);
    PRINT_ASSERT(pcmp==0);

    return rc;
  }

// BgNDReduce_subCommFlatScalarReduce() is based on BgNDReduce_allNodeFlatScalarReduce()
// TODO Implement BgNDReduce_allNodeFlatScalarReduce() using BgNDReduce_subCommFlatScalarReduce(), just needs a vector with all elements true!
  uint64_t BgNDReduce_subCommFlatScalarReduce(
    int numNodesInSubComm,
    int *IsNodeInSubComm,
    int rootNode,
    int (*willParticipate)(void *parms, int node, int link),
    void *willParticipate_parms,
    int maxParticipant,
    uint64_t *vFlatNodeExceptions,
    void *vFlatNodeResult,

    int elementWords,
    int32_t opCode,
    char *Tree,
    void *vFlatContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
    ) {

    void *vContribution[maxParticipant+1];
    void *pNodeResults[maxParticipant+1];
    uint64_t *pNodeExceptions[maxParticipant+1];
    int i;
    for (i=0; i<= maxParticipant; i++) {
      // (uint8_t *) use below for ptr arithmetic might not be needed.
      vContribution[i] = (uint8_t *)vFlatContribution + i*(4*elementWords);
      pNodeResults[i] = (uint8_t *)vFlatNodeResult + i*(4*elementWords);
      pNodeExceptions[i] = vFlatNodeExceptions ? (uint64_t *)vFlatNodeExceptions + i : NULL;
    }

    uint64_t rc = BgNDReduce_subCommScalarReduce(
      numNodesInSubComm,
      IsNodeInSubComm,
      rootNode,
      willParticipate,
      willParticipate_parms,
      maxParticipant,
      vFlatNodeExceptions ? pNodeExceptions : NULL,
      pNodeResults,
    
      0, // offset,
      elementWords,
      opCode,
      Tree,
      vContribution,
      vFlagsContribution,
      vFlagsResult,
      Result
      );

    // BELOW A Check on above pointer math. The result must be the same as for this node.
    int pcmp = Bg_memcmp(pNodeResults[rootNode],Result,4*elementWords);
    PRINT_ASSERT(pcmp==0);

    return rc;
  }

  // Return value is Exceptions in this reduction.
  uint64_t BgNDReduce_multiNodeVectorReduce(
    int thisNode,
    int (*willParticipate)(void *parms, int node, int link),
    void *willParticipate_parms,
    int maxParticipant,
    uint64_t **vNodeExceptions,
    void **vNodeResult,

    int *participationGeneration,
    int *nodeParticipation,
    int *totalParticipation,

    int numElements,
    int elementWords,
    int32_t opCode,
    char *Tree,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *vResult,
    uint64_t *vExceptions
    ) {
  uint64_t r = 0;
  // PRINT_INFO1(numElements);
  for (int e=0; e<numElements; e++) {
    // PRINT_INFO2(e,numElements);
    // *((uint32_t *)vResult + e*elementWords)= __LINE__; // Unexpected get 0 back.
    vExceptions[e] = BgNDReduce_multiNodeScalarReduce(thisNode, willParticipate, willParticipate_parms, maxParticipant, vNodeExceptions, vNodeResult, 
						      *participationGeneration, nodeParticipation, totalParticipation,
           e*elementWords, elementWords, opCode, Tree, vContribution, vFlagsContribution, vFlagsResult, (uint32_t *)vResult + e*elementWords);
    ++*participationGeneration;
    r |= vExceptions[e];
  }
  return r;
  }


  // Return value is Exceptions in this reduction.
  uint64_t BgNDReduce_allNodeVectorReduce(
    int rootNode,
    int (*willParticipate)(void *parms, int node, int link),
    void *willParticipate_parms,
    int maxParticipant,
    uint64_t **vNodeExceptions,
    void **vNodeResult,

    int numElements,
    int elementWords,
    int32_t opCode,
    char *Tree,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *vResult,
    uint64_t *vExceptions
  ) {
#if 1
    // Better like this:
    // - test BgNDReduce_multiNodeVectorReduce().
    if (maxParticipant<0) {
      return 0;
    }
    // PRINT_INFO3(rootNode,maxParticipant,numElements);
    PRINT_ASSERT2(rootNode>=0 && rootNode<=maxParticipant,rootNode,maxParticipant);

    int participationGeneration = 0;
    int totalParticipation = 0;
    int i;
    int nodeParticipation[maxParticipant+1];
    for (i=0; i<=maxParticipant; i++) {
      nodeParticipation[i]=participationGeneration;
    }

    uint64_t r = BgNDReduce_multiNodeVectorReduce(
    rootNode,
    willParticipate,
    willParticipate_parms,
    maxParticipant,
    vNodeExceptions,
    vNodeResult,
    &participationGeneration,
    nodeParticipation,
    &totalParticipation,

    numElements,
    elementWords,
    opCode,
    Tree,
    vContribution,
    vFlagsContribution,
    vFlagsResult,
    vResult,
    vExceptions
    );

    if (numElements>0) {
      // Handle numElements down here, so that this use of allNode also verifies above call to multiNode.
      PRINT_ASSERT2(participationGeneration==numElements,participationGeneration,numElements);
      PRINT_ASSERT3(totalParticipation == participationGeneration*(maxParticipant+1),totalParticipation,maxParticipant,participationGeneration);
      for (i=0; i<=maxParticipant; i++) {
        assert(nodeParticipation[i]==participationGeneration);
      }
    }
    return r;
#else
  uint64_t r = 0;
  for (int e=0; e<numElements; e++) {
    // *((uint32_t *)vResult + e*elementWords)= __LINE__; // Unexpected get 0 back.
    vExceptions[e] = BgNDReduce_allNodeScalarReduce(rootNode, willParticipate, willParticipate_parms, maxParticipant, vNodeExceptions, vNodeResult, e*elementWords, elementWords, opCode, Tree, vContribution, vFlagsContribution, vFlagsResult, (uint32_t *)vResult + e*elementWords);
    // PRINT_INFO4(vResult, (uint32_t *)vResult + e*elementWords, e, elementWords);
    r |= vExceptions[e];
  }
  return r;
#endif
  }


// END   code new to BgNDReduce_C.h, not ported from C++ BgNDReduce.h --------------------------

__END_DECLS
