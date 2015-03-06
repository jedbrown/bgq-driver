#! /usr/bin/python
# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# ================================================================ 
#                                                                  
# Licensed Materials - Property of IBM                             
#                                                                  
# Blue Gene/Q                                                      
#                                                                  
# (C) Copyright IBM Corp.  2009, 2011                              
#                                                                  
# US Government Users Restricted Rights -                          
# Use, duplication or disclosure restricted                        
# by GSA ADP Schedule Contract with IBM Corp.                      
#                                                                  
# This software is available to you under the                      
# Eclipse Public License (EPL).                                    
#                                                                  
# ================================================================ 
#                                                                  
# end_generated_IBM_copyright_prolog                               

from ibm.teal import registry, alert
from ibm.teal.analyzer.analyzer import EventAnalyzer
from ibm.teal.registry import get_logger, get_service, SERVICE_DB_INTERFACE
from ibm.bgq.analyzer.bgq_BaseAnalyzer import bgqBaseAnalyzer
from ibm.teal.database import db_interface
from datetime import datetime
from datetime import timedelta
import re
import subprocess


BGQ_TEAL_THRESHOLD_ANALYZER = 'event_analyzer.bgqThresholdExceededEventAnalyzer'
BGQ_TEAL_THRESHOLD_EXCLUDE_IDS = 'excludeMsgIDs'

def get_eventList():
    '''Get the list of RAS events with thresdhold count.
    '''
    # Get the exclude list of message IDs from the configuration file
    excludeMsgList = get_excludeMsgIDs()

    # Search the tbgmsgtypes for ras events that have threshold counts
    # Note: do not include ras events already handled by HardwareInError and JobFatal analyzers.
    schema = str(db_interface.TABLE_TEMPLATE).split('.')
    msgtypesTable = schema[0] + '.' + 'tbgqmsgtypes'
    count_query = "select msg_id, thresholdcount, svcaction, relevantdiags from " + msgtypesTable + " where thresholdcount is not NULL and (ctlaction is NULL or (ctlaction not like '%END_JOB%' and (ctlaction not like '%_IN_ERROR%' or ctlaction like '%SOFTWARE_IN_ERROR%')))"
    dbi = registry.get_service(SERVICE_DB_INTERFACE)
    dbConn = dbi.get_connection()
    cursor = dbConn.cursor()
    cursor.execute(count_query)
    rows = cursor.fetchall()
    msgIDs = list()
    msgidCount = dict()
    msgidService = dict()
    for r in rows:
        msgid = r[0].strip()
        if excludeMsgList.find(msgid) >= 0:
            registry.get_logger().debug(' excluding ' + msgid)
            continue
	msgIDs.append(msgid)
        msgidCount[msgid] = r[1]
        sa = 'Service action: '
        if r[2]:
            sa += r[2].strip()
        else:
            sa += "None."
        if r[3]:
            sa += ' Relevant diagnostic bucket(s): ' + r[3].strip()
        msgidService[msgid] = sa

    # Search the tbgqmsgtypes for ras events that have threshold period
    period_query = "select msg_id, thresholdperiod from " + msgtypesTable + " where thresholdperiod is not NULL"
    cursor.execute(period_query)
    rows = cursor.fetchall()
    msgidPeriod = dict()
    for r in rows:
        msgidPeriod[r[0]] = r[1]

    return msgIDs, msgidService, msgidCount, msgidPeriod


def get_excludeMsgIDs():
    '''Get the list of excluded message IDs.
    '''
    cfg = registry.get_service(registry.SERVICE_CONFIGURATION)
    excludeMsgIDs = ''
    try: 
        excludeMsgIDs = cfg.get(BGQ_TEAL_THRESHOLD_ANALYZER, BGQ_TEAL_THRESHOLD_EXCLUDE_IDS)
        registry.get_logger().debug('Exclude List = ' + excludeMsgIDs)
    except Exception, e:
        registry.get_logger().debug(e)

    return excludeMsgIDs

