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
#ifndef RUNJOB_SERVER_JOB_EXIT_STATUS_H
#define RUNJOB_SERVER_JOB_EXIT_STATUS_H

#include "common/error.h"

#include <utility/include/ExitStatus.h>

#include <string>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief
 */
class ExitStatus
{
public:
    /*!
     * \brief ctor.
     */
    ExitStatus();

    /*!
     * \brief Set exit status and message.
     */
    void set(
            const std::string& message, //!< [in]
            error_code::rc error        //!< [in]
            );

    /*!
     * \brief Set error.
     */
    void set(
            error_code::rc error    //!< [in]
            );

    /*!
     * \brief Update exit status from a node.
     */
    void set(
            int status,     //!< [in]
            uint32_t rank   //!< [in]
            );

    /*!
     * \brief
     */
    void setRecordId(
            int recid   //!< [in]
            );

    /*!
     * \brief
     */
    void setDetails(
            const std::string& details  //!< [in]
            );

    /*!
     * \brief Get the message.
     */
    const std::string& getMessage() const { return _message; }

    /*!
     * \brief Get the error.
     */
    error_code::rc getError() const { return _error; }

    /*!
     * \brief Get exit status.
     */
    const bgq::utility::ExitStatus& getStatus() const { return _status; }

    /*!
     * \brief Get control action record ID.
     */
    int getRecId() const { return _controlActionRecordId; }

private:
    std::string _message;                                   //!<
    error_code::rc _error;                                  //!<
    bgq::utility::ExitStatus _status;                       //!<
    uint32_t _offender;                                     //!<
    int _controlActionRecordId;                             //!<
    std::string _details;                                   //!<
};

} // job
} // server
} // runjob

#endif
