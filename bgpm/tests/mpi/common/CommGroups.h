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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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
#ifndef CommGroups_h
#define CommGroups_h

#include <vector>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <strstream>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include "TestArgs.h"
using namespace std;


class CommGroups
{
  public:
    CommGroups(const string & fileName);
    ~CommGroups();
    int GetWorldRank(int testRank);

    bool iamExcluded;         // indicate if this rank is supposed to be excluded from this pass
    int numExcluded;           
    bool iamAGroupHead;       // indicate if this rank is a group head.
    MPI::Intracomm headComm;  // head of each simultaineous test comm (note: procs not part
                              // of this comm cannot test it - it's values will be MPI::COMM_NULL
                              // test headGrp instead.
    MPI::Intracomm bmComm;    // all procs included in bechmark test (use for barriers, & results)
    MPI::Intracomm testComm;  // procs part of the group this proc belongs to.
    MPI::Group worldGrp;      // use to translate ranks for debug
    MPI::Group testGrp;
    MPI::Group headGrp;       // use so all groups can test size or ranks
 
  private:   
    void GetNextGroup(istrstream & grpFile, vector<int>&ranks, string & grpLabel);
    void VerifyRanksInFile(const string & fileName, const char *pFileBuff, int len);
    void GetBroadCastFile(char *& pFileBuff, int & len);
    void BuildCommGroups(char *& pFileBuff, int & len);
    size_t GetFileSize(const string & fileName);
};


#endif
