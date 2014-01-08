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

#include <utility/include/CableBadWires.h>

using namespace std;

/**
 * Test the node board and IO drawer link chip bad fiber mask to the Torus IO cable bad wire mask
 * conversion routine implemented by the BgqCableBadWires class.
 *
 */

bool samelocation(string x, string y)
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

bool testbadwiremask(
		string board, 			// Node board or IO drawer location ("Rxx-Mx-Nxx", "R/Qxx-Ix")
		string linkChipPos, 	// Link chip position ("Uxx")
		string reg, 			// Link chip register ("C01", "C23")
		int fiberMask, 			// Bad fiber mask (12 bits)
		string expPortPos, 		// Expected node board or IO drawer port position ("Txx")
		long int expWireMask,	// Expected bad wire mask (12 or 48 bits)
		string fromPort,		// Transmitting port location ("Rxx-Mx-Nxx-Txx", "R/Qxx/Ix-Txx')
		long int aggWireMask,	// Aggregated bad wire mask from the database
		int expTxMask,			// Expected TX fiber mask
		int expRxMask,			// Expected RX fiber mask
		bool expExcp)			// true = expecting exception
{
	bool match = false;
	try {
		string linkChipLocation = board + "-" + linkChipPos;
		string expPortLocation = board + "-" + expPortPos;
		cout << "RxLinkChip=" << linkChipLocation << "(" << hex << reg << ") fiberMask=" << hex << fiberMask
				<< " TxPort=" << fromPort << " aggWireMask=" << hex << aggWireMask << endl;
		cout << "   --> (exp-RxPort=" << expPortLocation << " exp-wiremask=" << hex << expWireMask << ")" << endl;

		bgq::utility::CableBadWires badWireMask(linkChipLocation, reg, fiberMask);
		string port = badWireMask.getPortLocation();
		long int wireMask = badWireMask.getBadWireMask();

		badWireMask.setTxPortAndBadWireMask(fromPort, aggWireMask);
		string txChip = badWireMask.getTxLinkChipLocation();
		string txReg = badWireMask.getTxRegister();
		long int aggFiberMask = badWireMask.getAggregatedBadFiberMask();
		int rxMask = badWireMask.getRxFiberMask();
		int txMask = badWireMask.getTxFiberMask();

		cout << "RxLinkChip=" << linkChipLocation << "(" << hex << reg << ") FiberMask=" << hex << fiberMask
				<< " TxPort=" << fromPort << " AggWireMask=" << hex << aggWireMask << endl;
		cout << "  --> RxPort=" << port << " TxChip=" << txChip << "(" << txReg << ")"
				<< " WireMask=" << hex << wireMask << " AggFiberMask=" << hex << aggFiberMask
				<< " TxMask=" << hex << txMask << " RxMask=" << hex << rxMask << endl;
		if (samelocation(port, expPortLocation) && wireMask == expWireMask && txMask == expTxMask && rxMask == expRxMask) {
			match = true;
			cout << "  >>> PASS" << endl;
		}
		else {
			string errorMsg = "  !!! FAIL: Unexpected";
			if (!samelocation(port, expPortLocation)) {
				errorMsg += " [port]";
			}
			if (wireMask != expWireMask) {
				errorMsg += " [bad wire mask]";
			}
			if (txMask != expTxMask) {
				errorMsg += " [TX mask]";
			}
			if (rxMask != expRxMask) {
				errorMsg += " [RX mask]";
			}
			cout << errorMsg << endl;
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


bool testbadwiremask(
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
		bgq::utility::CableBadWires badWireMask(linkChipLocation, reg, fiberMask);
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

	cout << "Start the TX/RX fiber mask tests" << endl;
	++tests; if (testbadwiremask("Q00-I0", "U00", "C23", 0xfc0, "T01", 0x03f, "R00-M0-N00-T00", 0x43f, 0xfc0, 0xfc0, false)) ++pass;
	++tests; if (testbadwiremask("Q01-I1", "U00", "C23", 0xfc0, "T01", 0x03f, "Q01-I2-T13", 0x83f, 0xfc0, 0xfc0, false)) ++pass;
	++tests; if (testbadwiremask("Q10-I2", "U05", "C23", 0x840, "T04", 0x021, "R01-M0-N01-T01", 0xaa9, 0x029, 0xa40, false)) ++pass;
	++tests; if (testbadwiremask("R11-M1-N02", "U04", "C23", 0x025, "T01", 0x940, "R11-IC-T04", 0xdf6, 0xdc0, 0x037, false)) ++pass;
	++tests; if (testbadwiremask("Q12-ID", "U03", "C01", 0x01c, "T18", 0x01c, "R12-M1-N02-T01", 0xb9c, 0x01c, 0x01c, false)) ++pass;
	++tests; if (testbadwiremask("R13-M1-N03", "U04", "C23", 0x01c, "T01", 0x700, "Q13-IE-T18", 0x72e, 0x01c, 0x01c, false)) ++pass;
	++tests; if (testbadwiremask("R14-M0-N04", "U01", "C23", 0x124, "T04", 0x124000000000, "R15-M0-N04-T05", 0x36c000000000, 0x36c, 0x36c, false)) ++pass;
	++tests; if (testbadwiremask("R15-M1-N05", "U02", "C01", 0x124, "T07", 0x124000000, "R16-M1-N05-T06", 0x36c000000, 0x36c, 0x36c, false)) ++pass;
	++tests; if (testbadwiremask("R17-M0-N06", "U05", "C23", 0x124, "T08", 0x124000, "R18-M1-N06-T09", 0x36c000, 0x36c, 0x36c, false)) ++pass;
	++tests; if (testbadwiremask("R18-M1-N07", "U08", "C01", 0x124, "T11", 0x124, "R19-M0-N07-T09", 0x36c, 0x36c, 0x36c, false)) ++pass;
	++tests; if (testbadwiremask("Q19-I3", "U04", "C23", 0x8c0, "T12", 0x023, "R19-M0-N03-T02", 0xd6b, 0xac0, 0xac0, false)) ++pass;
	++tests; if (testbadwiremask("R20-M0-N04", "U04", "C01", 0x940, "T02", 0x940, "Q20-I3-T12", 0xd6b, 0xd40, 0xd40, false)) ++pass;
	++tests; if (testbadwiremask("Q21-I4", "U02", "C01", 0x025, "T23", 0x025, "R21-M1-N04-T02", 0xe75, 0xd40, 0x035, false)) ++pass;
	++tests; if (testbadwiremask("R22-M1-N05", "U04", "C01", 0xa40, "T02", 0xa40, "Q22-I5-T23", 0xe75, 0x039, 0xe40, false)) ++pass;

	cout << "TEST SUMMARY: " << dec << (tests - pass) << " fails out of " << tests << " test cases." << endl;

	return 0;
}
