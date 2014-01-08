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


testFprs:
	.global testFprs

	xor	5,5,5
	
	qvlfdx	0,0,4
	isync
	sync
	
	qvstfdx	0,5,3
	addi	5,5,32
	
	qvfmr	1,0
	qvstfdx	1,5,3
	addi	5,5,32

	qvfmr	2,0
	qvstfdx	2,5,3
	addi	5,5,32

	qvfmr	3,0
	qvstfdx	3,5,3
	addi	5,5,32

	
	qvfmr	4,0
	qvstfdx	4,5,3
	addi	5,5,32

	qvfmr	5,0
	qvstfdx	5,5,3
	addi	5,5,32

	qvfmr	6,0
	qvstfdx	6,5,3
	addi	5,5,32
	
	qvfmr	7,0
	qvstfdx	7,5,3
	addi	5,5,32
	
	qvfmr	8,0
	qvstfdx	8,5,3
	addi	5,5,32
	
	qvfmr	9,0
	qvstfdx	9,5,3
	addi	5,5,32

	
	qvfmr	10,0
	qvstfdx	10,5,3
	addi	5,5,32
	
	qvfmr	11,0
	qvstfdx	11,5,3
	addi	5,5,32

	qvfmr	12,0
	qvstfdx	12,5,3
	addi	5,5,32

	qvfmr	13,0
	qvstfdx	13,5,3
	addi	5,5,32

	
	qvfmr	14,0
	qvstfdx	14,5,3
	addi	5,5,32

	qvfmr	15,0
	qvstfdx	15,5,3
	addi	5,5,32

	qvfmr	16,0
	qvstfdx	16,5,3
	addi	5,5,32
	
	qvfmr	17,0
	qvstfdx	17,5,3
	addi	5,5,32
	
	qvfmr	18,0
	qvstfdx	18,5,3
	addi	5,5,32
	
	qvfmr	19,0
	qvstfdx	19,5,3
	addi	5,5,32

	
	qvfmr	20,0
	qvstfdx	20,5,3
	addi	5,5,32
	
	qvfmr	21,0
	qvstfdx	21,5,3
	addi	5,5,32

	qvfmr	22,0
	qvstfdx	22,5,3
	addi	5,5,32

	qvfmr	23,0
	qvstfdx	23,5,3
	addi	5,5,32

	
	qvfmr	24,0
	qvstfdx	24,5,3
	addi	5,5,32

	qvfmr	25,0
	qvstfdx	25,5,3
	addi	5,5,32

	qvfmr	26,0
	qvstfdx	26,5,3
	addi	5,5,32
	
	qvfmr	27,0
	qvstfdx	27,5,3
	addi	5,5,32
	
	qvfmr	28,0
	qvstfdx	28,5,3
	addi	5,5,32
	
	qvfmr	29,0
	qvstfdx	29,5,3
	addi	5,5,32

		
	qvfmr	30,0
	qvstfdx	30,5,3
	addi	5,5,32
	
	qvfmr	31,0
	qvstfdx	31,5,3
	addi	5,5,32

	
	blr
	
