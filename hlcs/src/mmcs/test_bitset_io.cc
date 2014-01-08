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

#include <vector>
#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "RackBitset.h"

int main() {
    // First, set some
    std::string rackid = "Q00";
    IOBoardBitset bs(rackid);
    std::string test_string = "Q00-I0-J00";
    bs.Set(test_string);
    std::cout << "step 1:"
              << test_string << " " << bs.Flagged(test_string)
              << " = " << test_string << " " << "1" << std::endl;

    bs.Reset(test_string);
    std::cout << "step 2:"
              << test_string << " " << bs.Flagged(test_string)
              << " = " << test_string << " " << "0" << std::endl;

    bs.Set(test_string);

    test_string = "Q00-IF-J07";
    std::cout << "step 3:"
              << test_string << " " << bs.Flagged(test_string)
              << " = " << test_string << " " << "0" << std::endl;

    bs.Set(test_string);
    std::cout << "step 4:"
              << test_string << " " << bs.Flagged(test_string)
              << " = " << test_string << " " << "1" << std::endl;

    bs.Reset(test_string);
    std::cout << "step 5:"
              << test_string << " " << bs.Flagged(test_string)
              << " = " << test_string << " " << "0" << std::endl;

    bs.Set(test_string);
    std::cout << "step 6:"
              << test_string << " " << bs.Flagged(test_string)
              << " = " << test_string << " " << "1" << std::endl;

    test_string = "Q00-IF-J07";
    bs.Set(test_string);

    std::vector<std::string> locs;
    bs.FindUnset(locs);
    for(std::vector<std::string>::iterator it = locs.begin();
        it != locs.end(); ++it) {
        std::cout << *it << std::endl;
    }

    std::cout << "unset count is " << locs.size() << ".  It should be 126"
              << std::endl;

    // Now start over setting =all=
    IOBoardBitset cs(rackid);

    for(int board = 0; board <= 15; ++board) {
        for(int node = 0; node <= 7; ++node) {
            std::ostringstream locstr;
            locstr << "Q00-";
            locstr << "I" << std::hex << std::uppercase << board << "-";
            locstr << "J0" << node;

            std::string setstr = locstr.str();
            cs.Reset(setstr);
            cs.Set(setstr);
        }
    }

    std::vector<std::string> newlocs;
    cs.FindUnset(newlocs);
    for(std::vector<std::string>::iterator it = newlocs.begin();
        it != newlocs.end(); ++it) {
    }

    std::cout << "unset count is " << newlocs.size() << ".  It should be 0"
              << std::endl;

    // Now test batch settings for compute racks
    IOBoardBitset ds(rackid);
    // First, set all.
    std::cout << "Testing setting all bits..." << std::endl;
    test_string = "Q00";
    ds.Set(test_string);
    std::cout << "Set count is " << ds.BitsSet() << ".  It should be >= 128." << std::endl;
    ds.Reset();

    std::cout << "Testing setting all bits..." << std::endl;
    test_string = "R00";
    ds.Set(test_string);
    std::cout << "Set count is " << ds.BitsSet() << ".  It should be >= 128." << std::endl;
    ds.Reset();

    std::cout << "Testing setting a board..." << std::endl;
    test_string = "Q00-I0";
    ds.Set(test_string);
    std::cout << "Set count is " << ds.BitsSet() << ".  It should be 8." << std::endl;
    std::vector<std::string> unset;
    ds.FindUnset(unset);
    for(std::vector<std::string>::iterator i = unset.begin(); i != unset.end(); ++i) {
        if(i->find("R00-M1") != std::string::npos) {
            std::cerr << "FAIL.  Found a location that should have been set in the unset list: " << *i << std::endl;
            exit(1);
        }
    }
    std::cout << "Board setting PASS" << std::endl;

    return 0;
}
