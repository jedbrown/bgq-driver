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
    std::string rackid = "R00";
    RackBitset bs(rackid);
    std::string test_string = "R00-M0-N00-J00";
    bs.Set(test_string);
    std::cout << "step 1:"
              << test_string << " " << bs.Flagged(test_string)
              << " = " << test_string << " " << "1" << std::endl;

    bs.Reset(test_string);
    std::cout << "step 2:"
              << test_string << " " << bs.Flagged(test_string)
              << " = " << test_string << " " << "0" << std::endl;

    bs.Set(test_string);

    test_string = "R00-M1-N12-J31";
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

    test_string = "R00-M1-N15-J31";
    bs.Set(test_string);

    std::vector<std::string> locs;
    bs.FindUnset(locs);
    for(std::vector<std::string>::iterator it = locs.begin();
        it != locs.end(); ++it) {
        std::cout << *it << std::endl;
    }

    std::cout << "unset count is " << locs.size() << ".  It should be 1021"
              << std::endl;

    // Now start over setting =all=
    RackBitset cs(rackid);

    for(int midplane = 0; midplane <= 1; ++midplane) {
        for(int board = 0; board <= 15; ++board) {
            for(int node = 0; node <= 31; ++node) {
                std::ostringstream locstr;
                locstr << "R00-";
                if(midplane == 0)
                    locstr << "M0-";
                else locstr << "M1-";
                if(board < 10) {
                    locstr << "N0" << boost::lexical_cast<std::string>(board) << "-";
                } else {
                    locstr << "N" << boost::lexical_cast<std::string>(board) << "-";
                }
                if(node < 10) {
                    locstr << "J0" << boost::lexical_cast<std::string>(node);
                } else {
                    locstr << "J" << boost::lexical_cast<std::string>(node);
                }
                std::string setstr = locstr.str();
                cs.Reset(setstr);
                cs.Set(setstr);
            }
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
    RackBitset ds(rackid);
    // First, set all.
    std::cout << "Testing setting all bits..." << std::endl;
    test_string = "R00";
    ds.Set(test_string);
    std::cout << "Set count is " << ds.BitsSet() << ".  It should be 1024." << std::endl;
    ds.Reset();

    std::cout << "Testing setting a midplane..." << std::endl;
    test_string = "R00-M1";
    ds.Set(test_string);
    std::cout << "Set count is " << ds.BitsSet() << ".  It should be 512." << std::endl;
    std::vector<std::string> unset;
    ds.FindUnset(unset);
    for(std::vector<std::string>::iterator i = unset.begin(); i != unset.end(); ++i) {
        if(i->find("R00-M1") != std::string::npos) {
            std::cerr << "FAIL.  Found a location that should have been set in the unset list: " << *i << std::endl;
            exit(1);
        }
    }
    std::cout << "Midplane setting PASS" << std::endl;

    std::cout << "Testing setting Node board bits..." << std::endl;
    ds.Reset();
    unset.clear();
    test_string = "R00-M1-N10";
    ds.Set(test_string);
    std::cout << "Set count is " << ds.BitsSet() << ".  It should be 32." << std::endl;
    if(ds.BitsSet() != 32)
        exit(1);

    ds.FindUnset(unset);
    for(std::vector<std::string>::iterator i = unset.begin(); i != unset.end(); ++i) {
        if(i->find("R00-M1-N10") != std::string::npos) {
            std::cerr << "FAIL.  Found a location that should have been set in the unset list: " << *i << std::endl;
            exit(1);
        }
    }
    std::cout << "Node board setting PASS" << std::endl;

    return 0;
}
