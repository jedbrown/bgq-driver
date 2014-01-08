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

#ifndef __JTAG_TAP_DECODER_H__
#define __JTAG_TAP_DECODER_H__

#include <string>
#include <vector>
#include <stdint.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <bgq_util/include/ArbitraryLengthNumber.h>
#include "JtagTapState.h"

using namespace std;

//
// decode state machine to help with debug of jtag data blocks.
class JtagTapDecoder : public JtagTapState {
public:
    JtagTapDecoder() :
        m_traceLevel(0), 
        m_nTdoBits(0),
        m_nTdiBits(0),
        m_nTapState(TS_TLR),
        m_ulCurrInst(0xF2000101),
        m_jtagOut(NULL) {};
    virtual ~JtagTapDecoder() {};
    enum {
        JTAG_TRACE_TDO     = (1<<0),        // trace tdo only
        JTAG_TRACE_TDO_TDI = (1<<1),        // trace tdi
        JTAG_TRACE_STATE   = (1<<2),        // trace state
        JTAG_TRACE_TCK     = (1<<3)         // trace all tck transitions
    };


    
    void jtagTCK(uint8_t tms, uint8_t tdo);
    void jtagTCK(uint8_t *tdodata, unsigned cycles, uint8_t finalTms);
    void jtagTCKconst(uint8_t tdo, unsigned cycles, uint8_t finalTms);
    
    void jtagTDO(uint8_t *tdodata, unsigned cycles);
    void jtagTDI(uint8_t *tdidata, unsigned cycles);
    void jtagTDOconst(uint8_t tdo, unsigned cycles);
    void jtagTDO(uint8_t tdo);
    void jtagTDI(uint8_t tdi);
    void jtagTMS(const string &tmsStr);
    void clearData();

    /*!
    // set the trace level
    //
    // @param level -- levelt to set
    //         JTAG_TRACE_TDO       trace tdo only
    //         JTAG_TRACE_TDO_TDI   trace tdi
    //         JTAG_TRACE_STATE     trace state
    // @returns none.
    */
    void setTraceLevel(unsigned level) {
        m_traceLevel = level;
    };
    unsigned getTraceLevel() {
        return(m_traceLevel); };

    /*!
    // set the prefix to print when displaying trace information.
    //
    // @param prefix -- prefix to set...
    */
    void setPrefix(const string &prefix) {
        m_prefix = prefix; };
    void setSuffix(const string &suffix) {
        m_suffix = suffix; };

    string &getPrefix() {
        return(m_prefix); };

    //
    // retrieve the current tap state.
    //
    unsigned tapState() {
        return(m_nTapState); };     

    const ArbitraryLengthNumber & getIrData() const;
    const ArbitraryLengthNumber & getDrData() const;

    virtual void displayTdoTdi(const string &szPrefix);

    /*!
    // set the output stream pointer.
    //
    // @param jtagout -- pointer to the out stream to use, set to NULL for cout
    //                   this class does not own this pointer
    //                   so don't delete it from under it.
    // NOTE: this output is only as threadsafe as the ostream 
    //       object is thread safe.
    */
    void setJtagOut(std::ostream *jtagOut) {
        m_jtagOut = jtagOut; };
    /*!
    // retrieve the jtag out stream refrence
    // @param none
    // @returns -- reference outstream
    */
    std::ostream &jtagOut() {
        if (m_jtagOut == NULL)
            return(std::cout);
        else
            return(*m_jtagOut);
    };


    /*!
    // retrieve the statename given the state number
    // @param state -- state number
    // @returns -- state string
    //
    */
    string stateName(unsigned state);
    

protected:
    void jtagNextState(uint8_t tms);
    void jtagTCKstate(uint8_t tdo);
    virtual void updateIR();
    virtual void updateDR();
    virtual void captureIR() {};
    virtual void captureDR() {};
    virtual void pauseIR() {};
    virtual void pauseDR() {};
    virtual void exitIR() {};
    virtual void exitDR() {};

    typedef struct {
        unsigned nState;
        unsigned nStateIf0;
        unsigned nStateIf1;
    } TS_STATE_TABLE;
    static TS_STATE_TABLE TsStateTable[];
    
    unsigned m_traceLevel;
    unsigned m_nTdoBits;
    unsigned m_nTdiBits;
    unsigned m_nTapState;
    unsigned m_ulCurrInst;             // current 32 bit instruction.
    vector<uint8_t>  m_tdoData;        // current tdo data...
    vector<uint8_t>  m_tdiData;        // current tdi data if any...
    string           m_tdiStr;         // TDI data as a string.

    ArbitraryLengthNumber m_irReg;
    ArbitraryLengthNumber m_drReg;

    string m_prefix;    // display prefix
    string m_suffix;

    std::ostream *m_jtagOut;
private:
};


#endif


