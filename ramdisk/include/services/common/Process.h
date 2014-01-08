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

//! \file  Process.h
//! \brief Declaration and methods for bgcios::Process class.

#ifndef COMMON_PROCESS_H
#define COMMON_PROCESS_H

// Includes
#include <ramdisk/include/services/common/MessageResult.h>
#include <sys/wait.h>
#include <errno.h>
#include <string>
#include <vector>

namespace bgcios
{

//! \brief  Manage a child process.

class Process
{
public:

   //! \brief  Default constructor.

   Process()
   {
      _processId = 0;
      _status = NeedStatus;
   }

   //! \brief  Constructor.
   //! \param  program Path to executable program.

   Process(std::string program)
   {
      _program = program;
      _processId = 0;
      _status = -1;
   }

   //! \brief  Default destructor.

   virtual ~Process() { }

   //! \brief  Add an argument to the list.
   //! \param  arg Argument string.
   //! \return Nothing.

   void addArgument(std::string arg) { _arguments.push_back(arg); }

   //! \brief  Add a list of arguments.
   //! \param  args Vector of argument strings.
   //! \return Nothing.

   void addArguments(std::vector <std::string>& args)
   {
      for (std::vector<std::string>::iterator iter = args.begin(); iter != args.end(); ++iter) {
         _arguments.push_back(*iter);
      }
      return;
   }

   //! \brief  Add an environment variable to the list.
   //! \param  env Environment variable string.
   //! \return Nothing.

   void addEnvironVariable(std::string env) { _environVariables.push_back(env); }

   //! \brief  Add a list of environment variables.
   //! \param  envs Vector of environment variable strings.
   //! \return Nothing.

   void addEnvironVariables(std::vector <std::string>& envs)
   {
      for (std::vector<std::string>::iterator iter = envs.begin(); iter != envs.end(); ++iter) {
         _environVariables.push_back(*iter);
      }
      return;
   }

   //! \brief  Create a new process and start the program.
   //! \return Result of operation.

   virtual bgcios::MessageResult start(void)
   { 
      bgcios::MessageResult result(bgcios::RequestFailed, ENOTSUP);
      return result;
   }

   //! \brief  Send a signal to the process.
   //! \param  signo Signal number to send.
   //! \return 0 when successful, errno when unsuccessful.

   virtual int signal(int signo)
   {
      int rc = kill(_processId, signo);
      if (rc != 0) rc = errno;
      return rc;
   }
   
   //! \brief  Tell the process to stop.
   //! \return 0 when successful, errno when unsuccessful.

   int stop(void)
   { 
      int rc = signal(SIGSTOP);
      if (rc != 0) rc = errno;
      return rc;
   }

   //! \brief  Tell the process to end.
   //! \return 0 when successful, errno when unsuccessful.

   int end(void)
   {
      int rc = signal(SIGTERM);
      if (rc != 0) rc = errno;
      return rc;
   }

   //! \brief  Wait for the process to end.
   //! \return 0 when successful, errno when unsuccessful.

   int waitFor(void)
   {
      int rc = 0;
      if (!isWaitedFor()) {
         pid_t child = waitpid(_processId, &_status, 0);
         if (child == -1) rc = errno;
      }
      return rc;
   }

   //! \brief  Wait for the process to end.
   //! \param  status Pointer to integer for storing status of process.
   //! \return 0 when successful, errno when unsuccessful.

   int waitFor(int *status)
   {
      int rc = 0;
      if (!isWaitedFor()) {
         pid_t child = waitpid(_processId, &_status, 0);
         if ((child == _processId) && (status != NULL)) *status = _status;
         if (child == -1) rc = errno;
      }
      return rc;
   }

   //! \brief  Wait for the process to end with a timeout.
   //! \param  timeoutSeconds Number of seconds to wait for process to end.
   //! \return 0 when successful, errno when unsuccessful.

   int waitFor(time_t timeoutSeconds);

   //! \brief  Wait for any child process to end.
   //! \param  child Pointer to pid_t for storing process id of child process that ended.
   //! \param  status Pointer to integer for storing status of process.
   //! \return 0 when successful, errno when unsuccessful.

   static int waitForAny(pid_t *child, int *status)
   {
      int rc = 0;
      *child = waitpid(-1, status, 0);
      if (*child == -1) rc = errno;
      return rc;
   }

   //! \brief  Check if process has been waited for and status is available.
   //! \return Return true if status is available, otherwise false.

   bool isWaitedFor(void) const { return (_status == NeedStatus ? false : true); }

   //! \brief  Check if process is running (i.e. not ended).
   //! \return True if process is running, otherwise false.

   bool isRunning(void)
   {
      if (!isWaitedFor()) {
         pid_t child = waitpid(_processId, &_status, WNOHANG);
         if (child == 0) {
            return true;
         }
      }
      return false;
   }

   //! \brief  Check if process has exited.
   //! \return True if process exited, otherwise false.

   bool isExited(void) const { return WIFEXITED(_status); }

   //! \brief  Get the exit status of the process.
   //! \return Exit status value.

   int getExitStatus(void) const { return WEXITSTATUS(_status); }

   //! \brief  Check if process has been signaled.
   //! \return True if process signaled, otherwise false.

   bool isSignaled(void) const { return WIFSIGNALED(_status); }

   //! \brief  Get the terminating signal number.
   //! \return Signal number.

   int getTermSignal(void) const { return WTERMSIG(_status); }

   //! \brief  Get the status of the process.
   //! \return Status value.

   int getStatus(void) const { return _status; }

   //! \brief  Get the process id.
   //! \return Process id.

   pid_t getProcessId(void) const { return _processId; }

   //! \brief  Get name of the process.
   //! \return Path to executable program.

   const std::string& getName(void) const { return _program; }

protected:

   //! \brief  Prepare child process for running executable program.
   //! \param  args Array of pointers to argument strings.
   //! \param  envs Array of pointers to environment variable strings.
   //! \return Nothing.

   void prepareChild(char *args[], char *envs[]);

   //! \brief  Run the executable program in the child process.
   //! \return No return when successful, errno when unsuccessful.

   int runChild(void);

   //! Process id.
   pid_t _processId;

   //! Status of process.
   int _status;

   //! Special value indicating status has not yet been obtained.
   static const int NeedStatus = -1;

   //! Path name to program.
   std::string _program;

   //! List of arguments for program.
   std::vector <std::string> _arguments;

   //! List of environment variables for program.
   std::vector <std::string> _environVariables;

};

} // namespace bgcios

#endif // COMMON_PROCESS_H

