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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#include <string>
#include <sstream>
#include <bitset>
#include <signal.h>
#include <stdlib.h>
#include <bgq_util/include/LocationUtil.h>
#include <bgq_util/include/Time.h>

#include <control/include/mc/cardsDefs.h>

#include <control/include/mcServer/MCServerRef.h>
#include <control/include/mcServer/MCServer_errno.h>
#include <control/include/mcServer/MCServerAPIHelpers.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <db/include/api/tableapi/dbbasic.h>
#include <db/include/api/tableapi/TxObject.h>

#include <db/include/api/BGQDBlib.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>

#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/assign/list_of.hpp>
#include <boost/scoped_ptr.hpp>

#include "MMCSThread.h"
#include "MMCSCommandProcessor.h"
#include "MMCSEnvMonitor.h"
#include "MMCSCommandReply.h"
#include "MMCSProperties.h"
#include "HardwareBlockList.h"
#include "RunJobConnection.h"
#include "DBBlockController.h"

using std::string;
using std::ostringstream;
using std::vector;

LOG_DECLARE_FILE( "mmcs" );

#define SHORT_POLLING_PERIOD  5   // 5 seconds to check for thread being killed
#define NORMAL_POLLING_PERIOD 10  // 10 seconds to check for threads being killed
#define ENVS_POLLING_PERIOD 300   // 5 minutes to check cards for new environmental data
#define CARD_NOT_PRESENT    52    // special value returned in the _error field
#define CARD_NOT_UP         11    // special value returned in the _error field


//******************************************************************************
// Class for the node cards
//******************************************************************************
class NodeCardThread : public MMCSThread
{
public:
    NodeCardThread()
        :  seconds(ENVS_POLLING_PERIOD), pollEnvs(false)
    {};
    void* threadStart();
    unsigned seconds;
    bool pollEnvs;
};

//******************************************************************************
// Class for the IO cards
//******************************************************************************
class IOCardThread : public MMCSThread
{
public:
    IOCardThread()
        :  seconds(ENVS_POLLING_PERIOD), pollEnvs(false)
    {};
    void* threadStart();
    unsigned seconds;
    bool pollEnvs;
};

//******************************************************************************
// Class for the service cards
//******************************************************************************
class ServiceCardThread : public MMCSThread
{
public:
    ServiceCardThread()
        :  seconds(ENVS_POLLING_PERIOD), pollEnvs(false)
    {};
    void* threadStart();
    unsigned seconds;
    bool pollEnvs;
};

//******************************************************************************
// Class for the performance data
//******************************************************************************
class PerfDataThread : public MMCSThread
{
public:
    PerfDataThread()
        :  seconds(ENVS_POLLING_PERIOD), pollEnvs(false)
    {};
    void* threadStart();
    unsigned seconds;
    bool pollEnvs;
};


//******************************************************************************
// Class for the health check
//******************************************************************************
class HealthCheckThread : public MMCSThread
{
public:
    HealthCheckThread()
    :  seconds(ENVS_POLLING_PERIOD)
    {};
    void* threadStart();
    unsigned seconds;
};

//******************************************************************************
// Class for the bulk power modules
//******************************************************************************
class BulkPowerThread : public MMCSThread
{
public:
    BulkPowerThread()
        :  seconds(ENVS_POLLING_PERIOD), pollEnvs(false)
    {};
    void* threadStart();
    unsigned seconds;
    bool pollEnvs;
};

//******************************************************************************
// Class for the coolant monitor
//******************************************************************************
class CoolantThread : public MMCSThread
{
public:
    CoolantThread()
        :  seconds(ENVS_POLLING_PERIOD), pollEnvs(false)
    {};
    void* threadStart();
    unsigned seconds;
    bool pollEnvs;
};

//******************************************************************************
// Class for the optical modules
//******************************************************************************
class OpticalThread : public MMCSThread
{
public:
    OpticalThread()
        :  seconds(ENVS_POLLING_PERIOD), pollEnvs(false)
    {};
    void* threadStart();
    unsigned seconds;
    bool pollEnvs;
};


//******************************************************************************
// Thread start function for the EnvMonitor thread
//******************************************************************************

void*
EnvMonitorThread::threadStart()
{
    unsigned pollingPeriod = NORMAL_POLLING_PERIOD;

    int pollProperty;

    pollProperty = strtol(MMCSProperties::getProperty(MMCS_ENVS_NC_INTERVAL_SECONDS).c_str(), NULL, 10);
    if (((pollProperty >= 60)  && (pollProperty <= 3600))  || pollProperty == 0)
        ncseconds = pollProperty;

    pollProperty = strtol(MMCSProperties::getProperty(MMCS_ENVS_IO_INTERVAL_SECONDS).c_str(), NULL, 10);
    if (((pollProperty >= 60)  && (pollProperty <= 3600))  || pollProperty == 0)
        ioseconds = pollProperty;

    pollProperty = strtol(MMCSProperties::getProperty(MMCS_ENVS_SC_INTERVAL_SECONDS).c_str(), NULL, 10);
    if (((pollProperty >= 60)  && (pollProperty <= 3600))  || pollProperty == 0)
        scseconds = pollProperty;

     pollProperty = strtol(MMCSProperties::getProperty(MMCS_ENVS_BULK_INTERVAL_SECONDS).c_str(), NULL, 10);
    if (((pollProperty >= 60)  && (pollProperty <= 3600))  || pollProperty == 0)
        bulkseconds = pollProperty;

    pollProperty = strtol(MMCSProperties::getProperty(MMCS_ENVS_COOLANT_INTERVAL_SECONDS).c_str(), NULL, 10);
    if (((pollProperty >= 60)  && (pollProperty <= 3600))  || pollProperty == 0)
        coolantsecs = pollProperty;

    pollProperty = strtol(MMCSProperties::getProperty(MMCS_ENVS_OPT_INTERVAL_SECONDS).c_str(), NULL, 10);
    if (((pollProperty >= 60)  && (pollProperty <= 3600))  || pollProperty == 0)
        opticalsecs = pollProperty;

    pollProperty = strtol(MMCSProperties::getProperty(MMCS_PERFDATA_INTERVAL_SECONDS).c_str(), NULL, 10);
    if (((pollProperty >= 60) && (pollProperty <= 1800))   || pollProperty == 0)  // only use value from 1 to 30 minutes
        perfseconds = pollProperty;

    // Create a thread for node cards
    NodeCardThread* nCardThread = new NodeCardThread;
    if (ncseconds > 0) {
        nCardThread->seconds = ncseconds;
        nCardThread->pollEnvs = true;
    }
    nCardThread->start();

    // Create a thread for IO cards
    IOCardThread* ioCardThread = new IOCardThread;
    if (ioseconds > 0) {
        ioCardThread->seconds = ioseconds;
        ioCardThread->pollEnvs = true;
    }
    ioCardThread->start();

    // Create a thread for service cards
    ServiceCardThread* sCardThread = new ServiceCardThread;
    if (scseconds > 0) {
        sCardThread->seconds = scseconds;
        sCardThread->pollEnvs = true;
    }
    sCardThread->start();

    // Create a thread for bulks
    BulkPowerThread* bulkThread = new BulkPowerThread;
    if (bulkseconds > 0) {
        bulkThread->seconds = bulkseconds;
        bulkThread->pollEnvs = true;
    }
    bulkThread->start();

    // Create a thread for coolant monitor
    CoolantThread* coolantThread = new CoolantThread;
    if (coolantsecs > 0) {
        coolantThread->seconds = coolantsecs;
        coolantThread->pollEnvs = true;
    }
    coolantThread->start();

    // Create a thread for optical modules
    OpticalThread* opticalThread = new OpticalThread;
    if (opticalsecs > 0) {
        opticalThread->seconds = opticalsecs;
        opticalThread->pollEnvs = true;
    }
    opticalThread->start();

    // Create a thread for health check  (NOTE: this cannot be turned off or changed)
    HealthCheckThread* healthThread = new HealthCheckThread;
    healthThread->seconds = healthseconds;
    healthThread->start();


    // Create a thread for perf data
    PerfDataThread* perfThread = new PerfDataThread;
    if (perfseconds > 0) {
        perfThread->seconds = perfseconds;
        perfThread->pollEnvs = true;
    }
    perfThread->start();


    // loop until mmcs_server stops us
    while (isThreadStopping() == false)
    {
        sleep(pollingPeriod);	// polling period in seconds
    }


    nCardThread->stop(SIGUSR1 );
    ioCardThread->stop(SIGUSR1 );
    sCardThread->stop(SIGUSR1 );
    perfThread->stop(SIGUSR1 );
    bulkThread->stop(SIGUSR1 );
    coolantThread->stop(SIGUSR1 );
    opticalThread->stop(SIGUSR1 );
    healthThread->stop(SIGUSR1 );

    delete nCardThread;
    delete ioCardThread;
    delete sCardThread;
    delete perfThread;
    delete bulkThread;
    delete coolantThread;
    delete opticalThread;
    delete healthThread;

    return NULL;
}

//******************************************************************************
// RAS event meta-data for all RAS events generated by the env monitor
//******************************************************************************
// All must be in the range of 0x00061000  to 0x00061fff
//******************************************************************************


/*
<rasevent  id="0x00061001"  component="MMCS"
  category="Node_Board"
  severity="WARN"
  message="MMCS could not contact node board at location $(BG_LOC)."
  description="MMCS attempted to read environmentals from the node board at the specified location, and received an error.  No environmentals will be reported for this polling interval, for the board."
  service_action="If the board continues to have this error, run diagnostics on the node board."
  relevant_diags="nodeboard"
 />

<rasevent  id="0x00061002"  component="MMCS"
  category="Service_Card"
  severity="WARN"
  message="MMCS could not contact service card at location $(BG_LOC)."
  description="MMCS attempted to read environmentals from the service card at the specified location, and received an error.  No environmentals will be reported for this polling interval, for the card."
  service_action="If the card continues to have this error, run diagnostics on the service card."
  relevant_diags="servicecard"
 />

<rasevent  id="0x00061003"  component="MMCS"
  category="AC_TO_DC_PWR"
  severity="WARN"
  message="MMCS could not contact bulk power module at location $(BG_LOC)."
  description="MMCS attempted to read environmentals from the bulk power module at the specified location, and received an error.  No environmentals will be reported for this polling interval, for the module."
  service_action="If the bulk power module continues to have this error, look at the LEDs on the modules to determine if there is a failure."
  relevant_diags=""
 />

<rasevent  id="0x00061004"  component="MMCS"
  category="IO_Board"
  severity="WARN"
  message="MMCS could not contact IO board at location $(BG_LOC)."
  description="MMCS attempted to read environmentals from the IO board at the specified location, and received an error.  No environmentals will be reported for this polling interval, for the board."
  service_action="If the board continues to have this error, run diagnostics on the failing board."
  relevant_diags="ioboard"
 />

 <rasevent  id="0x00061005"  component="MMCS"
  category="Coolant_Monitor"
  severity="WARN"
  message="MMCS could not contact coolant monitor at location $(BG_LOC)."
  description="MMCS attempted to read environmentals from the coolant monitor at the specified location, and received an error.  No environmentals will be reported for this polling interval."
  service_action="If the coolant monitor continues to have this error, check the power modules on the midplane to make sure they are supplying power to the coolant monitor."
  relevant_diags=""
 />

 <rasevent  id="0x00061006"  component="MMCS"
  category="Coolant_Monitor"
  severity="WARN"
  message="Health Check detected an error on the coolant monitor connected to the service card at location $(BG_LOC). The condition is related to $(COND)."
  description="The Health Check analyzed the coolant monitor at the specified location, and received an indication that one or more faults occurred. This is an indication that one or more of the coolant monitor alarm thresholds has been reached.  The faults indicated above did not result in water and or power being shut-off on the rack."
  service_action="If the coolant monitor continues to post this error, and can not be explained by some specific environmental characteristic of the facility or specific action done to the rack,  then a more in depth inspection of the rack and and its connections is warranted."
  relevant_diags=""
  details="COND"
 />

 <rasevent  id="0x00061007"  component="MMCS"
  category="Service_Card"
  severity="WARN"
  message="Health Check detected an incorrect clock frequency $(FREQ) on the service card at location $(BG_LOC)."
  description="The Health Check analyzed the service card at the specified location, and found a clock frequency that was outside the expected range."
  service_action="If the card continues to have this error, run diagnostics on the service card. The problem may also be originating from the master clock card."
  relevant_diags="servicecard"
  details="FREQ"
  />

  <rasevent  id="0x00061008"  component="MMCS"
  category="Service_Card"
  severity="WARN"
  message="Health Check detected an overtemp condition on the service card at location $(BG_LOC).  The temperature $(ACTUAL) is above the expected maximum temperature of $(EXP). "
  description="The Health Check analyzed the service card at the specified location, and found a temperature that was outside the expected range."
  service_action="If the card continues to have this error, run diagnostics on the service card.  Check for temperature-related RAS events on other hardware to determine if the problem is widespread or isolated to this card."
  relevant_diags="servicecard"
  details="ACTUAL, EXP"
  />

  <rasevent  id="0x00061009"  component="MMCS"
  category="IO_Board"
  severity="WARN"
  message="Health Check detected an incorrect clock frequency $(FREQ) on the IO board at location $(BG_LOC)."
  description="The Health Check analyzed the IO board at the specified location, and found a clock frequency that was outside the expected range."
  service_action="If the card continues to have this error, run diagnostics on the IO board. The problem may also be originating from the master clock card."
  relevant_diags="ioboard"
  details="FREQ"
  />

  <rasevent  id="0x0006100A"  component="MMCS"
  category="IO_Board"
  severity="WARN"
  message="Health Check detected an abnormal status flag set for the IO board at location $(BG_LOC).  The status flag is for $(COMP)."
  description="The Health Check analyzed the IO board at the specified location, and found a status flag set for an abnormal condition."
  service_action="If the card continues to have this error, run diagnostics on the IO board."
  relevant_diags="ioboard"
  details="COMP"
  />

  <rasevent  id="0x0006100B"  component="MMCS"
  category="DCA"
  severity="WARN"
  message="Health Check detected an abnormal condition for the Direct Current Assembly (DCA) card at location $(BG_LOC).  The condition is $(COND).  The invalid value is $(BADVAL)."
  description="The Health Check analyzed the IO board at the specified location, and found an abnormal condition for a DCA card."
  service_action="If the card continues to have this error, run diagnostics on the IO board."
  relevant_diags="ioboard"
  details="COND, BADVAL"
  />

  <rasevent  id="0x0006100C"  component="MMCS"
  category="DCA"
  severity="WARN"
  message="Health Check detected an abnormal condition for the Direct Current Assembly (DCA) card at location $(BG_LOC).  The condition is related to $(COND).  The invalid value is $(BADVAL)."
  description="The Health Check analyzed the node board at the specified location, and found an abnormal condition for a DCA card."
  service_action="If the card continues to have this error, run diagnostics on the node board."
  relevant_diags="nodeboard"
  details="COND, BADVAL"
  />

  <rasevent  id="0x0006100D"  component="MMCS"
  category="Optical_Module"
  severity="WARN"
  message="Health Check detected an abnormal condition for the optical module at location $(BG_LOC).  The condition is related to $(COND)."
  description="The Health Check analyzed the IO board at the specified location, and found an abnormal condition for an optical module."
  service_action="If the card continues to have this error, run diagnostics on the IO board."
  relevant_diags="ioboard"
  details="COND"
  />

  <rasevent  id="0x0006100E"  component="MMCS"
  category="Optical_Module"
  severity="WARN"
  message="Health Check detected an abnormal condition for the optical module at location $(BG_LOC).  The condition is related to $(COND)."
  description="The Health Check analyzed the node board at the specified location, and found an abnormal condition for an optical module."
  service_action="If the card continues to have this error, run diagnostics on the node board."
  relevant_diags="nodeboard"
  details="COND"
  />

  <rasevent  id="0x0006100F"  component="MMCS"
  category="AC_TO_DC_PWR"
  severity="WARN"
  message="Health Check detected an abnormal condition for the bulk power module at location $(BG_LOC).  The condition is related to $(COND).  The invalid value is $(BADVAL)."
  description="The Health Check analyzed the bulk power module at the specified location, and found an abnormal condition."
  service_action="If the bulk power module continues to have this error, look at the LEDs on the modules to determine if there is a failure."
  relevant_diags=""
  details="COND, BADVAL"
  />

  <rasevent  id="0x00061010"  component="MMCS"
  category="Node_Board"
  severity="WARN"
  message="Health Check detected an incorrect clock frequency $(FREQ) on the node board at location $(BG_LOC)."
  description="The Health Check analyzed the node board at the specified location, and found a clock frequency that was outside the expected range."
  service_action="If the card continues to have this error, run diagnostics on the node board. The problem may also be originating from the master clock card."
  relevant_diags="nodeboard"
  details="FREQ"
  />

  <rasevent  id="0x00061011"  component="MMCS"
  category="Node_Board"
  severity="WARN"
  message="Health Check detected an abnormal status flag set for the node board at location $(BG_LOC).  The status flag is for $(COMP)."
  description="The Health Check analyzed the node board at the specified location, and found a status flag set for an abnormal condition."
  service_action="If the card continues to have this error, run diagnostics on the node board."
  relevant_diags="nodeboard"
  details="COMP"
  />

  <rasevent  id="0x00061012"  component="MMCS"
  category="AC_TO_DC_PWR"
  severity="FATAL"
  message="Health Check detected multiple failed bulk power modules in an enclosure.  Hardware at location $(BG_LOC) is being marked in error."
  description="The Health Check analyzed the bulk power modules in the midplane, and found multiple failures. Individual RAS events have been sent for each condition.  Hardware has been marked in Error to prevent further use.  Any running jobs have been killed and blocks freed."
  service_action="If the bulk power modules continue to have this error, replace the failed modules."
  relevant_diags=""
  control_action="BOARD_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK"
  />

  <rasevent  id="0x00061013"  component="MMCS"
  category="AC_TO_DC_PWR"
  severity="FATAL"
  message="Health Check detected multiple failed bulk power modules in an enclosure.  Hardware at location $(BG_LOC) is being marked in error."
  description="The Health Check analyzed the bulk power modules in the midplane, and found multiple failures. Individual RAS events have been sent for each condition.  Hardware has been marked in Error to prevent further use."
  service_action="If the bulk power modules continue to have this error, replace the failed modules."
  relevant_diags=""
  control_action="BOARD_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK"
  />

 <rasevent  id="0x00061014"  component="MMCS"
  category="Coolant_Monitor"
  severity="FATAL"
  message="Health Check detected the coolant monitor connected to the service card at location $(BG_LOC) has been shut-off: $(COND)"
  description="The Health Check analyzed the coolant monitor at the specified location, and received an indication that its shut-off register has latched."
  service_action="Repair the coolant water problem, recover the rack using a Service Action."
  relevant_diags=""
  details="COND"
  threshold_count="1"
 />

  <rasevent  id="0x00061015"  component="MMCS"
  category="BQC"
  severity="WARN"
  message="Health Check detected an overtemp condition on the compute node at location $(BG_LOC).  The temperature $(ACTUAL) is above the expected maximum temperature of $(EXP). "
  description="The Health Check analyzed the node at the specified location, and found a temperature that was outside the expected range."
  service_action="It's possible the compute is not properly seated, the thermal interface material (TIM) is missing or was not properly applied, the TIM needs replacement, or the clear plastic TIM protector was not removed prior to installing the compute. Inspect the compute node and if no obvious reason for the high temperatures is noted, replace the compute node."
  relevant_diags="nodeboard"
  details="ACTUAL, EXP"
  />

*/

void processSC(MCServerMessageSpec::ReadServiceCardEnvReply* mcSCReply,  BGQDB::TxObject* tx) {

    // service card database details
    BGQDB::DBTServicecardenvironment sce;
    SQLRETURN rc;

    std::bitset<30> smap;
    smap.set();
    smap.reset(sce.TIME);
    sce._columns = smap.to_ulong();

    for(unsigned sc = 0 ; sc < mcSCReply->_serviceCards.size() ; sc++) {

        if (mcSCReply->_serviceCards[sc]._error == CARD_NOT_PRESENT) { } else  // do nothing if the card is not present
        if (mcSCReply->_serviceCards[sc]._error == CARD_NOT_UP) { } else  // do nothing if the card is not up
        if (mcSCReply->_serviceCards[sc]._error != 0) {
            LOG_INFO_MSG("Error occurred reading environmentals from: " << mcSCReply->_serviceCards[sc]._location);
            RasEventImpl noContact(0x00061002);
            noContact.setDetail(RasEvent::LOCATION, mcSCReply->_serviceCards[sc]._location.c_str());
            RasEventHandlerChain::handle(noContact);
            BGQDB::putRAS(noContact);
        } else {
            strcpy(sce._location,mcSCReply->_serviceCards[sc]._location.c_str());

            sscanf(mcSCReply->_serviceCards[sc]._powerRailV12PVoltage.c_str(), "%lf", &sce._voltagev12p);
            sscanf(mcSCReply->_serviceCards[sc]._powerRailV12R5Voltage.c_str(), "%lf", &sce._voltagev12r5);
            sscanf(mcSCReply->_serviceCards[sc]._powerRailV15PVoltage.c_str(), "%lf", &sce._voltagev15p);
            sscanf(mcSCReply->_serviceCards[sc]._powerRailV25PVoltage.c_str(), "%lf", &sce._voltagev25p);
            sscanf(mcSCReply->_serviceCards[sc]._powerRailV33PVoltage.c_str(), "%lf", &sce._voltagev33p);
            sscanf(mcSCReply->_serviceCards[sc]._powerRailV50PVoltage.c_str(), "%lf", &sce._voltagev50p);

            rc = tx->insert(&sce);
            if(rc != SQL_SUCCESS) {
                LOG_INFO_MSG("Error writing service card environmentals: location " << string(sce._location)  << " return code " << rc);
            }
        }
    } // for loop that goes through each service card

}

