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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

/*!
 * \file Plugx.h
 * \brief 
 */

#ifndef PLUGXCONST_H_
#define PLUGXCONST_H_

	static const uint32_t PLUGIN_VERSION = 1;
	static const uint32_t PLUGIN_OP_TYPE_MSG_ECHO = 5;
        static const uint32_t PLUGIN_OP_TYPE_MSG_ECHO_REPLY = 6;
        static const uint32_t PLUGIN_OP_TYPE_MSG_GETHOSTNAME = 7;
        static const uint32_t PLUGIN_OP_TYPE_MSG_GETHOSTNAME_REPLY = 8;
	static const uint32_t PLUGIN_OP_TYPE_WRITE_DMA = 11;
	static const uint32_t PLUGIN_OP_TYPE_READ_DMA = 12;
	static const uint32_t PLUGIN_OP_TYPE_WRITE_FILE = 21;
	static const uint32_t PLUGIN_OP_TYPE_WRITE_FILE_REPLY = 22;
	static const uint32_t PLUGIN_OP_TYPE_READ_FILE = 23;
        static const uint32_t PLUGIN_OP_TYPE_READ_FILE_REPLY = 24;


	static const uint32_t PLUGIN_ERROR_INVALID_VERSION = 700;
	static const uint32_t PLUGIN_ERROR_INVALID_VERSION2 = 701;
	static const uint32_t PLUGIN_ERROR_INVALID_OP1 = 702;
	static const uint32_t PLUGIN_ERROR_INVALID_OP2 = 703;
	static const uint32_t PLUGIN_ERROR_SIZE_MISMATCH = 800;
	static const uint32_t PLUGIN_ERROR_READ_FILE1_ERRNO = 801;
	static const uint32_t PLUGIN_ERROR_READ_FILE1_RC = 802;
	static const uint32_t PLUGIN_ERROR_READ_FILE1_MISCOMPARE = 803;
	static const uint32_t PLUGIN_ERROR_READ_FILE2_ERRNO = 810;
	static const uint32_t PLUGIN_ERROR_READ_FILE2_RC = 811;
	static const uint32_t PLUGIN_ERROR_READ_FILE2_MISCOMPARE = 812;
	static const uint32_t PLUGIN_ERROR_READ_RDMA_ERROR = 900;
	static const uint32_t PLUGIN_ERROR_READ_RDMA_MISCOMPARE = 901;
	static const uint32_t PLUGIN_ERROR_WRITE_FILE1_RC = 910;
	static const uint32_t PLUGIN_ERROR_WRITE_FILE1_ERRNO = 911;
	static const uint32_t PLUGIN_ERROR_WRITE_FILE2_RC = 912;
	static const uint32_t PLUGIN_ERROR_WRITE_FILE2_ERRNO = 913;




#endif //PLUGXCONST_H_
