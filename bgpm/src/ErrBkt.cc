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

#include <stdio.h>
#include <stdarg.h>
#include <firmware/include/personality.h>
#include <hwi/include/common/uci.h>
#include "bgpm/include/err.h"
#include "ErrBkt.h"
#include "globals.h"


//! \todo I shouldn't need this delay - remove it later.
#define EXIT_DELAY  50000000    // delay to allow errors to print - for some reason they don't make it otherwise



extern "C" int Bgpm_LastErr() { return lastErr.LastErrNum(); }


extern "C" const char * Bgpm_LastErrStrg() {
    return lastErr.LastErrStrg();
}


extern "C" const char *Bgpm_ErrStrg(int errnum) {
    return lastErr.ErrStrg(errnum);
}



extern const BgpmErrTblRec bgpmOkStrgTbl[];
extern const BgpmErrTblRec upciStrgTbl[];
extern const BgpmErrTblRec bgpmStrgTbl[];
extern const BgpmErrTblRec bgpmWarnStrgTbl[];






using namespace bgpm;




void bgpm::GetErrStrg(int err, char *strg, size_t strgSize) {

    if (err == 0) {
        snprintf(strg, strgSize, "%s",  bgpmOkStrgTbl[err].desc);
    }
    else if (err < 0) {
        // errno value
        if (err > UPCI_MIN_ERRNO) {
            snprintf(strg, strgSize, "%s", strerror(-err));
        }
        // upci error
        else if ((err <= UPCI_MIN_ERRNO) && (err >= UPCI_MAX_ERRNO)) {
            int msgIdx = -err + UPCI_MIN_ERRNO;
            if (err != upciStrgTbl[msgIdx].errNum) {
                snprintf(strg, strgSize, "Cannot extract error description string; errnum(%d) does not match string table entry(%d)",
                        err, upciStrgTbl[msgIdx].errNum);
            }
            else {
                snprintf(strg, strgSize, "%s",  upciStrgTbl[msgIdx].desc);
            }
        }
        // bgpm error
        else if ((err <= BGPM_MIN_ERRNO) && (err >= BGPM_MAX_ERRNO)) {
            int msgIdx = -err + BGPM_MIN_ERRNO;
            if (err != bgpmStrgTbl[msgIdx].errNum) {
                snprintf(strg, strgSize, "Cannot extract error description string; errnum(%d) does not match string table entry(%d)",
                        err, bgpmStrgTbl[msgIdx].errNum);
            }
            else {
                snprintf(strg, strgSize, "%s",  bgpmStrgTbl[msgIdx].desc);
            }
        }
        // unrecognized
        else {
            snprintf(strg, strgSize, "Invalid parm: unrecognized error value (%d) passed to bgpm::GetErrStrg",
                    err);
        }
    }
    // warning
    else if ((err >= BGPM_MIN_WARNING_NO) && (err <= BGPM_MAX_WARNING_NO)) {
        int msgIdx = err - BGPM_MIN_WARNING_NO;
        if (err != bgpmWarnStrgTbl[msgIdx].errNum) {
            snprintf(strg, strgSize, "Cannot extract error description string; errnum(%d) does not match string table entry(%d)",
                    err, bgpmWarnStrgTbl[msgIdx].errNum);
        }
        else {
            snprintf(strg, strgSize, "%s",  bgpmWarnStrgTbl[msgIdx].desc);
        }
    }
    // unrecognized
    else {
        snprintf(strg, strgSize, "Invalid parm: unrecognized error value (%d) passed to bgpm::GetErrStrg",
                err);
    }
}




void ErrBkt::Init()
{
    BGPM_EXEMPT_L2;
    lastErrNum = 0;
    lastTopLevel = NULL;
    lastErrStrg[0] = '\0';

    Personality_t pers;
    BG_UniversalComponentIdentifier uci;
    Kernel_GetPersonality( &pers, sizeof(pers) );
    uci = pers.Kernel_Config.UCI;
    if (bg_uci_toString( uci, ucStrg) < 0) {
        ucStrg[0] = '\0';
    }
}



const char *ErrBkt::LastErrStrg() {
    GetErrStrg(lastErrNum, lastErrStrg, BGPM_MAXERRSTRG);
    return lastErrStrg;
}



const char *ErrBkt::ErrStrg(int err) {
    static __thread char tmpErrStrg[BGPM_MAXERRSTRG];
    GetErrStrg(err, tmpErrStrg, BGPM_MAXERRSTRG);
    return tmpErrStrg;
}



ErrBkt & ErrBkt::PrintOrExitOp(int errNum, const char *op, const char *loc)
{
    BGPM_EXEMPT_L2;
    lastErrNum = errNum;
    char prtStrg[BGPM_MAXERRSTRG*2];
    if ((errNum < 0) && GetPrintOnError()) {
        if (!lastTopLevel) {
            lastTopLevel = "Bgpm";
        }
        int pos = 0;
        pos = snprintf(&prtStrg[pos], BGPM_MAXERRSTRG*2, "%s(%s:%02d) ", lastTopLevel, ucStrg, Kernel_ProcessorID());

        if (loc) pos += snprintf(&prtStrg[pos], BGPM_MAXERRSTRG*2-pos, "Error at %s: ", loc);
        else     pos += snprintf(&prtStrg[pos], BGPM_MAXERRSTRG*2-pos, "Error: ");

        if (op)  pos += snprintf(&prtStrg[pos], BGPM_MAXERRSTRG*2-pos, "%s ", op);

        pos += snprintf(&prtStrg[pos], BGPM_MAXERRSTRG*2-pos, "err=%d; %s.\n", errNum, lastErr.ErrStrg(errNum));

        fprintf(stderr, prtStrg);
    }
    if ((errNum < 0) && GetDumpOnError()) {
        fprintf(stderr, "Bgpm assertion dump on error\n");
        UPC_Assert(0);
    }
    if ((errNum < 0) && GetExitOnError()) {
        Upci_Delay(EXIT_DELAY);
        exit(errNum);
    }
    return *this;
}



// We'll create are own print routine for these messages in case we want to start redirecting to log preformat and such
ErrBkt & ErrBkt::PrintMsg(const char* format, ...)
{
    BGPM_EXEMPT_L2;
    if (GetPrintOnError()) {
        va_list  args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
    }
    return *this;
}