class bgqThresholdExceededEventAnalyzer(bgqBaseAnalyzer):

    '''The ThresholdExceededAnalyzer class determines if RAS event
    thresholds have been reached or execeeded.
    '''
    
    def __init__(self, name, inEventQueue, outQueue, config_dict=None, number=0, checkpoint=None):
        '''The constructor.
        '''
        EventAnalyzer.__init__(self, name, inEventQueue, outQueue, config_dict, number, checkpoint)

        self.severity = "W"
        self.recommendation = "Diagnose the problem that caused the threshold to be reached or exceeded. "
        self.alert_id = 'THRESH01'

        # Get the list of ras events that have threshold counts
        self.msgIDs, self.msgidService, self.msgidCount, self.msgidPeriod = get_eventList()

        for msgID in self.msgIDs:
            registry.get_logger().debug('msgId = ' + msgID)

        # define query for count with no period specified
        eventTable = self.appendSchema('tbgqeventlog')
        self.count_query = "select count(*) from " + eventTable + " where msg_id = ? and location LOC and serialnumber SN and event_time <= ?"
        self.count_query2 = "select sum(bigint(count)) from " + eventTable + " where msg_id = ? and location LOC and serialnumber SN and event_time <= ?"

        # define query for count exceeded with period specified
        self.period_query = "select count(*) from " + eventTable + " where msg_id = ? and location LOC and serialnumber SN and event_time <= ? and event_time > (timestamp('MYTIME') - PERIOD)"
        self.period_query2 = "select sum(bigint(count)) from " + eventTable + " where msg_id = ? and location LOC and serialnumber SN and event_time <= ? and event_time > (timestamp('MYTIME') - PERIOD)"

        # the number of consecutive periods that the threshold has to exceed
        self.msgidConsecutivePeriods = dict()
        self.msgidConsecutivePeriods['0008002F'] = 7
        self.msgidConsecutivePeriods['00080030'] = 3
        
        return

 


    def will_analyze_event(self, event):
        '''Indicate this analyzer handles all events from tbgqmsgtypes table.
        '''
        event_id = event.get_event_id()
        if event_id in self.msgIDs:
           registry.get_logger().debug('matched event id ' + event_id + ' ' + str(event.get_rec_id()))
           return True
        else:
           registry.get_logger().debug('not matched ' + event_id + ' ' + str(event.get_rec_id()))
           return False

    def delta_period(self, period):
        pindex = period.find(' ')
        pdigit=''
        punits=''
        if pindex != -1:
            pdigit = period[0:pindex]
            punits = period[(pindex+1):]
        # registry.get_logger().error("delta_period digit=" + pdigit + ", unit=" + punits)
        if punits == 'MONTH' or punits == 'MONTHS' or punits == 'month' or punits == 'months':
            return timedelta(months=int(pdigit))
        if punits == 'DAY' or punits == 'DAYS' or punits == 'day' or punits == 'days':
            return timedelta(days=int(pdigit))
        if punits == 'MINUTE' or punits == 'MINUTES' or punits == 'minute' or punits == 'minutes':
            return timedelta(minutes=int(pdigit))
        if punits == 'SECOND' or punits == 'SECONDS' or punits == 'second' or punits == 'seconds':
            return timedelta(seconds=int(pdigit))
        if punits == 'MICROSECOND' or punits == 'MICROSECONDS' or punits == 'microsecond' or punits == 'microseconds':
            return timedelta(microseconds=int(pdigit))
        registry.get_logger().error("No timedelta possible for " + period)
        return None
    


    def analyze_event(self, event):
        '''Analyze a RAS event and determine whether threshold has been
        reached or exceeded.
        '''
        msg_id = event.get_event_id()
        rec_id = event.get_rec_id()
        registry.get_logger().info("Analyzing msgid = " + msg_id + " recid = " + str(rec_id))

        count = event.get_event_cnt()
        event_time = event.get_time_logged()

        location = str(event.get_src_loc())
        location = location[3:].strip()
        # Exclude event logged from DIAG run
        if event.raw_data['diags'] == 'T':
            registry.get_logger().debug('RAS Event generated by Diagnostics, skip creating an alert')
            return

        # Set threshold value
        threshold = self.msgidCount[msg_id]
        tmsg = "Error threshold of " + str(threshold) + " has been reached or exceeded, total count is "
        
        # Check if threshold has been reached or exceeded
        xmsg = ""
        query = self.period_query
        if msg_id in self.msgidPeriod:
            # Query for the count of the RAS event with threhold period
            xmsg = " in a period of " + self.msgidPeriod[msg_id].strip()
            query = self.period_query
            if count:
                query = self.period_query2
            query = query.replace('PERIOD',self.msgidPeriod[msg_id].strip())
            query = query.replace('MYTIME', str(event_time))
        else:
            # Query for the count of the RAS event without threshold period
            query = self.count_query
            if count:
                query = self.count_query2
        
        if event.raw_data['serialnumber'] is not None:
            serialnumber = event.raw_data['serialnumber'].strip()
            sn = "= '" + serialnumber + "'"
            query = query.replace('SN', sn)
        else:
            serialnumber = None
            query = query.replace('SN', 'is NULL')
        if location:
            loc = "= '" + location + "'"
            query = query.replace('LOC', loc)
        else:
            query = query.replace('LOC', 'is NULL')
        
        registry.get_logger().debug(query + " msgId=" + msg_id + " event_time=" + str(event_time)) 

        dbi = registry.get_service(SERVICE_DB_INTERFACE)
        dbConn = dbi.get_connection()
        cursor = dbConn.cursor()
        cursor.execute(query, msg_id, event_time)
        row = cursor.fetchone()
        msgCount = row[0]

        if msgCount < threshold:
            registry.get_logger().info("Alert is not sent for msgid " + msg_id + " recid " + str(rec_id) + " because the count " + str(msgCount) + " is less than the threshold " + str(threshold) + ".")
            return

        if msg_id in self.msgidConsecutivePeriods:
            # repeat the query for M-1 more periods
            numPeriods = self.msgidConsecutivePeriods[msg_id]
            numPeriods = numPeriods - 1
            period = self.msgidPeriod[msg_id].strip()
            period = period.strip()
            deltaPeriod = self.delta_period(period)
            registry.get_logger().debug("Checking whether " + msg_id + " recid " + str(rec_id) + " has exceeded its threshold for " + str(self.msgidConsecutivePeriods[msg_id]) + " consecutive periods of " + period + ". Delta period = " + str(deltaPeriod))
            qry_time = event_time
            for nums in range(numPeriods):
                query = self.period_query2
                if serialnumber:
                    sn = "= '" + serialnumber + "'"
                    query = query.replace('SN', sn)
                else:
                    query = query.replace('SN', 'is NULL')
                if location:
                    loc = "= '" + location + "'"
                    query = query.replace('LOC', loc)
                else:
                    query = query.replace('LOC', 'is NULL')
                query = query.replace('PERIOD',period)
                qry_time = qry_time - deltaPeriod
                query = query.replace('MYTIME', str(qry_time))
                registry.get_logger().debug(query + " msgId=" + msg_id + " event_time=" + str(qry_time)) 
                cursor.execute(query, msg_id, qry_time)
                row = cursor.fetchone()
                msgCount = row[0]
                if msgCount < threshold:
                    registry.get_logger().info("Alert is not sent for msgid " + msg_id + " recid " + str(rec_id) + " because the count " + str(msgCount) + " is less than the threshold " + str(threshold) + ".  period=" + str(nums+2) + " of " + str(numPeriods+1))
                    return
                else:
                    registry.get_logger().debug("Threshold exceeded " + msg_id + " recid " + str(rec_id) + " for consecutive period " + str(nums+2) + " of " + str(numPeriods+1))

        msgText = event.raw_data['message'].strip()
        if msg_id == '00040020':
            skipAlert = False
            index = msgText.find('StatusWord=0x0002')
            if (index >= 0):
                skipAlert = True
            else:
                index = msgText.find('StatusWord=0x4001')
                if (index >= 0):
                    skipAlert = True
                else:
                    index = msgText.find('StatusWord=0x4005')
                    if (index >= 0):
                        skipAlert = True
            if (skipAlert):
                registry.get_logger().debug("Supressing alert for " + msg_id + " because " + msgText[index:index+len('StatusWord=0x0002')])
                return
 

        tmsg = tmsg + str(msgCount) + xmsg;
        reason = tmsg + "\nRAS event details:" \
                 " message id = " + msg_id + \
                 ", recid = " + str(rec_id) + \
                 ", timestamp = " + str(event.get_time_occurred()) + \
                 ", serial number = " + str(event.raw_data['serialnumber']) + \
                 ", ecid = " + self.ecidString(event.raw_data['ecid']) + \
                 ", jobid = " + str(event.raw_data['jobid']) + \
                 ", block = " + str(event.raw_data['block'])

        rasMessage = "RAS Message: " + msgText

        recommendation = self.recommendation
        if location:
            recommendation = recommendation + " Schedule part replacement if this is hardware problem. " + self.msgidService[msg_id]
        else:
            recommendation = recommendation + " " + self.msgidService[msg_id]

        alert_dict = {alert.ALERT_ATTR_SEVERITY:self.severity,
                      alert.ALERT_ATTR_URGENCY:'I',
                      alert.ALERT_ATTR_EVENT_LOC_OBJECT:event.get_src_loc(),
                      alert.ALERT_ATTR_RECOMMENDATION:recommendation,
                      alert.ALERT_ATTR_REASON:reason,
                      alert.ALERT_ATTR_RAW_DATA:rasMessage,
                      alert.ALERT_ATTR_SRC_NAME:self.get_name(),
                      alert.ALERT_ATTR_CONDITION_EVENTS:set((event,))
                      }

        # Get the alert manager to create/allocate/commit the alert
        alertMgr = registry.get_service(registry.SERVICE_ALERT_MGR)
        alert_id = 'THRES01'
        bg_alert = alertMgr.allocate(alert_id, in_dict=alert_dict)
        alertMgr.commit(bg_alert, disable_dup=False)

        # Now the alert is created and can be reported through the pipeline
        registry.get_logger().info("Sending alert for msgid = " + msg_id + " recid = " + str(rec_id))
        self.send_alert(bg_alert)
        return


    def handle_control_msg(self, control_msg):
        ''' Handle any control messages that have been sent. No special action
        required
        '''
        registry.get_logger().debug('...Control message received: {0}'.format(control_msg))
