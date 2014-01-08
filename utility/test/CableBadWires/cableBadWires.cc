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
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include <utility/include/CableBadWires.h>

using namespace std;

/**
 * Test the node board and IO drawer link chip bad fiber mask to the Torus IO cable bad wire mask
 * conversion routine implemented by the BgqCableBadWires class.
 *
 * HOW TO RUN THIS TEST:
 *
 * 1. cd <sandbox>/utility/test/CableBadWires
 * 2. make clean
 * 3. make test
 * 4. ./test_cableBadWires
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
		string cableStatus = badWireMask.cableInError() ? "ok" : "bad";
		string port = badWireMask.getPortLocation();
		long int wireMask = badWireMask.getBadWireMask();
		cout << "linkChipLocation=" << linkChipLocation << " reg=" << hex << reg << " fiberMask=" << hex << fiberMask
				<< " --> port=" << port << " wiremask=" << hex << wireMask << " cable=" << cableStatus << endl;
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


bool testopticalpair(
		string fromPort, 		// From port location ("Rxx-Mx-Nxx-Txx", "R/Qxx-Ix-Txx")
		string toPort,	 		// To port location ("Rxx-Mx-Nxx-Txx", "R/Qxx-Ix-Txx")
		long int badWireMask,	// Bad wire mask (12 or 48 bits)
		vector<string> exp,		// Expected optical module pairs
		bool expExcp)			// true = expecting exception
{
	bool match = false;
	cout << "FromPort=" << fromPort << " ToPort=" << toPort << " BadWireMask=0x" << hex << badWireMask << endl;
	if (exp.size() > 0) {
		cout << "     ";
		for (size_t i = 0; i < exp.size(); ++i) {
			cout << " exp[" << i << "]=" + exp[i];
		}
		cout << endl;
	}
	try {
		vector<string> list = bgq::utility::CableBadWires::getBadOpticalConnections(fromPort, toPort, badWireMask);
		cout << "  -->";
		for (size_t i = 0; i < list.size(); ++i) {
			cout << " rec[" << i << "]=" + list[i];
		}
		cout << endl;
		if (list.size() == exp.size()) {
			match = true;
			for (size_t i = 0; i < list.size(); ++i) {
				if (list[i].compare(exp[i]) != 0) {
					match = false;
					cout << "  !!! FAIL: Unexpected pairs returned!" << endl;
					break;
				}
			}
		}
		else {
			cout << "  !!! FAIL: Unexpected number of pairs!" << endl;
		}
		if (match) {
			cout << "  >>> PASS" << endl;
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

} // End testopticalpair

int main(int argc, char **argv)
{
	int tests = 0;
	int pass = 0;

	cout << "\nStart testBgqCableBadWires\n" << endl;

	cout << "\nStart the IO Drawer tests\n" << endl;
	++tests; if (testbadwiremask("Q02-I2", "U05", "C01", 0x020, "T02", 0x800, false)) ++pass;
	++tests; if (testbadwiremask("Q03-I3", "U00", "C23", 0x008, "T03", 0x008, false)) ++pass;
	++tests; if (testbadwiremask("Q04-I4", "U05", "C23", 0x080, "T04", 0x002, false)) ++pass;
	++tests; if (testbadwiremask("Q05-I5", "U00", "C01", 0x100, "T05", 0x100, false)) ++pass;
	++tests; if (testbadwiremask("Q06-I6", "U05", "C23", 0x001, "T06", 0x001, false)) ++pass;
	++tests; if (testbadwiremask("Q07-I7", "U00", "C01", 0x020, "T07", 0x800, false)) ++pass;

	++tests; if (testbadwiremask("Q00-I8", "U04", "C01", 0x800, "T08", 0x800, false)) ++pass;
	++tests; if (testbadwiremask("Q01-I9", "U01", "C23", 0x400, "T09", 0x010, false)) ++pass;
	++tests; if (testbadwiremask("Q02-IA", "U04", "C01", 0x020, "T10", 0x800, false)) ++pass;
	++tests; if (testbadwiremask("Q03-IB", "U01", "C23", 0x010, "T11", 0x010, false)) ++pass;
	++tests; if (testbadwiremask("R04-IC", "U04", "C23", 0x200, "T12", 0x008, false)) ++pass;
	++tests; if (testbadwiremask("R05-ID", "U01", "C01", 0x100, "T13", 0x100, false)) ++pass;
	++tests; if (testbadwiremask("R06-IE", "U04", "C23", 0x008, "T14", 0x008, false)) ++pass;
	++tests; if (testbadwiremask("R07-IF", "U01", "C01", 0x004, "T15", 0x100, false)) ++pass;

	++tests; if (testbadwiremask("Q90-I0", "U03", "C01", 0x080, "T16", 0x080, false)) ++pass;
	++tests; if (testbadwiremask("Q91-I1", "U02", "C23", 0x040, "T17", 0x001, false)) ++pass;
	++tests; if (testbadwiremask("Q92-I2", "U03", "C01", 0x002, "T18", 0x002, false)) ++pass;
	++tests; if (testbadwiremask("Q93-I3", "U02", "C23", 0x001, "T19", 0x001, false)) ++pass;
	++tests; if (testbadwiremask("R94-IC", "U03", "C23", 0x800, "T20", 0x020, false)) ++pass;
	++tests; if (testbadwiremask("R95-ID", "U02", "C01", 0x400, "T21", 0x400, false)) ++pass;
	++tests; if (testbadwiremask("R96-IE", "U03", "C23", 0x020, "T22", 0x020, false)) ++pass;
	++tests; if (testbadwiremask("R97-IF", "U02", "C01", 0x010, "T23", 0x010, false)) ++pass;
	++tests; if (testbadwiremask("R97-IF", "U02", "C01", 0x030, "T23", 0x030, true)) ++pass;

	cout << "\nStart the Node board IO port tests\n" << endl;
	++tests; if (testbadwiremask("RA0-M0-N00", "U04", "C23", 0x080, "T00", 0x080, false)) ++pass;
	++tests; if (testbadwiremask("RA1-M1-N01", "U04", "C23", 0x020, "T01", 0x800, false)) ++pass;
	++tests; if (testbadwiremask("RA2-M0-N02", "U04", "C01", 0x040, "T02", 0x040, false)) ++pass;
	++tests; if (testbadwiremask("RA3-M1-N03", "U04", "C01", 0x001, "T03", 0x040, false)) ++pass;

	cout << "\nStart the Node board Torus port tests\n" << endl;
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

	++tests; if (testbadwiremask("RC0-M1-N08", "U00", "C23", 0x020, "T08", 0x020000000000, false)) ++pass;
	++tests; if (testbadwiremask("RC1-M0-N09", "U03", "C23", 0x400, "T08", 0x000400000000, false)) ++pass;
	++tests; if (testbadwiremask("RC2-M1-N10", "U05", "C23", 0x200, "T08", 0x000000200000, false)) ++pass;
	++tests; if (testbadwiremask("RC3-M0-N11", "U08", "C23", 0x004, "T08", 0x000000000004, false)) ++pass;

	/*
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
	*/
	cout << "\nStart the TX/RX fiber mask tests\n" << endl;
	++tests; if (testbadwiremask("Q00-I0", "U00", "C23", 0x800, "T01", 0x020, "R00-M0-N00-T00", 0x420, 0x800, 0x800, false)) ++pass;
	++tests; if (testbadwiremask("Q01-I1", "U00", "C23", 0x040, "T01", 0x001, "Q01-I2-T13", 0x801, 0x040, 0x040, false)) ++pass;
	++tests; if (testbadwiremask("Q10-I2", "U05", "C23", 0x100, "T04", 0x004, "R01-M0-N01-T01", 0x044, 0x004, 0x100, false)) ++pass;
	++tests; if (testbadwiremask("R11-M1-N02", "U04", "C23", 0x001, "T01", 0x040, "R11-IC-T04", 0x041, 0x040, 0x001, false)) ++pass;
	++tests; if (testbadwiremask("Q12-ID", "U03", "C01", 0x010, "T18", 0x010, "R12-M1-N02-T01", 0x050, 0x010, 0x010, false)) ++pass;
	++tests; if (testbadwiremask("R13-M1-N03", "U04", "C23", 0x008, "T01", 0x200, "Q13-IE-T18", 0x208, 0x008, 0x008, false)) ++pass;

	++tests; if (testbadwiremask("R14-M0-N04", "U01", "C23", 0x124, "T04", 0x124000000000, "R15-M0-N04-T05", 0x36c000000000, 0x36c, 0x36c, false)) ++pass;
	++tests; if (testbadwiremask("R15-M1-N05", "U02", "C01", 0x124, "T07", 0x124000000, "R16-M1-N05-T06", 0x36c000000, 0x36c, 0x36c, false)) ++pass;
	++tests; if (testbadwiremask("R17-M0-N06", "U05", "C23", 0x124, "T08", 0x124000, "R18-M1-N06-T09", 0x36c000, 0x36c, 0x36c, false)) ++pass;
	++tests; if (testbadwiremask("R18-M1-N07", "U08", "C01", 0x124, "T11", 0x124, "R19-M0-N07-T09", 0x36c, 0x36c, 0x36c, false)) ++pass;

	++tests; if (testbadwiremask("Q19-I3", "U04", "C23", 0x400, "T12", 0x010, "R19-M0-N03-T02", 0x050, 0x400, 0x400, false)) ++pass;
	++tests; if (testbadwiremask("R20-M0-N04", "U04", "C01", 0x100, "T02", 0x100, "Q20-I3-T12", 0x121, 0x100, 0x100, false)) ++pass;
	++tests; if (testbadwiremask("Q21-I4", "U02", "C01", 0x020, "T23", 0x020, "R21-M1-N04-T02", 0x801, 0x040, 0x001, false)) ++pass;
	++tests; if (testbadwiremask("R22-M1-N05", "U04", "C01", 0x400, "T02", 0x400, "Q22-I5-T23", 0x801, 0x020, 0x800, false)) ++pass;

	/*
	 * Test the getBadOpticalConnections method.
	 */
	cout << "\nStart the getBadOpticalConnections tests\n" << endl;
	vector<string> exp;
	const string NB1 = "R00-M0-N00";
	const string NB2 = "R01-M1-N01";
	const string IO1 = "Q02-I2";

	++tests;
	if (testopticalpair(NB1 + "-T00", NB2 + "-T04", 0, exp, true)) ++pass;

	++tests;
	if (testopticalpair(NB1 + "-T04", IO1 + "-T00", 0, exp, true)) ++pass;

	++tests;
	if (testopticalpair(NB1 + "-T12", IO1 + "-T00", 0, exp, true)) ++pass;

	++tests;
	if (testopticalpair(NB1 + "-T00", IO1 + "-T24", 0, exp, true)) ++pass;

	++tests;
	if (testopticalpair(IO1 + "-T24", NB1 + "-T11", 0, exp, true)) ++pass;

	++tests;
	if (testopticalpair(IO1 + "-T23", NB1 + "-T12", 0, exp, true)) ++pass;

	++tests;
	exp.push_back(NB1 + "-O30");
	exp.push_back(NB2 + "-O29");
	if (testopticalpair(NB1 + "-T05", NB2 + "-T04", 0x000000000001, exp, false)) ++pass;
	exp.clear();

	++tests;
	exp.push_back(NB2 + "-O14");
	exp.push_back(NB1 + "-O13");
	if (testopticalpair(NB1 + "-T08", NB2 + "-T09", 0x000800000000, exp, false)) ++pass;
	exp.clear();

	++tests;
	exp.push_back(NB1 + "-O32");
	exp.push_back(NB2 + "-O35");
	exp.push_back(NB1 + "-O20");
	exp.push_back(NB2 + "-O23");
	exp.push_back(NB1 + "-O12");
	exp.push_back(NB2 + "-O15");
	exp.push_back(NB1 + "-O00");
	exp.push_back(NB2 + "-O03");
	if (testopticalpair(NB1 + "-T10", NB2 + "-T11", 0x800010008001, exp, false)) ++pass;
	exp.clear();

	++tests;
	exp.push_back(NB1 + "-O17");
	exp.push_back(IO1 + "-O08");
	if (testopticalpair(NB1 + "-T00", IO1 + "-T23", 0x800, exp, false)) ++pass;
	exp.clear();

	++tests;
	exp.push_back(NB1 + "-O18");
	exp.push_back(IO1 + "-O11");
	exp.push_back(NB1 + "-O17");
	exp.push_back(IO1 + "-O08");
	if (testopticalpair(NB1 + "-T00", IO1 + "-T23", 0x801, exp, false)) ++pass;
	exp.clear();

	++tests;
	exp.push_back(NB1 + "-O16");
	exp.push_back(IO1 + "-O21");
	exp.push_back(NB1 + "-O19");
	exp.push_back(IO1 + "-O22");
	if (testopticalpair(NB1 + "-T02", IO1 + "-T06", 0x060, exp, false)) ++pass;
	exp.clear();

	++tests;
	exp.push_back(IO1 + "-O21");
	exp.push_back(NB1 + "-O16");
	exp.push_back(IO1 + "-O22");
	exp.push_back(NB1 + "-O19");
	if (testopticalpair(IO1 + "-T06", NB1 + "-T02", 0x090, exp, false)) ++pass;
	exp.clear();

	++tests;
	exp.push_back(IO1 + "-O13");
	exp.push_back(IO1 + "-O08");
	if (testopticalpair(IO1 + "-T20", IO1 + "-T21", 0x020, exp, false)) ++pass;
	exp.clear();

	++tests;
	exp.push_back(IO1 + "-O11");
	exp.push_back(IO1 + "-O14");
	if (testopticalpair(IO1 + "-T21", IO1 + "-T20", 0x040, exp, false)) ++pass;
	exp.clear();

	cout << "\nTEST SUMMARY: " << dec << (tests - pass) << " fails out of " << tests << " test cases." << endl;

	return 0;
}
