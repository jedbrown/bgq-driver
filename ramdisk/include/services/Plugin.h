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
 * \file bgq/ramdisk/include/services/sysiod/Plugin.h
 * \brief definition and implementation of abstract base class bgcios::sysio::Plugin
 */

#ifndef CIOS_SYSIOD_PLUGIN_H_
#define CIOS_SYSIOD_PLUGIN_H_
#include <ramdisk/include/services/MessageHeader.h>
#include <ramdisk/include/services/UserMessages.h>
#include <errno.h>


namespace bgcios
{

namespace sysio
{

/*!
 * \brief The cios plug-in interface for sysiod function-shipping extensions
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER on DOCUMENTATION_DATE.
 *
 * \section Overview
 *
 * This base class is used to extend the function-ship messages handled by sysiod to allow end-user customization
 *
 * Two symbols are <b>required</b> to be present in the shared library for
 * sysiod to successfully load it. They are
\verbatim
bgcios::sysiod::Plugin* create()
\endverbatim
* and
\verbatim
void destroy(bgcios::sysiod::Plugin*)
\endverbatim
 *
 * They must be declared as extern "C" to prevent mangling of symbol names. The path to
 * the plug-in, and any flags required for dlopen  can be specified in the bg.properties file
 * in the following section.
\verbatim
[cios.sysiod]
sysiod_plugin          = /full/path/to/plugin.so
sysiod_plugin_flags    = 0x0101 # RTLD_LAZY | RTLD_GLOBAL
\endverbatim
 *

 *
 *
 * \section linking Linking
 *
 * When linking your plugin, you will need to link to sysiod like so:
 * <pre>
 * /todo TODO  Update to sysiod use
 * sysiod_dir=\$(BGQ_INSTALL_DIR)/ramdisk/src/services/sysiod
 * </pre>
 *
 * Then use these LDFLAGs:
 * <pre>
 * /todo TODO  Update to sysiod use
 * LDFLAGS += -L\$(hlcs_library_dir) -lbgsched -Wl,-rpath,\$(hlcs_library_dir)
 * </pre>
 *
 * \section thread_safety Thread Safety
 *
 * Multiple threads are not supported
 *
 * \section Warning
 *
 * The plug-in must be compiled with the exact same version of gcc as the sysiod:
 * GCC_VERSION.
 */

class Plugin
{

public:
 

    /** 
     * \enum Plugin::RDMAop 
     * \brief specifies the RDMA operation to execute
     * \note  The RDMA is done after a call into an RDMA virtual function in the class and the results into a class coninuation or continue method
     */  
    enum RDMAop {RDMAdone=0,    /**< RDMA operation is done or no RDMA operation is to be executed */
                 RDMAread=1,    /**< RDMA read from client compute node memory to the IO node memory*/  
                 RDMAwrite=2  /**< RDMA write to client compute node memory from the IO node memory */
    };
    
    struct RDMArequest {
      RDMAop rdmaOp;             
      int cnkRDMAlistSize;   /**< number of valid elements starting at cnkRDMlist */
      int statusReturn;      /**< Status on the RDMA operation--intended for continue or finish method */
      int errnoReturn;       /**< errno on the RDMA operations--intended for continue or finish method */
      struct UserRDMA cnkRDMAlist[MostRdmaRegions];  /**< list of CNK memory regions with addressing, key, and lengths */
      uint64_t bytesMoved;   /**< Bytes moved by RDMA operation                                        */

      /** \brief Initialize struct RDMArequest
       *  \description Initializes the structure to harmless initial values 
       */
      RDMArequest() : rdmaOp(RDMAdone), cnkRDMAlistSize(0), statusReturn(0),errnoReturn(0), 
                     bytesMoved(0){}  
    };

    /** 
     * \brief Take a UserMessageFdRDM message with file and initialize the RDMA information in the RDMArequest
     * \param[in] receivedMsg Message format in UserMessages.h with file descriptors and CNK memory list for RDMA
     * \param[in] rdmaOP Specifiy whether an RDMA operation is to be done upon exit from messageFdRDMA() method
     * \return Total byte length of memory regions if valid and (uint64_t)(-1) if an error with errnoReturn set in _rdmaRequest
     * \post The base class RDMArequest private member will be initialized from the received message
     */
    uint64_t setRDMArequestOnMsg(const bgcios::UserMessageFdRDMA& receivedMsg, RDMAop rdmaOp){
      _rdmaRequest.rdmaOp=rdmaOp;
      _rdmaRequest.cnkRDMAlistSize = receivedMsg.numberOfRdmaRegions;
      _rdmaRequest.statusReturn = 0;
      _rdmaRequest.errnoReturn  = 0;
      _rdmaRequest.bytesMoved = 0;
      if ( (_rdmaRequest.cnkRDMAlistSize <= MostRdmaRegions) && (_rdmaRequest.cnkRDMAlistSize > 0) ){
        memcpy(_rdmaRequest.cnkRDMAlist,receivedMsg.uRDMA,sizeof(UserRDMA)*_rdmaRequest.cnkRDMAlistSize);
        uint64_t total_length = 0;
        for ( int j=0; j< _rdmaRequest.cnkRDMAlistSize; j++) total_length += _rdmaRequest.cnkRDMAlist[j].cnk_bytes;
        return total_length;
      }
      else if (_rdmaRequest.cnkRDMAlistSize == 0){
        return 0;
      }
      else{
       _rdmaRequest.cnkRDMAlistSize = 0;
       _rdmaRequest.rdmaOp = RDMAdone;
       _rdmaRequest.statusReturn = -1;
       _rdmaRequest.errnoReturn = EINVAL;
       return (uint64_t)-1;
      }
      return 0;
    }
    
