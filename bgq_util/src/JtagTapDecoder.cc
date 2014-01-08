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


/*!
// Jtag Tap decoder.
//
// Used to display what the jtat tap is doing based on tms/tdo twiddles
//
*/

#include <iomanip>
#include <sstream>
#include <iostream>

#include "JtagTapDecoder.h"

JtagTapDecoder::TS_STATE_TABLE JtagTapDecoder::TsStateTable[] = {
    // nState                       0,                      1,
    {TS_TLR,                TS_RTI,             TS_TLR},
    {TS_RTI,                TS_RTI,             TS_DR_SCAN},
    {TS_DR_SCAN,            TS_CAPTURE_DR,      TS_IR_SCAN},
    {TS_CAPTURE_DR,         TS_SHIFT_DR,        TS_EXIT_DR},
    {TS_SHIFT_DR,           TS_SHIFT_DR,        TS_EXIT_DR},
    {TS_EXIT_DR,            TS_PAUSE_DR,        TS_UPDATE_DR},
    {TS_PAUSE_DR,           TS_PAUSE_DR,        TS_EXIT2_DR},
    {TS_EXIT2_DR,           TS_SHIFT_DR,        TS_UPDATE_DR},
    {TS_UPDATE_DR,          TS_RTI,             TS_DR_SCAN},
    {TS_IR_SCAN,            TS_CAPTURE_IR,      TS_TLR},
    {TS_CAPTURE_IR,         TS_SHIFT_IR,        TS_EXIT_IR},
    {TS_SHIFT_IR,           TS_SHIFT_IR,        TS_EXIT_IR},
    {TS_EXIT_IR,            TS_PAUSE_IR,        TS_UPDATE_IR},
    {TS_PAUSE_IR,           TS_PAUSE_IR,        TS_EXIT2_IR},
    {TS_EXIT2_IR,           TS_SHIFT_IR,        TS_UPDATE_IR},
    {TS_UPDATE_IR,          TS_RTI,             TS_DR_SCAN}
};

/*!
// Append one bit to the tdo bits collected in shift ir and shift DR state...
//
// @param tdo - tdo bit to append.
*/
void JtagTapDecoder::jtagTDO(uint8_t tdo)
{
    if ((m_nTdoBits % 8) == 0)
        m_tdoData.push_back(0);
    m_tdoData[m_nTdoBits / 8] |= ((tdo ? 1 : 0) << (m_nTdoBits % 8));
    m_nTdoBits++;
}

/*!
// Append one bit to the tdi bits collected in shift ir and shift DR state...
//
// @param tdo - tdo bit to append. (0,1,'0','1','X')
*/
void JtagTapDecoder::jtagTDI(uint8_t tdi)
{
    if ((m_nTdiBits % 8) == 0)
        m_tdiData.push_back(0);
    if (tdi > ' ')
    {
        switch (tdi) 
        {
            case '0':  m_tdiStr.append("0"); break;
            case '1':  m_tdiStr.append("1"); break;
            default:   m_tdiStr.append("X"); break;
        }
        tdi = (tdi == '1') ? 1 : 0;     // convert to a number for the next step
    }
    else
        m_tdiStr.append(tdi ? "1" : "0");

    m_tdiData[m_nTdiBits / 8] |= ((tdi ? 1 : 0) << (m_nTdiBits % 8));
    
    m_nTdiBits++;
}



/*!
// Append append jtag tdo constant daqta
//
// @param tdo data -- pointer to tdo data to append.
// @param cycles -- number of cycles to apply
//
// NOTE: this will always round up to the nearest 8 byte boundary...
*/
void JtagTapDecoder::jtagTDOconst(uint8_t tdo, unsigned cycles)
{
    for (unsigned n = 0; n < (cycles+7)/8; n++)
        m_tdoData.push_back(tdo ? 0xFF : 00);
    m_nTdoBits += cycles;

}


/*!
// Append multiple bits
//
// @param tdo data -- pointer to tdo data to append.
// @param cycles -- number of cycles to apply
//
// NOTE: this will always round up to the nearest 8 byte boundary...
*/
void JtagTapDecoder::jtagTDO(uint8_t *tdodata, unsigned cycles)
{
    // unaligned, have to do this bit by bit...
    if ((m_nTdoBits % 8) != 0)
    {
        uint8_t mask = 1;
        uint8_t *p = tdodata;
        for (unsigned n = 0; n < cycles; n++)
        {
            jtagTDO((*p & mask) ? 1 : 0);
            mask <<= 1;
            if (mask == 0)
            {
                p++;
                mask = 1;
            }
        }
        return;
    }
    for (unsigned n = 0; n < (cycles+7)/8; n++)
        m_tdoData.push_back(tdodata[n]);
    m_nTdoBits += cycles;
}


