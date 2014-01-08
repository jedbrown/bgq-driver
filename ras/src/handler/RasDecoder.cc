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
#include "RasDecoder.h"
#include "MailboxHandler.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include "RasLog.h"
#include <dlfcn.h>
#include <dirent.h>
#include "RasSmartLock.h"
#include "RasLog.h"

using namespace std;

#ifdef __linux
LOG_DECLARE_FILE("ras");
#endif

// Signature of RAS Decoder functions
typedef void (*_BG_RAS_Decoder_Fcn_t) (RasEvent& event, const vector<uint64_t>& mbox_details);

pthread_mutex_t RasDecoder::_dcLock = PTHREAD_MUTEX_INITIALIZER;
map<string, RasDecoder*> RasDecoder::_decoders;

RasDecoder::RasDecoder() :
  _name("RasDecoder"),
  _libName(),
  _decoder(),
  _dlhandle(NULL)
{
}

RasDecoder::RasDecoder (const string& lib, const string& decoder)
  : _libName(lib), _decoder(decoder), _dlhandle(NULL)
{
}

RasDecoder::~RasDecoder()
{
}

string RasDecoder::_installLibPath = "/bgsys/drivers/ppcfloor";
void RasDecoder::setInstallLibPath(string path) {
  _installLibPath = path;
}
string RasDecoder::getInstallLibPath() {
  return _installLibPath;
}

string RasDecoder::_testLibPath = "";
void RasDecoder::setTestLibPath(string path) {
  _testLibPath = path;
}
string RasDecoder::getTestLibPath() {
  return _testLibPath;
}

#include <c_api/RasDecoders.h>
using namespace BgRasDecoder;

void RasDecoder::readRasDecoder() {

  map<string, string> decoderFiles;

  string iPath = getInstallLibPath();
  if (iPath != "")
    findDecoderFiles(iPath, decoderFiles);

  string tPath = getTestLibPath();
  if (tPath != "")
    findDecoderFiles(tPath, decoderFiles);

  map<string, string>::iterator iter;
  for (iter = decoderFiles.begin(); iter != decoderFiles.end(); iter++) {
    string file = iter->second;
    LOG_TRACE_MSG("Open decoder file: " << file);
    ifstream fin(file.c_str());
    if (fin) {
      BgRasDecoders decoders;
      bool ok = decoders.read(fin);
      if (ok) {
	vector<BgRasDecoders::decoder>& vDecoders = decoders._decoders;
        if (tPath != "")
	  decoders._libname = tPath + "/" + decoders._libname;
	else
	  decoders._libname = iPath + "/" + decoders._libname;
	for (unsigned i=0; i < vDecoders.size(); ++i) {
	  RasDecoder *ptr = new RasDecoder(decoders._libname, vDecoders[i]._name);
	  setDecoder(vDecoders[i]._name, ptr);
	}
      }
    }
  }

}

void RasDecoder::findDecoderFiles(string& dir, map<string, string>& files) {

  if (dir == "") {
    LOG_ERROR_MSG("RAS decoder path is not specified." << dir);
    return;
  }

  DIR *d = opendir(dir.c_str());
  if (d == NULL) {
    LOG_ERROR_MSG("Can't open RAS decoder path: " << dir);
    return;
  }

  LOG_TRACE_MSG("Find decoder files in " << dir);
  string pat = "_decoder_lib.xml";
  struct dirent *entry = readdir (d);
  while (entry != NULL) {
    string name = entry->d_name;
    string dir2 = dir + "/" + name + "/" + "ras/decoder";
    DIR *d2 = opendir(dir2.c_str());
    if (d2 == NULL) {
      entry = readdir(d);
      continue;
    }

    struct dirent *entry2 = readdir(d2);
    while (entry2 != NULL) {
      string name2 = entry2->d_name;
      string::size_type index = name2.find(pat);
      if (index != string::npos && (index == (name2.size() - pat.size()))) {
	files[name2] = dir2 + "/" + name2;
      }
      entry2 = readdir(d2);
    }
    closedir(d2);
    entry = readdir(d);
  }
  closedir(d);
}


RasEvent& RasDecoder::handle(RasEvent& event, const RasEventMetadata& metadata)
{

  LOG_TRACE_MSG("RasDecoder handling event id=" << event.getDetail(RasEvent::MSG_ID).c_str() << " msg=" <<  event.getDetail(RasEvent::MESSAGE).c_str());

  // Handle mailbox processing first.
  // Retrieve the mailbox data string from the metadata
  string mboxPayload = event.getDetail(RasEvent::MBOX_PAYLOAD);
  MailboxHandler mboxHandler;
  if (mboxPayload != "")
    mboxHandler.handleMailbox(event, mboxPayload, metadata);
  else
    mboxHandler.handleNonMailbox(event, metadata);

  string decoder = event.getDetail(RasEvent::DECODER);
  if (decoder == "")
    return event;

  RasDecoder* ptr = NULL;
  pthread_mutex_lock(&_dcLock);
  map<string, RasDecoder*>::iterator iter = _decoders.find(decoder);
  if (iter != _decoders.end())
    ptr = _decoders[decoder];
  else {
    pthread_mutex_unlock(&_dcLock);
    LOG_ERROR_MSG("can't find the decoder " << decoder << " in the map");
    return event;
  }

  pthread_mutex_unlock(&_dcLock);

  // Open the library
  string lib = ptr->_libName;
  LOG_DEBUG_MSG("Opening " << lib);
  ptr->_dlhandle = dlopen(lib.c_str(), RTLD_LAZY); 
  if (ptr->_dlhandle == NULL) {
    LOG_ERROR_MSG("Error opening " << lib);
    return event;
  }

  // Load the symbol
  LOG_DEBUG_MSG("Loading " << decoder);
  _BG_RAS_Decoder_Fcn_t fcn = (void (*) (RasEvent&, const vector<uint64_t>&)) dlsym(ptr->_dlhandle, decoder.c_str());
  char* error = dlerror();
  if (error) {
    LOG_ERROR_MSG("Error loading: " << decoder);
    return event;
  }
  if (!fcn) {
    LOG_ERROR_MSG("Symbol not found: " << decoder);
    return event;
  }

  vector<uint64_t> mboxDetails;
  populateMailboxVector( event, mboxDetails );

  // Call the decoder
  fcn(event, mboxDetails);

  return event;
}

void RasDecoder::setDecoder(const string& decoder, RasDecoder* ptr) {
  RasSmartLock lock(&_dcLock);
  _decoders[decoder] = ptr;
}


void RasDecoder::populateMailboxVector(const RasEvent& event, std::vector<uint64_t>& mbox) {

  // Retrieve the mailbox data string from the metadata
  string mboxStr = event.getDetail(RasEvent::MBOX_PAYLOAD);

  // Pull out the "details" array and convert it into a vector:

  istringstream in(mboxStr, istringstream::in);

  uint64_t data;

  in >> hex >> data; // UCI
  in >> hex >> data; // message id, reserved, num args

  uint16_t numDetails = (data & 0xFFFF);

  while ( ! in.eof()  && (numDetails > 0) ) {
    in >> hex >> data;
    mbox.push_back(data);
    numDetails--;
  }
}

void RasDecoder::clearDecoder() {
  // Clear memory allocated by RasDecoder
  map<string, RasDecoder*>::iterator iter;
  for( iter = _decoders.begin(); iter != _decoders.end(); iter++ ) {
      delete iter->second;
  }
  _decoders.clear();
}
