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


#ifndef MMCS_LITE_BLOCK_CONTROLLER_H_
#define MMCS_LITE_BLOCK_CONTROLLER_H_


#include "server/BlockHelper.h"

#include <ras/include/RasEvent.h>

#include <xml/include/c_api/MCServerMessageSpec.h>

#include <string>


class BGQMachineXML;


namespace mmcs {
namespace lite {

/*!
 * \brief sqlite'd version of a BlockController.
 */
class BlockController : public server::BlockHelper
{
public:
    /*!
     * \brief ctor
     */
    BlockController(
            BGQMachineXML* machine,         //!< [in] machine XML description.
            const std::string& userName,    //!< [in] user name
            const std::string& blockName    //!< [in] block name
            );
    BlockController(server::BlockPtr ptr);

    /*!
     * \brief handle a RAS event
     */
    int processRASMessage(
                           RasEvent &rasEvent  //!< [in]
                           );

     /*!
     * \brief handle a console event
     */
    void processConsoleMessage(
                               MCServerMessageSpec::ConsoleMessage& consoleMessage
                               );

};

} } // namespace mmcs::lite

#endif
