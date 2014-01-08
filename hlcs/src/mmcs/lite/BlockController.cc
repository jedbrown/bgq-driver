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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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


#include "BlockController.h"

#include "Database.h"

#include "server/BlockControllerBase.h"
#include "server/CNBlockController.h"
#include "server/IOBlockController.h"

#include "common/Properties.h"

#include <utility/include/Log.h>

#include <iostream>
#include <string>
#include <vector>


using namespace std;

using mmcs::common::Properties;


LOG_DECLARE_FILE( "mmcs.lite" );


namespace mmcs {
namespace lite {


BlockController::BlockController(
        BGQMachineXML* machine,
        const std::string& userName,
        const std::string& blockName
        ) :
    BlockHelper(new server::BlockControllerBase(machine, userName, blockName, false))
{
    // nothing to do
}

BlockController::BlockController(server::BlockPtr ptr) :
    BlockHelper(ptr) {}

void
BlockController::processConsoleMessage(
        MCServerMessageSpec::ConsoleMessage& consoleMessage
        )
{
    // unformatted raw XML
    // cout << consoleMessage << endl;

    // nicely formatted console
    for (vector<string>::iterator it = consoleMessage._lines.begin(); it != consoleMessage._lines.end(); ++it) {

        // The following code reverts the unprintable characters to their original hex value
        // Since unprintable characters (tab, line feeds, etc.) do not flow through XML properly,
        // MC converts them into   ~^XX^~  and then this code converts them back.
        char buf[4096];
        char* buf_ptr = buf;
        unsigned chars_written;
        char *source;
        char *source2;
        char convunit[3];
        unsigned result;
        int midlen;
        source = const_cast<char*>( strstr((*it).c_str(),"~^") );
        if ((source  != NULL) && isxdigit(source[2]) && isxdigit(source[3]) && (source[4] == '^') && (source[5] == '~') ) {
            source[0] = '\0';
            chars_written = sprintf(buf, "%s", (*it).c_str());
            buf_ptr += chars_written;

            convunit[0] = source[2];
            convunit[1] = source[3];
            convunit[2] = '\0';
            result = strtoul(convunit, 0, 16);
            *(buf_ptr++) = (unsigned char) result;
            *(buf_ptr) = '\0';
            source += 6;

            while(1) {
                source2 = strstr(source,"~^");
                if ((source2  != NULL) && isxdigit(source2[2]) && isxdigit(source2[3]) && (source2[4] == '^') && (source2[5] == '~') ) {
                    source2[0] = '\0';
                    if (source != source2)
                        strcat(buf,source);

                    midlen = strlen(source);
                    buf_ptr += midlen;
                    source += midlen;

                    convunit[0] = source2[2];
                    convunit[1] = source2[3];
                    convunit[2] = '\0';
                    result = strtoul(convunit, 0, 16);

                    *(buf_ptr++) = (unsigned char) result;
                    *(buf_ptr) = '\0';
                    source += 6;
                } else {
                    strcat(buf,source);
                    strcat(buf,"\n");
                    chars_written = strlen(buf);
                    break;
                }
            }
            if (log_logger_->isTraceEnabled()) {
                LOG_TRACE_MSG( "{" <<  consoleMessage._cardLocation << "-" << NODE_CARD_JTAGPORT_TO_LCTN[consoleMessage._jtagPort] << "}" << consoleMessage._cpu << ":" << consoleMessage._threadId<<  " " << string(buf) );
            } else {
                cout << "{" <<  consoleMessage._cardLocation << "-" << NODE_CARD_JTAGPORT_TO_LCTN[consoleMessage._jtagPort] << "}" << consoleMessage._cpu << ":" << consoleMessage._threadId<<  " " << string(buf);
            }

        } else  // no special characters

            if (log_logger_->isTraceEnabled()) {
                LOG_TRACE_MSG(  "{" <<  consoleMessage._cardLocation << "-" << NODE_CARD_JTAGPORT_TO_LCTN[consoleMessage._jtagPort] << "}" << consoleMessage._cpu << ":" << consoleMessage._threadId<<  " " << *it << endl );
            } else {
                cout << "{" <<  consoleMessage._cardLocation << "-" << NODE_CARD_JTAGPORT_TO_LCTN[consoleMessage._jtagPort] << "}" << consoleMessage._cpu << ":" << consoleMessage._threadId<<  " " << *it << endl;
            }

        if (((*it).find("Software Test PASS") != string::npos)||((*it).find("SoftwareTestPASS") != string::npos)) {
            if (getBase()->_terminatedNodes < 0)
                --getBase()->_terminatedNodes;
            else
                ++getBase()->_terminatedNodes;
        } else
            if (((*it).find("Software Test FAIL") != string::npos)||((*it).find("SoftwareTestFAIL") != string::npos)) {
                if (getBase()->_terminatedNodes < 0)
                    --getBase()->_terminatedNodes;
                else {
                    ++getBase()->_terminatedNodes;
                    getBase()->_terminatedNodes = getBase()->_terminatedNodes * -1;
                }

            }
    }

}

int
BlockController::processRASMessage(
        RasEvent& rasEvent
        )
{
    LOG_TRACE_MSG("Processing RAS event");
    LOG_TRACE_MSG(rasEvent);
    // let BlockController do its part
    bool filtered = getBase()->processRASMessage(rasEvent);

    if (!Properties::getProperty("print_all_ras").empty() &&
        Properties::getProperty("print_all_ras") != "false") {

        server::CNBlockPtr compute_block = boost::dynamic_pointer_cast<server::CNBlockController>( getBase() );
        server::IOBlockPtr io_block = boost::dynamic_pointer_cast<server::IOBlockController>( getBase() );

        if (!compute_block && !io_block) {
            getBase()->printRASMessage(rasEvent);
        }

    }

    // insert into database
    if (!filtered)
    {
        lite::Database db;
        char sqlstr[2048];
        char *zErrMsg = 0;
        snprintf(sqlstr, sizeof(sqlstr), "insert into EVENTLOG (msgid, category, component, severity, block, location, message,jobid) values('%s','%s','%s','%s','%s','%s','%s',%lu)",
                rasEvent.getDetails()[RasEvent::MSG_ID].c_str(),
                rasEvent.getDetails()[RasEvent::CATEGORY].c_str(),
                rasEvent.getDetails()[RasEvent::COMPONENT].c_str(),
                rasEvent.getDetails()[RasEvent::SEVERITY].c_str(),
                getBase()->_blockName.c_str(),
                rasEvent.getDetails()[RasEvent::LOCATION].c_str(),
                rasEvent.getDetails()[RasEvent::MESSAGE].c_str(),
                0l /* job ID */ );
        (void)sqlite3_exec( db.getHandle(), sqlstr, NULL, 0 , &zErrMsg);
    }

    return filtered;
}

} } // namespace mmcs::lite