    //! \brief Getter of the rdmaOP from the internal RDMArequest structure
    RDMAop getRDMAop(){return _rdmaRequest.rdmaOp;}

    //! \brief Setter of the rdmaOP from the internal RDMArequest structure
    void setRDMAop(RDMAop op){_rdmaRequest.rdmaOp=op;}

    //! \brief Getter of the errnoReturn from the internal RDMArequest structure
    uint32_t getRDMAerrno(){ return (uint32_t)_rdmaRequest.errnoReturn; }

    //! \brief Getter of the bytesMoved from the internal RDMArequest structure
    uint64_t getBytesMoved(){ return _rdmaRequest.bytesMoved; }

    //! \brief Getter of the cnkRDMAlistSize from the internal RDMArequest structure
    int getCnkRDMAlistSize() const { return _rdmaRequest.cnkRDMAlistSize; }

    //! \brief Getter of the address of the cnkRDMAlist from the internal RDMArequest structure
    struct UserRDMA * getCnkRDMAlist(){ return _rdmaRequest.cnkRDMAlist; }

 
    typedef struct RDMArequest RDMArequest_t;
    /*!
     * \brief ctor.
     */
    Plugin():_maxLength4Reply( ImmediateMessageSize ) { }

    /*!
     * \brief dtor.
     */
    virtual ~Plugin() { }

    /*!
     * \brief Receive Usermessage and forumulate a reply 
     * \param[in] receivedMsg  Is the UserMessage sent from the compute node application, format in UserMessages.h
     * \param[in] replyMsg Is the reply message returned to the compute node application.
     * \note Expected to have a derived class in a plugin which overrides this.  Default of base class is to send back an error
     */
    virtual void message(const bgcios::UserMessage& receivedMsg, struct MessageHeader& replyMsg){
      setReplyHeader(receivedMsg, replyMsg, 0);
      setErrno(replyMsg);
      return;
    }

    /*!
     * \brief Receive UserMessageFdRDMA message, formulate RDMA processing, and let sysiod do the RDMA after setting the RDMArequest structure
     * \param[in] receivedMsg  Is the UserMessageFdRDMA sent from the compute node application, format in UserMessages.h
     * \param[in] replyMsg Is the reply message returned to the compute node application.
     * \param[in] RDMAbuffer Is the address of the RDMA region used by sysiod for RDMA transfer
     * \param[in] size Is the size of the RDMA buffer on the IO node used by sysiod for RDMA transfer.  The size will be the current value of
     *            of large_region size in bg.properties.  For example, 
     * [cios]
     * large_regions_size=1048576
     * \note Expected to have a derived class in a plugin which overrides this.  Default of base class is to send back an error
     *
     * \section RDMA Write Advice
     * Fill in the RDMA buffer using the file descriptors and/or other implementation from the start of the buffer for the number of bytes covered
     * by the memory list total number of bytes.  If the RDMA buffer is sufficient to write to the CNK RDMA memory pieces in the message, 
     * take advantage of the method setRDMArequestOnMsg as in
     * setRDMArequestOnMsg(receivedMsg, RDMAwrite)
     * return to the sysiod caller and let sysiod do the RDMA processing and be prepared to handle the call into finishFdRDMA method
     *
     *
     * \section RDMA Read Advice 
     * If the RDMA buffer is sufficient to read from the CNK RDMA memory pieces in the message, 
     * take advantage of the method setRDMArequestOnMsg as in
     * setRDMArequestOnMsg(receivedMsg, RDMAread)
     * return to the sysiod caller and let sysiod do the RDMA processing and be prepared to handle the call into finishFdRDMA method
     *
     * \note  If the buffer size is smaller than that in the list, the list will need adjusting to avoid having
     * an error set in RDMArequest returnErrno (EOVERFLOW).
     */
    virtual void messageFdRDMA(const bgcios::UserMessageFdRDMA& receivedMsg, struct MessageHeader& replyMsg, void * RDMAbuffer, uint64_t size){
      setReplyHeader(receivedMsg, replyMsg, 0);
      setRDMArequestOnMsg(receivedMsg, RDMAdone);
      setErrno(replyMsg);
      return;
    }

