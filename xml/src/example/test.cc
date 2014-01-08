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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#include <unistd.h>

#include <iostream>
#include <fstream>

#include "xml/include/c_api/Picture.h"


using namespace std;

int main(int argc, char *argv[]) {

  if (argc != 2) {
    cerr << "usage: " << argv[0] << " file.xml" << endl;
    exit(1);
  }


  ifstream fin(argv[1]);

  if (!fin) {
    cerr << "Cannot open file " << argv[1] << endl;
    exit(1);
  }

  Picture myPicture;

  bool rc = myPicture.read(fin);
  if (!rc) {
    cerr << "Failed to load machine picture" << endl;
    exit(1);
  }

  cout << myPicture << endl;

  return 0;

};

