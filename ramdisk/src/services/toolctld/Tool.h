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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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

//! \file  Tool.h
//! \brief Declaration and inline methods for bgcios::toolctl::Tool class.

#ifndef TOOLCTL_TOOL_H
#define TOOLCTL_TOOL_H

// Includes
#include <ramdisk/include/services/common/LocalSocket.h>
#include <tr1/memory>
#include <string>
#include <sstream>

namespace bgcios
{

namespace toolctl
{

//! \brief Tool attached to a compute node process.

class Tool
{
public:

   //! \brief  Constructor.
   //! \param  jobId Job identifer.
   //! \param  toolId Unique identifier for tool.
   //! \param  name Name of tool.

   Tool(uint64_t jobId, uint32_t toolId, std::string name)
   {
      set(jobId, toolId, name);
   }

   //! \brief  Constructor.
   //! \param  channel Data channel for tool.

   Tool(bgcios::LocalStreamSocketPtr channel)
   {
      // Initialize data members.
      _channel = channel;
      _jobId = 0;
      _toolId = 0;
      _tag = "[Tool ?] ";
      std::ostringstream prefix;
      prefix << "Job " << _jobId << ": " << _tag;
      _prefix = prefix.str();
   }

   //! \brief  Set information identifying tool.
   //! \param  jobId Job identifier.
   //! \param  toolId Unique identifier for tool.
   //! \param  name Name of tool.
   //! \return Nothing.

   void set(uint64_t jobId, uint32_t toolId, std::string name)
   {
      _jobId = jobId;
      _toolId = toolId;
      _name = name;
      std::ostringstream tag;
      if (_name.empty()) {
         tag << "[Tool ";
      }
      else {
         tag << "[" << _name << " ";
      }
      tag << _toolId << "] ";
      _tag = tag.str();
      std::ostringstream prefix;
      prefix << "Job " << _jobId << ": " << _tag;
      _prefix = prefix.str();
   }

   //! \brief  Get the job identifier.
   //! \return Job id value.

   uint64_t getJobId(void) const { return _jobId; }

   //! \brief  Get the unique tool id.
   //! \return Tool id value.

   uint32_t getToolId(void) const { return _toolId; }

   //! \brief  Get the data channel pointer for tool.
   //! \return Pointer to data channel object.

   const bgcios::LocalStreamSocketPtr& getChannel(void) const { return _channel; } 

   //! \brief  Get the name of the tool.
   //! \return Name string.

   const std::string getName(void) const { return _name; }

   //! \brief  Get the tag to identify tool in trace points.
   //! \return Tag string.

   const std::string getTag(void) const { return _tag; }

   //! \brief  Get the prefix to identify tool and job in trace points.
   //! \return Prefix string.

   const std::string getPrefix(void) const { return _prefix; }


private:

   //! Job identifier.
   uint64_t _jobId;

   //! Unique identifier for tool.
   uint32_t _toolId;

   //! Pointer to data channel object.
   bgcios::LocalStreamSocketPtr _channel;

   //! Name of tool (reported by tool in Attach message).
   std::string _name;

   //! Tag to identify tool in trace points.
   std::string _tag;

   //! Prefix to identify job and tool in trace points.
   std::string _prefix;
};

//! Smart pointer for Tool object.
typedef std::tr1::shared_ptr<Tool> ToolPtr;

} // namespace toolctl

} // namespace bgcios

#endif // TOOLCTL_TOOL_H