void processNC(MCServerMessageSpec::ReadNodeCardEnvReply* mcNCReply,  BGQDB::TxObject* tx) {

    int MAXNODETEMP = 90; // Celcius
    if (!MMCSProperties::getProperty("MAXNODETEMP").empty() ) {
        MAXNODETEMP = boost::lexical_cast<int>(MMCSProperties::getProperty("MAXNODETEMP"));
    }

    // node card database details
    BGQDB::DBTNodecardenvironment nce;
    BGQDB::DBTNodeenvironment nde;
    BGQDB::DBTLinkchipenvironment lce;
    SQLRETURN rc;

    std::bitset<30> map, nmap, lmap;
    map.set();
    map.reset(nce.TIME);
    nmap.set();
    nmap.reset(nde.TIME);
    lmap.set();
    lmap.reset(lce.TIME);
    nce._columns = map.to_ulong();
    nde._columns = nmap.to_ulong();
    lce._columns = lmap.to_ulong();

    for(unsigned nc = 0 ; nc < mcNCReply->_nodeCards.size() ; nc ++) {
        if (mcNCReply->_nodeCards[nc]._error == CARD_NOT_PRESENT) { } else  // do nothing if the card is not present
        if (mcNCReply->_nodeCards[nc]._error == CARD_NOT_UP) { } else       // do nothing if the card is not up
        if (mcNCReply->_nodeCards[nc]._error != 0) {
            LOG_INFO_MSG("Error occurred reading environmentals from: " << mcNCReply->_nodeCards[nc]._lctn);
            RasEventImpl noContact(0x00061001);
            noContact.setDetail(RasEvent::LOCATION, mcNCReply->_nodeCards[nc]._lctn.c_str());
            RasEventHandlerChain::handle(noContact);
            BGQDB::putRAS(noContact);

        } else {

            strcpy(nce._location,mcNCReply->_nodeCards[nc]._lctn.c_str());
            sscanf(mcNCReply->_nodeCards[nc]._powerRailV08Voltage.c_str(), "%lf", &nce._voltagev08);
            sscanf(mcNCReply->_nodeCards[nc]._powerRailV14Voltage.c_str(), "%lf", &nce._voltagev14);
            sscanf(mcNCReply->_nodeCards[nc]._powerRailV25Voltage.c_str(), "%lf", & nce._voltagev25);
            sscanf(mcNCReply->_nodeCards[nc]._powerRailV33Voltage.c_str(), "%lf", &nce._voltagev33);
            sscanf(mcNCReply->_nodeCards[nc]._powerRailV120PVoltage.c_str(), "%lf", &nce._voltagev120p);
            sscanf(mcNCReply->_nodeCards[nc]._powerRailV15Voltage.c_str(), "%lf", &nce._voltagev15);
            sscanf(mcNCReply->_nodeCards[nc]._powerRailV09Voltage.c_str(), "%lf", &  nce._voltagev09);
            sscanf(mcNCReply->_nodeCards[nc]._powerRailV10Voltage.c_str(), "%lf", &nce._voltagev10);
            sscanf(mcNCReply->_nodeCards[nc]._powerRailV33PVoltage.c_str(), "%lf", & nce._voltagev33p);
            sscanf(mcNCReply->_nodeCards[nc]._powerRailV12AVoltage.c_str(), "%lf", &nce._voltagev12a);
            sscanf(mcNCReply->_nodeCards[nc]._powerRailV12BVoltage.c_str(), "%lf", &  nce._voltagev12b);
            sscanf(mcNCReply->_nodeCards[nc]._powerRailV18Voltage.c_str(), "%lf", &nce._voltagev18);
            sscanf(mcNCReply->_nodeCards[nc]._powerRailV25PVoltage.c_str(), "%lf", &nce._voltagev25p);
            sscanf(mcNCReply->_nodeCards[nc]._powerRailV12PVoltage.c_str(), "%lf", &nce._voltagev12p);
            sscanf(mcNCReply->_nodeCards[nc]._powerRailV18PVoltage.c_str(), "%lf", & nce._voltagev18p);

            nce._tempmonitor0 = mcNCReply->_nodeCards[nc]._onboardTemp0;
            nce._tempmonitor1 = mcNCReply->_nodeCards[nc]._onboardTemp1;

            rc = tx->insert(&nce);
            if(rc != SQL_SUCCESS) {
                LOG_INFO_MSG("Error writing node card environmentals: location " <<  string(nce._location)  << " return code " << rc);
            }

            for(unsigned compute = 0 ; compute < mcNCReply->_nodeCards[nc]._computes.size() ; ++compute) {
                if (mcNCReply->_nodeCards[nc]._computes[compute]._error != 0) {
                    LOG_INFO_MSG("Error occurred reading environmentals from: " << mcNCReply->_nodeCards[nc]._computes[compute]._lctn);
                } else {
                    strcpy(nde._location, mcNCReply->_nodeCards[nc]._computes[compute]._lctn.c_str());

                    // use TVSense temp if its valid, use I2C temp otherwise
                    if (mcNCReply->_nodeCards[nc]._computes[compute]._tempTvSense == 0 ||
                        mcNCReply->_nodeCards[nc]._computes[compute]._tempTvSense >= 254)
                        nde._asictemp = mcNCReply->_nodeCards[nc]._computes[compute]._tempI2c;
                    else
                        nde._asictemp = mcNCReply->_nodeCards[nc]._computes[compute]._tempTvSense;

                    if ( nde._asictemp > MAXNODETEMP ) {
                        RasEventImpl ras(0x00061015);
                        ras.setDetail(RasEvent::LOCATION, nde._location);
                        ras.setDetail("ACTUAL", boost::lexical_cast<string>(nde._asictemp));
                        ras.setDetail("EXP", boost::lexical_cast<std::string>(MAXNODETEMP));
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }

                    rc = tx->insert(&nde);
                    if(rc != SQL_SUCCESS) {
                        LOG_INFO_MSG("Error writing node environmentals: location " <<  string(nde._location)  << " return code " << rc);
                    }
                }
            }

            for(unsigned blink = 0 ; blink < mcNCReply->_nodeCards[nc]._blinks.size() ; ++blink) {
                if (mcNCReply->_nodeCards[nc]._blinks[blink]._error != 0) {
                    LOG_INFO_MSG("Error occurred reading environmentals from: " << mcNCReply->_nodeCards[nc]._blinks[blink]._lctn);
                } else {
                    strcpy(lce._location, mcNCReply->_nodeCards[nc]._blinks[blink]._lctn.c_str());

                    // use TVSense temp if its valid, use I2C temp otherwise
                    if (mcNCReply->_nodeCards[nc]._blinks[blink]._tempTvSense == 0 ||
                        mcNCReply->_nodeCards[nc]._blinks[blink]._tempTvSense >= 253)
                        lce._asictemp = mcNCReply->_nodeCards[nc]._blinks[blink]._tempI2c;
                    else
                        lce._asictemp = mcNCReply->_nodeCards[nc]._blinks[blink]._tempTvSense;

                    rc = tx->insert(&lce);
                    if(rc != SQL_SUCCESS) {
                        LOG_INFO_MSG("Error writing link chip environmentals: location " <<  string(lce._location)  << " return code " << rc);
                    }
                }
            }

        }  // else leg (_error flag not set for this node card
    } // for loop that goes through each node card

}

void processIO(MCServerMessageSpec::ReadIoCardEnvReply*  mcIOReply,  BGQDB::TxObject* tx) {

    BGQDB::DBTFanenvironment fane;
    BGQDB::DBTIocardenvironment ioe;
    BGQDB::DBTNodeenvironment nde;
    BGQDB::DBTLinkchipenvironment lce;
    SQLRETURN rc;

    std::bitset<30> imap, fmap, nmap, lmap;
    imap.set();
    imap.reset(ioe.TIME);
    fmap.set();
    fmap.reset(fane.TIME);
    nmap.set();
    nmap.reset(nde.TIME);
    lmap.set();
    lmap.reset(lce.TIME);
    nde._columns = nmap.to_ulong(); // indicate which columns have data provided
    lce._columns = lmap.to_ulong(); // indicate which columns have data provided
    fane._columns = fmap.to_ulong(); // indicate which columns have data provided
    ioe._columns = imap.to_ulong(); // indicate which columns have data provided

    for(unsigned io = 0 ; io < mcIOReply->_ioCards.size() ; io++) {

        if (mcIOReply->_ioCards[io]._error == CARD_NOT_PRESENT) { } else  // do nothing if the card is not present
        if (mcIOReply->_ioCards[io]._error == CARD_NOT_UP) { } else       // do nothing if the card is not up
        if (mcIOReply->_ioCards[io]._error != 0) {
            LOG_INFO_MSG("Error occurred reading environmentals from: " << mcIOReply->_ioCards[io]._lctn);
            RasEventImpl noContact(0x00061004);
            noContact.setDetail(RasEvent::LOCATION, mcIOReply->_ioCards[io]._lctn);
            RasEventHandlerChain::handle(noContact);
            BGQDB::putRAS(noContact);
        } else {
            strcpy(ioe._location,mcIOReply->_ioCards[io]._lctn.c_str());


            sscanf(mcIOReply->_ioCards[io]._powerRailV08Voltage.c_str(), "%lf", &ioe._voltagev08);
            sscanf(mcIOReply->_ioCards[io]._powerRailV14Voltage.c_str(), "%lf", &ioe._voltagev14);
            sscanf(mcIOReply->_ioCards[io]._powerRailV25Voltage.c_str(), "%lf", & ioe._voltagev25);
            sscanf(mcIOReply->_ioCards[io]._powerRailV33Voltage.c_str(), "%lf", & ioe._voltagev33);
            sscanf(mcIOReply->_ioCards[io]._powerRailV120Voltage.c_str(), "%lf", & ioe._voltagev120);
            sscanf(mcIOReply->_ioCards[io]._powerRailV15Voltage.c_str(), "%lf", & ioe._voltagev15);
            sscanf(mcIOReply->_ioCards[io]._powerRailV09Voltage.c_str(), "%lf", & ioe._voltagev09);
            sscanf(mcIOReply->_ioCards[io]._powerRailV10Voltage.c_str(), "%lf", &  ioe._voltagev10);
            sscanf(mcIOReply->_ioCards[io]._powerRailV120PVoltage.c_str(), "%lf", & ioe._voltagev120p);
            sscanf(mcIOReply->_ioCards[io]._powerRailV33PVoltage.c_str(), "%lf", &  ioe._voltagev33p);
            sscanf(mcIOReply->_ioCards[io]._powerRailV12Voltage.c_str(), "%lf", &  ioe._voltagev12);
            sscanf(mcIOReply->_ioCards[io]._powerRailV18Voltage.c_str(), "%lf",&ioe._voltagev18);

            sscanf(mcIOReply->_ioCards[io]._powerRailV12PVoltage.c_str(), "%lf",&ioe._voltagev12p);
            sscanf(mcIOReply->_ioCards[io]._powerRailV15PVoltage.c_str(), "%lf",&ioe._voltagev15p);
            sscanf(mcIOReply->_ioCards[io]._powerRailV18PVoltage.c_str(), "%lf",&ioe._voltagev18p);
            sscanf(mcIOReply->_ioCards[io]._powerRailV25PVoltage.c_str(), "%lf",&ioe._voltagev25p);

            ioe._tempmonitor = mcIOReply->_ioCards[io]._onboardTemp;

            rc = tx->insert(&ioe);
            if(rc != SQL_SUCCESS) {
                LOG_INFO_MSG("Error writing IO card environmentals: location " <<  string(ioe._location)  << " return code " << rc);
            }

            for(unsigned fan = 0 ; fan < mcIOReply->_ioCards[io]._fanRpms.size() ; fan++) {
                strcpy(fane._location,mcIOReply->_ioCards[io]._lctn.c_str());
                fane._fannumber = fan;
                fane._rpms = mcIOReply->_ioCards[io]._fanRpms[fan]._rpmsFans;

                rc=tx->insert(&fane);
                if(rc != SQL_SUCCESS) {
                    LOG_INFO_MSG("Error writing fan environmentals: location " <<  string(fane._location)  << " return code " << rc);
                }
            }

            for(unsigned compute = 0 ; compute < mcIOReply->_ioCards[io]._computes.size() ; ++compute) {
                if (mcIOReply->_ioCards[io]._computes[compute]._error != 0) {
                    LOG_INFO_MSG("Error occurred reading environmentals from: " << mcIOReply->_ioCards[io]._computes[compute]._lctn);
                } else {
                    strcpy(nde._location, mcIOReply->_ioCards[io]._computes[compute]._lctn.c_str());

                    // use TVSense temp if its valid, use I2C temp otherwise
                    if (mcIOReply->_ioCards[io]._computes[compute]._tempTvSense == 0 ||
                        mcIOReply->_ioCards[io]._computes[compute]._tempTvSense >= 254)
                        nde._asictemp = mcIOReply->_ioCards[io]._computes[compute]._tempI2c;
                    else
                        nde._asictemp = mcIOReply->_ioCards[io]._computes[compute]._tempTvSense;

                    rc = tx->insert(&nde);
                    if(rc != SQL_SUCCESS) {
                        LOG_INFO_MSG("Error writing node environmentals: location " <<  string(nde._location)  << " return code " << rc);
                    }
                }
            }

            for(unsigned blink = 0 ; blink < mcIOReply->_ioCards[io]._blinks.size() ; ++blink) {
                if (mcIOReply->_ioCards[io]._blinks[blink]._error != 0) {
                    LOG_INFO_MSG("Error occurred reading environmentals from: " << mcIOReply->_ioCards[io]._blinks[blink]._lctn);
                } else {
                    strcpy(lce._location, mcIOReply->_ioCards[io]._blinks[blink]._lctn.c_str());

                    // use TVSense temp if its valid, use I2C temp otherwise
                    if (mcIOReply->_ioCards[io]._blinks[blink]._tempTvSense == 0 ||
                        mcIOReply->_ioCards[io]._blinks[blink]._tempTvSense >= 253)
                        lce._asictemp = mcIOReply->_ioCards[io]._blinks[blink]._tempI2c;
                    else
                        lce._asictemp = mcIOReply->_ioCards[io]._blinks[blink]._tempTvSense;

                    rc = tx->insert(&lce);
                    if(rc != SQL_SUCCESS) {
                        LOG_INFO_MSG("Error writing link chip environmentals: location " <<  string(lce._location)  << " return code " << rc);
                    }
                }
            }
        } //  else of card in error
    }  // for loop that goes thru all IO cards
}

void processBulks(MCServerMessageSpec::ReadBulkPowerEnvReply* mcBPReply,  BGQDB::TxObject* tx) {

    BGQDB::DBTBulkpowerenvironment bpe;
    std::bitset<30> map;
    SQLRETURN rc;

    map.set();
    map.reset(bpe.TIME);
    bpe._columns = map.to_ulong();

    for(unsigned bp = 0 ; bp < mcBPReply->_bpms.size() ; ++bp) {

        if (mcBPReply->_bpms[bp]._error == CARD_NOT_PRESENT) { } else  // do nothing if the card is not present
        if (mcBPReply->_bpms[bp]._error == CARD_NOT_UP) { } else       // do nothing if the card is not up
        if (mcBPReply->_bpms[bp]._error != 0) {
            LOG_INFO_MSG("Error occurred reading environmentals from: " << mcBPReply->_bpms[bp]._location);
            RasEventImpl noContact(0x00061003);
            noContact.setDetail(RasEvent::LOCATION, mcBPReply->_bpms[bp]._location.c_str());
            RasEventHandlerChain::handle(noContact);
            BGQDB::putRAS(noContact);
        } else {
            for(unsigned bpm = 0 ; bpm < mcBPReply->_bpms[bp]._bpms.size() ; ++bpm) {
                strcpy(bpe._location,mcBPReply->_bpms[bp]._bpms[bpm]._location.c_str());
                sscanf(mcBPReply->_bpms[bp]._bpms[bpm]._inputVoltage.c_str(), "%lf", &bpe._inputvoltage);
                sscanf(mcBPReply->_bpms[bp]._bpms[bpm]._inputCurrent.c_str(), "%lf", &bpe._inputcurrent);
                sscanf(mcBPReply->_bpms[bp]._bpms[bpm]._outputVoltage51V.c_str(), "%lf", &bpe._outputvoltage);
                sscanf(mcBPReply->_bpms[bp]._bpms[bpm]._outputCurrent51V.c_str(), "%lf", &bpe._outputcurrent);

                rc = tx->insert(&bpe);
                if(rc != SQL_SUCCESS) {
                    LOG_INFO_MSG("Error writing bulk power environmentals: location " << string(bpe._location)  << " return code " << rc);
                }
            }
        }
    }
}



//******************************************************************************
// Thread start function for the NodeCardThread
//******************************************************************************
void*
NodeCardThread::threadStart()
{
    unsigned pollingPeriod = NORMAL_POLLING_PERIOD;

    MCServerRef* server;
    string sqlstr;

    LOG_INFO_MSG( "thread started for Node Cards" << ", interval is " << seconds << " seconds" << ", polling is " << (pollEnvs? "ON":"OFF") );

    MMCSCommandReply rep;
    BlockControllerBase::mcserver_connect(server, "EnvMonNC", rep);
    if(!server) {
        LOG_ERROR_MSG("NodeCardThread failed to connect to mc_server");
        return NULL;
    }

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection())
    {
        LOG_INFO_MSG("unable to connect to database");
        return NULL;
    }

    // Get list of midplanes from tbgqmidplane
    SQLHANDLE hMidplanes;
    SQLLEN index;
    SQLRETURN sqlrc, result;
    char mp[7];

    sqlstr.clear();
    sqlstr.append("select LOCATION from BGQMidplane where location like 'R%-M0'");

    result = tx.execQuery( sqlstr.c_str(), &hMidplanes );

    SQLBindCol(hMidplanes, 1, SQL_C_CHAR, mp, 7, &index);

    // Turn SQL reply into vector of strings
    vector<string> midplaneLocs;
    sqlrc = SQLFetch(hMidplanes);
    while ( sqlrc == SQL_SUCCESS )   {
        midplaneLocs.push_back( mp );
        //LOG_DEBUG_MSG("MidplaneLoc = " << mp);
        sqlrc = SQLFetch(hMidplanes);
    }

    SQLCloseCursor(hMidplanes);

    // Turn strings into target sets, one target set for each rack
    //int counter = 0;
    vector<string> targetSetHandles;
    string charHandle = "EnvMonNC00";
    MCServerMessageSpec::MakeTargetSetRequest   mcMakeRequest(charHandle,"EnvMonNC",  true);
    MCServerMessageSpec::MakeTargetSetReply     mcMakeReply;

    unsigned mpindex;
    for( mpindex = 0; mpindex < midplaneLocs.size(); mpindex++ ) {
        // Add rack to request
        mcMakeRequest._expression.push_back(midplaneLocs[mpindex].substr(0,3).append("-M.-N..$"));

        // Handle is EnvMonNCxx where xx is rack row,column
        charHandle[8] = midplaneLocs[mpindex][1];
        charHandle[9] = midplaneLocs[mpindex][2];
        mcMakeRequest._set = charHandle;
        server->makeTargetSet(mcMakeRequest, mcMakeReply);
        if( mcMakeReply._rc == BGERR_MCSERVER_OK )
            targetSetHandles.push_back( charHandle );
        // start over for next rack
        mcMakeRequest._expression.clear();
    }

    // set up for inserting performance counter info
    bgq::utility::performance::Mode::Value::Type mode = bgq::utility::performance::Mode::instance().getMode();

    BGQDB::DBTComponentperf perf;
    std::bitset<30> smap;
    smap.set();
    smap.reset(perf.ENTRYDATE);
    smap.reset(perf.DETAIL);
    smap.reset(perf.QUALIFIER);
    SQLRETURN rc;
    perf._columns = smap.to_ulong();
    strcpy(perf._id,"EnvMon");
    strcpy(perf._component,"mmcs");
    strcpy(perf._function,"EnvMon");
    strcpy(perf._subfunction,"node card environmentals");
    boost::posix_time::ptime  start;
    boost::posix_time::time_duration duration;

    // loop until someone stops us
    while (isThreadStopping() == false) {
        if (pollEnvs) {  // check if we're polling at all
            try {
                // Loop over target set handles
                vector<string>::iterator handle;
                if(HardwareBlockList::list_size() == 0) {

                    start = boost::posix_time::microsec_clock::local_time();

                    for( handle = targetSetHandles.begin(); handle != targetSetHandles.end(); ) {
                        // open target set in monitor mode
                        MCServerMessageSpec::OpenTargetRequest   mcOpenRequest( *handle, "EnvMonNC", MCServerMessageSpec::RAAW  , true);
                        MCServerMessageSpec::OpenTargetReply     mcOpenReply;
                        server->openTarget(mcOpenRequest, mcOpenReply);

                        if (mcOpenReply._rc == BGERR_MCSERVER_TARGETSET_ACCESS_DENIED ) {
                            LOG_INFO_MSG(mcOpenReply._rt);
                            handle++;
                            continue; // must be locked for service action; move on to the next one
                        }

                        if (mcOpenReply._rc != 0) {
                            LOG_INFO_MSG("unable to open target set in mcServer: " << mcOpenReply._rt);
                            if( targetSetHandles.size() == 1 )
                                return NULL; // If there's no node cards to monitor, just exit

                            vector<string>::iterator next = handle;
                            next++;
                            targetSetHandles.erase( handle ); // something wrong with this target set, maybe missing a rack
                            handle = next;
                            continue; // move on to the next one
                        }

                        // read values from cards
                        MCServerMessageSpec::ReadNodeCardEnvRequest mcNCRequest;
                        MCServerAPIHelpers::copyTargetRequest( mcOpenRequest, mcNCRequest );
                        MCServerMessageSpec::ReadNodeCardEnvReply   mcNCReply;
                        server->readNodeCardEnv(mcNCRequest, mcNCReply);

                        // close target set, we have our data
                        MCServerMessageSpec::CloseTargetRequest mcCloseRequest = MCServerAPIHelpers::createCloseRequest( mcOpenRequest, mcOpenReply);
                        MCServerMessageSpec::CloseTargetReply   mcCloseReply;
                        server->closeTarget(mcCloseRequest, mcCloseReply);

                        processNC(&mcNCReply,&tx);

                        handle++;
                    } // end loop over target set handles

                    if (mode != bgq::utility::performance::Mode::Value::None) {
                        duration = boost::posix_time::microsec_clock::local_time() - start;
                        perf._duration = static_cast<double>(duration.total_microseconds()) / 1000000;
                        rc = tx.insert(&perf);
                    }

                } else {
                    LOG_INFO_MSG("Subnet failover in progress.  Node card environmental polling suspended.");
                }
            } catch (std::exception& e) {
                LOG_ERROR_MSG(e.what());
            }

        } // end check pollEnvs

        //sleep for <seconds>, but wake up every <pollingPeriod> to check if we've been killed
        for (unsigned slept = 0 ; (isThreadStopping() == false) && (slept < seconds) ; slept+= pollingPeriod)  {
            sleep(pollingPeriod);
            // check to see if the interval has changed
            unsigned int pollProperty = strtol(MMCSProperties::getProperty(MMCS_ENVS_NC_INTERVAL_SECONDS).c_str(), NULL, 10);

            if (MMCSProperties::getProperty("bypass_envs") == "true")
                pollProperty = 0;
            
            if ((pollProperty >= 60  && pollProperty <= 3600 && (pollProperty != seconds || pollEnvs == false))  ||
                (pollProperty == 0 && pollEnvs == true)) {
                if (pollProperty > 0) {
                    seconds = pollProperty; // only honor valid intervals, 1 - 60 minutes
                    pollEnvs = true;
                    LOG_INFO_MSG( "thread for Node Cards: interval changed to " << seconds << " seconds" );

                } else {           // don't poll if its 0
                    pollEnvs = false;
                    seconds = ENVS_POLLING_PERIOD;
                    LOG_INFO_MSG( "thread for Node Cards: not polling" );
                }
            }
        }

        // just in case the performance mode was changed
        if ( mode != bgq::utility::performance::Mode::instance().getMode() ) {
            mode = bgq::utility::performance::Mode::instance().getMode();
        }
    }  // while  (isThreadStopping() == false)

    return NULL;
}


