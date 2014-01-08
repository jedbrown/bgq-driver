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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef BGQ_DB_EXTRACT_DB_H
#define BGQ_DB_EXTRACT_DB_H

#include <sstream>
#include <string>
#include <vector>

/*!
 * \brief Return code values from Extract DB APIs
 */
enum extract_db_status {
    DB_OK  = 0,                     //!< Call was successful
    DB_ERROR = -1,                  //!< DB error occurred
    DB_NO_DATA = -2,                //!< No data error
    DB_COMM_ERR = -3                //!< DB communication error occurred
};

/*!
 * \brief
 */
extract_db_status
extract_compact_block(
        std::ostream &os,               //!< [out]
        const std::string& block,       //!< [in]
        bool diags                      //!< [in]
        );

/*!
 * \brief
 */
extract_db_status extract_compact_machine(
	std::ostream &os,                                   //!< [out]
	std::vector<std::string>* invalid_memory_locations  //!< [out]
    );

/*!
 * \brief
 */
extract_db_status extract_db_bpwires(
        std::ostream &os                    //!< [out]
        );

/*!
 * \brief
 */
extract_db_status extract_db_iowires(
        std::ostream &os,                   //!< [out]
        const std::string& midplane         //!< [in]
        );

/*!
 * \brief
 */
extract_db_status extract_db_iolinks(
        std::ostream &os,                   //!< [out]
        const std::string& block            //!< [in]
        );

/*!
 * \brief
 */
extract_db_status extract_db_switches(
        std::ostream &os                    //!< [out]
        );

/*!
 * \brief
 */
extract_db_status extract_db_bplist(
        std::ostream &os                    //!< [out]
        );

/*!
 * \brief
 */
extract_db_status extract_db_nodecards(
        std::ostream &os,                   //!< [out]
        const std::string& midplane         //!< [in]
        );

#endif