/*!
// Append multiple bits
//
// @param tdi data -- pointer to tdo data to append.
// @param cycles -- number of cycles to apply
//
// NOTE: this will always round up to the nearest 8 byte boundary...
*/
void JtagTapDecoder::jtagTDI(uint8_t *tdidata, unsigned cycles)
{
    if ((m_nTdiBits % 8) != 0)
    {
        jtagOut() << "<<<<<<<< JtagTapDecoder::jtagTDI(uint8_t *tdodata, unsigned cycles) not called on an 8 byte boundary(" 
             << dec << m_nTdiBits << ", " << cycles << ")"
             << endl;
    }
    for (unsigned n = 0; n < (cycles+7)/8; n++)
    {
        if (tdidata)
            m_tdiData.push_back(tdidata[n]);
        else
            m_tdiData.push_back(0);
    }
    m_nTdiBits += cycles;
}

void JtagTapDecoder::jtagTMS(const string &tmsStr)
{
    const char *pd = tmsStr.c_str();
    while (*pd)
        jtagTCK((*pd++ == '1') ? 1 : 0, 0);
}


/*~
// tms value to bounce us into the next jtag state.
//
// @param tms -- tms value to bump us into the next state.
// @returns none.
*/
void JtagTapDecoder::jtagNextState(uint8_t tms)
{

    unsigned nState = m_nTapState;
    if (tms)
        m_nTapState = TsStateTable[m_nTapState].nStateIf1;
    else
        m_nTapState = TsStateTable[m_nTapState].nStateIf0;

    if (m_traceLevel & JTAG_TRACE_STATE)
    {
        if ((nState != m_nTapState))
        {
            string pf;
            if (m_prefix.size())
                pf = m_prefix + " " ;
            jtagOut() << pf << "JtagTapState, " 
                 << stateName(m_nTapState) <<  ","
                 << " tms = " << (unsigned)tms << m_suffix
                 << endl << flush;
        }
    }        
}
/*!
// retrieve the statename given the state number
// @param state -- state number
// @returns -- state string
//
*/
string JtagTapDecoder::stateName(unsigned state)
{
    static const char *tapStateStr[TS_SIZE] = {
        "TLR       ",
        "RTI       ",
        "DR_SCAN   ",
        "CAPTURE_DR",
        "SHIFT_DR  ",
        "EXIT_DR   ",
        "PAUSE_DR  ",
        "EXIT2_DR  ",
        "UPDATE_DR ",
        "IR_SCAN   ",
        "CAPTURE_IR",
        "SHIFT_IR  ",
        "EXIT_IR   ",
        "PAUSE_IR  ",
        "EXIT2_IR  ",
        "UPDATE_IR "
    };
    return((state < TS_SIZE) ? tapStateStr[state] : "????");
};

/*!
// process the current jtag tck state...
//
// @param tdo -- tdo value to use for the state.
// @returns none.
*/
void JtagTapDecoder::jtagTCKstate(uint8_t tdo)
{
    switch (m_nTapState)
    {
        case TS_CAPTURE_IR:
            clearData();
            captureIR();
            break;
        case TS_CAPTURE_DR:
            clearData();
            captureDR();
            break;
        case TS_SHIFT_DR:
        case TS_SHIFT_IR:
            jtagTDO(tdo);       // collect the bits...
            break;
        case TS_UPDATE_DR:
            updateDR();
            break;
        case TS_PAUSE_IR:
            pauseIR();
            break;
        case TS_PAUSE_DR:
            pauseDR();
            break;
        case TS_EXIT_IR:
            exitIR();
            break;
        case TS_EXIT_DR:
            exitDR();
            break;
        case TS_UPDATE_IR:
            updateIR();
            break;
        case TS_TLR:
            //printf("TS_TLR\n");
            break;
    }

}

