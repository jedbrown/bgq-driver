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
 * \brief Derived class based on Class Plugin.
 */

#ifndef PLUGX_H_
#define PLUGX_H_
#include <ramdisk/include/services/UserMessages.h>
#include <ramdisk/include/services/Plugin.h>
#include <ramdisk/include/services/PlugxConst.h>


namespace bgcios
{

namespace sysio
{
class Plugx : public Plugin
{

public:


    /*!
     * \brief ctor.
     */
    Plugx():Plugin() { }

    /*!
     * \brief dtor.
     */
    virtual ~Plugx() { }

    /*!
     * \brief Receive Usermessage and forumulate a reply 
     * \param[in] receivedMsg  Is the UserMessage sent from the compute node application, format in UserMessages.h
     * \param[in] replyMsg Is the reply message returned to the compute node application.
     * \note Expected to have a derived class in a plugin which overrides this.  Default of base class is to send back an error
     */
    virtual void message(const bgcios::UserMessage& receivedMsg, struct MessageHeader& replyMsg);

    /*!
     * \brief Receive UserMessageFdRDMA message, formulate RDMA processing, and let sysiod do the RDMA after setting the RDMArequest structure
     * \param[in] receivedMsg  Is the UserMessageFdRDMA sent from the compute node application, format in UserMessages.h
     * \param[in] replyMsg Is the reply message returned to the compute node application.
     * \param[in] RDMAbuffer Is the address of the RDMA region used by sysiod for RDMA transfer
     * \param[in] size Is the size of the RDMA buffer used by sysiod for RDMA transfer
     * 
     * \notes
     * The RDMAbuffer is IO node pinned storage and the void * is where it starts.  It is of number of bytes "size".  
     * The cnk storage is the list of descriptors of address, length, and key.
     *
     * \notes
     * The RDMA read will sequentially move data from the CNK memory descriptors into the RDMAbuffer.  
     * The RDMA write will sequentially move dat to the CNK memory from the RDMAbuffer.
     *  
     */
    virtual void messageFdRDMA(const bgcios::UserMessageFdRDMA& receivedMsg, struct MessageHeader& replyMsg, void * RDMAbuffer, uint64_t size);

    /*!
     * \brief Finish RDMA buffer processing, and last chance to update replymsg, all based on inspecting results of RDMArequest 
     * \param[in] receivedMsg  Is the UserMessageFdRDMA sent from the compute node application, format in UserMessages.h
     * \param[in] replyMsg Is the reply message returned to the compute node application.
     * \param[in] RDMAbuffer Is the address of the RDMA region used by sysiod for RDMA transfer
     * \param[in] size Is the size of the RDMA buffer used by sysiod for RDMA transfer
     */
    virtual void finishFdRDMA(const bgcios::UserMessageFdRDMA& receivedMsg, struct MessageHeader& replyMsg, void * RDMAbuffer, uint64_t size);


private: 

};


} // sysio
} // bgcios



#endif //PLUGX_H_
