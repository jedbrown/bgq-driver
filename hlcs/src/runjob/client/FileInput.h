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
#ifndef RUNJOB_CLIENT_FILE_INPUT_H
#define RUNJOB_CLIENT_FILE_INPUT_H

#include "client/Input.h"

namespace runjob {
namespace client {

class MuxConnection;

/*!
 * \brief Handle output for stdin from a file.
 */
class FileInput : public Input
{
public:
    /*!
     * \brief ctor.
     */
    FileInput(
            const boost::weak_ptr<MuxConnection>& mux,      //!< [in]
            int fd                                          //!< [in]
         );

    /*!
     * \brief
     */
    void stop() { }

    /*!
     * \copydoc Input::read
     */
    void read(
            uint32_t rank,
            size_t length,
            const Uci& location
            );

private:
    const int _fd;
};

} // client
} // runjob

#endif

