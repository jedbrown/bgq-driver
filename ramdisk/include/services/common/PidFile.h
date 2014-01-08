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

//! \file  PidFile.h 
//! \brief Declaration and methods for bgcios::PidFile class.

#ifndef COMMON_PIDFILE_H
#define COMMON_PIDFILE_H

// Includes
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <tr1/memory>

namespace bgcios
{

//! Create a pid file for a daemon.

class PidFile
{
public:

   //! \brief  Default constructor.
   //! \param  name Base name of pid file.
   //! \param  isHardware True if running on hardware.

   PidFile(std::string name, bool isHardware)
   {
      // Get the process id of the daemon.
      _processId = getpid();

      // Store the process id of the daemon in the pid file when running on hardware.
      if (isHardware) {
         std::ostringstream fileName;
         fileName << "/var/run/" << name << ".pid";
         _path = fileName.str();
         std::ofstream pidFile(_path.c_str(), std::ios_base::trunc);
         pidFile << _processId << std::endl;
      }
   }

   //! \brief  Default destructor.

   ~PidFile()
   {
      ::unlink(_path.c_str());
   }

   //! \brief  Get the process id of the daemon.
   //! \return Process id.

   pid_t getPid(void) const { return _processId; }

   //! \brief  Get the path to the pid file.
   //! \return Pid file path.

   const std::string getPath(void) const { return _path; }

private:

   //! Process id of the daemon.
   pid_t _processId;

   //! Path to pid file for the daemon.
   std::string _path;

};


//! Smart pointer for PidFile object.
typedef std::tr1::shared_ptr<PidFile> PidFilePtr;

} // namespace bgcios

#endif // COMMON_PIDFILE_H


