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

#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include "RasEventMetadata.h"
#include "RasEventMetadataImpl.h"
#include "RasLog.h"

using namespace std;

#ifdef __linux
LOG_DECLARE_FILE("ras");
#endif

pthread_mutex_t RasEventMetadata::_mdLock = PTHREAD_MUTEX_INITIALIZER;
std::map<std::string, RasEventMetadata*> RasEventMetadata::_metadatas;

string RasEventMetadata::installPath = "/bgsys/drivers/ppcfloor";
void RasEventMetadata::setInstallPath(string path) { 
  installPath = path; 
  LOG_TRACE_MSG( "RasEventMetadata install path=" <<  path );
}
string RasEventMetadata::getInstallPath() { return installPath; }

string RasEventMetadata::testPath = "";
void RasEventMetadata::setTestPath(string path) { 
  testPath = path; 
  LOG_TRACE_MSG( "RasEventMetadata test path=" << path );
}
string RasEventMetadata::getTestPath() { return testPath; }

#include <c_api/RasEventMetadatas.h>
using namespace RasMeta;

void RasEventMetadata::readRasMetadata() {
  // create a map of metadata file names to path plus filename
  map<string, string> metadataFiles;
  // for each location of the metadata files
  //   add file names to map (replacing previous entries of the 
  //   same filename)
  //   directory order is 
  //      1. default install location - /bgsys/drivers/ppcfloor/component-dir/ras/metadata
  //      2. floor 
  //      3. working directory 
  //   The floor and working directory won't exist on a production system
  string ipath = getInstallPath();
  if (ipath != "") 
    findMetadataFiles(ipath, metadataFiles);
  string tpath = getTestPath();
  if (tpath != "" && tpath != ipath) 
    findMetadataFiles(tpath, metadataFiles);
  map<string,string>::iterator iter;   
  for( iter = metadataFiles.begin(); iter != metadataFiles.end(); iter++ ) {
    string file = iter->second;
    LOG_TRACE_MSG("RasEventMetatdata reading file: " << file);
    ifstream fin(file.c_str());
    if (fin) {
      BgRasEventMetadatas metadatas;
      bool ok = metadatas.read(fin);
      if (ok) {
	vector<BgRasEventMetadatas::rasevent>& metas = metadatas._metadatas;
	for (unsigned i = 0; i < metas.size(); ++i) {
	  RasEventMetadata* ptr =  new RasEventMetadataImpl(metas[i]._id, metas[i]._category, metas[i]._component, metas[i]._severity, metas[i]._message, metas[i]._decoder, metas[i]._control_action);
	  setMetadata(metas[i]._id, ptr); 
	}	
      }
      else
	LOG_WARN_MSG("RasEventMetadata failed to read Ras metadata: " << file);
    }
    else
      LOG_WARN_MSG("RasEventMetadata cannot open file " << file);
  }
}

#include <dirent.h>

void RasEventMetadata::findMetadataFiles(string& dir, map<string,string>& files) {
  if (dir == "") {
    LOG_ERROR_MSG("RasEventMetadata install path is not set");
    return;
  }

  DIR *d = opendir(dir.c_str());
  if (d == NULL) {
    LOG_ERROR_MSG("RasEventMetadata install path " << dir << " does not exist.");
    return;
  } 

  string pat = "_ras.xml"; 
  struct dirent *entry = readdir(d);
  bool foundMetadata = false;
  while (entry != NULL) {
    string name = entry->d_name;
    string dir2 = dir + "/" + name + "/" + "ras/metadata";
    DIR *d2 = opendir(dir2.c_str());
    if (d2 == NULL) {
      entry = readdir(d);
      continue;
    }

    struct dirent *entry2 = readdir(d2);
    while (entry2 != NULL) {
      string name2 = entry2->d_name;
      string::size_type index = name2.find(pat); 
      // if the entry is a ras extract add it to the map (replacing existing entries)
      if (index != string::npos && (index == (name2.size() - pat.size()))) {
	files[name2] = dir2 + "/" + name2;
	foundMetadata = true;
      }
      entry2 = readdir(d2);      
    }
    closedir(d2);
  
    entry = readdir(d);
  }
  closedir( d );

  if (foundMetadata == false)
    LOG_ERROR_MSG("Can't find RasEventMetadata files under path " << dir);
}

#include "RasSmartLock.h"

void RasEventMetadata::setMetadata(const string& msgId, RasEventMetadata* ptr)
{
  RasSmartLock lock(&_mdLock);
  _metadatas[msgId] = ptr;
}


const RasEventMetadata& RasEventMetadata::factory(uint32_t eventId)
{
  stringstream stm;
  stm << hex << uppercase << setfill('0')
      << setw(8) << eventId;
  string id = stm.str();
  return factory(id);
}

const RasEventMetadata& RasEventMetadata::factory(const std::string& msgId)
{
  // return the metadata if in the cache
  RasEventMetadata* ptr;
  pthread_mutex_lock(&_mdLock);
  map<string,RasEventMetadata*>::iterator iter = _metadatas.find(msgId);
  if( iter != _metadatas.end() ) {
    ptr = _metadatas[msgId];
    pthread_mutex_unlock(&_mdLock);
    return *ptr;
  }
  pthread_mutex_unlock(&_mdLock);
  return factory_default(msgId);
}

const RasEventMetadata& RasEventMetadata::factory_default(const std::string& msgId)
{
  RasEventMetadata* ptr;
  ptr = new RasEventMetadataImpl(msgId);
  setMetadata(msgId, ptr);
  return *ptr;
}

void RasEventMetadata::clearMetadata()
{
  RasSmartLock lock(&_mdLock);
  // Clear memory allocated by RasEventMetadata
  map<string, RasEventMetadata*>::iterator iter;
  for( iter = _metadatas.begin(); iter != _metadatas.end(); iter++ ) {
      delete iter->second;
  }
  _metadatas.clear();
}

