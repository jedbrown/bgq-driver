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

#include <sys/types.h>
#include <exception>
#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>

#include "BgqCableBadWires.h"

using namespace std;

/**
 * Test the node board and IO drawer link chip bad fiber mask to the Torus IO cable bad wire mask
 * conversion routine implemented by the BgqCableBadWires class.
 *
 */

static bool samelocation(string x, string y)
{
	bool match = true;
	if (x.length() == y.length()) {
		for (uint i = 0; i < x.length(); ++i) {
			if (x[i] != y[i]) {
				match = false;
				break;
			}
		}
	}
	else {
		match = false;
	}
	return match;
}

static bool testbadwiremask(
		string board, 			// Node board or IO drawer location ("Rxx-Mx-Nxx", "R/Qxx-Ix")
		string linkChipPos, 	// Link chip position ("Uxx")
		string reg, 			// Link chip register ("C01", "C23")
		int fiberMask, 			// Bad fiber mask (12 bits)
		string expPortPos, 		// Expected node board or IO drawer port position ("Txx")
		long int expWireMask,	// Expected bad wire mask (12 or 48 bits)
		string fromPort,		// Transmitting port location ("Rxx-Mx-Nxx-Txx", "R/Qxx/Ix-Txx')
		long int aggWireMask,	// Aggregated bad wire mask from the database
		bool expExcp)			// true = expecting exception
{
	bool match = false;
	try {
		string linkChipLocation = board + "-" + linkChipPos;
		string expPortLocation = board + "-" + expPortPos;
		cout << "RxLinkChip=" << linkChipLocation << "(" << hex << reg << ") fiberMask=" << hex << fiberMask
				<< " TxPort=" << fromPort << " aggWireMask=" << hex << aggWireMask << endl;
		cout << "   --> (exp-RxPort=" << expPortLocation << " exp-wiremask=" << hex << expWireMask << ")" << endl;

		BgqCableBadWires badWireMask(linkChipLocation, reg, fiberMask);
		string port = badWireMask.getPortLocation();
		long int wireMask = badWireMask.getBadWireMask();

		badWireMask.setTxPortAndBadWireMask(fromPort, aggWireMask);
		string txChip = badWireMask.getTxLinkChipLocation();
		string txReg = badWireMask.getTxRegister();
		long int aggFiberMask = badWireMask.getAggregatedBadFiberMask();

		cout << "RxLinkChip=" << linkChipLocation << "(" << hex << reg << ") FiberMask=" << hex << fiberMask
				<< " TxPort=" << fromPort << " AggWireMask=" << hex << aggWireMask << endl;
		cout << "  --> RxPort=" << port << " TxChip=" << txChip << "(" << txReg << ")"
				<< " WireMask=" << hex << wireMask << " AggFiberMask=" << hex << aggFiberMask << endl;
		if (samelocation(port, expPortLocation) && wireMask == expWireMask) {
			match = true;
			cout << "  >>> PASS" << endl;
		}
		else {
			if (samelocation(port, expPortLocation) && wireMask != expWireMask) {
				cout << "  !!! FAIL: Unexpected port and bad wire mask!" << endl;
			}
			else if (!samelocation(port, expPortLocation)) {
				cout << "  !!! FAIL: Unexpected port!" << endl;
			}
			else if (wireMask != expWireMask) {
				cout << "  !!! FAIL: Unexpected bad wire mask!" << endl;
			}
		}
	}
	catch (const invalid_argument& iae) {
		if (expExcp) {
			cout << "  >>> PASS: Expected exception: " << iae.what() << endl;
			match = true;
		}
		else {
			cout << "  !!! FAIL: Unexpected exception: " << iae.what() << endl;
		}
	}
	return match;

} // End testbadwiremask