//******************************************************************************
// Thread start function for the IOCardThread
//******************************************************************************
void*
IOCardThread::threadStart()
{
    unsigned pollingPeriod = NORMAL_POLLING_PERIOD;

    MCServerRef* server;

    LOG_INFO_MSG("thread started for IO Cards"<< ", interval is " << seconds << " seconds" << ", polling is " << (pollEnvs? "ON":"OFF"));

    MMCSCommandReply rep;
    BlockControllerBase::mcserver_connect(server, "EnvMonIO", rep);
    if(!server) {
        LOG_ERROR_MSG("IoCardThread failed to connect to mc_server");
        return NULL;
    }

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection())
    {
        LOG_INFO_MSG("unable to connect to database");
        return NULL;
    }

    // make the target set
    MCServerMessageSpec::MakeTargetSetRequest   mcMakeRequest( "EnvMonIO", "EnvMonIO", true);
    MCServerMessageSpec::MakeTargetSetReply     mcMakeReply;

    // Get list of IO drawers
    SQLHANDLE ioDrawers;
    SQLLEN index;
    SQLRETURN sqlrc, result;
    char ioDrawerLoc[7];
    string sqlstr;
    sqlstr.clear();
    sqlstr.append("select LOCATION from BGQIODrawer where status <> 'M' ");
    result = tx.execQuery( sqlstr.c_str(), &ioDrawers );
    SQLBindCol(ioDrawers, 1, SQL_C_CHAR, ioDrawerLoc, 7, &index);
    sqlrc = SQLFetch(ioDrawers);
    while ( sqlrc == SQL_SUCCESS )   {
        mcMakeRequest._expression.push_back(ioDrawerLoc);
        sqlrc = SQLFetch(ioDrawers);
    }
    SQLCloseCursor(ioDrawers);


    server->makeTargetSet(mcMakeRequest, mcMakeReply);

    // set up for inserting performance counter info
    bgq::utility::performance::Mode::Value::Type mode = bgq::utility::performance::Mode::instance().getMode();

    BGQDB::DBTComponentperf perf;
    std::bitset<30> smap;
    smap.set();
    smap.reset(perf.ENTRYDATE);
    smap.reset(perf.DETAIL);
    smap.reset(perf.QUALIFIER);
    SQLRETURN rc;
    perf._columns = smap.to_ulong();
    strcpy(perf._id,"EnvMon");
    strcpy(perf._component,"mmcs");
    strcpy(perf._function,"EnvMon");
    strcpy(perf._subfunction,"IO card environmentals");
    boost::posix_time::ptime  start;
    boost::posix_time::time_duration duration;

    // loop until someone stops us
    while (isThreadStopping() == false) {
        if (pollEnvs) {  // check if we're polling at all
            try {
                if(HardwareBlockList::list_size() == 0) {

                    start = boost::posix_time::microsec_clock::local_time();

                    // open target set in monitor mode
                    MCServerMessageSpec::OpenTargetRequest   mcOpenRequest("EnvMonIO", "EnvMonIO", MCServerMessageSpec::RAAW  , true);
                    MCServerMessageSpec::OpenTargetReply     mcOpenReply;
                    server->openTarget(mcOpenRequest, mcOpenReply);

                    if (mcOpenReply._rc != 0) {
                        LOG_INFO_MSG("unable to open target set in mcServer: " << mcOpenReply._rt);
                    } else {
                        // process env data
                        MCServerMessageSpec::ReadIoCardEnvRequest mcIORequest;
                        MCServerAPIHelpers::copyTargetRequest ( mcOpenRequest, mcIORequest );
                        MCServerMessageSpec::ReadIoCardEnvReply mcIOReply;
                        server->readIoCardEnv(mcIORequest, mcIOReply);

                        // close the target set, we have our data
                        MCServerMessageSpec::CloseTargetRequest mcCloseRequest = MCServerAPIHelpers::createCloseRequest( mcOpenRequest, mcOpenReply );
                        MCServerMessageSpec::CloseTargetReply   mcCloseReply;
                        server->closeTarget(mcCloseRequest, mcCloseReply);

                        processIO(&mcIOReply,&tx);

                        if (mode != bgq::utility::performance::Mode::Value::None) {
                            duration = boost::posix_time::microsec_clock::local_time() - start;
                            perf._duration = static_cast<double>(duration.total_microseconds()) / 1000000;
                            rc = tx.insert(&perf);
                        }

                    } // else of unable to open target set
                } else {
                    LOG_INFO_MSG("Subnet failover in progress.  IO card environmental polling suspended.");
                }
            } catch(std::exception& e) {
                LOG_ERROR_MSG(e.what());
            }

        } // end check pollEnvs


        //sleep for <seconds>, but wake up every <pollingPeriod> to check if we've been killed
        for (unsigned slept = 0 ; (isThreadStopping() == false) && (slept < seconds) ; slept+= pollingPeriod) {
            sleep(pollingPeriod);
            // check to see if the interval has changed
            unsigned int pollProperty = strtol(MMCSProperties::getProperty(MMCS_ENVS_IO_INTERVAL_SECONDS).c_str(), NULL, 10);

            if (MMCSProperties::getProperty("bypass_envs") == "true")
                pollProperty = 0;
            
            if ((pollProperty >= 60  && pollProperty <= 3600 && (pollProperty != seconds || pollEnvs == false))  ||
                (pollProperty == 0 && pollEnvs == true)) {
                if (pollProperty > 0) {
                    seconds = pollProperty; // only honor valid intervals, 1 - 60 minutes
                    pollEnvs = true;
                    LOG_INFO_MSG( "thread for IO Cards: interval changed to " << seconds << " seconds" );

                } else {           // don't poll if its 0
                    pollEnvs = false;
                    seconds = ENVS_POLLING_PERIOD;
                    LOG_INFO_MSG( "thread for IO Cards: not polling" );
                }
            }
        }

        // just in case the performance mode was changed
        if ( mode != bgq::utility::performance::Mode::instance().getMode() ) {
            mode = bgq::utility::performance::Mode::instance().getMode();
        }
    }  // while  (isThreadStopping() == false)

    return NULL;
}


//******************************************************************************
// Thread start function for the ServiceCardThread
//******************************************************************************
void*
ServiceCardThread::threadStart()
{
    unsigned pollingPeriod = NORMAL_POLLING_PERIOD;

    MCServerRef* server;

    LOG_INFO_MSG("thread started for Service Cards"<< ", interval is " << seconds << " seconds" << ", polling is " << (pollEnvs? "ON":"OFF"));

    MMCSCommandReply rep;
    BlockControllerBase::mcserver_connect(server, "EnvMonSC", rep);
    if(!server) {
        LOG_ERROR_MSG("ServiceCardThread failed to connect to mc_server");
        return NULL;
    }

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection())
    {
        LOG_INFO_MSG("unable to connect to database");
        return NULL;
    }

    // make the target set
    MCServerMessageSpec::MakeTargetSetRequest   mcMakeRequest("EnvMonSC","EnvMonSC",  true);
    MCServerMessageSpec::MakeTargetSetReply     mcMakeReply;
    mcMakeRequest._expression.push_back("R..");
    server->makeTargetSet(mcMakeRequest, mcMakeReply);

    // set up for inserting performance counter info
    bgq::utility::performance::Mode::Value::Type mode = bgq::utility::performance::Mode::instance().getMode();

    BGQDB::DBTComponentperf perf;
    std::bitset<30> smap;
    smap.set();
    smap.reset(perf.ENTRYDATE);
    smap.reset(perf.DETAIL);
    smap.reset(perf.QUALIFIER);
    SQLRETURN rc;
    perf._columns = smap.to_ulong();
    strcpy(perf._id,"EnvMon");
    strcpy(perf._component,"mmcs");
    strcpy(perf._function,"EnvMon");
    strcpy(perf._subfunction,"service card environmentals");
    boost::posix_time::ptime  start;
    boost::posix_time::time_duration duration;

    // loop until someone stops us
    while (isThreadStopping() == false)    {

        if (pollEnvs) {  // check if we're polling at all
            try {
                if(HardwareBlockList::list_size() == 0) {

                    start = boost::posix_time::microsec_clock::local_time();

                    // open target set in monitor mode
                    MCServerMessageSpec::OpenTargetRequest   mcOpenRequest( "EnvMonSC","EnvMonSC", MCServerMessageSpec::RAAW  , true);
                    MCServerMessageSpec::OpenTargetReply     mcOpenReply;
                    server->openTarget(mcOpenRequest, mcOpenReply);

                    if (mcOpenReply._rc != 0) {
                        LOG_INFO_MSG("unable to open target set in mcServer: " << mcOpenReply._rt);
                    } else {
                        // read values from cards
                        MCServerMessageSpec::ReadServiceCardEnvRequest mcSCRequest;
                        MCServerAPIHelpers::copyTargetRequest ( mcOpenRequest, mcSCRequest );
                        MCServerMessageSpec::ReadServiceCardEnvReply mcSCReply;
                        server->readServiceCardEnv(mcSCRequest, mcSCReply);

                        // close target set, we have our data
                        MCServerMessageSpec::CloseTargetRequest mcCloseRequest = MCServerAPIHelpers::createCloseRequest( mcOpenRequest, mcOpenReply );
                        MCServerMessageSpec::CloseTargetReply   mcCloseReply;
                        server->closeTarget(mcCloseRequest, mcCloseReply);

                        processSC(&mcSCReply,&tx);

                        if (mode != bgq::utility::performance::Mode::Value::None) {
                            duration = boost::posix_time::microsec_clock::local_time() - start;
                            perf._duration = static_cast<double>(duration.total_microseconds()) / 1000000;
                            rc = tx.insert(&perf);
                        }

                    }
                } else {
                    LOG_INFO_MSG("Subnet failover in progress.  Service card environmental polling suspended.");
                }
            } catch(std::exception& e) {
                LOG_ERROR_MSG(e.what());
            }
        }  // end check pollEnvs

        //sleep for <seconds>, but wake up every <pollingPeriod> to check if we've been killed
        for (unsigned slept = 0 ; (isThreadStopping() == false) && (slept < seconds) ; slept+= pollingPeriod) {
            sleep(pollingPeriod);
            // check to see if the interval has changed
            unsigned int pollProperty = strtol(MMCSProperties::getProperty(MMCS_ENVS_SC_INTERVAL_SECONDS).c_str(), NULL, 10);
            
            if (MMCSProperties::getProperty("bypass_envs") == "true")
                pollProperty = 0;
            
            if ((pollProperty >= 60  && pollProperty <= 3600 && (pollProperty != seconds || pollEnvs == false))  ||
                (pollProperty == 0 && pollEnvs == true)) {
                if (pollProperty > 0) {
                    seconds = pollProperty; // only honor valid intervals, 1 - 60 minutes
                    pollEnvs = true;
                    LOG_INFO_MSG( "thread for Service Cards: interval changed to " << seconds << " seconds" );
                } else {           // don't poll if its 0
                    pollEnvs = false;
                    seconds = ENVS_POLLING_PERIOD;
                    LOG_INFO_MSG( "thread for Service Cards: not polling" );
                }
            }
        }

        // just in case the performance mode was changed
        if ( mode != bgq::utility::performance::Mode::instance().getMode() ) {
            mode = bgq::utility::performance::Mode::instance().getMode();
        }

    } //  while (isThreadStopping() == false)

    return NULL;
}


//******************************************************************************
// Thread start function for the PerfDataThread
//******************************************************************************
void*
PerfDataThread::threadStart()
{
    unsigned pollingPeriod = NORMAL_POLLING_PERIOD;

    MCServerRef* server;

    LOG_INFO_MSG("thread started for Performance Data"<< ", interval is " << seconds << " seconds" << ", polling is " << (pollEnvs? "ON":"OFF"));

    MMCSCommandReply rep;
    BlockControllerBase::mcserver_connect(server, "EnvMonPerf", rep);
    if(!server) {
        LOG_ERROR_MSG("PerfDataThread failed to connect to mc_server");
        return NULL;
    }

    bgq::utility::performance::Mode::Value::Type mode = bgq::utility::performance::Mode::instance().getMode();
    LOG_INFO_MSG("thread for Performance Data, mode is " << bgq::utility::performance::Mode::toString(mode) << ".");

    BGQDB::DBTComponentperf perf;
    std::bitset<30> smap;
    SQLRETURN rc;
    double tStamp;

    smap.set();
    // smap.reset(perf.ENTRYDATE);      We're going to provide the entry date

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection())
    {
        LOG_INFO_MSG("unable to connect to database");
        return NULL;
    }

    // loop until someone stops us
    while (isThreadStopping() == false) {

        if (pollEnvs && mode != bgq::utility::performance::Mode::Value::None) {  // check if we're polling at all
            if(HardwareBlockList::list_size() == 0) { // Don't do perf monitoring if we're in a failover.
                try {
                    // read values from cards
                    MCServerMessageSpec::ReadPerfStatsRequest perfRequest;
                    MCServerMessageSpec::ReadPerfStatsReply   perfReply;
                    server->readPerfStats(perfRequest, perfReply);

                    for(unsigned ss = 0 ; ss < perfReply._statSet.size() ; ss++) {
                        for(unsigned dp = 0 ; dp < perfReply._statSet[ss]._dataPoints.size() ; dp++) {

                            strncpy(perf._id,perfReply._statSet[ss]._dataPoints[dp]._id.c_str(), sizeof(perf._id));
                            perf._id[sizeof(perf._id) - 1] = '\0';
                            strncpy(perf._component,perfReply._statSet[ss]._dataPoints[dp]._component.c_str(), sizeof(perf._component));
                            perf._component[sizeof(perf._component) - 1] = '\0';
                            strncpy(perf._function,perfReply._statSet[ss]._dataPoints[dp]._function.c_str(), sizeof(perf._function));
                            perf._function[sizeof(perf._function) - 1] = '\0';
                            strncpy(perf._subfunction,perfReply._statSet[ss]._dataPoints[dp]._subfunction.c_str(), sizeof(perf._subfunction));
                            perf._subfunction[sizeof(perf._subfunction) - 1] = '\0';

                            // convert from microseconds to seconds
                            perf._duration = static_cast<double>(perfReply._statSet[ss]._dataPoints[dp]._duration) / 1000000;

                            tStamp = perfReply._statSet[ss]._dataPoints[dp]._timestamp;
                            Time tm = new Time(tStamp);
                            strcpy(perf._entrydate,tm.asDB2timestamp().c_str());  // convert from epoch seconds to db2 timestamp
                            strncpy(perf._detail,perfReply._statSet[ss]._dataPoints[dp]._otherData.c_str(), sizeof(perf._detail));
                            perf._detail[sizeof(perf._detail) - 1] = '\0';
                            strncpy(perf._qualifier,perfReply._statSet[ss]._dataPoints[dp]._qualifier.c_str(), sizeof(perf._qualifier));
                            perf._qualifier[sizeof(perf._qualifier) - 1] = '\0';
                            strncpy(perf._mode,perfReply._statSet[ss]._dataPoints[dp]._mode.c_str(), sizeof(perf._mode));
                            perf._mode[sizeof(perf._mode) - 1] = '\0';

                            perf._columns = smap.to_ulong();
                            rc = tx.insert(&perf);
                            if(rc != SQL_SUCCESS) {
                                LOG_INFO_MSG("Error writing performance data, return code " << rc);
                            }
                        }
                    }
                } catch(std::exception& e) {
                    LOG_ERROR_MSG(e.what());
                }
            }
        }  // end check pollEnvs

        //sleep for <seconds>, but wake up every <pollingPeriod> to check if we've been killed
        for (unsigned slept = 0 ; (isThreadStopping() == false) && (slept < seconds) ; slept+= pollingPeriod)
            {
                sleep(pollingPeriod);
                // check to see if the interval has changed
                unsigned int pollProperty = strtol(MMCSProperties::getProperty(MMCS_PERFDATA_INTERVAL_SECONDS).c_str(), NULL, 10);
                if ((pollProperty >= 60  && pollProperty <= 1800 && (pollProperty != seconds || pollEnvs == false))  ||
                    (pollProperty == 0 && pollEnvs == true)) {
                    if (pollProperty > 0) {
                        seconds = pollProperty; // only honor valid intervals, 1 - 30 minutes
                        pollEnvs = true;
                        LOG_INFO_MSG( "thread for Performance Data: interval changed to " << seconds << " seconds" );
                    } else {           // don't poll if its 0
                        pollEnvs = false;
                        seconds = ENVS_POLLING_PERIOD;
                        LOG_INFO_MSG( "thread for Performance Data: not polling" );
                    }
                }
                // check if performance statistics collection mode has changed
                if ( mode != bgq::utility::performance::Mode::instance().getMode() ) {
                    mode = bgq::utility::performance::Mode::instance().getMode();
                    LOG_INFO_MSG("thread for Performance Data, mode is " << bgq::utility::performance::Mode::toString(mode) << ".");
                }
            }

    } //  while (isThreadStopping() == false)

    return NULL;
}