    /*!
     * \brief Finish RDMA buffer processing, and last chance to update replymsg, all based on inspecting results of RDMArequest 
     * \param[in] receivedMsg  Is the UserMessageFdRDMA sent from the compute node application, format in UserMessages.h
     * \param[in] replyMsg Is the reply message returned to the compute node application.
     * \param[in] RDMAbuffer Is the address of the RDMA region used by sysiod for RDMA transfer
     * \param[in] size Is the size of the RDMA buffer used by sysiod for RDMA transfer
     *
     * \note Expect to have a derived class in a plugin which overrides this.  Default of base class is to send back an error
     * 
     * \section RDMA Write Advice
     * Inspect the RDMArequest for any errors noted in returnErrno or statusReturn and validate the bytesMoved is what was expected.
     * Update the replyMsg and return.  
     *
     * \section RDMA Read Advice
     * Inspect the RDMArequest for any errors noted in returnErrno or statusReturn and validate the bytesMoved is what was expected.
     * Use the bytes in the RDMA buffer utilizing file descriptors and/or other implementation from the start of the buffer for the bytesMoved.
     * Update the replyMsg and return.
     */
    virtual void finishFdRDMA(const bgcios::UserMessageFdRDMA& receivedMsg, struct MessageHeader& replyMsg, void * RDMAbuffer, uint64_t size){
      if ( getRDMAerrno() ){
        setErrno(replyMsg,getRDMAerrno(),(-1));
        return;
      }
      // if RDMA read from compute node, consider doing something here with data and file descriptors.  For example, write the data against the file.
      setErrno(replyMsg,0,getBytesMoved());//update Ack message with errno=0 and return the number of bytes Moved
      return;
    }

    //! \brief Utility setter of the Reply message to a UserMessage
    uint32_t setReplyHeader(const bgcios::UserMessage& receivedMsg, struct MessageHeader& replyMsg, uint16_t typeForReply,uint32_t length4Reply=32){
      return setReplyHeader( (const bgcios::MessageHeader&) receivedMsg, replyMsg, typeForReply,length4Reply);
    }

    //! \brief Utility setter of the Reply message to a UserMessageFdRDMA
    uint32_t setReplyHeader(const bgcios::UserMessageFdRDMA& receivedMsg, struct MessageHeader& replyMsg, uint16_t typeForReply,uint32_t length4Reply=32){
      return setReplyHeader( (const bgcios::MessageHeader&) receivedMsg, replyMsg, typeForReply,length4Reply);
    }

    //! \brief Utility setter of the Reply message for received Messages
    uint32_t setReplyHeader(const bgcios::MessageHeader& receivedMsg, struct MessageHeader& replyMsg, uint16_t typeForReply,uint32_t length4Reply=32){
       
         replyMsg.service     = receivedMsg.service;          //!< Service to process message.
         replyMsg.version     = receivedMsg.version;          //!< Protocol version number.
         replyMsg.type        = typeForReply;                 //!< Content of message.
         replyMsg.rank        = receivedMsg.rank;             //!< Rank message is associated with.
         replyMsg.sequenceId  = receivedMsg.sequenceId;       //!< Correlate requests and acknowledgements.
         replyMsg.returnCode  = 0;                            //!< Result of previous request.
         replyMsg.errorCode   = 0;                            //!< Error detail (typically errno value).
         replyMsg.length      = length4Reply;               //!< Amount of data in message (including this header).
         replyMsg.jobId       = receivedMsg.jobId;            //!< Job message is associated with.
         if (length4Reply > _maxLength4Reply) replyMsg.length = _maxLength4Reply;
         return replyMsg.length;
    }

    //! \brief Utility setter of the Reply message length field
    uint32_t setReplyLength(struct MessageHeader& replyMsg, const uint32_t length4Reply) const {
         replyMsg.length      = length4Reply;               //!< Amount of data in message (including this header).
         if (length4Reply > _maxLength4Reply) replyMsg.length = _maxLength4Reply;
         return replyMsg.length;
    }

        //! \brief Utility setter of the Reply message type field
    void setReplyType(struct MessageHeader& replyMsg, const uint16_t typeForReply) const {
      replyMsg.type = typeForReply;
    }

    //! \brief Utility setter of the Reply message returnCode and ErrorCode fields
    void setErrno(struct MessageHeader& replyMsg, const uint32_t errorCodeForReply=ENOSYS, const uint32_t returnCodeCodeForReply=(-1) ) const {
      replyMsg.returnCode  = returnCodeCodeForReply;                           
      replyMsg.errorCode   = errorCodeForReply;                            
    }

    //! \brief Utility getter of the RDMArequest for more control but greater risks for errors
    RDMArequest_t * getRdmaRequestAddress(){return &_rdmaRequest;}
private:
    uint32_t _maxLength4Reply;

    RDMArequest_t _rdmaRequest;  

};


} // sysio
} // bgcios

extern "C" {

/*!
 * \brief declaration of library method to create a Plugin.
 * \returns pointer to a Plugin object.
 */
bgcios::sysio::Plugin* create();

/*!
 * \brief declaration of a library method to destroy a Plugin.
 * \param[in] plugin pointer to the Plugin to destroy.
 */
void destroy(bgcios::sysio::Plugin* plugin);

}

#endif //CIOS_SYSIOD_PLUGIN_H_
