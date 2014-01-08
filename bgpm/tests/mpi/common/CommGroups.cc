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

#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <strstream>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "TestArgs.h"
#include "CommGroups.h"
#include "Timer.h"



CommGroups::CommGroups(const string & fileName)
: iamExcluded(false), numExcluded(0), iamAGroupHead(false), bmComm(MPI::COMM_NULL), testComm(MPI::COMM_NULL)
{
    int fileSize = 0;
    char *pFileBuff = NULL;

    worldGrp = MPI::COMM_WORLD.Get_group();
    if (fileName.empty()) {
        testGrp = MPI::COMM_WORLD.Get_group();
        bmComm = MPI::COMM_WORLD;
        testComm = MPI::COMM_WORLD;
        
        int heads[2] = { 0 };
        headGrp = worldGrp.Incl(1, heads);
        headComm = MPI::COMM_WORLD.Create(headGrp);
        iamAGroupHead = (MPI::COMM_WORLD.Get_rank() == 0);
        return;
    }
    
    
    if (MPI::COMM_WORLD.Get_rank() == 0) {
        ifstream file(fileName.c_str());
        if (! file) {
            die2("ERROR: Can't open '%s'; %s\n", fileName.c_str(), strerror(errno));
        }

        //PerfTimer timer;
        //timer.Start();
        // get file contents
        fileSize = GetFileSize(fileName);
        pFileBuff = new char[fileSize + 1];
        file.read(pFileBuff, fileSize);
        if (!file) {
            die2("ERROR: Read file problem '%s'; %s\n", fileName.c_str(), strerror(errno));
        }
        fileSize = file.gcount();
        pFileBuff[fileSize] = '\0';
        //timer.Stop();
        //cout << "group file read time = " << timer.ElapsedTime() * 1.0e6 << " us" << ", size=" << fileSize << endl;

        //timer.Reset();
        //timer.Start();
        VerifyRanksInFile(fileName, pFileBuff, fileSize);
        //timer.Stop();
        //cout << "group file verify time = " << timer.ElapsedTime() * 1.0e6 << " us" << endl;
    }
    
    MPI::COMM_WORLD.Barrier();
    GetBroadCastFile(pFileBuff, fileSize);
    
    BuildCommGroups(pFileBuff, fileSize);
    
    delete pFileBuff;
}


CommGroups::~CommGroups() 
{
    if ((bmComm != MPI::COMM_NULL) && (bmComm != MPI::COMM_WORLD)) {
        bmComm.Free();
    }
    if ((testComm != MPI::COMM_NULL) && (testComm != MPI::COMM_WORLD)) {
        testComm.Free();
    }
    if ((headComm != MPI::COMM_NULL) && (headComm != MPI::COMM_WORLD)) {
        headComm.Free();
    }
    //worldGrp.Free();
    //headGrp.Free();
    //testGrp.Free();
}

int CommGroups::GetWorldRank(int testRank)
{
    int targRank = 0;
    MPI::Group::Translate_ranks(testGrp, 1, &testRank, worldGrp, &targRank);
    return targRank;
}

size_t CommGroups::GetFileSize(const string & fileName)
{
    struct stat buf;
    if (stat(fileName.c_str(), &buf)) {
        die2("ERROR: stat operation on file '%s' failed; %s\n", fileName.c_str(), strerror(errno));
    }
    return (buf.st_size);
}

void CommGroups::GetNextGroup(istrstream & grpFile, vector<int> & ranks, string & grpLabel)
{
    string line;
    int linenum = 1;
    bool done = false;
    while (!done && getline(grpFile, line)) {
        // cout << linenum << ": '" << line << "'" << endl;
        grpLabel.clear();
        
        istringstream lineStrm(line);
        lineStrm >> grpLabel;
        //cout << linenum << ": label:" << "'" << label << "'" << endl;

        if ((grpLabel == "") || (grpLabel[0] == '#')) { }
        else if ((grpLabel == "EXCLRANKS:") || (grpLabel == "RANKGROUP:")) {
            int rank;
            while (lineStrm >> rank) {
                ranks.push_back(rank);
            }
            done = true;
        }
        else {
            die2("ERROR: Don't recognize label '%s' in group file line %d\n",
                 grpLabel.c_str(), linenum);
        }
        linenum++;
    }
}

void CommGroups::VerifyRanksInFile(const string & fileName, const char *pFileBuff, int len)
{
    // Verify that all world ranks are represeted.
    vector<int> ranks;
    // Reserve elements - We'll guess that the avg rank is 5 chars + a blank
    // (won't worry about new lines and labels - close enough)
    ranks.reserve(len/6);

    string label;
    istrstream grpFile(pFileBuff, len);
    while (grpFile) {
        GetNextGroup(grpFile, ranks, label); 
    }
    
    if (ranks.empty()) {
        die1("ERROR: no ranks found in group file '%s'\n", fileName.c_str());
    }

    //PerfTimer timer;
    //timer.Start();
    sort(ranks.begin(), ranks.end());
    //timer.Stop();
    //cout << "group ranks sort time = " << timer.ElapsedTime() * 1.0e6 << " us" << endl;

    bool foundError = false;
    if (ranks.size() != (unsigned int)MPI::COMM_WORLD.Get_size()) {
        foundError = true;
        cout << "ERROR: The number of ranks found in the group file '" << fileName << "' (" << ranks.size()
          << ") do not match the mpi world size (" << MPI::COMM_WORLD.Get_size()
          << ")" << endl;
    }

    // make sure there are no repeats or holes in the ranks.
    int curRank = 0;
    vector<int> errorRanks;
    vector<int>::iterator pos = ranks.begin();
    while (pos<ranks.end()) {
        if (*pos > curRank) {
            foundError = true;
            errorRanks.push_back(curRank);
            curRank++;
        }
        else if (*pos < curRank) {  // must be a duplicate
            foundError = true;
            errorRanks.push_back(curRank);
            ++pos;
        }
        else {
            ++curRank;
            ++pos;
        }
    }

    if (foundError) {
        PRINT_ELEMENTS(cout, errorRanks, "ERROR: the following ranks in the group file are duplicates or missing:\n");
        exit(-1);
    }        
}