//******************************************************************************
// Thread start function for the HealthCheckThread
//******************************************************************************
void*
HealthCheckThread::threadStart()
{
    unsigned pollingPeriod = NORMAL_POLLING_PERIOD;

    MCServerRef* server;

    // the following interval specification for health check will not be documented to customers, since they must run
    // a 5 minute (300 second) interval. This hidden option was added because early hardware has been generating too many
    // RAS events to deal with.
    unsigned int pollProperty = strtol(MMCSProperties::getProperty("health_check_seconds").c_str(), NULL, 10);
    if (pollProperty > 0)
        seconds = pollProperty;

    LOG_INFO_MSG("thread started for Health Check, interval is " << seconds << " seconds");

    MMCSCommandReply rep;
    BlockControllerBase::mcserver_connect(server, "EnvMonHC", rep);
    if(!server) {
        LOG_ERROR_MSG("HealthCheckThread failed to connect to mc_server");
        return NULL;
    }

    BGQDB::DBTServicecardtemp sct;
    SQLRETURN rc;

    std::bitset<30> smap;
    smap.set();
    smap.reset(sct.TIME);
    sct._columns = smap.to_ulong();

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_INFO_MSG("unable to connect to database");
            return NULL;
    }

    // Get list of midplanes from tbgqmidplane
    SQLHANDLE hMidplanes;
    SQLLEN index;
    SQLRETURN sqlrc, result;
    char mp[7];
    string sqlstr;

    sqlstr.clear();
    sqlstr.append("select LOCATION from BGQMidplane where location like 'R%-M0'");

    result = tx.execQuery( sqlstr.c_str(), &hMidplanes );

    SQLBindCol(hMidplanes, 1, SQL_C_CHAR, mp, 7, &index);

    // Turn SQL reply into vector of strings
    vector<string> midplaneLocs;
    sqlrc = SQLFetch(hMidplanes);
    while ( sqlrc == SQL_SUCCESS )   {
        midplaneLocs.push_back( mp );
        //LOG_DEBUG_MSG("MidplaneLoc = " << mp);
        sqlrc = SQLFetch(hMidplanes);
    }

    SQLCloseCursor(hMidplanes);


    // make the target set for service cards
    MCServerMessageSpec::MakeTargetSetRequest   scMakeRequest("EnvMonSCHealth","EnvMonHC",  true);
    MCServerMessageSpec::MakeTargetSetReply     scMakeReply;
    scMakeRequest._expression.push_back("R..");
    server->makeTargetSet(scMakeRequest, scMakeReply);

    // make the target set for bulk power
    MCServerMessageSpec::MakeTargetSetRequest   bpMakeRequest("EnvMonBulkHealth","EnvMonHC",  true);
    MCServerMessageSpec::MakeTargetSetReply     bpMakeReply;
    bpMakeRequest._expression.push_back("R..-M.-S$");
    server->makeTargetSet(bpMakeRequest, bpMakeReply);

    // make the target set for coolant monitor
    MCServerMessageSpec::MakeTargetSetRequest   cmMakeRequest("EnvMonCMHealth","EnvMonHC",  true);
    MCServerMessageSpec::MakeTargetSetReply     cmMakeReply;
    cmMakeRequest._expression.push_back("R..-L$");
    server->makeTargetSet(cmMakeRequest, cmMakeReply);

    // Get list of IO drawers in IO racks
    SQLHANDLE ioDrawers;
    char ioDrawerLoc[7];

    sqlstr.clear();
    sqlstr.append("select LOCATION from BGQIODrawer where status <> 'M' and location like 'Q%'  ");
    result = tx.execQuery( sqlstr.c_str(), &ioDrawers );
    SQLBindCol(ioDrawers, 1, SQL_C_CHAR, ioDrawerLoc, 7, &index);
    sqlrc = SQLFetch(ioDrawers);
    while ( sqlrc == SQL_SUCCESS )   {
        bpMakeRequest._expression.push_back(ioDrawerLoc);
        sqlrc = SQLFetch(ioDrawers);
    }
    SQLCloseCursor(ioDrawers);

     // make the target set for IO boards
    MCServerMessageSpec::MakeTargetSetRequest   ioMakeRequest( "EnvMonIOHealth", "EnvMonHC", true);
    MCServerMessageSpec::MakeTargetSetReply     ioMakeReply;

    //  Get either compute rack or IO rack locations
    sqlstr.clear();
    sqlstr.append("select LOCATION from BGQIODrawer where status <> 'M'  ");
    result = tx.execQuery( sqlstr.c_str(), &ioDrawers );
    SQLBindCol(ioDrawers, 1, SQL_C_CHAR, ioDrawerLoc, 7, &index);
    sqlrc = SQLFetch(ioDrawers);
    while ( sqlrc == SQL_SUCCESS )   {
        ioMakeRequest._expression.push_back(ioDrawerLoc);
        sqlrc = SQLFetch(ioDrawers);
    }
    SQLCloseCursor(ioDrawers);

    server->makeTargetSet(ioMakeRequest, ioMakeReply);

    // make the target sets for node boards, one target set for each rack
    vector<string> targetSetHandles;
    vector<string>::iterator handle;
    string charHandle = "EnvMonNC00Health";
    MCServerMessageSpec::MakeTargetSetRequest   ncMakeRequest(charHandle,"EnvMonHC",  true);
    MCServerMessageSpec::MakeTargetSetReply     ncMakeReply;
    unsigned mpindex;
    for( mpindex = 0; mpindex < midplaneLocs.size(); mpindex++ ) {

        // Add rack to request
        ncMakeRequest._expression.push_back(midplaneLocs[mpindex].substr(0,3).append("-M.-N..$"));

        // Handle is EnvMonNCxx where xx is rack row,column
        charHandle[8] = midplaneLocs[mpindex][1];
        charHandle[9] = midplaneLocs[mpindex][2];
        ncMakeRequest._set = charHandle;
        server->makeTargetSet(ncMakeRequest, ncMakeReply);
        if( ncMakeReply._rc == BGERR_MCSERVER_OK )
            targetSetHandles.push_back( charHandle );
        // start over for next rack
        ncMakeRequest._expression.clear();
    }


    // The following code is just in case we ever want to override the thresholds, we can
    // do it via undocumented keys in the properties file, rather than an efix
    unsigned int MINFREQ = 1520;
    if (!MMCSProperties::getProperty("MINFREQ").empty() ) {
        MINFREQ = strtol(MMCSProperties::getProperty("MINFREQ").c_str(), NULL, 10);
    }
    unsigned int MAXFREQ = 1680;
    if (!MMCSProperties::getProperty("MAXFREQ").empty() ) {
        MAXFREQ = strtol(MMCSProperties::getProperty("MAXFREQ").c_str(), NULL, 10);
    }
    unsigned int MAXTEMP = 50;
    if (!MMCSProperties::getProperty("MAXTEMP").empty() ) {
        MAXTEMP = strtol(MMCSProperties::getProperty("MAXTEMP").c_str(), NULL, 10);
    }
    double MINRPM = 0;
    if (!MMCSProperties::getProperty("MINRPM").empty() ) {
        MINRPM = strtod(MMCSProperties::getProperty("MINRPM").c_str(), NULL);
    }
    double MAXRPM = 25000;
    if (!MMCSProperties::getProperty("MAXRPM").empty() ) {
        MAXRPM = strtod(MMCSProperties::getProperty("MAXRPM").c_str(), NULL);
    }
    double MIN5VVOLTAGE = 4.0;
    if (!MMCSProperties::getProperty("MIN5VVOLTAGE").empty() ) {
        MIN5VVOLTAGE = strtod(MMCSProperties::getProperty("MIN5VVOLTAGE").c_str(), NULL);
    }
    double MAX5VVOLTAGE = 5.6;
    if (!MMCSProperties::getProperty("MAX5VVOLTAGE").empty() ) {
        MAX5VVOLTAGE = strtod(MMCSProperties::getProperty("MAX5VVOLTAGE").c_str(), NULL);
    }
    double MIN5VCURRENT = 0.0;
    if (!MMCSProperties::getProperty("MIN5VCURRENT").empty() ) {
        MIN5VCURRENT = strtod(MMCSProperties::getProperty("MIN5VCURRENT").c_str(), NULL);
    }
    double MAX5VCURRENT = 2.2;
    if (!MMCSProperties::getProperty("MAX5VCURRENT").empty() ) {
        MAX5VCURRENT = strtod(MMCSProperties::getProperty("MAX5VCURRENT").c_str(), NULL);
    }
    double MAXBPMTEMP = 80;
    if (!MMCSProperties::getProperty("MAXBPMTEMP").empty() ) {
        MAXBPMTEMP = strtod(MMCSProperties::getProperty("MAXBPMTEMP").c_str(), NULL);
    }
    double BPMTEMPRANGE = 10;
    if (!MMCSProperties::getProperty("BPMTEMPRANGE").empty() ) {
        BPMTEMPRANGE = strtod(MMCSProperties::getProperty("BPMTEMPRANGE").c_str(), NULL);
    }

    // these thresholds are in mV for voltage and mA for current - hence the large numbers

    double MINDCAVOLTAGE_N[] = {0.0, 700,   1200,  2250, 3300, 0.0,   1450, 700,  900,  11400};
    double MAXDCAVOLTAGE_N[] = {0.0, 1100,  1500,  2750, 3600, 0.0,   1550, 1200, 1100, 14400};
    //double MAXDCAVOLTAGE_N[] = {0.0, 1100,  1600,  2750, 3600, 0.0,   1550, 1200, 1100, 14400};  DD1 values
    double MINDCAVOLTAGE_I[] = {0.0, 700,   1200,  2250, 3300, 11000, 1450, 700,  900,  11000};
    double MAXDCAVOLTAGE_I[] = {0.0, 1100,  1500,  2750, 3600, 13000, 1550, 1200, 1100, 13000};
    //double MAXDCAVOLTAGE_I[] = {0.0, 1100,  1600,  2750, 3600, 13000, 1550, 1200, 1100, 13000};  DD1 values
    double MAXDCACURRENT_N[] = {0.0, 57280, 26080, 2115, 2203, 0.0,   4771, 4771, 1740, 0.0};
    double MAXDCACURRENT_I[] = {0.0, 13480, 6490,  0.0,  0.0,  6370,  0.0,  0.0,  0.0,  0.0};


    // set up for inserting performance counter info
    bgq::utility::performance::Mode::Value::Type mode = bgq::utility::performance::Mode::instance().getMode();

    BGQDB::DBTComponentperf perf;
    std::bitset<30> pmap;
    pmap.set();
    pmap.reset(perf.ENTRYDATE);
    pmap.reset(perf.DETAIL);
    pmap.reset(perf.QUALIFIER);

    perf._columns = pmap.to_ulong();
    strcpy(perf._id,"EnvMon");
    strcpy(perf._component,"mmcs");
    strcpy(perf._function,"EnvMon");
    strcpy(perf._subfunction,"full system health check");
    boost::posix_time::ptime  start;
    boost::posix_time::time_duration duration;

    // loop until someone stops us
    while (isThreadStopping() == false)         {

        // start the clock
        start = boost::posix_time::microsec_clock::local_time();

        // *********  HANDLE SERVICE CARDS   ***************

        // open target set for service cards
        MCServerMessageSpec::OpenTargetRequest   scOpenRequest( "EnvMonSCHealth","EnvMonHC", MCServerMessageSpec::RAAW  , true);
        MCServerMessageSpec::OpenTargetReply     scOpenReply;
        server->openTarget(scOpenRequest, scOpenReply);

        if (scOpenReply._rc != 0) {
            LOG_INFO_MSG("unable to open target set in mcServer, for service card health check: " << scOpenReply._rt);
        } else {
            // read values from cards
            MCServerMessageSpec::ReadServiceCardEnvRequest mcSCRequest;
            MCServerAPIHelpers::copyTargetRequest ( scOpenRequest, mcSCRequest );
            MCServerMessageSpec::ReadServiceCardEnvReply mcSCReply;
            mcSCRequest._shortForm = true;
            server->readServiceCardEnv(mcSCRequest, mcSCReply);

            // close target set, we have our data
            MCServerMessageSpec::CloseTargetRequest scCloseRequest = MCServerAPIHelpers::createCloseRequest( scOpenRequest, scOpenReply );
            MCServerMessageSpec::CloseTargetReply   scCloseReply;
            server->closeTarget(scCloseRequest, scCloseReply);

            for(unsigned sc = 0 ; sc < mcSCReply._serviceCardsShort.size() ; sc++) {
                if(isThreadStopping() == true) return 0;
                if (mcSCReply._serviceCardsShort[sc]._error == CARD_NOT_PRESENT) { } else  // do nothing if the card is not present
                if (mcSCReply._serviceCardsShort[sc]._error == CARD_NOT_UP) { } else  // do nothing if the card is not up
                if (mcSCReply._serviceCardsShort[sc]._error != 0) {
                    LOG_INFO_MSG("Error occurred reading environmentals from: " << mcSCReply._serviceCardsShort[sc]._location);
                    RasEventImpl noContact(0x00061002);
                    noContact.setDetail(RasEvent::LOCATION, mcSCReply._serviceCardsShort[sc]._location.c_str());
                    RasEventHandlerChain::handle(noContact);
                    BGQDB::putRAS(noContact);
                } else {
                    strcpy(sct._location,mcSCReply._serviceCardsShort[sc]._location.c_str());
                    sct._onboardtemp = mcSCReply._serviceCardsShort[sc]._onboardTemp;

                    // Note:  onboard temps for service cards are the only inserted value anywhere in the health check
                    // All the rest of the health check logic is for checking against expected values and issuing RAS events
                    rc = tx.insert(&sct);
                    if(rc != SQL_SUCCESS) {
                        LOG_INFO_MSG("Error writing service card temps: location " << string(sct._location)  << " return code " << rc);
                    }

                    if (mcSCReply._serviceCardsShort[sc]._leakDetectorFaultBpe0 ||
                        mcSCReply._serviceCardsShort[sc]._leakDetectorFaultBpe1) {
                        // send RAS
                        RasEventImpl ras(0x00061006);
                        ras.setDetail(RasEvent::LOCATION, mcSCReply._serviceCardsShort[sc]._location.c_str());
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }
                    if (mcSCReply._serviceCardsShort[sc]._clockFreq*16 > MAXFREQ ||
                        mcSCReply._serviceCardsShort[sc]._clockFreq*16 < MINFREQ) {
                        // send RAS  (and fix the values that we're comparing against
                        RasEventImpl ras(0x00061007);
                        ras.setDetail(RasEvent::LOCATION, mcSCReply._serviceCardsShort[sc]._location.c_str());
                        ras.setDetail("FREQ", boost::lexical_cast<string>(mcSCReply._serviceCardsShort[sc]._clockFreq*16));
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }
                    if (mcSCReply._serviceCardsShort[sc]._onboardTemp > MAXTEMP) {
                        // send RAS  (and fix the value that we're comparing against
                        RasEventImpl ras(0x00061008);
                        ras.setDetail(RasEvent::LOCATION, mcSCReply._serviceCardsShort[sc]._location.c_str());
                        ras.setDetail("ACTUAL", boost::lexical_cast<string>(mcSCReply._serviceCardsShort[sc]._onboardTemp));
                        ras.setDetail("EXP", boost::lexical_cast<string>(MAXTEMP));
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }

                }
            } // for loop that goes through each service card
        }

        // *********  HANDLE IO BOARDS   ***************

        // open target set for IO boards
        MCServerMessageSpec::OpenTargetRequest   ioOpenRequest( "EnvMonIOHealth","EnvMonHC", MCServerMessageSpec::RAAW  , true);
        MCServerMessageSpec::OpenTargetReply     ioOpenReply;
        server->openTarget(ioOpenRequest, ioOpenReply);

        if (ioOpenReply._rc != 0) {
            LOG_INFO_MSG("unable to open target set in mcServer, for IO board health check: " << ioOpenReply._rt);
        } else {

            // process env data
            MCServerMessageSpec::ReadIoCardEnvRequest mcIORequest;
            MCServerAPIHelpers::copyTargetRequest ( ioOpenRequest, mcIORequest );
            MCServerMessageSpec::ReadIoCardEnvReply mcIOReply;
            mcIORequest._shortForm = true;
            server->readIoCardEnv(mcIORequest, mcIOReply);

            // close the target set, we have our data
            MCServerMessageSpec::CloseTargetRequest ioCloseRequest = MCServerAPIHelpers::createCloseRequest( ioOpenRequest, ioOpenReply );
            MCServerMessageSpec::CloseTargetReply   ioCloseReply;
            server->closeTarget(ioCloseRequest, ioCloseReply);

            for(unsigned io = 0 ; io < mcIOReply._ioCardsShort.size() ; io++) {
                if(isThreadStopping() == true) return 0;
                if (mcIOReply._ioCardsShort[io]._error == CARD_NOT_PRESENT) { } else  // do nothing if the card is not present
                if (mcIOReply._ioCardsShort[io]._error == CARD_NOT_UP) { } else       // do nothing if the card is not up
                if (mcIOReply._ioCardsShort[io]._error != 0) {
                    LOG_INFO_MSG("Error occurred reading environmentals from: " << mcIOReply._ioCardsShort[io]._lctn);
                    RasEventImpl noContact(0x00061004);
                    noContact.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn);
                    RasEventHandlerChain::handle(noContact);
                    BGQDB::putRAS(noContact);
                } else {

                    if (mcIOReply._ioCardsShort[io]._clockFreq*16 > MAXFREQ ||
                        mcIOReply._ioCardsShort[io]._clockFreq*16 < MINFREQ) {
                        // send RAS  (and fix the values that we're comparing against
                        RasEventImpl ras(0x00061009);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("FREQ", boost::lexical_cast<string>(mcIOReply._ioCardsShort[io]._clockFreq*16));
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }

                    if (mcIOReply._ioCardsShort[io]._tempCriticalBlink ||
                        mcIOReply._ioCardsShort[io]._tempWarningBlink ) {
                        // put RAS
                        RasEventImpl ras(0x0006100A);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("COMP", "TEMPERATURE");
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }

                    /* right now this is disabled because analysis by Don, Kahn, and others determined it was covered elsewhere
                       if (mcIOReply._ioCardsShort[io]._alertsComputes != 0xFF) {
                        // put RAS
                        RasEventImpl ras(0x0006100A);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("COMP", "COMPUTE ALERTS");
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }
                    */

                    /*  right now this is disabled because unbooted computes are causing RAS
                        if (mcIOReply._ioCardsShort[io]._pgoodComputes != 0xFF) {
                        // put RAS
                        RasEventImpl ras(0x0006100A);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("COMP", "COMPUTE PGOOD");
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                        }
                    */

                    if (mcIOReply._ioCardsShort[io]._alertsBlinks != 0x3F) {
                        // put RAS
                        RasEventImpl ras(0x0006100A);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("COMP", "LINK CHIP ALERTS");
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }

                     if (mcIOReply._ioCardsShort[io]._pgoodBlinks != 0x3F) {
                        // put RAS
                        RasEventImpl ras(0x0006100A);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("COMP", "LINK CHIP PGOOD");
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }

                    if (mcIOReply._ioCardsShort[io]._alertsOptics != 0xFFFFFF) {
                        // put RAS
                        RasEventImpl ras(0x0006100A);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("COMP", "OPTICS");
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }

                    if (mcIOReply._ioCardsShort[io]._statusDca != 1) {
                        // put RAS
                        RasEventImpl ras(0x0006100A);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("COMP", "DCA");
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }

                    if (mcIOReply._ioCardsShort[io]._statusVtmDomains != 0x3FF) {
                        // put RAS
                        RasEventImpl ras(0x0006100A);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("COMP", "VTM DOMAINS");
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }

                    for(unsigned iod = 0 ; iod < mcIOReply._ioCardsShort[io]._dcas.size() ; iod++) {
                        if(isThreadStopping() == true) return 0;
                        if (mcIOReply._ioCardsShort[io]._dcas[iod]._error != 0) {
                            LOG_INFO_MSG("Error occurred reading environmentals from: " <<  mcIOReply._ioCardsShort[io]._dcas[iod]._lctn);
                            // put RAS
                            RasEventImpl ras(0x0006100B);
                            ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._dcas[iod]._lctn.c_str());
                            ras.setDetail("COND", "CARD ERROR");
                            ras.setDetail("BADVAL", boost::lexical_cast<string>(mcIOReply._ioCardsShort[io]._dcas[iod]._error));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        } else {
                            for(unsigned iodp = 0 ; iodp < mcIOReply._ioCardsShort[io]._dcas[iod]._domains.size() ; iodp++) {
                                if(isThreadStopping() == true) return 0;
                                if ((mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._status != 0x90 &&
                                     mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._status != 0xB0) ||
                                    mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain < 0 ||
                                    mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain > 9) {
                                    // put RAS
                                    RasEventImpl ras(0x0006100B);
                                    ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._dcas[iod]._lctn.c_str());
                                    if (mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._status != 0x90 &&
                                        mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._status != 0xB0) {
                                        ras.setDetail("COND", string("INCORRECT STATUS, DOMAIN ") +  boost::lexical_cast<string>(mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain));
                                        ras.setDetail("BADVAL", boost::lexical_cast<string>(int64_t(mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._status)));
                                    } else {
                                        ras.setDetail("COND", "INCORRECT POWER DOMAIN VALUE");
                                        ras.setDetail("BADVAL", boost::lexical_cast<string>(int64_t(mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain)));
                                    }
                                    RasEventHandlerChain::handle(ras);
                                    BGQDB::putRAS(ras);
                                } else {
                                    if (MINDCAVOLTAGE_I[mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain] != 0.0 &&
                                        (mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._voltage < MINDCAVOLTAGE_I[mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain]  ||
                                         mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._voltage > MAXDCAVOLTAGE_I[mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain])) {
                                        // put RAS
                                        RasEventImpl ras(0x0006100B);
                                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._dcas[iod]._lctn.c_str());
                                        ras.setDetail("COND", string("INCORRECT VOLTAGE, DOMAIN ") +  boost::lexical_cast<string>(mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain));
                                        ras.setDetail("BADVAL", boost::lexical_cast<string>(mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._voltage));
                                        RasEventHandlerChain::handle(ras);
                                        BGQDB::putRAS(ras);
                                    }
                                    if (MAXDCACURRENT_I[mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain] != 0.0 &&
                                        (mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._current < 0 ||
                                         mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._current > MAXDCACURRENT_I[mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain])) {
                                        // put RAS
                                        RasEventImpl ras(0x0006100B);
                                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._dcas[iod]._lctn.c_str());
                                        ras.setDetail("COND", string("INCORRECT CURRENT, DOMAIN ") +  boost::lexical_cast<string>(mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain));
                                        ras.setDetail("BADVAL", boost::lexical_cast<string>(mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._current));
                                        RasEventHandlerChain::handle(ras);
                                        BGQDB::putRAS(ras);
                                    }
                                }

                            }

                        }
                    }

                    for(unsigned ioo = 0 ; ioo < mcIOReply._ioCardsShort[io]._optics.size() ; ioo++) {
                        if(isThreadStopping() == true) return 0;
                        if (mcIOReply._ioCardsShort[io]._optics[ioo]._error != 0) {
                            LOG_INFO_MSG("Error occurred reading environmentals from: " <<  mcIOReply._ioCardsShort[io]._optics[ioo]._lctn);
                            // put RAS
                            RasEventImpl ras(0x0006100D);
                            ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._optics[ioo]._lctn.c_str());
                            ras.setDetail("COND", "MODULE ERROR");
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        } else {
                            if ((mcIOReply._ioCardsShort[io]._optics[ioo]._status & 0x02) != 0) {  // check interrupt bit
                                // put RAS
                                RasEventImpl ras(0x0006100D);
                                ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._optics[ioo]._lctn.c_str());
                                string condition = "MODULE STATUS";
                                if ((mcIOReply._ioCardsShort[io]._optics[ioo]._los & ~mcIOReply._ioCardsShort[io]._optics[ioo]._losMask) != 0) {
                                    condition.append(", CHANNEL ");
                                    // apply mask to get channel information
                                    const int channels = mcIOReply._ioCardsShort[io]._optics[ioo]._los  & ~mcIOReply._ioCardsShort[io]._optics[ioo]._losMask;

                                    // iterate through all 11 channels looking at which ones have loss of
                                    // signal alarms, see mc.h for the format
                                    for ( unsigned channel = 0; channel < 12; ++channel ) {
                                        const uint64_t bit = 0x1 << channel;
                                        if (channels & bit) condition.append( boost::lexical_cast<std::string>(channel) + " ");
                                    }
                                    condition.append("LOSS OF SIGNAL");
                                }
                                if ((mcIOReply._ioCardsShort[io]._optics[ioo]._faults & ~mcIOReply._ioCardsShort[io]._optics[ioo]._faultsMask) != 0) {
                                    condition.append(", CHANNEL ");
                                    // apply mask to get channel information
                                    const int channels = mcIOReply._ioCardsShort[io]._optics[ioo]._faults & ~mcIOReply._ioCardsShort[io]._optics[ioo]._faultsMask;

                                    // iterate through all 11 channels looking at which ones have fault
                                    // alarms, see mc.h for the format
                                    for ( unsigned channel = 0; channel < 12; ++channel ) {
                                        const uint64_t bit = 0x1 << channel;
                                        if (channels & bit) condition.append( boost::lexical_cast<std::string>(channel) + " ");
                                    }
                                    condition.append("FAULT");
                                }
                                if ((mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsTemp & ~mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsTempMask) != 0) {
                                    condition.append(", TEMPERATURE");
                                }
                                if ((mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsVoltage & ~mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsVoltageMask) != 0) {
                                    condition.append(", VOLTAGE");
                                }
                                if ((mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsBiasCurrent & ~mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsBiasCurrentMask) != 0) {
                                    condition.append(", BIAS CURRENT ");
                                    // apply mask to get channel information
                                    const uint64_t channels = mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsBiasCurrent & ~mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsBiasCurrentMask;

                                    // iterate through all 11 channels looking at which ones have power
                                    // alarms, see mc.h for the format
                                    for ( unsigned channel = 0; channel < 12; ++channel ) {
                                        const uint64_t bit = 0xFULL << (channel * 4);
                                        if (channels & bit) condition.append( boost::lexical_cast<std::string>(channel) + " ");
                                    }
                                }
                                if ((mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsPower & ~mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsPowerMask) != 0) {
                                    condition.append(", POWER ");
                                    // apply mask to get channel information
                                    const uint64_t channels = mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsPower & ~mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsPowerMask;

                                    // iterate through all 11 channels looking at which ones have power
                                    // alarms, see mc.h for the format
                                    for ( unsigned channel = 0; channel < 12; ++channel ) {
                                        const uint64_t bit = 0xFULL << (channel * 4);
                                        if (channels & bit) condition.append( boost::lexical_cast<std::string>(channel) + " ");
                                    }
                                }
                                // remove MODULE_STATUS if something else was found
                                if (condition != "MODULE STATUS") {
                                    condition = condition.substr(15);
                                }

                                ras.setDetail("COND", condition);
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                        }
                    } // end of loop thru optics
                }
            } // end of loop thru IO cards
        }

        // *********  HANDLE BULK POWER MODULES  *****************

        // open target set for bulk power modules
        MCServerMessageSpec::OpenTargetRequest   bpOpenRequest( "EnvMonBulkHealth","EnvMonHC", MCServerMessageSpec::RAAW  , true);
        MCServerMessageSpec::OpenTargetReply     bpOpenReply;
        server->openTarget(bpOpenRequest, bpOpenReply);

        if (bpOpenReply._rc != 0) {
            LOG_INFO_MSG("unable to open target set in mcServer, for bulk power health check: " << bpOpenReply._rt);
        } else {
            // read values from cards
            MCServerMessageSpec::ReadBulkPowerEnvRequest mcBPRequest;
            MCServerAPIHelpers::copyTargetRequest ( bpOpenRequest, mcBPRequest );
            MCServerMessageSpec::ReadBulkPowerEnvReply   mcBPReply;
            server->readBulkPowerEnv(mcBPRequest, mcBPReply);
            // Note that the above call is not a "short form".
            // The bulk power modules do not have a short form, since there was no difference between the data needed


            // close target set, we have our data
            MCServerMessageSpec::CloseTargetRequest bpCloseRequest = MCServerAPIHelpers::createCloseRequest( bpOpenRequest, bpOpenReply );
            MCServerMessageSpec::CloseTargetReply   bpCloseReply;
            server->closeTarget(bpCloseRequest, bpCloseReply);

            for(unsigned bp = 0 ; bp < mcBPReply._bpms.size() ; ++bp) {
                if(isThreadStopping() == true) return 0;
                if (mcBPReply._bpms[bp]._error == CARD_NOT_PRESENT) { } else  // do nothing if the card is not present
                if (mcBPReply._bpms[bp]._error == CARD_NOT_UP) { } else       // do nothing if the card is not up
                if (mcBPReply._bpms[bp]._error != 0) {
                    LOG_INFO_MSG("Error occurred reading environmentals from: " << mcBPReply._bpms[bp]._location);
                    RasEventImpl noContact(0x00061003);
                    noContact.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._location.c_str());
                    RasEventHandlerChain::handle(noContact);
                    BGQDB::putRAS(noContact);
                } else {

                    std::map<std::string, int> failedBpms;

                    for(unsigned bpm = 0 ; bpm < mcBPReply._bpms[bp]._bpms.size() ; ++bpm) {
                        if(isThreadStopping() == true) return 0;
                        // first check for failed BPM.  This has different criteria than sending RAS

                        if  ( ((mcBPReply._bpms[bp]._bpms[bpm]._statusWord & 0x0A78) != 0) ||
                              ((mcBPReply._bpms[bp]._bpms[bpm]._statusVout & 0x0095) != 0) ||
                              ((mcBPReply._bpms[bp]._bpms[bpm]._statusIout & 0x00CA) != 0) ||
                              ((mcBPReply._bpms[bp]._bpms[bpm]._statusInput & 0x009C) != 0)||
                              ((mcBPReply._bpms[bp]._bpms[bpm]._statusTemperature & 0x0090) != 0 ) ||
                              ((mcBPReply._bpms[bp]._bpms[bpm]._status5V   & 0x009B) != 0) ||
                              ((mcBPReply._bpms[bp]._bpms[bpm]._statusFans & 0xC2) != 0) ) {
                            // the mask values are used to ignore warnings and non-critical faults
                            // see the spec from Delta, and issue 3007 for details
                            failedBpms[mcBPReply._bpms[bp]._bpms[bpm]._location.substr(0,6)]++;

                            if (failedBpms[mcBPReply._bpms[bp]._bpms[bpm]._location.substr(0,6)] > 1) {
                                // We have more than one BPM failure, time to start marking hardware in error

                                string B0cards[] = {"-M0-N00","-M0-N01","-M0-N02","-M0-N03","-M0-N04","-M0-N05","-M0-N06","-M0-N07"};
                                string B1cards[] = {"-M0-N08","-M0-N09","-M0-N10","-M0-N11","-M0-N12","-M0-N13","-M0-N14","-M0-N15"};
                                string B2cards[] = {"-M1-N00","-M1-N01","-M1-N02","-M1-N03","-M1-N04","-M1-N05","-M1-N06","-M1-N07"};
                                string B3cards[] = {"-M1-N08","-M1-N09","-M1-N10","-M1-N11","-M1-N12","-M1-N13","-M1-N14","-M1-N15"};

                                string IODrawersTH[] = {"-IE", "-ID", "-IC", "-IF"};
                                string IODrawersIOR[] = {"-I0","-I1","-I2","-I3","-I4","-I5","-I6","-I7","-I8","-I9","-IA","-IB"};

                                SQLHANDLE hIODrawer;
                                SQLLEN index;
                                SQLRETURN sqlrc, result;
                                char iod[7];
                                string sqlstr, iod_loc, nc_loc;

                                // clear the failure, so we don't get duplicate RAS events
                                failedBpms[mcBPReply._bpms[bp]._bpms[bpm]._location.substr(0,6)] = 0;

                                LOG_INFO_MSG("Multiple BPM failures from: " << mcBPReply._bpms[bp]._bpms[bpm]._location.substr(0,6) );

                                if(mcBPReply._bpms[bp]._bpms[bpm]._location.substr(0,1) == "R") {  //this is a compute rack
                                    for (int card = 0; card < 8; card++) {
                                        if (mcBPReply._bpms[bp]._bpms[bpm]._location.substr(5,1) == "0")
                                            nc_loc = mcBPReply._bpms[bp]._location.substr(0,3) + B0cards[card];
                                        else
                                            if (mcBPReply._bpms[bp]._bpms[bpm]._location.substr(5,1) == "1")
                                                nc_loc = mcBPReply._bpms[bp]._location.substr(0,3) + B1cards[card];
                                            else
                                                if (mcBPReply._bpms[bp]._bpms[bpm]._location.substr(5,1) == "2")
                                                    nc_loc = mcBPReply._bpms[bp]._location.substr(0,3) + B2cards[card];
                                                else
                                                    nc_loc = mcBPReply._bpms[bp]._location.substr(0,3) + B3cards[card];

                                        RasEventImpl bpmFails(0x00061012);
                                        bpmFails.setDetail(RasEvent::LOCATION, nc_loc);
                                        RasEventHandlerChain::handle(bpmFails);

                                        vector<BGQDB::job::Id> jobs;
                                        uint32_t recid;
                                        const int signal = bgcios::jobctl::SIGHARDWAREFAILURE;
                                        const std::string block;
                                        const uint32_t qualifier = 0;
                                        const BGQDB::job::Id job = 0;
                                        BGQDB::putRAS(bpmFails, block, job, qualifier, &jobs, &recid);

                                        if (!jobs.empty()) {
                                            // First, check to see if we ever connected to runjob_server
                                            bool connected = RunJobConnection::isConnected();
                                            
                                            for (unsigned i = 0; i < jobs.size(); ++i) {
                                                LOG_INFO_MSG("Killing job " << jobs[i] << " due to RAS event from " << nc_loc);
                                              
                                                if(!connected || RunJobConnection::Kill(jobs[i], signal, recid) != 0) {
                                                    LOG_ERROR_MSG("runjob_server was not connected and cannot be reached, job cannot be killed.");
                                                    DBBlockController::addPendingKill(jobs[i], signal);
                                                }
                                            }
                                        }
                                    }

                                    // Now determine if an IO drawer also has to be disabled

                                    if (mcBPReply._bpms[bp]._bpms[bpm]._location.substr(5,1) == "0")
                                        iod_loc = mcBPReply._bpms[bp]._location.substr(0,3) + IODrawersTH[0];
                                    else
                                        if (mcBPReply._bpms[bp]._bpms[bpm]._location.substr(5,1) == "1")
                                            iod_loc = mcBPReply._bpms[bp]._location.substr(0,3) + IODrawersTH[1];
                                        else
                                            if (mcBPReply._bpms[bp]._bpms[bpm]._location.substr(5,1) == "2")
                                                iod_loc = mcBPReply._bpms[bp]._location.substr(0,3) + IODrawersTH[2];
                                            else
                                                iod_loc = mcBPReply._bpms[bp]._location.substr(0,3) + IODrawersTH[3];

                                    sqlstr.clear();
                                    sqlstr = "select LOCATION from BGQIODrawer where status = 'A' and location = '" + iod_loc + string("'");
                                    result = tx.execQuery( sqlstr.c_str(), &hIODrawer );
                                    SQLBindCol(hMidplanes, 1, SQL_C_CHAR, iod, 7, &index);
                                    sqlrc = SQLFetch(hIODrawer);
                                    if ( sqlrc == SQL_SUCCESS )   {   //there is an IO drawer at this location
                                        RasEventImpl bpmFails(0x00061013);
                                        bpmFails.setDetail(RasEvent::LOCATION, iod_loc);
                                        RasEventHandlerChain::handle(bpmFails);
                                        BGQDB::putRAS(bpmFails);
                                    }
                                    SQLCloseCursor(hMidplanes);

                                } else {  //this is an IO rack

                                    RasEventImpl bpmFails(0x00061013);

                                    for (int io = 0; io < 12; io++) {  // loop thru 12 possible drawer locations
                                        iod_loc = mcBPReply._bpms[bp]._location.substr(0,3) + IODrawersIOR[io];
                                        sqlstr.clear();
                                        sqlstr = "select LOCATION from BGQIODrawer where status = 'A' and location = '" + iod_loc + string("'");
                                        result = tx.execQuery( sqlstr.c_str(), &hIODrawer );
                                        SQLBindCol(hMidplanes, 1, SQL_C_CHAR, iod, 7, &index);
                                        sqlrc = SQLFetch(hIODrawer);
                                        if ( sqlrc == SQL_SUCCESS )   {   //there is an IO drawer at this location
                                            RasEventImpl bpmFails(0x00061013);
                                            bpmFails.setDetail(RasEvent::LOCATION, iod_loc);
                                            RasEventHandlerChain::handle(bpmFails);
                                            BGQDB::putRAS(bpmFails);
                                        }
                                        SQLCloseCursor(hMidplanes);
                                    }
                                }
                            }
                        }

                        // now check for RAS conditions

                        if  (( mcBPReply._bpms[bp]._bpms[bpm]._statusWord != 0 ) ||
                             ( mcBPReply._bpms[bp]._bpms[bpm]._statusVout != 0 ) ||
                             ( mcBPReply._bpms[bp]._bpms[bpm]._statusIout != 0 ) ||
                             ( mcBPReply._bpms[bp]._bpms[bpm]._statusInput != 0 ) ||
                             ( mcBPReply._bpms[bp]._bpms[bpm]._statusTemperature != 0 ) ||
                             ( mcBPReply._bpms[bp]._bpms[bpm]._statusCml != 0 ) ||
                             ( mcBPReply._bpms[bp]._bpms[bpm]._status5V != 0 ) ||
                             ( (mcBPReply._bpms[bp]._bpms[bpm]._statusFans & 0xF3) != 0 )) {

                            stringstream badval;

                            // send RAS
                            RasEventImpl ras(0x0006100F);
                            ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());

                            if ( mcBPReply._bpms[bp]._bpms[bpm]._statusWord != 0 ) {
                                ras.setDetail("COND", "MODULE STATUS");
                                badval << "0x" << std::hex << int64_t(mcBPReply._bpms[bp]._bpms[bpm]._statusWord);
                                ras.setDetail("BADVAL", badval.str());
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                            if ( mcBPReply._bpms[bp]._bpms[bpm]._statusVout != 0 ) {
                                ras.setDetail("COND", "OUTPUT VOLTAGE");
                                badval << "0x" << std::hex << int64_t(mcBPReply._bpms[bp]._bpms[bpm]._statusVout);
                                ras.setDetail("BADVAL", badval.str());
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                            if ( mcBPReply._bpms[bp]._bpms[bpm]._statusIout != 0 ) {
                                ras.setDetail("COND", "OUTPUT CURRENT");
                                badval << "0x" << std::hex << int64_t(mcBPReply._bpms[bp]._bpms[bpm]._statusIout);
                                ras.setDetail("BADVAL", badval.str());
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                            if ( mcBPReply._bpms[bp]._bpms[bpm]._statusInput != 0 ) {
                                ras.setDetail("COND", "INPUT POWER");
                                badval << "0x" << std::hex << int64_t(mcBPReply._bpms[bp]._bpms[bpm]._statusInput);
                                ras.setDetail("BADVAL", badval.str());
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                            if ( mcBPReply._bpms[bp]._bpms[bpm]._statusTemperature != 0 ) {
                                ras.setDetail("COND", "TEMPERATURE");
                                badval << "0x" << std::hex <<  int64_t(mcBPReply._bpms[bp]._bpms[bpm]._statusTemperature);
                                ras.setDetail("BADVAL", badval.str());
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                            if ( mcBPReply._bpms[bp]._bpms[bpm]._statusCml != 0 ) {
                                ras.setDetail("COND", "COMM, LOGIC, AND MEMORY");
                                badval << "0x" << std::hex << int64_t(mcBPReply._bpms[bp]._bpms[bpm]._statusCml);
                                ras.setDetail("BADVAL", badval.str());
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                            if ( mcBPReply._bpms[bp]._bpms[bpm]._status5V != 0 ) {
                                ras.setDetail("COND", "5V OUTPUT");
                                badval << "0x" << std::hex << int64_t(mcBPReply._bpms[bp]._bpms[bpm]._status5V);
                                ras.setDetail("BADVAL", badval.str());
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                            if (( mcBPReply._bpms[bp]._bpms[bpm]._statusFans & 0xF3) != 0 ) {  // masks off fan speed override bits
                                ras.setDetail("COND", "FANS");
                                badval << "0x" << std::hex << int64_t(mcBPReply._bpms[bp]._bpms[bpm]._statusFans);
                                ras.setDetail("BADVAL", badval.str());
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                        }
                        double outVoltage5v, outCurrent5v, temp[3], rpmsFan1, rpmsFan2;
                        sscanf(mcBPReply._bpms[bp]._bpms[bpm]._outputVoltage5V.c_str(), "%lf", &outVoltage5v);
                        if ( outVoltage5v > MAX5VVOLTAGE ||
                             outVoltage5v < MIN5VVOLTAGE) {
                            // send RAS
                            RasEventImpl ras(0x0006100F);
                            ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                            ras.setDetail("COND", "5V OUTPUT VOLTAGE");
                            ras.setDetail("BADVAL", boost::lexical_cast<string>(outVoltage5v));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        sscanf(mcBPReply._bpms[bp]._bpms[bpm]._outputCurrent5V.c_str(), "%lf", &outCurrent5v);
                        if ( outCurrent5v > MAX5VCURRENT ||
                             outCurrent5v < MIN5VCURRENT) {
                            // send RAS
                            RasEventImpl ras(0x0006100F);
                            ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                            ras.setDetail("COND", "5V OUTPUT CURRENT");
                            ras.setDetail("BADVAL", boost::lexical_cast<string>(outCurrent5v));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        sscanf(mcBPReply._bpms[bp]._bpms[bpm]._temp1.c_str(), "%lf", &temp[0]);
                        sscanf(mcBPReply._bpms[bp]._bpms[bpm]._temp2.c_str(), "%lf", &temp[1]);
                        sscanf(mcBPReply._bpms[bp]._bpms[bpm]._temp3.c_str(), "%lf", &temp[2]);


                        if (!MMCSProperties::getProperty("BPMTEMPS").empty() ) {
                            // test code to record temps for profiling.
                            // the purpose is to use actual temperatures to determine the MAXBPMTEMP value and
                            // the BPMTEMPRANGE based on accumulated data
                            BGQDB::DBTBulkpowertemp bpt;
                            std::bitset<30> map;
                            SQLRETURN rc;
                            map.set();
                            map.reset(bpt.TIME);
                            bpt._columns = map.to_ulong();
                            strcpy(bpt._location,mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                            sscanf(mcBPReply._bpms[bp]._bpms[bpm]._temp1.c_str(), "%lf", &bpt._module1temp);
                            sscanf(mcBPReply._bpms[bp]._bpms[bpm]._temp2.c_str(), "%lf", &bpt._module2temp);
                            sscanf(mcBPReply._bpms[bp]._bpms[bpm]._temp3.c_str(), "%lf", &bpt._module3temp);

                            rc = tx.insert(&bpt);
                            if(rc != SQL_SUCCESS) {
                                LOG_INFO_MSG("Error writing bulk power temperatures: location " << string(bpt._location)  << " return code " << rc);
                            }

                        }

                        if ( temp[0] > MAXBPMTEMP ) {
                            // send RAS
                            RasEventImpl ras(0x0006100F);
                            ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                            ras.setDetail("COND", "DC/DC MODULE 1 TEMPERATURE");
                            ras.setDetail("BADVAL", boost::lexical_cast<string>(temp[0]));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        if ( temp[1] > MAXBPMTEMP ) {
                            // send RAS
                            RasEventImpl ras(0x0006100F);
                            ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                            ras.setDetail("COND", "DC/DC MODULE 2 TEMPERATURE");
                            ras.setDetail("BADVAL", boost::lexical_cast<string>(temp[1]));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        if ( temp[2] > MAXBPMTEMP ) {
                            // send RAS
                            RasEventImpl ras(0x0006100F);
                            ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                            ras.setDetail("COND", "DC/DC MODULE 3 TEMPERATURE");
                            ras.setDetail("BADVAL", boost::lexical_cast<string>(temp[2]));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        sscanf(mcBPReply._bpms[bp]._bpms[bpm]._rpmsFan1.c_str(), "%lf", &rpmsFan1);
                        sscanf(mcBPReply._bpms[bp]._bpms[bpm]._rpmsFan2.c_str(), "%lf", &rpmsFan2);
                        if ( rpmsFan1 > MAXRPM ||
                             rpmsFan1 < MINRPM ) {
                            // send RAS
                            RasEventImpl ras(0x0006100F);
                            ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                            ras.setDetail("COND", "FAN 1 SPEED");
                            ras.setDetail("BADVAL", boost::lexical_cast<string>(rpmsFan1));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        if ( rpmsFan2 > MAXRPM ||
                             rpmsFan2 < MINRPM ) {
                            // send RAS
                            RasEventImpl ras(0x0006100F);
                            ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                            ras.setDetail("COND", "FAN 2 SPEED");
                            ras.setDetail("BADVAL", boost::lexical_cast<string>(rpmsFan2));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }

                    }

                }
            }
        }

        // *********  HANDLE NODE BOARDS   ***************

        for( handle = targetSetHandles.begin(); handle != targetSetHandles.end(); ) {
            if(isThreadStopping() == true) return 0;
            // open target set in monitor mode
            MCServerMessageSpec::OpenTargetRequest   ncOpenRequest( *handle, "EnvMonHC", MCServerMessageSpec::RAAW  , true);
            MCServerMessageSpec::OpenTargetReply     ncOpenReply;
            server->openTarget(ncOpenRequest, ncOpenReply);

            if (ncOpenReply._rc != 0) {
                LOG_INFO_MSG("unable to open target set in mcServer: " << ncOpenReply._rt);
            } else {
                // read values from cards
                MCServerMessageSpec::ReadNodeCardEnvRequest mcNCRequest;
                MCServerAPIHelpers::copyTargetRequest( ncOpenRequest, mcNCRequest );
                MCServerMessageSpec::ReadNodeCardEnvReply   mcNCReply;
                mcNCRequest._shortForm = true;
                server->readNodeCardEnv(mcNCRequest, mcNCReply);

                // close target set, we have our data
                MCServerMessageSpec::CloseTargetRequest ncCloseRequest = MCServerAPIHelpers::createCloseRequest( ncOpenRequest, ncOpenReply);
                MCServerMessageSpec::CloseTargetReply   ncCloseReply;
                server->closeTarget(ncCloseRequest, ncCloseReply);

                for(unsigned nc = 0 ; nc < mcNCReply._nodeCardsShort.size() ; nc ++) {
                    if(isThreadStopping() == true) return 0;
                    if (mcNCReply._nodeCardsShort[nc]._error == CARD_NOT_PRESENT) { } else  // do nothing if the card is not present
                    if (mcNCReply._nodeCardsShort[nc]._error == CARD_NOT_UP) { } else  // do nothing if the card is not up
                    if (mcNCReply._nodeCardsShort[nc]._error != 0) {
                        LOG_INFO_MSG("Error occurred reading environmentals from: " << mcNCReply._nodeCardsShort[nc]._lctn);
                        RasEventImpl noContact(0x00061001);
                        noContact.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                        RasEventHandlerChain::handle(noContact);
                        BGQDB::putRAS(noContact);
                    } else {

                        if (mcNCReply._nodeCardsShort[nc]._clockFreq*16 > MAXFREQ ||
                            mcNCReply._nodeCardsShort[nc]._clockFreq*16 < MINFREQ) {
                            // send RAS  (and fix the values that we're comparing against
                            RasEventImpl ras(0x00061010);
                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                            ras.setDetail("FREQ", boost::lexical_cast<string>(mcNCReply._nodeCardsShort[nc]._clockFreq*16));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }

                        /* right now this is disabled because analysis by Don, Kahn, and others determined it was covered elsewhere
                        if (mcNCReply._nodeCardsShort[nc]._alertsComputes != 0x0F) {
                            // put RAS
                            RasEventImpl ras(0x00061011);
                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                            ras.setDetail("COMP", "COMPUTE ALERTS");
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        */

                        /*  right now this is disabled because unbooted computes are causing RAS
                         if (mcNCReply._nodeCardsShort[nc]._pgoodComputes != 0xFFFFFFFF) {
                            // put RAS
                            RasEventImpl ras(0x00061011);
                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                            ras.setDetail("COMP", "COMPUTE PGOOD");
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        */

                        if (mcNCReply._nodeCardsShort[nc]._alertsBlinks != 1) {
                            // put RAS
                            RasEventImpl ras(0x00061011);
                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                            ras.setDetail("COMP", "LINK CHIP ALERTS");
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }

                        if (mcNCReply._nodeCardsShort[nc]._pgoodBlinks != 1) {
                            // put RAS
                            RasEventImpl ras(0x00061011);
                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                            ras.setDetail("COMP", "LINK CHIP PGOOD");
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }

                        if (mcNCReply._nodeCardsShort[nc]._alertsOptics != 1) {
                            // put RAS
                            RasEventImpl ras(0x00061011);
                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                            ras.setDetail("COMP", "OPTICS");
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }

                        if (mcNCReply._nodeCardsShort[nc]._statusDca != 3) {
                            // put RAS
                            RasEventImpl ras(0x00061011);
                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                            ras.setDetail("COMP", "DCA");
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        if (mcNCReply._nodeCardsShort[nc]._statusVtmDomains123 != 0x3FFFFFFF ||
                            (mcNCReply._nodeCardsShort[nc]._statusVtmDomains468 != 0x0FFF)
                             ) {
                            const std::map<int,std::string> domain123 = boost::assign::map_list_of
                                (0x20000000, "02 domain 3")
                                (0x10000000, "01 domain 3")
                                (0x08000000, "09 domain 2")
                                (0x04000000, "08 domain 2")
                                (0x02000000, "07 domain 2")
                                (0x01000000, "06 domain 2")
                                (0x00800000, "05 domain 2")
                                (0x00400000, "04 domain 2")
                                (0x00200000, "03 domain 2")
                                (0x00100000, "02 domain 2")
                                (0x00080000, "01 domain 2")
                                (0x00040000, "19 domain 1")
                                (0x00020000, "18 domain 1")
                                (0x00010000, "17 domain 1")
                                (0x00008000, "16 domain 1")
                                (0x00004000, "15 domain 1")
                                (0x00002000, "14 domain 1")
                                (0x00001000, "13 domain 1")
                                (0x00000800, "12 domain 1")
                                (0x00000400, "11 domain 1")
                                (0x00000200, "10 domain 1")
                                (0x00000100, "09 domain 1")
                                (0x00000080, "08 domain 1")
                                (0x00000040, "07 domain 1")
                                (0x00000020, "06 domain 1")
                                (0x00000010, "05 domain 1")
                                (0x00000008, "04 domain 1")
                                (0x00000004, "03 domain 1")
                                (0x00000002, "02 domain 1")
                                (0x00000001, "01 domain 1");
                            const std::map<int,std::string> domain468 = boost::assign::map_list_of
                                (0x00000800, "04 domain 7")
                                (0x00000400, "03 domain 7")
                                (0x00000200, "02 domain 7")
                                (0x00000100, "01 domain 7")
                                (0x00000080, "02 domain 8")
                                (0x00000040, "01 domain 8")
                                (0x00000020, "04 domain 6")
                                (0x00000010, "03 domain 6")
                                (0x00000008, "02 domain 6")
                                (0x00000004, "01 domain 6")
                                (0x00000002, "02 domain 4")
                                (0x00000001, "01 domain 4");

                            // add specific VTM location to event
                            std::ostringstream component;
                            component << "VTM DOMAINS: ";
                            for ( std::map<int,std::string>::const_iterator i = domain123.begin(); i != domain123.end(); ++i ) {
                                if (!(mcNCReply._nodeCardsShort[nc]._statusVtmDomains123 & i->first)) component << i->second << ", ";
                            }
                            for ( std::map<int,std::string>::const_iterator i = domain468.begin(); i != domain468.end(); ++i ) {
                                if (!(mcNCReply._nodeCardsShort[nc]._statusVtmDomains468 & i->first)) component << i->second << ", ";
                            }

                            // put RAS
                            RasEventImpl ras(0x00061011);
                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                            ras.setDetail("COMP", component.str());
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }

                        for(unsigned ncd = 0 ; ncd < mcNCReply._nodeCardsShort[nc]._dcas.size() ; ncd++) {
                            if(isThreadStopping() == true) return 0;
                            if (mcNCReply._nodeCardsShort[nc]._dcas[ncd]._error != 0) {
                                LOG_INFO_MSG("Error occurred reading environmentals from: " <<  mcNCReply._nodeCardsShort[nc]._dcas[ncd]._lctn);
                                // put RAS
                                RasEventImpl ras(0x0006100C);
                                ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._dcas[ncd]._lctn.c_str());
                                ras.setDetail("COND", "CARD ERROR");
                                ras.setDetail("BADVAL", boost::lexical_cast<string>(mcNCReply._nodeCardsShort[nc]._dcas[ncd]._error));
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            } else {
                                for(unsigned ncdp = 0 ; ncdp < mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains.size() ; ncdp++) {
                                    if(isThreadStopping() == true) return 0;
                                    if ((mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._status != 0x90 &&
                                         mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._status != 0xB0) ||
                                        mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain < 0 ||
                                        mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain > 9) {
                                        // put RAS
                                        RasEventImpl ras(0x0006100C);
                                        ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._dcas[ncd]._lctn.c_str());
                                        if (mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._status != 0x90 &&
                                            mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._status != 0xB0) {
                                            ras.setDetail("COND", string("INCORRECT STATUS, DOMAIN ") + boost::lexical_cast<string>(mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain));
                                            ras.setDetail("BADVAL", boost::lexical_cast<string>(int64_t(mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._status)));
                                        } else {
                                            ras.setDetail("COND", "INCORRECT POWER DOMAIN VALUE");
                                            ras.setDetail("BADVAL", boost::lexical_cast<string>(int64_t(mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain)));
                                        }
                                        RasEventHandlerChain::handle(ras);
                                        BGQDB::putRAS(ras);
                                    } else {
                                        if (MINDCAVOLTAGE_N[mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain] != 0.0 &&
                                            (mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._voltage < MINDCAVOLTAGE_N[mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain]  ||
                                             mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._voltage > MAXDCAVOLTAGE_N[mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain])) {
                                            // put RAS
                                            RasEventImpl ras(0x0006100C);
                                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._dcas[ncd]._lctn.c_str());
                                            ras.setDetail("COND", string("INCORRECT VOLTAGE, DOMAIN ") + boost::lexical_cast<string>(mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain));
                                            ras.setDetail("BADVAL", boost::lexical_cast<string>(mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._voltage));
                                            RasEventHandlerChain::handle(ras);
                                            BGQDB::putRAS(ras);
                                        }
                                        if (MAXDCACURRENT_N[mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain] != 0.0 &&
                                            (mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._current < 0 ||
                                             mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._current > MAXDCACURRENT_N[mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain])) {
                                            // put RAS
                                            RasEventImpl ras(0x0006100C);
                                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._dcas[ncd]._lctn.c_str());
                                            ras.setDetail("COND", string("INCORRECT CURRENT, DOMAIN ") + boost::lexical_cast<string>(mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain));
                                            ras.setDetail("BADVAL", boost::lexical_cast<string>(mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._current));
                                            RasEventHandlerChain::handle(ras);
                                            BGQDB::putRAS(ras);
                                        }
                                    }
                                }  // end for loop

                            }
                        }

                        for(unsigned nco = 0 ; nco < mcNCReply._nodeCardsShort[nc]._optics.size() ; nco++) {
                            if(isThreadStopping() == true) return 0;
                            if (mcNCReply._nodeCardsShort[nc]._optics[nco]._error != 0) {
                                LOG_INFO_MSG("Error occurred reading environmentals from: " << mcNCReply._nodeCardsShort[nc]._optics[nco]._lctn);
                                // put RAS
                                RasEventImpl ras(0x0006100E);
                                ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._optics[nco]._lctn.c_str());
                                ras.setDetail("COND", "MODULE ERROR");
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            } else {
                                if ((mcNCReply._nodeCardsShort[nc]._optics[nco]._status & 0x02) != 0) {  // check interrupt bit

                                    const std::map<int,std::string> channels = boost::assign::map_list_of
                                        (0x0800, "11")
                                        (0x0400, "10")
                                        (0x0200, "9")
                                        (0x0100, "8")
                                        (0x0080, "7")
                                        (0x0040, "6")
                                        (0x0020, "5")
                                        (0x0010, "4")
                                        (0x0008, "3")
                                        (0x0004, "2")
                                        (0x0002, "1")
                                        (0x0001, "0");

                                    // put RAS
                                    RasEventImpl ras(0x0006100E);
                                    ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._optics[nco]._lctn.c_str());
                                    string condition = "MODULE STATUS";
                                    if ((mcNCReply._nodeCardsShort[nc]._optics[nco]._los & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._losMask) != 0) {
                                        condition.append(", CHANNEL ");
                                        // apply mask to get channel information
                                        const int channels = mcNCReply._nodeCardsShort[nc]._optics[nco]._los  & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._losMask;

                                        // iterate through all 11 channels looking at which ones have loss of
                                        // signal alarms, see mc.h for the format
                                        for ( unsigned channel = 0; channel < 12; ++channel ) {
                                            const uint64_t bit = 0x1 << channel;
                                            if (channels & bit) condition.append( boost::lexical_cast<std::string>(channel) + " ");
                                        }
                                        condition.append("LOSS OF SIGNAL");
                                    }
                                    if ((mcNCReply._nodeCardsShort[nc]._optics[nco]._faults & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._faultsMask) != 0) {
                                        condition.append(", CHANNEL ");
                                        for ( std::map<int,std::string>::const_iterator i = channels.begin(); i != channels.end(); ++i ) {
                                            if (mcNCReply._nodeCardsShort[nc]._optics[nco]._faults & i->first) condition.append( i->second + " ");
                                        }
                                        condition.append("FAULT");
                                    }
                                    if ((mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsTemp & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsTempMask) != 0) {
                                        condition.append(", TEMPERATURE");
                                    }
                                    if ((mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsVoltage & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsVoltageMask) != 0) {
                                        condition.append(", VOLTAGE");
                                    }
                                    if ((mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsBiasCurrent & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsBiasCurrentMask) != 0) {
                                        condition.append(", BIAS CURRENT ");
                                        // apply mask to get channel information
                                        const uint64_t channels = mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsBiasCurrent & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsBiasCurrentMask;

                                        // iterate through all 11 channels looking at which ones have bias
                                        // current alarms, see mc.h for the format
                                        for ( unsigned channel = 0; channel < 12; ++channel ) {
                                            const uint64_t bit = 0xFULL << (channel * 4);
                                            if (channels & bit) condition.append( boost::lexical_cast<std::string>(channel) + " ");
                                        }
                                    }
                                    if ((mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsPower & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsPowerMask) != 0) {
                                        condition.append(", POWER ");
                                        // apply mask to get channel information
                                        const uint64_t channels = mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsPower & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsPowerMask;

                                        // iterate through all 11 channels looking at which ones have power
                                        // alarms, see mc.h for the format
                                        for ( unsigned channel = 0; channel < 12; ++channel ) {
                                            const uint64_t bit = 0xFULL << (channel * 4);
                                            if (channels & bit) condition.append( boost::lexical_cast<std::string>(channel) + " ");
                                        }
                                    }
                                    // remove MODULE_STATUS if something else was found
                                    if (condition != "MODULE STATUS") {
                                        condition = condition.substr(15);
                                    }

                                    ras.setDetail("COND", condition);
                                    RasEventHandlerChain::handle(ras);
                                    BGQDB::putRAS(ras);
                                }
                            }
                        }  // end of loop through optical modules
                    }
                }

            }

            handle++;

        }

        // *********  HANDLE COOLANT MONITORS   ***************


        // open target set for coolant monitors (uses service card target set?)
        MCServerMessageSpec::OpenTargetRequest   cmOpenRequest( "EnvMonCMHealth","EnvMonHC", MCServerMessageSpec::RAAW  , true);
        MCServerMessageSpec::OpenTargetReply     cmOpenReply;
        server->openTarget(cmOpenRequest, cmOpenReply);

        if (cmOpenReply._rc != 0) {
            LOG_INFO_MSG("unable to open target set in mcServer, for coolant monitor health check: " << cmOpenReply._rt);
        } else {
            // read values from cards

            // read values from cards
            MCServerMessageSpec::ReadCoolantMonitorEnvRequest mcCMRequest;
            MCServerAPIHelpers::copyTargetRequest ( cmOpenRequest, mcCMRequest );
            MCServerMessageSpec::ReadCoolantMonitorEnvReply   mcCMReply;
            server->readCoolantMonitorEnv(mcCMRequest, mcCMReply);


            // close target set, we have our data
            MCServerMessageSpec::CloseTargetRequest cmCloseRequest = MCServerAPIHelpers::createCloseRequest( cmOpenRequest, cmOpenReply );
            MCServerMessageSpec::CloseTargetReply   cmCloseReply;
            server->closeTarget(cmCloseRequest, cmCloseReply);

            for(unsigned cmon = 0 ; cmon < mcCMReply._coolMons.size() ; cmon++) {
                if(isThreadStopping() == true) return 0;
                if (mcCMReply._coolMons[cmon]._error == CARD_NOT_PRESENT) { } else  // do nothing if the card is not present
                if (mcCMReply._coolMons[cmon]._error == CARD_NOT_UP) { } else  // do nothing if the card is not up
                if (mcCMReply._coolMons[cmon]._error != 0) {
                    LOG_INFO_MSG("Error occurred reading environmentals from: " << mcCMReply._coolMons[cmon]._lctn);
                    RasEventImpl ras(0x00061005);
                    ras.setDetail(RasEvent::LOCATION, mcCMReply._coolMons[cmon]._lctn.c_str());
                    RasEventHandlerChain::handle(ras);
                    BGQDB::putRAS(ras);
                } else {
                    if (mcCMReply._coolMons[cmon]._supplyFlowStatus != 0  ||
                        mcCMReply._coolMons[cmon]._returnFlowStatus != 0  ||
                        mcCMReply._coolMons[cmon]._leakStatus > 1   ||
                        mcCMReply._coolMons[cmon]._supplyTempStatus != 0  ||
                        mcCMReply._coolMons[cmon]._returnTempStatus != 0  ||
                        mcCMReply._coolMons[cmon]._supplyPressureStatus != 0  ||
                        mcCMReply._coolMons[cmon]._diffPressureStatus != 0  ||
                        mcCMReply._coolMons[cmon]._ambientTempStatus != 0  ||
                        mcCMReply._coolMons[cmon]._humidityStatus != 0 ||
                        mcCMReply._coolMons[cmon]._dewpointStatus != 0 ||
                        mcCMReply._coolMons[cmon]._powerConsumptionStatus != 0 ||
                        mcCMReply._coolMons[cmon]._impedanceStatus != 0) {
                        // put RAS
                        RasEventImpl ras(0x00061006);
                        ras.setDetail(RasEvent::LOCATION, mcCMReply._coolMons[cmon]._lctn.c_str());
                        string condition = "MONITOR STATUS";

                        switch (mcCMReply._coolMons[cmon]._supplyFlowStatus) {
                        case 1:   condition.append(", SUPPLY HIGH FLOW ALARM"); break;
                        case 2:   condition.append(", SUPPLY LOW FLOW ALARM"); break;
                        case 3:   condition.append(", SUPPLY HIGH FLOW FAULT"); break;
                        case 4:   condition.append(", SUPPLY LOW FLOW FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._returnFlowStatus) {
                        case 1:   condition.append(", RETURN HIGH FLOW ALARM"); break;
                        case 2:   condition.append(", RETURN LOW FLOW ALARM"); break;
                        case 3:   condition.append(", RETURN HIGH FLOW FAULT"); break;
                        case 4:   condition.append(", RETURN LOW FLOW FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._leakStatus) {
                        case 2:   condition.append(", SLOW LEAK DETECTED"); break;
                        case 3:   condition.append(", LEAK DETECTED"); break;
                        case 4:   condition.append(", CATASTROPHIC LEAK DETECTED"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._valveStatus) {
                        case 0:   condition.append(", SOLENOID VALVE CLOSED"); break;
                        case 1:   condition.append(", SOLENOID VALVE OPEN"); break;
                        default: break;
                        }

                        switch (mcCMReply._coolMons[cmon]._supplyTempStatus) {
                        case 1:   condition.append(", SUPPLY HIGH TEMP ALARM"); break;
                        case 2:   condition.append(", SUPPLY LOW TEMP ALARM"); break;
                        case 3:   condition.append(", SUPPLY HIGH TEMP FAULT"); break;
                        case 4:   condition.append(", SUPPLY LOW TEMP FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._returnTempStatus) {
                        case 1:   condition.append(", RETURN HIGH TEMP ALARM"); break;
                        case 2:   condition.append(", RETURN LOW TEMP ALARM"); break;
                        case 3:   condition.append(", RETURN HIGH TEMP FAULT"); break;
                        case 4:   condition.append(", RETURN LOW TEMP FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._supplyPressureStatus) {
                        case 1:   condition.append(", SUPPLY HIGH PRESSURE ALARM"); break;
                        case 2:   condition.append(", SUPPLY LOW PRESSURE ALARM"); break;
                        case 3:   condition.append(", SUPPLY HIGH PRESSURE FAULT"); break;
                        case 4:   condition.append(", SUPPLY LOW PRESSURE FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._diffPressureStatus) {
                        case 1:   condition.append(", DIFF HIGH PRESSURE ALARM"); break;
                        case 2:   condition.append(", DIFF LOW PRESSURE ALARM"); break;
                        case 3:   condition.append(", DIFF HIGH PRESSURE FAULT"); break;
                        case 4:   condition.append(", DIFF LOW PRESSURE FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._ambientTempStatus) {
                        case 1:   condition.append(", AMBIENT HIGH TEMP ALARM"); break;
                        case 2:   condition.append(", AMBIENT LOW TEMP ALARM"); break;
                        case 3:   condition.append(", AMBIENT HIGH TEMP FAULT"); break;
                        case 4:   condition.append(", AMBIENT LOW TEMP FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._humidityStatus) {
                        case 1:   condition.append(", HIGH HUMIDITY ALARM"); break;
                        case 2:   condition.append(", LOW HUMIDITY ALARM"); break;
                        case 3:   condition.append(", HIGH HUMIDITY FAULT"); break;
                        case 4:   condition.append(", LOW HUMIDITY FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._dewpointStatus) {
                        case 1:   condition.append(", HIGH DEW POINT ALARM"); break;
                        case 2:   condition.append(", LOW DEW POINT ALARM"); break;
                        case 3:   condition.append(", HIGH DEW POINT FAULT"); break;
                        case 4:   condition.append(", LOW DEW POINT FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._powerConsumptionStatus) {
                        case 1:   condition.append(", HIGH POWER CONSUMPTION ALARM"); break;
                        case 2:   condition.append(", LOW POWER CONSUMPTION ALARM"); break;
                        case 3:   condition.append(", HIGH POWER CONSUMPTION FAULT"); break;
                        case 4:   condition.append(", LOW POWER CONSUMPTION FAULT"); break;
                        default:;
                        }
                             
                        switch (mcCMReply._coolMons[cmon]._impedanceStatus) {
                        case 1:   condition.append(", HIGH FLOW IMPEDANCE ALARM"); break;
                        case 2:   condition.append(", LOW FLOW IMPEDANCE ALARM"); break;
                        case 3:   condition.append(", HIGH FLOW IMPEDANCE FAULT"); break;
                        case 4:   condition.append(", LOW FLOW IMPEDANCE FAULT"); break;
                        default:;
                        }
                                
                        // remove MODULE_STATUS if something else was found
                        if (condition != "MONITOR STATUS") {
                            condition = condition.substr(16);
                        }

                        // omit RAS event if solenoid open is the only condition
                        if ( condition != "SOLENOID VALVE OPEN" ) {
                            ras.setDetail("COND", condition);
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                    }

                    if (mcCMReply._coolMons[cmon]._shutoffCauseStatus) {
                        RasEventImpl ras(0x00061014);
                        ras.setDetail(RasEvent::LOCATION, mcCMReply._coolMons[cmon]._lctn.c_str());
                        string condition;
                        switch (mcCMReply._coolMons[cmon]._shutoffCauseStatus) {
                        case 0x1:   condition.append("SUPPLY FLOW LOW"); break;
                        case 0x2:   condition.append("SUPPLY TEMPERATURE LOW"); break;
                        case 0x4:   condition.append("SUPPLY PRESSURE HIGH"); break;
                        case 0x8:   condition.append("SLOW LEAK"); break;
                        case 0x10:  condition.append("CATASTROPHIC LEAK"); break;
                        default:;
                        }
                        ras.setDetail("COND", condition);
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }
                }
            }     //  end loop through coolant monitors
        }

        // **********  END OF HEALTH CHECK LOOP ***************

        if (mode != bgq::utility::performance::Mode::Value::None) {
            duration = boost::posix_time::microsec_clock::local_time() - start;
            perf._duration = static_cast<double>(duration.total_microseconds()) / 1000000;
            rc = tx.insert(&perf);
        }

        //sleep for <seconds>, but wake up every <pollingPeriod> to check if we've been killed
        for (unsigned slept = 0 ; (isThreadStopping() == false) && (slept < seconds) ; slept+= pollingPeriod)
            {
                sleep(pollingPeriod);
            }

        // just in case the performance mode was changed
        if ( mode != bgq::utility::performance::Mode::instance().getMode() ) {
            mode = bgq::utility::performance::Mode::instance().getMode();
        }
    } //  while (isThreadStopping() == false)

    return NULL;
}


//******************************************************************************
// Thread start function for the BulkPowerThread
//******************************************************************************
void*
BulkPowerThread::threadStart()
{
    unsigned pollingPeriod = NORMAL_POLLING_PERIOD;

    MCServerRef* server;

    LOG_INFO_MSG("thread started for Bulk Power Data"<< ", interval is " << seconds << " seconds" << ", polling is " << (pollEnvs? "ON":"OFF"));
    MMCSCommandReply rep;
    BlockControllerBase::mcserver_connect(server, "EnvMonBulk", rep);
    if(!server) {
        LOG_ERROR_MSG("BulkPowerThread failed to connect to mc_server");
        return NULL;
    }

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection())
        {
            LOG_INFO_MSG("unable to connect to database");
            return NULL;
        }

    // make the target set
    MCServerMessageSpec::MakeTargetSetRequest   mcMakeRequest("EnvMonBulk","EnvMonBulk",  true);
    MCServerMessageSpec::MakeTargetSetReply     mcMakeReply;
    mcMakeRequest._expression.push_back("R..-M.-S$");

    // Get list of IO drawers in IO racks
    SQLHANDLE ioDrawers;
    SQLLEN index;
    SQLRETURN sqlrc, result;
    char ioDrawerLoc[7];
    string sqlstr;
    sqlstr.clear();
    sqlstr.append("select LOCATION from BGQIODrawer where status <> 'M' and location like 'Q%'  ");
    result = tx.execQuery( sqlstr.c_str(), &ioDrawers );
    SQLBindCol(ioDrawers, 1, SQL_C_CHAR, ioDrawerLoc, 7, &index);
    sqlrc = SQLFetch(ioDrawers);
    while ( sqlrc == SQL_SUCCESS )   {
        mcMakeRequest._expression.push_back(ioDrawerLoc);
        sqlrc = SQLFetch(ioDrawers);
    }
    SQLCloseCursor(ioDrawers);

    server->makeTargetSet(mcMakeRequest, mcMakeReply);

    // set up for inserting performance counter info
    bgq::utility::performance::Mode::Value::Type mode = bgq::utility::performance::Mode::instance().getMode();

    BGQDB::DBTComponentperf perf;
    std::bitset<30> smap;
    smap.set();
    smap.reset(perf.ENTRYDATE);
    smap.reset(perf.DETAIL);
    smap.reset(perf.QUALIFIER);
    SQLRETURN rc;
    perf._columns = smap.to_ulong();
    strcpy(perf._id,"EnvMon");
    strcpy(perf._component,"mmcs");
    strcpy(perf._function,"EnvMon");
    strcpy(perf._subfunction,"bulk power environmentals");
    boost::posix_time::ptime  start;
    boost::posix_time::time_duration duration;

    // loop until someone stops us
    while (isThreadStopping() == false) {

        if (pollEnvs) {  // check if we're polling at all
            try {
                if(HardwareBlockList::list_size() == 0) {

                    start = boost::posix_time::microsec_clock::local_time();

                    // open target set in monitor mode
                    MCServerMessageSpec::OpenTargetRequest   mcOpenRequest( "EnvMonBulk","EnvMonBulk", MCServerMessageSpec::RAAW  , true);
                    MCServerMessageSpec::OpenTargetReply     mcOpenReply;
                    server->openTarget(mcOpenRequest, mcOpenReply);

                    if (mcOpenReply._rc != 0) {
                        LOG_INFO_MSG("unable to open target set in mcServer: " << mcOpenReply._rt);
                    } else {
                        // read values from cards
                        MCServerMessageSpec::ReadBulkPowerEnvRequest mcBPRequest;
                        MCServerAPIHelpers::copyTargetRequest ( mcOpenRequest, mcBPRequest );
                        MCServerMessageSpec::ReadBulkPowerEnvReply   mcBPReply;
                        server->readBulkPowerEnv(mcBPRequest, mcBPReply);

                        // close target set, we have our data
                        MCServerMessageSpec::CloseTargetRequest mcCloseRequest = MCServerAPIHelpers::createCloseRequest( mcOpenRequest, mcOpenReply );
                        MCServerMessageSpec::CloseTargetReply   mcCloseReply;
                        server->closeTarget(mcCloseRequest, mcCloseReply);

                        processBulks(&mcBPReply,&tx);

                        if (mode != bgq::utility::performance::Mode::Value::None) {
                            duration = boost::posix_time::microsec_clock::local_time() - start;
                            perf._duration = static_cast<double>(duration.total_microseconds()) / 1000000;
                            rc = tx.insert(&perf);
                        }
                    }
                } else {
                    LOG_INFO_MSG("Subnet failover in progress.  Bulk power environmental polling suspended.");
                }
            } catch(std::exception& e) {
                LOG_ERROR_MSG(e.what());
            }
        } // end check pollEnvs

        //sleep for <seconds>, but wake up every <pollingPeriod> to check if we've been killed
        for (unsigned slept = 0 ; (isThreadStopping() == false) && (slept < seconds) ; slept+= pollingPeriod) {
            sleep(pollingPeriod);
            // check to see if the interval has changed
            unsigned int pollProperty = strtol(MMCSProperties::getProperty(MMCS_ENVS_BULK_INTERVAL_SECONDS).c_str(), NULL, 10);
            
            if (MMCSProperties::getProperty("bypass_envs") == "true")
                pollProperty = 0;
            
            if ((pollProperty >= 60  && pollProperty <= 3600 && (pollProperty != seconds || pollEnvs == false))  ||
                (pollProperty == 0 && pollEnvs == true)) {
                if (pollProperty > 0) {
                    seconds = pollProperty; // only honor valid intervals, 1 - 60 minutes
                    pollEnvs = true;
                    LOG_INFO_MSG( "thread for Bulk Power: interval changed to " << seconds << " seconds" );
                } else {           // don't poll if its 0
                    pollEnvs = false;
                    seconds = ENVS_POLLING_PERIOD;
                    LOG_INFO_MSG( "thread for Bulk Power: not polling" );
                }
            }
        }

        // just in case the performance mode was changed
        if ( mode != bgq::utility::performance::Mode::instance().getMode() ) {
            mode = bgq::utility::performance::Mode::instance().getMode();
        }

    } //  while (isThreadStopping() == false)

    return NULL;
}


//******************************************************************************
// Thread start function for the CoolantThread
//******************************************************************************
void*
CoolantThread::threadStart()
{
    unsigned pollingPeriod = NORMAL_POLLING_PERIOD;

    MCServerRef* server;

    LOG_INFO_MSG("thread started for Coolant Monitor Data"<< ", interval is " << seconds << " seconds" << ", polling is " << (pollEnvs? "ON":"OFF"));
    MMCSCommandReply rep;
    BlockControllerBase::mcserver_connect(server, "EnvMonCM", rep);
    if(!server) {
        LOG_ERROR_MSG("CoolantThread failed to connect to mc_server");
        return NULL;
    }

    BGQDB::DBTCoolantenvironment cool;
    std::bitset<30> map;
    SQLRETURN rc;

    map.set();
    map.reset(cool.TIME);
    cool._columns = map.to_ulong();

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection())
    {
        LOG_INFO_MSG("unable to connect to database");
        return NULL;
    }

     // make the target set
    MCServerMessageSpec::MakeTargetSetRequest   mcMakeRequest("EnvMonCoolant","EnvMonCoolant",  true);
    MCServerMessageSpec::MakeTargetSetReply     mcMakeReply;
    mcMakeRequest._expression.push_back("R..-L$");

    server->makeTargetSet(mcMakeRequest, mcMakeReply);

    // set up for inserting performance counter info
    bgq::utility::performance::Mode::Value::Type mode = bgq::utility::performance::Mode::instance().getMode();

    BGQDB::DBTComponentperf perf;
    std::bitset<30> smap;
    smap.set();
    smap.reset(perf.ENTRYDATE);
    smap.reset(perf.DETAIL);
    smap.reset(perf.QUALIFIER);

    perf._columns = smap.to_ulong();
    strcpy(perf._id,"EnvMon");
    strcpy(perf._component,"mmcs");
    strcpy(perf._function,"EnvMon");
    strcpy(perf._subfunction,"coolant monitor environmentals");
    boost::posix_time::ptime  start;
    boost::posix_time::time_duration duration;

    // loop until someone stops us
    while (isThreadStopping() == false)         {

        if (pollEnvs) {  // check if we're polling at all
            try {
                if(HardwareBlockList::list_size() == 0) {

                    start = boost::posix_time::microsec_clock::local_time();

                    // open target set in monitor mode
                    MCServerMessageSpec::OpenTargetRequest   mcOpenRequest( "EnvMonCoolant","EnvMonCoolant", MCServerMessageSpec::RAAW  , true);
                    MCServerMessageSpec::OpenTargetReply     mcOpenReply;
                    server->openTarget(mcOpenRequest, mcOpenReply);

                    if (mcOpenReply._rc != 0) {
                        LOG_INFO_MSG("unable to open target set in mcServer: " << mcOpenReply._rt);
                    } else {
                        // read values from cards
                        MCServerMessageSpec::ReadCoolantMonitorEnvRequest mcCMRequest;
                        MCServerAPIHelpers::copyTargetRequest ( mcOpenRequest, mcCMRequest );
                        MCServerMessageSpec::ReadCoolantMonitorEnvReply   mcCMReply;
                        server->readCoolantMonitorEnv(mcCMRequest, mcCMReply);

                        // close target set, we have our data
                        MCServerMessageSpec::CloseTargetRequest mcCloseRequest = MCServerAPIHelpers::createCloseRequest( mcOpenRequest, mcOpenReply );
                        MCServerMessageSpec::CloseTargetReply   mcCloseReply;
                        server->closeTarget(mcCloseRequest, mcCloseReply);

                        for(unsigned cmon = 0 ; cmon < mcCMReply._coolMons.size() ; cmon++) {
                            if(isThreadStopping() == true) return 0;
                            if (mcCMReply._coolMons[cmon]._error == CARD_NOT_PRESENT) { } else  // do nothing if the card is not present
                                if (mcCMReply._coolMons[cmon]._error == CARD_NOT_UP) { } else  // do nothing if the card is not up
                                    if (mcCMReply._coolMons[cmon]._error != 0) {
                                        LOG_INFO_MSG("Error occurred reading environmentals from: " << mcCMReply._coolMons[cmon]._lctn);
                                        RasEventImpl ras(0x00061005);
                                        ras.setDetail(RasEvent::LOCATION, mcCMReply._coolMons[cmon]._lctn.c_str());
                                        RasEventHandlerChain::handle(ras);
                                        BGQDB::putRAS(ras);
                                    } else {
                                        strcpy(cool._location, mcCMReply._coolMons[cmon]._lctn.c_str());
                                        cool._inletflowrate = mcCMReply._coolMons[cmon]._supplyFlowRate;
                                        cool._outletflowrate = mcCMReply._coolMons[cmon]._returnFlowRate;
                                        cool._coolantpressure = mcCMReply._coolMons[cmon]._supplyPresure;
                                        cool._diffpressure = mcCMReply._coolMons[cmon]._diffPresure;
                                        cool._inletcoolanttemp = mcCMReply._coolMons[cmon]._supplyCoolantTemp;
                                        cool._outletcoolanttemp = mcCMReply._coolMons[cmon]._returnCoolantTemp;
                                        cool._dewpointtemp = mcCMReply._coolMons[cmon]._ambientDewPoint;
                                        cool._ambienttemp = mcCMReply._coolMons[cmon]._ambientTemp;
                                        cool._ambienthumidity = mcCMReply._coolMons[cmon]._ambientHumidity;
                                        cool._systempower = mcCMReply._coolMons[cmon]._systemPower;
                                        cool._shutoffcause = mcCMReply._coolMons[cmon]._shutoffCauseStatus;
                                        rc = tx.insert(&cool);
                                        if(rc != SQL_SUCCESS) {
                                            LOG_INFO_MSG("Error writing coolant monitor environmentals: location " <<  string(cool._location)  << " return code " << rc);
                                        }

                                    }
                        }
                    }

                    if (mode != bgq::utility::performance::Mode::Value::None) {
                        duration = boost::posix_time::microsec_clock::local_time() - start;
                        perf._duration = static_cast<double>(duration.total_microseconds()) / 1000000;
                        rc = tx.insert(&perf);
                    }

                } else {
                    LOG_INFO_MSG("Subnet failover in progress.  Coolant monitor environmental polling suspended.");
                }
            } catch(std::exception& e) {
                LOG_ERROR_MSG(e.what());
            }
        }


        //sleep for <seconds>, but wake up every <pollingPeriod> to check if we've been killed
        for (unsigned slept = 0 ; (isThreadStopping() == false) && (slept < seconds) ; slept+= pollingPeriod) {
            sleep(pollingPeriod);
            // check to see if the interval has changed
            unsigned int pollProperty = strtol(MMCSProperties::getProperty(MMCS_ENVS_COOLANT_INTERVAL_SECONDS).c_str(), NULL, 10);

            if (MMCSProperties::getProperty("bypass_envs") == "true")
                pollProperty = 0;
            
            if ((pollProperty >= 60  && pollProperty <= 3600 && (pollProperty != seconds || pollEnvs == false))  ||
                (pollProperty == 0 && pollEnvs == true)) {
                if (pollProperty > 0) {
                    seconds = pollProperty; // only honor valid intervals, 1 - 60 minutes
                    pollEnvs = true;
                    LOG_INFO_MSG( "thread for Coolant Monitors: interval changed to " << seconds << " seconds" );
                } else {           // don't poll if its 0
                    pollEnvs = false;
                    seconds = ENVS_POLLING_PERIOD;
                    LOG_INFO_MSG( "thread for Coolant Monitors: not polling" );
                }
            }
        }

        // just in case the performance mode was changed
        if ( mode != bgq::utility::performance::Mode::instance().getMode() ) {
            mode = bgq::utility::performance::Mode::instance().getMode();
        }

    } //  while (isThreadStopping() == false)

    return NULL;
}


//******************************************************************************
// Thread start function for the OpticalThread
//******************************************************************************
void*
OpticalThread::threadStart()
{
    unsigned pollingPeriod = NORMAL_POLLING_PERIOD;

    MCServerRef* server;

    LOG_INFO_MSG("thread started for Optical Module Data"<< ", interval is " << seconds << " seconds" << ", polling is " << (pollEnvs? "ON":"OFF"));

    MMCSCommandReply rep;
    BlockControllerBase::mcserver_connect(server, "EnvMonOM", rep);
    if(!server) {
        LOG_ERROR_MSG("OpticalThread failed to connect to mc_server");
        return NULL;
    }

    BGQDB::DBTOpticalenvironment opt;
    std::bitset<30> map;
    SQLRETURN rc;

    map.set();
    map.reset(opt.TIME);
    opt._columns = map.to_ulong();

    BGQDB::DBTOpticaldata optdata;
    std::bitset<30> mapdata;
    mapdata.set();
    mapdata.reset(optdata.TIME);
    optdata._columns = mapdata.to_ulong();

    BGQDB::DBTOpticalchanneldata optchdata;
    std::bitset<30> mapchdata;
    mapchdata.set();
    mapchdata.reset(optchdata.TIME);
    optchdata._columns = mapchdata.to_ulong();
    

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection())
    {
        LOG_INFO_MSG("unable to connect to database");
        return NULL;
    }

     // Get list of midplanes from tbgqmidplane
    SQLHANDLE hMidplanes;
    SQLLEN index;
    SQLRETURN sqlrc, result;
    char mp[7];
    string sqlstr;

    sqlstr.clear();
    sqlstr.append("select LOCATION from BGQMidplane where location like 'R%-M0'");

    result = tx.execQuery( sqlstr.c_str(), &hMidplanes );

    SQLBindCol(hMidplanes, 1, SQL_C_CHAR, mp, 7, &index);

    // Turn SQL reply into vector of strings
    vector<string> midplaneLocs;
    sqlrc = SQLFetch(hMidplanes);
    while ( sqlrc == SQL_SUCCESS )   {
        midplaneLocs.push_back( mp );
        //LOG_DEBUG_MSG("MidplaneLoc = " << mp);
        sqlrc = SQLFetch(hMidplanes);
    }

    SQLCloseCursor(hMidplanes);

    // Turn strings into target sets, one target set for each rack
    //int counter = 0;
    vector<string> targetSetHandles;
    string charHandle = "EnvMonOM00";
    MCServerMessageSpec::MakeTargetSetRequest   mcMakeRequest(charHandle,"EnvMonOM",  true);
    MCServerMessageSpec::MakeTargetSetReply     mcMakeReply;

    unsigned mpindex;
    for( mpindex = 0; mpindex < midplaneLocs.size(); mpindex++ ) {
        if(isThreadStopping() == true) return 0;
        // Add rack to request
        mcMakeRequest._expression.push_back(midplaneLocs[mpindex].substr(0,3).append("-M.-N..$"));

        // Handle is EnvMonNCxx where xx is rack row,column
        charHandle[8] = midplaneLocs[mpindex][1];
        charHandle[9] = midplaneLocs[mpindex][2];
        mcMakeRequest._set = charHandle;
        server->makeTargetSet(mcMakeRequest, mcMakeReply);
        if( mcMakeReply._rc == BGERR_MCSERVER_OK )
            targetSetHandles.push_back( charHandle );
        // start over for next rack
        mcMakeRequest._expression.clear();
    }

    // make the IO target set
    MCServerMessageSpec::MakeTargetSetRequest   mcMakeIORequest( "EnvMonOM", "EnvMonOM", true);
    MCServerMessageSpec::MakeTargetSetReply     mcMakeIOReply;

    // Get list of IO drawers
    SQLHANDLE ioDrawers;
    char ioDrawerLoc[7];

    sqlstr.clear();
    sqlstr.append("select LOCATION from BGQIODrawer where status <> 'M' ");
    result = tx.execQuery( sqlstr.c_str(), &ioDrawers );
    SQLBindCol(ioDrawers, 1, SQL_C_CHAR, ioDrawerLoc, 7, &index);
    sqlrc = SQLFetch(ioDrawers);
    while ( sqlrc == SQL_SUCCESS )   {
        mcMakeIORequest._expression.push_back(ioDrawerLoc);
        sqlrc = SQLFetch(ioDrawers);
    }
    SQLCloseCursor(ioDrawers);

    server->makeTargetSet(mcMakeIORequest, mcMakeIOReply);

    // set up for inserting performance counter info
    bgq::utility::performance::Mode::Value::Type mode = bgq::utility::performance::Mode::instance().getMode();

    BGQDB::DBTComponentperf perf;
    std::bitset<30> smap;
    smap.set();
    smap.reset(perf.ENTRYDATE);
    smap.reset(perf.DETAIL);
    smap.reset(perf.QUALIFIER);

    perf._columns = smap.to_ulong();
    strcpy(perf._id,"EnvMon");
    strcpy(perf._component,"mmcs");
    strcpy(perf._function,"EnvMon");
    strcpy(perf._subfunction,"optical module environmentals");
    boost::posix_time::ptime  start;
    boost::posix_time::time_duration duration;

    // loop until someone stops us
    while (isThreadStopping() == false)         {


        if (pollEnvs) {  // check if we're polling at all
            try {
                // Loop over target set handles
                vector<string>::iterator handle;
                if(HardwareBlockList::list_size() == 0) {

                    start = boost::posix_time::microsec_clock::local_time();

                    // do the optical modules from node cards
                    for( handle = targetSetHandles.begin(); handle != targetSetHandles.end(); ) {
                        if(isThreadStopping() == true) return 0;
                        // open target set in monitor mode
                        MCServerMessageSpec::OpenTargetRequest   mcOpenRequest( *handle, "EnvMonOM", MCServerMessageSpec::RAAW  , true);
                        MCServerMessageSpec::OpenTargetReply     mcOpenReply;
                        server->openTarget(mcOpenRequest, mcOpenReply);

                        if (mcOpenReply._rc == BGERR_MCSERVER_TARGETSET_ACCESS_DENIED ) {
                            LOG_INFO_MSG(mcOpenReply._rt);
                            handle++;
                            continue; // must be locked for service action; move on to the next one
                        }

                        if (mcOpenReply._rc != 0) {
                            LOG_INFO_MSG("unable to open target set in mcServer: " << mcOpenReply._rt);
                            if( targetSetHandles.size() == 1 )
                                return NULL; // If there's no node cards to monitor, just exit

                            vector<string>::iterator next = handle;
                            next++;
                            targetSetHandles.erase( handle ); // something wrong with this target set, maybe missing a rack
                            handle = next;
                            continue; // move on to the next one
                        }

                        // read values from cards
                        MCServerMessageSpec::ReadNodeCardEnvRequest mcNCRequest;
                        MCServerAPIHelpers::copyTargetRequest( mcOpenRequest, mcNCRequest );
                        MCServerMessageSpec::ReadNodeCardEnvReply   mcNCReply;
                        server->readNodeCardEnv(mcNCRequest, mcNCReply);

                        // close target set, we have our data
                        MCServerMessageSpec::CloseTargetRequest mcCloseRequest = MCServerAPIHelpers::createCloseRequest( mcOpenRequest, mcOpenReply);
                        MCServerMessageSpec::CloseTargetReply   mcCloseReply;
                        server->closeTarget(mcCloseRequest, mcCloseReply);

                        for(unsigned nc = 0 ; nc < mcNCReply._nodeCards.size() ; nc ++) {
                            if(isThreadStopping() == true) return 0;
                            if (mcNCReply._nodeCards[nc]._error == CARD_NOT_PRESENT) { } else  // do nothing if the card is not present
                                if (mcNCReply._nodeCards[nc]._error == CARD_NOT_UP) { } else       // do nothing if the card is not up
                                    if (mcNCReply._nodeCards[nc]._error != 0) {
                                        LOG_INFO_MSG("Error occurred reading environmentals from: " << mcNCReply._nodeCards[nc]._lctn);
                                        RasEventImpl noContact(0x00061001);
                                        noContact.setDetail(RasEvent::LOCATION, mcNCReply._nodeCards[nc]._lctn.c_str());
                                        RasEventHandlerChain::handle(noContact);
                                        BGQDB::putRAS(noContact);

                                    } else {

                                        for(unsigned optical = 0 ; optical < mcNCReply._nodeCards[nc]._optics.size() ; ++optical) {
                                            if(isThreadStopping() == true) return 0;
                                            if (mcNCReply._nodeCards[nc]._optics[optical]._error != 0) {
                                                LOG_INFO_MSG("Error occurred reading environmentals from: " << mcNCReply._nodeCards[nc]._optics[optical]._lctn);
                                            } else {
                                                strcpy(opt._location, mcNCReply._nodeCards[nc]._optics[optical]._lctn.c_str());
                                                for(unsigned channel = 0 ; channel < mcNCReply._nodeCards[nc]._optics[optical]._OpticalEnvChannel.size() ; ++channel) {
                                                    if(isThreadStopping() == true) return 0;
                                                    opt._channel = channel;
                                                    opt._power = mcNCReply._nodeCards[nc]._optics[optical]._OpticalEnvChannel[channel]._opticalPower;
                                                    rc = tx.insert(&opt);
                                                    if(rc != SQL_SUCCESS) {
                                                        LOG_INFO_MSG("Error writing optical module environmentals: location " <<  string(opt._location)  << " return code " << rc);
                                                    }
                                                }


                                
                                                if (!MMCSProperties::getProperty("OPTICALDATA").empty() ) {

                                                    strcpy(optdata._location, mcNCReply._nodeCards[nc]._optics[optical]._lctn.c_str());
                                    
                                                    optdata._error = mcNCReply._nodeCards[nc]._optics[optical]._error;
                                                    optdata._status[0] =  mcNCReply._nodeCards[nc]._optics[optical]._status;
                                                    optdata._status[1] = 0;
                                                    optdata._los = mcNCReply._nodeCards[nc]._optics[optical]._los;
                                                    optdata._losmask = mcNCReply._nodeCards[nc]._optics[optical]._losMask;
                                                    optdata._faults = mcNCReply._nodeCards[nc]._optics[optical]._faults;
                                                    optdata._faultsmask = mcNCReply._nodeCards[nc]._optics[optical]._faultsMask;
                                    
                                                    optdata._alarmstemp = mcNCReply._nodeCards[nc]._optics[optical]._alarmsTemp;
                                                    optdata._alarmstempmask = mcNCReply._nodeCards[nc]._optics[optical]._alarmsTempMask;
                                                    optdata._alarmsvoltage = mcNCReply._nodeCards[nc]._optics[optical]._alarmsVoltage;
                                                    optdata._alarmsvoltagemask = mcNCReply._nodeCards[nc]._optics[optical]._alarmsVoltageMask;

                                                    optdata._alarmsbiascurrent = mcNCReply._nodeCards[nc]._optics[optical]._alarmsBiasCurrent;
                                                    optdata._alarmsbiascurrentmask = mcNCReply._nodeCards[nc]._optics[optical]._alarmsBiasCurrentMask;
                                                    optdata._alarmspower = mcNCReply._nodeCards[nc]._optics[optical]._alarmsPower;
                                                    optdata._alarmspowermask = mcNCReply._nodeCards[nc]._optics[optical]._alarmsPowerMask;
                                                    optdata._temp = mcNCReply._nodeCards[nc]._optics[optical]._temp;
                                                    optdata._voltagev33 = mcNCReply._nodeCards[nc]._optics[optical]._voltageV33;
                                                    optdata._voltagev25 = mcNCReply._nodeCards[nc]._optics[optical]._voltageV25;
                                    
                                                    optdata._eothours = mcNCReply._nodeCards[nc]._optics[optical]._eot;
                                    
                                                    optdata._firmwarelevel = mcNCReply._nodeCards[nc]._optics[optical]._firmwareLevel;
                                                    strcpy(optdata._serialnumber, mcNCReply._nodeCards[nc]._optics[optical]._serialNumber.c_str());
                                                    rc = tx.insert(&optdata);


                                                    strcpy(optchdata._location, mcNCReply._nodeCards[nc]._optics[optical]._lctn.c_str());
                                                    for(unsigned channel = 0 ; channel < mcNCReply._nodeCards[nc]._optics[optical]._OpticalEnvChannel.size() ; ++channel) {
                                                        optchdata._channel = channel;
                                                        optchdata._biascurrent =  mcNCReply._nodeCards[nc]._optics[optical]._OpticalEnvChannel[channel]._biasCurrent;
                                                        optchdata._power = mcNCReply._nodeCards[nc]._optics[optical]._OpticalEnvChannel[channel]._opticalPower;
                                                        rc = tx.insert(&optchdata);
                                        
                                                    }
                                                }
                                
                                            }
                                        }
                        
                        
                                    }  // else leg (_error flag not set for this node card
                        } // for loop that goes through each node card

                        handle++;
                    } // end loop over target set handles

                    // now do the IO board opticals

                    // open target set in monitor mode
                    MCServerMessageSpec::OpenTargetRequest   mcOpenRequest("EnvMonOM", "EnvMonOM", MCServerMessageSpec::RAAW  , true);
                    MCServerMessageSpec::OpenTargetReply     mcOpenReply;
                    server->openTarget(mcOpenRequest, mcOpenReply);

                    if (mcOpenReply._rc != 0) {
                        LOG_INFO_MSG("unable to open target set in mcServer: " << mcOpenReply._rt);
                    } else {
                        // process env data
                        MCServerMessageSpec::ReadIoCardEnvRequest mcIORequest;
                        MCServerAPIHelpers::copyTargetRequest ( mcOpenRequest, mcIORequest );
                        MCServerMessageSpec::ReadIoCardEnvReply mcIOReply;
                        server->readIoCardEnv(mcIORequest, mcIOReply);

                        // close the target set, we have our data
                        MCServerMessageSpec::CloseTargetRequest mcCloseRequest = MCServerAPIHelpers::createCloseRequest( mcOpenRequest, mcOpenReply );
                        MCServerMessageSpec::CloseTargetReply   mcCloseReply;
                        server->closeTarget(mcCloseRequest, mcCloseReply);

                        for(unsigned io = 0 ; io < mcIOReply._ioCards.size() ; io++) {
                            if(isThreadStopping() == true) return 0;
                            if (mcIOReply._ioCards[io]._error == CARD_NOT_PRESENT) { } else  // do nothing if the card is not present
                                if (mcIOReply._ioCards[io]._error == CARD_NOT_UP) { } else       // do nothing if the card is not up
                                    if (mcIOReply._ioCards[io]._error != 0) {
                                        LOG_INFO_MSG("Error occurred reading environmentals from: " << mcIOReply._ioCards[io]._lctn);
                                        RasEventImpl noContact(0x00061004);
                                        noContact.setDetail(RasEvent::LOCATION, mcIOReply._ioCards[io]._lctn.c_str());
                                        RasEventHandlerChain::handle(noContact);
                                        BGQDB::putRAS(noContact);

                                    } else {

                                        for(unsigned optical = 0 ; optical < mcIOReply._ioCards[io]._optics.size() ; ++optical) {
                                            if(isThreadStopping() == true) return 0;
                                            if (mcIOReply._ioCards[io]._optics[optical]._error != 0) {
                                                LOG_INFO_MSG("Error occurred reading environmentals from: " << mcIOReply._ioCards[io]._optics[optical]._lctn);
                                            } else {
                                                strcpy(opt._location, mcIOReply._ioCards[io]._optics[optical]._lctn.c_str());
                                                for(unsigned channel = 0 ; channel < mcIOReply._ioCards[io]._optics[optical]._OpticalEnvChannel.size() ; ++channel) {
                                                    if(isThreadStopping() == true) return 0;
                                                    opt._channel = channel;
                                                    opt._power = mcIOReply._ioCards[io]._optics[optical]._OpticalEnvChannel[channel]._opticalPower;
                                                    rc = tx.insert(&opt);
                                                    if(rc != SQL_SUCCESS) {
                                                        LOG_INFO_MSG("Error writing optical module environmentals: location " <<  string(opt._location)  << " return code " << rc);
                                                    }
                                                }
                                            }
                                        }

                                    } //  else of card in error
                        }  // for loop that goes thru all IO cards
                    }

                    if (mode != bgq::utility::performance::Mode::Value::None) {
                        duration = boost::posix_time::microsec_clock::local_time() - start;
                        perf._duration = static_cast<double>(duration.total_microseconds()) / 1000000;
                        rc = tx.insert(&perf);
                    }

                } else {
                    LOG_INFO_MSG("Subnet failover in progress.  Optical module environmental polling suspended.");
                }
            } catch(std::exception& e) {
                LOG_ERROR_MSG(e.what());
            }
        }  // end check pollEnvs


        //sleep for <seconds>, but wake up every <pollingPeriod> to check if we've been killed
        for (unsigned slept = 0 ; (isThreadStopping() == false) && (slept < seconds) ; slept+= pollingPeriod) {
            sleep(pollingPeriod);
            // check to see if the interval has changed
            unsigned int pollProperty = strtol(MMCSProperties::getProperty(MMCS_ENVS_OPT_INTERVAL_SECONDS).c_str(), NULL, 10);
            
            if (MMCSProperties::getProperty("bypass_envs") == "true")
                pollProperty = 0;
            
            if ((pollProperty >= 60  && pollProperty <= 3600 && (pollProperty != seconds || pollEnvs == false))  ||
                (pollProperty == 0 && pollEnvs == true)) {
                if (pollProperty > 0) {
                    seconds = pollProperty; // only honor valid intervals, 1 - 60 minutes
                    pollEnvs = true;
                    LOG_INFO_MSG( "thread for Optical Modules: interval changed to " << seconds << " seconds" );
                } else {           // don't poll if its 0
                    pollEnvs = false;
                    seconds = ENVS_POLLING_PERIOD;
                    LOG_INFO_MSG( "thread for Optical Modules: not polling" );
                }
            }
        }

        // just in case the performance mode was changed
        if ( mode != bgq::utility::performance::Mode::instance().getMode() ) {
            mode = bgq::utility::performance::Mode::instance().getMode();
        }
    } //  while (isThreadStopping() == false)

    return NULL;
}


//******************************************************************************
// Thread start function for the LocationThread
//******************************************************************************
void*
LocationThread::threadStart()
{
    LOG_INFO_MSG("thread started for location: " << location << ", interval is " << seconds << " seconds");

    MMCSCommandReply rep;
    MCServerRef* temp;
    BlockControllerBase::mcserver_connect(temp, "EnvMonLoc", rep);
    if(!temp) {
        LOG_ERROR_MSG("LocationThread failed to connect to mc_server");
        return NULL;
    }
    const boost::scoped_ptr<MCServerRef> server( temp );

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection())
        {
            LOG_INFO_MSG("unable to connect to database");
            return NULL;
        }

    // make the target set
    MCServerMessageSpec::MakeTargetSetRequest   mcMakeRequest(location,"EnvMonLoc",  true);
    MCServerMessageSpec::MakeTargetSetReply     mcMakeReply;
    mcMakeRequest._expression.push_back(location);
    server->makeTargetSet(mcMakeRequest, mcMakeReply);

    // loop until someone stops us
    while (isThreadStopping() == false)    {

        if(HardwareBlockList::list_size() == 0) {
            // open target set in monitor mode
            MCServerMessageSpec::OpenTargetRequest   mcOpenRequest( location,"EnvMonLoc", MCServerMessageSpec::RAAW  , true);
            MCServerMessageSpec::OpenTargetReply     mcOpenReply;
            server->openTarget(mcOpenRequest, mcOpenReply);

            if (mcOpenReply._rc != 0) {
                LOG_INFO_MSG("unable to open target set in mcServer: " << mcOpenReply._rt);
            } else {

                if (type == "service") {  // check if this is a service card request

                    // read values from cards
                    MCServerMessageSpec::ReadServiceCardEnvRequest mcSCRequest;
                    MCServerAPIHelpers::copyTargetRequest ( mcOpenRequest, mcSCRequest );
                    MCServerMessageSpec::ReadServiceCardEnvReply mcSCReply;
                    server->readServiceCardEnv(mcSCRequest, mcSCReply);

                    // close target set, we have our data
                    MCServerMessageSpec::CloseTargetRequest mcCloseRequest = MCServerAPIHelpers::createCloseRequest( mcOpenRequest, mcOpenReply );
                    MCServerMessageSpec::CloseTargetReply   mcCloseReply;
                    server->closeTarget(mcCloseRequest, mcCloseReply);

                    processSC(&mcSCReply,&tx);

                } else
                    if (type == "node") {

                        // read values from cards
                        MCServerMessageSpec::ReadNodeCardEnvRequest mcNCRequest;
                        MCServerAPIHelpers::copyTargetRequest( mcOpenRequest, mcNCRequest );
                        MCServerMessageSpec::ReadNodeCardEnvReply   mcNCReply;
                        server->readNodeCardEnv(mcNCRequest, mcNCReply);

                        // close target set, we have our data
                        MCServerMessageSpec::CloseTargetRequest mcCloseRequest = MCServerAPIHelpers::createCloseRequest( mcOpenRequest, mcOpenReply);
                        MCServerMessageSpec::CloseTargetReply   mcCloseReply;
                        server->closeTarget(mcCloseRequest, mcCloseReply);

                        processNC(&mcNCReply,&tx);

                    } // if type == node
                    else
                        if (type == "io") {

                            // process env data
                            MCServerMessageSpec::ReadIoCardEnvRequest mcIORequest;
                            MCServerAPIHelpers::copyTargetRequest ( mcOpenRequest, mcIORequest );
                            MCServerMessageSpec::ReadIoCardEnvReply mcIOReply;
                            server->readIoCardEnv(mcIORequest, mcIOReply);

                            // close the target set, we have our data
                            MCServerMessageSpec::CloseTargetRequest mcCloseRequest = MCServerAPIHelpers::createCloseRequest( mcOpenRequest, mcOpenReply );
                            MCServerMessageSpec::CloseTargetReply   mcCloseReply;
                            server->closeTarget(mcCloseRequest, mcCloseReply);

                            processIO(&mcIOReply,&tx);

                        } // if type == io
                        else
                            if (type == "bulk") {

                                // read values from cards
                                MCServerMessageSpec::ReadBulkPowerEnvRequest mcBPRequest;
                                MCServerAPIHelpers::copyTargetRequest ( mcOpenRequest, mcBPRequest );
                                MCServerMessageSpec::ReadBulkPowerEnvReply   mcBPReply;
                                server->readBulkPowerEnv(mcBPRequest, mcBPReply);

                                // close target set, we have our data
                                MCServerMessageSpec::CloseTargetRequest mcCloseRequest = MCServerAPIHelpers::createCloseRequest( mcOpenRequest, mcOpenReply );
                                MCServerMessageSpec::CloseTargetReply   mcCloseReply;
                                server->closeTarget(mcCloseRequest, mcCloseReply);

                                processBulks(&mcBPReply, &tx);

                            } // if type == bulk


            } // we were able to open target set
        } else {
            LOG_INFO_MSG("Subnet failover in progress.  Location specific environmental polling suspended.");
        }

        sleep(seconds);  // sleep for the designated inteval, can be woken up by a signal


    } //  while (isThreadStopping() == false)

    LOG_INFO_MSG("thread ended for location: " << location );


    return NULL;
}

// *******************************************************
// ****************** Location Thread list ***************
// *******************************************************

bool
LocationThreadList::add(LocationThread* loc_thread)
{
    bool success = true;
    PthreadMutexHolder mutex;
    int mutex_rc = mutex.Lock(&_locThreadListMutex);
    if (mutex_rc != 0) return false;

    for (unsigned int loc = 0;  loc < _locThreads.size(); loc++) {
        if (loc_thread->location == _locThreads[loc]->location) {
            success = false;
        }
    }

    if (success) {
        loc_thread->start();
        _locThreads.push_back(loc_thread);
    }


    mutex.Unlock();
    return success;
}

bool
LocationThreadList::remove(const std::string& location)
{
    PthreadMutexHolder mutex;
    bool found = false;
    unsigned int position( 0 );
    int mutex_rc = mutex.Lock(&_locThreadListMutex);
    if (mutex_rc != 0)  return false;
    for (unsigned int loc = 0;  loc < _locThreads.size(); loc++) {
        if (location == _locThreads[loc]->location) {
            _locThreads[loc]->stop(SIGUSR1);
            delete _locThreads[loc];
            position = loc;
            found = true;
        }
    }

    if (found)
        _locThreads.erase(_locThreads.begin() + position);

    mutex.Unlock();
    return found;
}

void
LocationThreadList::list(MMCSCommandReply& reply)
{
    PthreadMutexHolder mutex;

    int mutex_rc = mutex.Lock(&_locThreadListMutex);
    if (mutex_rc != 0) {
        reply << FAIL << ";unable to access list of polled locations";
        return;
    }

    reply << OK;

    for (unsigned int loc = 0;  loc < _locThreads.size(); loc++) {
        reply << "location: " << _locThreads[loc]->location << "  interval: " << _locThreads[loc]->seconds << endl;
    }

    mutex.Unlock();
    return;
}

