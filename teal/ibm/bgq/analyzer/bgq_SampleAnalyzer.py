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

class bgqSampleAnalyzer(bgqBaseAnalyzer):

    '''The bgqSampleAnalyzer class registers for all ras events and
    sends alerts for them as they arrive.
    '''
    def __init__(self, name, inEventQueue, outQueue, config_dict=None, number=0, checkpoint=None):
        '''The constructor.
        '''
        EventAnalyzer.__init__(self, name, inEventQueue, outQueue, config_dict, number, checkpoint)

        # Register to receive all ras events (note that a typical analyzer
        # would be interested in a small subset of the events). 
        msgtypesTable = self.appendSchema('tbgqmsgtypes')
        sample_query = "select msg_id from " + msgtypesTable
        rows = self.executeQuery(sample_query)
        self.msgIDs = list() 
        for r in rows:
            #registry.get_logger().debug('msgId = ' + r[0])
            self.msgIDs.append(r[0])
	
    def will_analyze_event(self, event):
        '''Indicate this analyzer handles all events from tbgqmsgtypes table.
        '''
        event_id = event.get_event_id()
        if event_id in self.msgIDs:
           registry.get_logger().debug('matched event id ' + str(event_id))
           return True
        else:
           registry.get_logger().debug('not matched ' + str(event_id))
           return False


    def analyze_event(self, event):
        '''Analyze a RAS event and send an alert.
        '''
        msg_id = event.get_event_id()
        rec_id = event.get_rec_id()
        registry.get_logger().info("Analyzing msgid = " + msg_id + " recid = " + str(rec_id))

        # get the ras event details 
        dbi = registry.get_service(SERVICE_DB_INTERFACE)
        dbConn = dbi.get_connection()
        cursor = dbConn.cursor()
        location, severity, serialnumber, ecid, event_time, jobid, block, msgText, diags, count = self.rasDetailQuery(cursor, int(rec_id))

	if location == None:
            registry.get_logger().info('No location found, skip creating an alert')
            return

        self.recommendation = 'Sample alert - please ignore.'
        alert_dict = {alert.ALERT_ATTR_SEVERITY:'I',
                      alert.ALERT_ATTR_URGENCY:'N',
                      alert.ALERT_ATTR_EVENT_LOC_OBJECT:event.get_src_loc(),
                      alert.ALERT_ATTR_RECOMMENDATION:self.recommendation,
                      alert.ALERT_ATTR_REASON:'No Reason',
                      alert.ALERT_ATTR_RAW_DATA:'No raw data',
                      alert.ALERT_ATTR_SRC_NAME:self.get_name(),
                      alert.ALERT_ATTR_CONDITION_EVENTS:set((event,))
                      }

        # Get the alert manager to create/allocate/commit the alert
        alertMgr = registry.get_service(registry.SERVICE_ALERT_MGR)
        alert_id = 'SAMPLE01'
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