static bool testbadwiremask(
		string board, 			// Node board or IO drawer location ("Rxx-Mx-Nxx", "R/Qxx-Ix")
		string linkChipPos, 	// Link chip position ("Uxx")
		string reg, 			// Link chip register ("C01", "C23")
		int fiberMask, 			// Bad fiber mask (12 bits)
		string expPortPos, 		// Expected node board or IO drawer port position ("Txx")
		long int expWireMask,	// Expected bad wire mask (12 or 48 bits)
		bool expExcp)			// true = expecting exception
{
	bool match = false;
	try {
		string linkChipLocation = board + "-" + linkChipPos;
		string expPortLocation = board + "-" + expPortPos;
		cout << "linkChipLocation=" << linkChipLocation << " reg=" << hex << reg << " fiberMask=" << hex << fiberMask
				<< " (exp-port=" << expPortLocation << " exp-wiremask=" << hex << expWireMask << ")" << endl;
		BgqCableBadWires badWireMask(linkChipLocation, reg, fiberMask);
		string port = badWireMask.getPortLocation();
		long int wireMask = badWireMask.getBadWireMask();
		cout << "linkChipLocation=" << linkChipLocation << " reg=" << hex << reg << " fiberMask=" << hex << fiberMask
				<< " --> port=" << port << " wiremask=" << hex << wireMask << endl;
		if (samelocation(port, expPortLocation) && wireMask == expWireMask) {
			match = true;
			cout << "  >>> PASS" << endl;
		}
		else {
			if (samelocation(port, expPortLocation) && wireMask != expWireMask) {
				cout << "  !!! FAIL: Unexpected port and bad wire mask!" << endl;
			}
			else if (!samelocation(port, expPortLocation)) {
				cout << "  !!! FAIL: Unexpected port!" << endl;
			}
			else if (wireMask != expWireMask) {
				cout << "  !!! FAIL: Unexpected bad wire mask!" << endl;
			}
		}
	}
	catch (const invalid_argument& iae) {
		if (expExcp) {
			cout << "  >>> PASS: Expected exception: " << iae.what() << endl;
			match = true;
		}
		else {
			cout << "  !!! FAIL: Unexpected exception: " << iae.what() << endl;
		}
	}
	return match;

} // End testbadwiremask


