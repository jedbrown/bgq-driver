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

def get_eventList():
    '''Get the list of RAS events with hardware '_in error' control actions.
    '''
    # Search the tbgqmsgtypes for ras events with 'hardware in error' control actions
    schema = str(db_interface.TABLE_TEMPLATE).split('.')
    msgtypesTable = schema[0] + '.' + 'tbgqmsgtypes'
    ctlaction_query = "select msg_id, svcaction from " + msgtypesTable + " where ctlaction is not NULL and ctlaction like '%_IN_ERROR%' and ctlaction not like '%SOFTWARE_IN_ERROR%'"
    dbi = registry.get_service(SERVICE_DB_INTERFACE)
    dbConn = dbi.get_connection()
    cursor = dbConn.cursor()
    cursor.execute(ctlaction_query)
    rows = cursor.fetchall()
    msgIDs = list()
    msgidService = dict()
    for r in rows:
         msgid = r[0].strip() 
         msgIDs.append(msgid)
         sa = 'Service action: ' + str(r[1]).strip()
         msgidService[msgid] = sa

    # add in the bqc serdes analysis events that conditionally set 
    # a service action of COMPUTE_IN_ERROR
    msgIDs.append('00090213')
    msgIDs.append('00090216')
    sa = 'Service action: COMPUTE_IN_ERROR'
    msgidService['00090213'] = sa
    msgidService['00090216'] = sa

    # check for control action overrides defined in the ras_environment_filter.xml
    config_service = registry.get_service('BGQ_CONFIG_SERVICE')
    ras_events = config_service.get('RasEventChangeSpec')
    for re in ras_events:
        if re[1] == 'BG_CTL_ACT':
            if (re[2].find('_IN_ERROR') > 0) and (re[2].find('SOFTWARE_IN_ERROR') < 0):
                msgIDs.append(re[0])
                query = "select svcaction from " + msgtypesTable + " where msg_id = ?"
                cursor.execute(query, str(re[0]))
                row = cursor.fetchone()
                sa = 'Service action: ' + row[0]
                msgidService[re[0]] = sa
            else:
                if re[0] in msgIDs:
                    msgIDs.remove(re[0])

    return msgIDs, msgidService


class bgqHardwareInErrorEventAnalyzer(bgqBaseAnalyzer):

    '''The bgqHardwareInErrorAnalyzer class sends an alert for
    RAS events that cause the hardware to be marked in error.
    '''

    def __init__(self, name, inEventQueue, outQueue, config_dict=None, number=0, checkpoint=None):
        '''The constructor.
        '''
        EventAnalyzer.__init__(self, name, inEventQueue, outQueue, config_dict, number, checkpoint)

        self.severity = "F"
        self.recommendation = "Diagnose hardware that has been placed in an error state. "

        # Get the list of ras events with 'hardware in error' control actions
        self.msgIDs, self.msgidService = get_eventList()

        for msgID in self.msgIDs:
            registry.get_logger().debug('msgId = ' + msgID)

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


    def analyze_event(self, event):
        '''Analyze a RAS event and send an alert.
        '''
        msg_id = event.get_event_id()
        rec_id = event.get_rec_id()
        registry.get_logger().info("Analyzing msgid = " + msg_id + " recid = " + str(rec_id))

        # Exclude event logged from DIAG run
        if event.raw_data['diags'] == 'T':
            registry.get_logger().debug('RAS Event generated by Diagnostics, skip creating an alert')
            return

        # Fill in alert with appropriate data 
        reason = "The hardware been put in an error state.  \nRAS event details:" \
                 " message id = " + msg_id + \
                 ", recid = " + str(rec_id) + \
                 ", timestamp = " + str(event.get_time_occurred()) + \
                 ", serial number = " + str(event.raw_data['serialnumber']) + \
                 ", ecid = " + self.ecidString(event.raw_data['ecid']) + \
                 ", jobid = " + str(event.raw_data['jobid']) + \
                 ", block = " + str(event.raw_data['block'])
        raw_data = "RAS Message: " + event.raw_data['message']
        recommendation = self.recommendation + " " + self.msgidService[msg_id]

        alert_dict = {alert.ALERT_ATTR_SEVERITY:self.severity,
                      alert.ALERT_ATTR_URGENCY:'I',
                      alert.ALERT_ATTR_EVENT_LOC_OBJECT:event.get_src_loc(),
                      alert.ALERT_ATTR_RECOMMENDATION:recommendation,
                      alert.ALERT_ATTR_REASON:reason,
                      alert.ALERT_ATTR_RAW_DATA:raw_data,
                      alert.ALERT_ATTR_SRC_NAME:self.get_name(),
                      alert.ALERT_ATTR_CONDITION_EVENTS:set((event,))
                      }

        # Get the alert manager to create/allocate/commit the alert
        alertMgr = registry.get_service(registry.SERVICE_ALERT_MGR)
        alert_id = 'HWERR01'
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
