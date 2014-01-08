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
import subprocess

BGQ_TEAL_BQL_ANALYZER = 'event_analyzer.bgqBqlEventAnalyzer'
BGQ_TEAL_BQL_EXCLUDE_IDS = 'excludeMsgIDs'

def get_eventList():
    '''Get the list of RAS events to be analyzed by this analzyer.
    '''
    msgIDs = list()
    msgIDs.append('00090200')
    msgIDs.append('00090201')
    msgIDs.append('00090202')
    msgIDs.append('00090210')
    msgIDs.append('00090211')

    return msgIDs

class bgqBqlEventAnalyzer(bgqBaseAnalyzer):

    '''The BqlEventAnalyzer class determines what action to take 
    for BQL RAS events of interest.
    '''
    
    def __init__(self, name, inEventQueue, outQueue, config_dict=None, number=0, checkpoint=None):
        '''The constructor.
        '''
        EventAnalyzer.__init__(self, name, inEventQueue, outQueue, config_dict, number, checkpoint)

        self.severity = "W"
        self.recommendation = '''Schedule service to isolate the BQL issue.  Possible causes are environmental, cable, or a board.  Multiple BQLs reporting issues on multiple boards in the same midplane may be a side effect of an environmental issue like neighboring boards being powered off unexpectedly.  Issues with a single cable may be due to poor seating of the cable into the connector or debris on the cable.   The cable and board can be cleaned with an optics cleaning tool.  Low voltage on all or most lanes on a single optical module may be an issue with the board at this location (the receiver) or its neighboring board (the transmitter) at the other end of the cable.'''
        self.alert_id = 'BQL01'

        # Get the exclude list of message IDs
        cfg = registry.get_service(registry.SERVICE_CONFIGURATION)
        excludeList = ''
        try: 
            excludeList = cfg.get(BGQ_TEAL_BQL_ANALYZER, excludeMsgIDs)
            registry.get_logger().debug('Exclude List = ' + excludeList)
        except Exception, e:
            registry.get_logger().debug(e)

        # ras events that have BQL_SPARE detail data
        self.msgIDs = get_eventList()
        for msgid in self.msgIDs:
            registry.get_logger().debug('msgId = ' + msgid)

        # set the threshold
        self.msgidCount = dict()
        self.msgidCount['00090200'] = 2
        self.msgidCount['00090201'] = 1
        self.msgidCount['00090202'] = 1
        self.msgidCount['00090210'] = 4
        self.msgidCount['00090211'] = 4

        # set the window = 2 X the period 
        self.msgidPeriod = dict()
        self.msgidPeriod['00090200'] = '11 seconds'
        self.msgidPeriod['00090201'] = '11 seconds'
        self.msgidPeriod['00090202'] = '11 seconds'
        self.msgidPeriod['00090210'] = '11 seconds'
        self.msgidPeriod['00090211'] = '11 seconds'

        # BQL related ras events
        self.bqlIDs = list() 

        # define query for count of recent events at this location
        # within a window (plus and minus the event time)
        #    parameter 1 = location
        #    parameter 2 = event time
        eventTable = self.appendSchema('tbgqeventlog')
        self.period_query = "select count(*) from " + eventTable + " where location like ? and category='BQL' and event_time <=  (timestamp('MYTIME') + PERIOD) and event_time > (timestamp('MYTIME') - PERIOD)"

        # define query for count of open alerts at this location
        # within a day from the event time
        #    parameter 1 = location
        #    parameter 2 = event time
        alertTable = self.appendSchema('x_tealalertlog')
        self.alert_period = '1 day'
        self.alert_query = "select count(*) from " + alertTable + " where \"alert_id\"='BQL01' and \"event_loc\"= ? and \"creation_time\" >= (timestamp('MYTIME') - PERIOD) and \"state\"=1"

        # database connection and cursor
        dbi = registry.get_service(SERVICE_DB_INTERFACE)
        self.dbConn = dbi.get_connection()
        self.cursor = self.dbConn.cursor()

        return

    def will_analyze_event(self, event):
        '''Indicate this analyzer handles certain BQL events.
        '''
        event_id = event.get_event_id()
        if event_id in self.msgIDs:
           registry.get_logger().debug('matched event id ' + event_id + ' ' + str(event.get_rec_id()))
           return True
        else:
           registry.get_logger().debug('not matched ' + event_id + ' ' + str(event.get_rec_id()))
           return False


    def analyze_event(self, event):
        '''Analyze a RAS event and determine whether the BQL threshold of errors has been reached or exceeded.
        '''
        msg_id = event.get_event_id()
        rec_id = event.get_rec_id()
        registry.get_logger().info("Analyzing msgid = " + msg_id + " recid = " + str(rec_id))

        location = str(event.get_src_loc())
        location = location[3:].strip()
        severity = event.raw_data['severity'].strip()
        serialnumber = event.raw_data['serialnumber']
        ecid = event.raw_data['ecid']
        event_time = event.get_time_logged()
        block = event.raw_data['block'].strip()
        jobid = event.raw_data['jobid']
        msgText = event.raw_data['message'].strip()
        rawdata = event.raw_data['rawdata'].strip()
        count = event.get_event_cnt()
        
        # Set threshold value
        threshold = self.msgidCount[msg_id]
        tmsg = "BQL error threshold of " + str(threshold) + " has been reached or exceeded, total count is "
        
        # check if thresholds have been reached or exceeded for events 
        xmsg = ""
        xmsg = " in a period of " + self.msgidPeriod[msg_id].strip()
        query = self.period_query.replace('PERIOD',self.msgidPeriod[msg_id].strip())
        query = query.replace('MYTIME', str(event_time))

        # search for events associated with this location's midplane or I/O board
        qryloc = location.strip()[0:6] + '%'
        registry.get_logger().debug(query + " xmsgId=" + msg_id + " loc=" + qryloc + " ev_time=" + str(event_time))
        
        msgCount = 0
        for x in range(5):
            try:
                self.cursor.execute(query, qryloc)
                row = self.cursor.fetchone()
                msgCount = row[0]
                break
            except Exception, e:
                registry.get_logger().debug(e)
                if x < 4:
                    dbi = registry.get_service(SERVICE_DB_INTERFACE)
                    self.dbConn = dbi.get_connection()
                    self.cursor = self.dbConn.cursor()
                else:
                    raise Exception('Error: bgq_BqlEventAnalyzer could not connect to the database')

        if msgCount < threshold:
            if msg_id == '00090200':
                registry.get_logger().info("The optical lane will be spared since only " + str(msgCount) + " BQL event(s) were logged during the window.")
                # perform the BQL sparing action
                self.perform_sparing(rec_id, location, rawdata)
                return

        aquery = self.alert_query.replace('PERIOD',self.alert_period)
        aquery = aquery.replace('MYTIME', str(event_time))
        registry.get_logger().debug(aquery + " xmsgId=" + msg_id + " loc=" + location.strip() + " ev_time=" + str(event_time))

        msgCount = 0
        for x in range(5):
            try:
                self.cursor.execute(aquery,location.strip())
                row = self.cursor.fetchone()
                msgCount = row[0]
                break
            except Exception, e:
                registry.get_logger().debug(e)
                if x < 4:
                    dbi = registry.get_service(SERVICE_DB_INTERFACE)
                    self.dbConn = dbi.get_connection()
                    self.cursor = self.dbConn.cursor()
                else:
                    raise Exception('Error: bgq_BqlEventAnalyzer could not connect to the database')


        # do not log more than one BQL alert per day for the same location 
        if msgCount > 0:
            registry.get_logger().debug("An active BQL01 alert for location " + location.strip() + " exist within a period of " + self.alert_period + ". Skip logging a duplicate.")
            return

        tmsg = tmsg + str(msgCount) + xmsg;
        reason = tmsg + "\nRAS event details:" \
                 " message id = " + msg_id + \
                 ", recid = " + str(rec_id) + \
                 ", timestamp = " + str(event_time) + \
                 ", serial number = " + str(serialnumber) + \
                 ", ecid = " + self.ecidString(ecid) + \
                 ", jobid = " + str(jobid) + \
                 ", block = " + str(block)
        raw_data = "RAS Message: " + msgText

        alert_dict = {alert.ALERT_ATTR_SEVERITY:self.severity,
                      alert.ALERT_ATTR_URGENCY:'I',
                      alert.ALERT_ATTR_EVENT_LOC_OBJECT:event.get_src_loc(),
                      alert.ALERT_ATTR_RECOMMENDATION:self.recommendation,
                      alert.ALERT_ATTR_REASON:reason,
                      alert.ALERT_ATTR_RAW_DATA:raw_data,
                      alert.ALERT_ATTR_SRC_NAME:self.get_name(),
                      alert.ALERT_ATTR_CONDITION_EVENTS:set((event,))
                      }

        # Get the alert manager to create/allocate/commit the alert
        alertMgr = registry.get_service(registry.SERVICE_ALERT_MGR)
        alert_id = 'BQL01'
        bg_alert = alertMgr.allocate(alert_id, in_dict=alert_dict)
        alertMgr.commit(bg_alert, disable_dup=False)

        # Now the alert is created and can be reported through the pipeline
        registry.get_logger().info("Sending alert for msgid = " + msg_id + " recid = " + str(rec_id))
        self.send_alert(bg_alert)
        return

    def perform_sparing(self, rec_id, location, rawdata): 
        # extract the mask and register from the message details
        mask = '0x000'
        mindex = rawdata.find('Mask=') 
        if (mindex >= 0):
            mindex = mindex +  len('Mask=')
            mask = rawdata[mindex:mindex+5]
        register = 'C23'
        rindex = rawdata.find('Register=') 
        if (rindex >= 0):
            rindex = rindex +  len('Register=')
            register = rawdata[rindex:rindex+3]
        # log a ras event to have the BQL lane spared
        command = list()
        command.append('/bgsys/drivers/ppcfloor/sbin/mc_server_log_ras')
        command.append('--location')
        command.append(location)
        command.append('--message-id')
        command.append('0x0009020D')
        command.append('--action')
        command.append('BQL_SPARE')
        command.append('--detail')
        m = 'Mask=' + mask
        command.append(m)
        command.append('--detail')
        r = 'Register=' + register
        command.append(r)
        command.append('--detail')
        command.append('Submitter=TEAL')
        command.append('--detail')
        command.append('Associated_Rec_Id=' + str(rec_id))
        registry.get_logger().debug(command) 
        subprocess.call(command)
        return

    def handle_control_msg(self, control_msg):
        ''' Handle any control messages that have been sent. No special action
        required
        '''
        registry.get_logger().debug('...Control message received: {0}'.format(control_msg))