int main(int argc, char **argv)
{
	int tests = 0;
	int pass = 0;

	cout << "Start testBgqCableBadWires" << endl;

	cout << "Start the IO Drawer tests" << endl;
	++tests; if (testbadwiremask("Q00-I0", "U05", "C01", 0xfc0, "T00", 0xfc0, "R00-M0-N00-T00", 0xfc1, false)) ++pass;
	++tests; if (testbadwiremask("Q01-I1", "U00", "C23", 0xfc0, "T01", 0x03f, "Q01-I2-T13", 0x83f, false)) ++pass;
	++tests; if (testbadwiremask("Q02-I2", "U05", "C01", 0x03f, "T02", 0xfc0, false)) ++pass;
	++tests; if (testbadwiremask("Q03-I3", "U00", "C23", 0x03f, "T03", 0x03f, false)) ++pass;
	++tests; if (testbadwiremask("Q04-I4", "U05", "C23", 0xfc0, "T04", 0x03f, false)) ++pass;
	++tests; if (testbadwiremask("Q05-I5", "U00", "C01", 0xfc0, "T05", 0xfc0, false)) ++pass;
	++tests; if (testbadwiremask("Q06-I6", "U05", "C23", 0x03f, "T06", 0x03f, false)) ++pass;
	++tests; if (testbadwiremask("Q07-I7", "U00", "C01", 0x03f, "T07", 0xfc0, false)) ++pass;

	++tests; if (testbadwiremask("Q00-I8", "U04", "C01", 0xfc0, "T08", 0xfc0, false)) ++pass;
	++tests; if (testbadwiremask("Q01-I9", "U01", "C23", 0xfc0, "T09", 0x03f, false)) ++pass;
	++tests; if (testbadwiremask("Q02-IA", "U04", "C01", 0x03f, "T10", 0xfc0, false)) ++pass;
	++tests; if (testbadwiremask("Q03-IB", "U01", "C23", 0x03f, "T11", 0x03f, false)) ++pass;
	++tests; if (testbadwiremask("R04-IC", "U04", "C23", 0xfc0, "T12", 0x03f, false)) ++pass;
	++tests; if (testbadwiremask("R05-ID", "U01", "C01", 0xfc0, "T13", 0xfc0, false)) ++pass;
	++tests; if (testbadwiremask("R06-IE", "U04", "C23", 0x03f, "T14", 0x03f, false)) ++pass;
	++tests; if (testbadwiremask("R07-IF", "U01", "C01", 0x03f, "T15", 0xfc0, false)) ++pass;

	++tests; if (testbadwiremask("Q90-I0", "U03", "C01", 0xfc0, "T16", 0xfc0, false)) ++pass;
	++tests; if (testbadwiremask("Q91-I1", "U02", "C23", 0xfc0, "T17", 0x03f, false)) ++pass;
	++tests; if (testbadwiremask("Q92-I2", "U03", "C01", 0x03f, "T18", 0x03f, false)) ++pass;
	++tests; if (testbadwiremask("Q93-I3", "U02", "C23", 0x03f, "T19", 0x03f, false)) ++pass;
	++tests; if (testbadwiremask("R94-IC", "U03", "C23", 0xfc0, "T20", 0x03f, false)) ++pass;
	++tests; if (testbadwiremask("R95-ID", "U02", "C01", 0xfc0, "T21", 0xfc0, false)) ++pass;
	++tests; if (testbadwiremask("R96-IE", "U03", "C23", 0x03f, "T22", 0x03f, false)) ++pass;
	++tests; if (testbadwiremask("R97-IF", "U02", "C01", 0x03f, "T23", 0x03f, false)) ++pass;

	cout << "Start the Node board IO port tests" << endl;
	++tests; if (testbadwiremask("RA0-M0-N00", "U04", "C23", 0xfc0, "T00", 0xfc0, false)) ++pass;
	++tests; if (testbadwiremask("RA1-M1-N01", "U04", "C23", 0x03f, "T01", 0xfc0, false)) ++pass;
	++tests; if (testbadwiremask("RA2-M0-N02", "U04", "C01", 0xfc0, "T02", 0xfc0, false)) ++pass;
	++tests; if (testbadwiremask("RA3-M1-N03", "U04", "C01", 0x03f, "T03", 0xfc0, false)) ++pass;

	cout << "Start the Node board Torus port tests" << endl;
	++tests; if (testbadwiremask("RB0-M0-N00", "U01", "C23", 0x03f, "T04", 0x03f000000000, false)) ++pass;
	++tests; if (testbadwiremask("RB1-M1-N01", "U02", "C23", 0x03f, "T04", 0x00003f000000, false)) ++pass;
	++tests; if (testbadwiremask("RB2-M0-N02", "U06", "C23", 0x03f, "T04", 0x00000003f000, false)) ++pass;
	++tests; if (testbadwiremask("RB3-M1-N03", "U07", "C23", 0x03f, "T04", 0x00000000003f, false)) ++pass;
	++tests; if (testbadwiremask("RB4-M0-N04", "U01", "C01", 0xfc0, "T07", 0xfc0000000000, false)) ++pass;
	++tests; if (testbadwiremask("RB5-M1-N05", "U02", "C01", 0xfc0, "T07", 0x000fc0000000, false)) ++pass;
	++tests; if (testbadwiremask("RB6-M0-N06", "U06", "C01", 0xfc0, "T07", 0x000000fc0000, false)) ++pass;
	++tests; if (testbadwiremask("RB7-M1-N07", "U07", "C01", 0xfc0, "T07", 0x000000000fc0, false)) ++pass;

	++tests; if (testbadwiremask("RB0-M1-N08", "U00", "C23", 0x03f, "T08", 0x03f000000000, false)) ++pass;
	++tests; if (testbadwiremask("RB1-M0-N09", "U03", "C23", 0xfc0, "T08", 0x000fc0000000, false)) ++pass;
	++tests; if (testbadwiremask("RB2-M1-N10", "U05", "C23", 0xfc0, "T08", 0x000000fc0000, false)) ++pass;
	++tests; if (testbadwiremask("RB3-M0-N11", "U08", "C23", 0x03f, "T08", 0x00000000003f, false)) ++pass;
	++tests; if (testbadwiremask("RB4-M1-N12", "U00", "C01", 0xfc0, "T11", 0xfc0000000000, false)) ++pass;
	++tests; if (testbadwiremask("RB5-M0-N13", "U03", "C01", 0x03f, "T11", 0x00003f000000, false)) ++pass;
	++tests; if (testbadwiremask("RB6-M1-N14", "U05", "C01", 0x03f, "T11", 0x00000003f000, false)) ++pass;
	++tests; if (testbadwiremask("RB7-M0-N15", "U08", "C01", 0xfc0, "T11", 0x000000000fc0, false)) ++pass;

	cout << "TEST SUMMARY: " << dec << (tests - pass) << " fails out of " << tests << " test cases." << endl;

	return 0;
}
