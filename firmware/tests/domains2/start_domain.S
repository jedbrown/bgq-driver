// begin_generated_IBM_copyright_prolog                             
//                                                                  
// This is an automatically generated copyright prolog.             
// After initializing,  DO NOT MODIFY OR MOVE                       
//  --------------------------------------------------------------- 
//                                                                  
// Licensed Materials - Property of IBM                             
// Blue Gene/Q                                                      
// (C) Copyright IBM Corp.  2010, 2012                              
// US Government Users Restricted Rights - Use, duplication or      
//   disclosure restricted by GSA ADP Schedule Contract with IBM    
//   Corp.                                                          
//                                                                  
// This software is available to you under the Eclipse Public       
// License (EPL).                                                   
//                                                                  
//  --------------------------------------------------------------- 
//                                                                  
// end_generated_IBM_copyright_prolog                               
/*------------------------------------------------------------------*/

#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

#include <hwi/include/common/asm_support.h>
#include <hwi/include/bqc/A2_core.h>

//#include "linkage.h"

         .file "start_domain.S"

	
//------------------------------------------------------------------------------
// Make the jump into C code at entry point "_test_entry".  We don't futz
// with stack here (that will be done on the other side).
//------------------------------------------------------------------------------
        .extern _enter_domain
        .global _start_domain
	.section ".text.start.domain"
	#.type _start_domain,@function	;
_start_domain:
        li     %r0,0
        IMM64( %r2, __FWTST_TOC_BASE )
        bl     _enter_domain
        nop
	b 0
	#.size _start_domain,.-_start_domain
	#.previous
//------------------------------------------------------------------------------


#define IVEC( label, code ) \
	.align 5; \
label:	li %r3, code; \
	b 0;

	.section ".text.domain"

	.align 11

    	.global _vec_domain_MCHK;

        IVEC( _vec_domain_MCHK,     IVO_MCHK     )
        IVEC( _vec_domain_CI,       IVO_CI       )
	IVEC( _vec_domain_DEBUG,    IVO_DEBUG    )
        IVEC( _vec_domain_DSI,      IVO_DSI      )
        IVEC( _vec_domain_ISI,      IVO_ISI      )
        IVEC( _vec_domain_EI,       IVO_EI       )
        IVEC( _vec_domain_ALGN,     IVO_ALGN     )
        IVEC( _vec_domain_PROG,     IVO_PROG     )
        IVEC( _vec_domain_FPU,      IVO_FPU      )
        IVEC( _vec_domain_SC,       IVO_SC       )
        IVEC( _vec_domain_APU,      IVO_APU      )
        IVEC( _vec_domain_DEC,      IVO_DEC      )
        IVEC( _vec_domain_FIT,      IVO_FIT      )
        IVEC( _vec_domain_WDT,      IVO_WDT      )
        IVEC( _vec_domain_DTLB,     IVO_DTLB     )
        IVEC( _vec_domain_ITLB,     IVO_ITLB     )
        IVEC( _vec_domain_VECTOR,   IVO_VECTOR   )
        IVEC( _vec_domain_Undef220, IVO_UNDEFINED )
        IVEC( _vec_domain_Undef240, IVO_UNDEFINED )
        IVEC( _vec_domain_Undef260, IVO_UNDEFINED )
        IVEC( _vec_domain_PDBI,     IVO_PDBI     )
        IVEC( _vec_domain_PDBCI,    IVO_PDBCI    )
        IVEC( _vec_domain_GDBI,     IVO_GDBI     )
        IVEC( _vec_domain_GDBCI,    IVO_GDBCI    )
        IVEC( _vec_domain_EHVSC,    IVO_EHVSC    )
        IVEC( _vec_domain_EHVPRIV,  IVO_EHVPRIV  )
        IVEC( _vec_domain_LRATE,    IVO_LRATE    )

	

