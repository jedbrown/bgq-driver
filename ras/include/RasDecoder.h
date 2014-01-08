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
/* (C) Copyright IBM Corp.  2007, 2011                              */
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
#ifndef RASDECODER_H_
#define RASDECODER_H_

#include "RasEventHandler.h"

#include <set>
#include <string>
#include <map>
#include <pthread.h>

class RasDecoder : public RasEventHandler
{
 public:
  virtual RasEvent& handle(RasEvent& event, const RasEventMetadata& metadata);
  virtual const std::string& name() { return _name; }
  RasDecoder();
  RasDecoder(const std::string& lib, const std::string& decoder);
  virtual ~RasDecoder();
  static void setInstallLibPath(std::string path);
  static std::string getInstallLibPath();
  static void setTestLibPath(std::string path);
  static std::string getTestLibPath();
  static void readRasDecoder();
  static void setDecoder(const std::string& decoder, RasDecoder* ptr);
  static void clearDecoder();

 private:
  const std::string _name;
  const std::string _libName;
  const std::string _decoder;
  void* _dlhandle;
  void* _dlsym;
  static std::string _installLibPath;
  static std::string _testLibPath;
  static pthread_mutex_t _dcLock;
  static std::map<std::string, RasDecoder*> _decoders;

  static void findDecoderFiles(std::string& dir, std::map<std::string, std::string>& files);
  static void populateMailboxVector(const RasEvent& event, std::vector<uint64_t>& mbox);
};

#endif /*RASDECODER_H_*/