/*!
// perform jtag TCK for ncycles with tdo data
//   normally we expect to be in the shift dr state when this happens.
//
// @param tdodata -- tdo data to tick with.
// @param cycles -- number of cycles to tick
// @param finalTms -- final tms bit,  or 1
*/
void JtagTapDecoder::jtagTCK(uint8_t *tdodata, unsigned cycles, uint8_t finalTms)
{


    switch (m_nTapState)
    {
        case TS_SHIFT_DR:
        case TS_SHIFT_IR:
            jtagTDO(tdodata, cycles);       // collect the bits...
            break;
        default: jtagTCKstate(0);

    };

    jtagNextState(finalTms);

}

/*!
// perform jtag TCK for ncycles with tdo data
//   normally we expect to be in the shift dr state when this happens.
//
// @param tdovlaue
// @param cycles -- number of cycles to tick
// @param finalTms -- final tms bit,  or 1
*/
void JtagTapDecoder::jtagTCKconst(uint8_t tdo, unsigned cycles, uint8_t finalTms)
{


    switch (m_nTapState)
    {
        case TS_SHIFT_DR:
        case TS_SHIFT_IR:
            jtagTDOconst(tdo, cycles);       // collect the bits...
            break;
        default: jtagTCKstate(0);

    };

    jtagNextState(finalTms);

}


//
// Perform one tck cycle with the indicated tms and tdo bits.
//
// @params tms -- tms bit (0..1)
// @params tdo -- tdo bit (0..1)
//
// returns m_nTapState m_tdoData set according to the state...

void JtagTapDecoder::jtagTCK(uint8_t tms, uint8_t tdo)
{
    if (m_traceLevel & JTAG_TRACE_TCK)
    {
        string pf;
        if (m_prefix.size())
            pf = m_prefix + " " ;
        cout << pf << "JtagTapTck," << dec 
             << (unsigned)(!!tms) << ", " 
             << (unsigned)(!!tdo) << m_suffix
             << endl;
    }
    jtagTCKstate(tdo);
    jtagNextState(tms);
}



/*!
// Clear the shifted data...
//
// @param none.
// @returns none.
*/
void JtagTapDecoder::clearData()
{
    m_tdoData.clear(); 
    m_tdiData.clear(); 
    m_tdiStr = "";
    m_nTdiBits = 0;
    m_nTdoBits = 0;
}
void JtagTapDecoder::updateIR()
{
    m_irReg.setRawData(m_nTdoBits, &m_tdoData[0]);      // put this in an arbitrary number for for easy access...

    if (m_traceLevel & (JTAG_TRACE_TDO | JTAG_TRACE_TDO_TDI))
        displayTdoTdi(m_prefix + " IR = " );
   
};
void JtagTapDecoder::updateDR() 
{
    m_drReg.setRawData(m_nTdoBits, &m_tdoData[0]);

    if (m_traceLevel & (JTAG_TRACE_TDO | JTAG_TRACE_TDO_TDI))
        displayTdoTdi(m_prefix + " DR = ");
};

/*!
// retrieve the current contents of the ir data register
//
// @param data [out] data ir data register.
*/
const ArbitraryLengthNumber & JtagTapDecoder::getIrData() const
{
    return(m_drReg);
}
/*!
// retrieve the current contents of the ir data register
//
// @param data [out] data ir data register.
*/
const ArbitraryLengthNumber & JtagTapDecoder::getDrData() const
{
    return(m_drReg);
}


/*!
// Display the TDO on the standard out display.
//
// @param szPrefix -- prefix to display
//
// @returns none.
*/
void JtagTapDecoder::displayTdoTdi(const string &szPrefix) 
{
    ostringstream ostr;
    ostr << szPrefix << "(" << dec << m_nTdoBits << ")";
    for (int n = int(m_tdoData.size())-1; n >= 0; n--)
        ostr << setfill('0') << setw(2) << hex << unsigned(m_tdoData[n]);
    if (m_tdiData.size() && (m_traceLevel & JTAG_TRACE_TDO_TDI))
    {
        ostr << ", ";
        for (int n = int(m_tdiData.size())-1; n >= 0; n--)
            ostr << setfill('0') << setw(2) << hex << unsigned(m_tdiData[n]);
        if (m_tdiStr.find_first_of("X") != string::npos)
        {
            ostr << " \'";
            for (int n = m_tdiStr.length()-1; n >= 0; n--)
                ostr << m_tdiStr[n];
            ostr << "\'";
        
        }

    }
    jtagOut() << ostr.str() << m_suffix << endl << flush;

};