void CommGroups::GetBroadCastFile(char *& pFileBuff, int & len)
{
    MPI::COMM_WORLD.Bcast(&len, 1, MPI_INT, 0);
    if (MPI::COMM_WORLD.Get_rank() > 0) {
        pFileBuff = new char[len+1];
        pFileBuff[len] = '\0';
    }
    MPI::COMM_WORLD.Bcast(pFileBuff, len, MPI::BYTE, 0);
}

void CommGroups::BuildCommGroups(char *& pFileBuff, int & len)
{
    // Verify that all world ranks are represeted.
    vector<int> ranks;
    vector<int> grpHeads;
    ranks.reserve(len/6);
    
    string label;
    istrstream grpFile(pFileBuff, len);
    bool curRankFound = false;
    while (grpFile) {
        ranks.clear();
        label.clear();
        
        GetNextGroup(grpFile, ranks, label); 
        
        // assumes rank in only one group.
        if (label == "EXCLRANKS:") {
            if (ranks.empty()) bmComm = MPI::COMM_WORLD;
            else {
                // Create comm is collective - every rank must make this call - whether part of the group or not.           
                MPI::Group worldGrp = MPI::COMM_WORLD.Get_group();
                MPI::Group grp = worldGrp.Excl(ranks.size(), &ranks[0]);
                bmComm = MPI::COMM_WORLD.Create(grp);
                //PRINT_ELEMENTS(cout, ranks, "excluded:");
                //cout << "bmComm.Size = " << bmComm.Get_size() << endl;
           
                numExcluded = ranks.size();
                if (MPI::COMM_WORLD.Get_rank() == 0) {
                    cout << "WARNING: " << numExcluded << " ranks have been explicitly excluded from test." << endl;
                }
                
                // The excluded rank processes now need to create a new communicator 
                // to sync with the collective created performed by all the other groups
                // processes in the RANKGROUP: match below. 
                vector<int>::iterator pos = find(ranks.begin(), ranks.end(), MPI::COMM_WORLD.Get_rank());
                if (pos < ranks.end()) {
                    iamExcluded = true;
                    curRankFound = true;
                    
                    MPI::Group worldGrp = MPI::COMM_WORLD.Get_group();
                    MPI::Group grp = worldGrp.Incl(ranks.size(), &ranks[0]);
                    testComm = MPI::COMM_WORLD.Create(grp);
                    testGrp = testComm.Get_group();
                }               
            }
        }
        else if (label == "RANKGROUP:") {
            // keep list of group heads
            grpHeads.push_back(ranks[0]);
            // cout << "groupHead?:" << MPI::COMM_WORLD.Get_rank() << endl;
   
            // see if the current rank is part of this group.
            if (!curRankFound) {
                //PRINT_ELEMENTS(cout, ranks, "rankgroup:");
                //cerr << "curRank1=" << MPI::COMM_WORLD.Get_rank() << endl;
                
                vector<int>::iterator pos = find(ranks.begin(), ranks.end(), MPI::COMM_WORLD.Get_rank());
                //cerr << "curRank2=" << MPI::COMM_WORLD.Get_rank() << endl;                
                
                if (pos < ranks.end()) {
                    //cerr << "curRank2a=" << MPI::COMM_WORLD.Get_rank() << endl;                
                    MPI::Group worldGrp = MPI::COMM_WORLD.Get_group();
                    //cerr << "curRank2b=" << MPI::COMM_WORLD.Get_rank() << endl;                                    
                    MPI::Group grp = worldGrp.Incl(ranks.size(), &ranks[0]);
                    //cerr << "curRank2c=" << MPI::COMM_WORLD.Get_rank() << endl;                   
                    testComm = MPI::COMM_WORLD.Create(grp);
                    testGrp = testComm.Get_group();
                    //cerr << "curRank2e=" << MPI::COMM_WORLD.Get_rank() << endl;                                                      
                    curRankFound = true;
                    //PRINT_ELEMENTS(cerr, ranks, "rankgroup:");
                }
                //cerr << "curRank3=" << MPI::COMM_WORLD.Get_rank() << endl;                
            }
        }
    }
    
    if (!curRankFound && ranks.empty()) {
        die1("ERROR: %d ranks found in group file\n", 0);
    }
    
    if (! grpHeads.empty()) {
        MPI::Group worldGrp = MPI::COMM_WORLD.Get_group();
        headGrp = worldGrp.Incl(grpHeads.size(), &grpHeads[0]);
        // PRINT_ELEMENTS(cout, grpHeads, "groupHeads:");
        headComm = MPI::COMM_WORLD.Create(headGrp);
        iamAGroupHead = ( find(grpHeads.begin(), grpHeads.end(), MPI::COMM_WORLD.Get_rank()) != grpHeads.end() );
    }
}
