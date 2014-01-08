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
# (C) Copyright IBM Corp.  2009, 2012                              
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
from ibm.teal.registry import get_logger, get_service, SERVICE_DB_INTERFACE,\
    SERVICE_ALERT_DELIVERY_Q, SERVICE_ALERT_MGR, SERVICE_ALERT_ANALYZER_Q
from ibm.teal.database import db_interface
from ibm.teal.analyzer.analyzer import AlertAnalyzer
from ibm.teal.location import Location

BGQ_TEAL_ALERT_ANALYZER = 'alert_analyzer.bgqCommonAlertAnalyzer'
BGQ_TEAL_ALERT_ANALYZER_WINDOW_TIME = 'windowTime'
BGQ_TEAL_ALERT_ANALYZER_THRESHOLD = 'threshold'
BGQ_DEFAULT_WINDOW_TIME = 60
BGQ_DEFAULT_THRESHOLD = 2


class bgqCommonAlertAnalyzer(AlertAnalyzer):
    '''The CommonAlertAnalyzer class sends an alert when certain number (threshold) of alerts
    with a common location have been raised within a certain time period.
    '''
    
    def __init__(self, name, inEventQueue, inAlertQueue, outQueue, config_dict=None, number=0):
        ''' The constructor '''
        AlertAnalyzer.__init__(self, name, inEventQueue, inAlertQueue, outQueue, config_dict, number)

        # Common mode alert info
        self.alertId = 'COMMON01'
        self.severity = 'W'
        self.recommendation = "Check the environmental monitor data for LOC_PARENT."
        self.reason = "Multiple alerts have been logged against LOC_NAMEs on LOC_PARENT. The cause may be due to a common mode failure.  Analyze LOC_PARENT environmental data for abnormalities prior to replacing individual LOC_NAMEs."

        # Get alert manager
        self.alertMgr = registry.get_service(registry.SERVICE_ALERT_MGR)

        # Get the configuration info for alert analyzer
        self.cfg = registry.get_service(registry.SERVICE_CONFIGURATION)
        self.window_time = self.get_window_time()
        self.threshold = self.get_threshold()

        # Alert table query
        schema = str(db_interface.TABLE_TEMPLATE).split('.')
        alertTable = schema[0] + '.x_tealalertlog'
        self.query = "select \"event_loc\" from " + alertTable + " where \"event_loc\" like 'PLOC%' and \"event_loc\" not like 'LOCATION' and \"creation_time\" >= current timestamp - WINDOW"
        self.dup_query = "select \"event_loc\" from " + alertTable + " where \"event_loc\" like 'LOC' and \"creation_time\" >= current timestamp - WINDOW"

        return
    

    def will_analyze_event(self, event):
        '''Whether or not to analyze an event
        '''
        # Will not analyze any event
        return False
    

    def will_analyze_alert(self, alert):
        '''Whether or not to analyze an alert
        '''
        # Will not analyze duplicate alert
        alertId = alert.get_rec_id()
        if self.alertMgr.is_duplicate(alertId):
            registry.get_logger().debug('Duplicate alert id ' + str(alertId) + ' is not analyzed.')
            return False

        # Will not analyze alert without hardware location (C: Compute; I: I/O)
        loc_type = alert.event_loc.get_id()
        if loc_type != 'C' and loc_type != 'I':
            registry.get_logger().debug('Alert id ' + str(alertId) + ' with location type ' + loc_type + ' is not analyzed.')
            return False

        # Will analyze all other alerts
        return True
           

    def analyze_event(self, event):
        '''Analyze a RAS event
        '''
        # Should never get here.  If it does, just print an error message
        registry.get_logger().error('Unexpected call to analyze event with recid ' + event.get_rec_id())
        return
    

    def analyze_alert(self, alert):
        '''Analyze an alert
        '''
        alert_recId = alert.get_rec_id()
        loc_type = alert.event_loc.get_id()
        location = alert.event_loc.get_location()
        #alert_msgId = alert.get_incident_id()
        registry.get_logger().info('Analyzing alert id ' +  str(alert_recId) + ', location ' + loc_type + ':' + location)

        # There should only be one condition event associated with the alert.  
        events = alert.condition_events
        if len(events) == 0:
            registry.get_logger().error('No event associated with the alert recid ' + str(alert_recId))
            registry.get_service(SERVICE_ALERT_DELIVERY_Q).put(alert)
            return
        event = events.pop()

        # Get db connection needed for query
        dbi = registry.get_service(SERVICE_DB_INTERFACE)
        dbConn = dbi.get_connection()
        cursor = dbConn.cursor()

        # Get the location 
        loc = Location(loc_type, location)
        locName = self.get_loc_name(loc)

        # No need to analyze alert with rack location
        if locName == 'rack':
            registry.get_logger().info('Nothing to analyze for alert recid ' + str(alert_recId) + ' with rack location')
            registry.get_service(SERVICE_ALERT_DELIVERY_Q).put(alert)
            return

        # Look for a common hardware problem if there are multiple alerts for different location
        # on the same hardware.
        sendAlert = self.has_common_location(loc, self.query, cursor)
        if sendAlert == True:
            # Send alert
            self.send_common_alert(loc, alert_recId, event, cursor)
        else:
            registry.get_logger().info('No common location for ' + location + ' found for alert id ' + str(alert_recId) + ' within the last ' + self.window_time)
            registry.get_service(SERVICE_ALERT_DELIVERY_Q).put(alert)

        return


    def get_window_time(self):
        ''' Get the window time config info for alert analyzer
        '''
        # Get the window time (in seconds)
        windowTime = 1
        try: 
            windowTime = self.cfg.get(BGQ_TEAL_ALERT_ANALYZER, BGQ_TEAL_ALERT_ANALYZER_WINDOW_TIME)
            if int(windowTime) <= 0:
                registry.get_logger().error('The value ' + windowTime + ' specified in the window time is not valid. The value must be greater than zero.')
                raise
            window_time = windowTime + ' SECONDS'
            registry.get_logger().debug('windowTime = ' + window_time)

        except Exception, e:
            registry.get_logger().warn('Configuring window time to default {0} seconds'.format(BGQ_DEFAULT_WINDOW_TIME))
            window_time = str(BGQ_DEFAULT_WINDOW_TIME) + ' SECONDS'

        return window_time


    def get_threshold(self):
        ''' Get the window time config info for alert analyzer
        '''
        # Get the threshold for a hardware location
        try:
            threshold = self.cfg.get(BGQ_TEAL_ALERT_ANALYZER, BGQ_TEAL_ALERT_ANALYZER_THRESHOLD)
            if int(threshold) <= 0:
                registry.get_logger().error('The value ' + threshold + ' specified in the threshold is not valid. The value must be greater than zero.')
                raise
            registry.get_logger().debug(loc + ' = ' + threshold)

        except Exception, e:
            threshold = str(BGQ_DEFAULT_THRESHOLD)
            registry.get_logger().warn('Configuring the threshold to default {0}'.format(threshold))

        return int(threshold)


    def get_loc_name(self, loc):
        ''' Get the name of the location
        '''
        loc_index = len(loc.location_code)
        lastLoc = loc.location_code[loc_index-1]
        for comp in loc.location_info:
            (id, pattern) = loc.location_info[comp].id_info
            if lastLoc[0] == id:
                return comp


    def get_loc_parent(self, loc):
        ''' Get the parent location
        '''
        parent_index = len(loc.location_code) - 1
        if parent_index < 0:
            return None
        parent_loc = ''
        for i in range(parent_index):
            if i == 0:
                parent_loc = loc.location_code[i]
            else:
                parent_loc += '-' + loc.location_code[i]

        return parent_loc


    def send_common_alert(self, loc, cur_alert_recid, event, cursor):
        ''' Send an alert for the common location.
        '''
        # Close current alert prior to creating a new common alert
        registry.get_logger().info('Closing current alert recid ' + str(cur_alert_recid) + ' prior to creating a common mode alert')
        registry.get_service(SERVICE_ALERT_MGR).close(cur_alert_recid)

        # Get the location 
        loc_name = self.get_loc_name(loc)
        loc_type = loc.get_id()
        loc_parent = self.get_loc_parent(loc)
        loc_parent_object = Location(loc_type, loc_parent)

        # Check if there is already an existing alert with the same location.
        # If found, no need to create the same alert, just return
        dup = self.has_duplicate(loc_parent, self.dup_query, cursor)
        if dup == True:
            registry.get_logger().info('Not creating a common alert as there is one or more alerts with the same location ' + loc_parent)
            return

        # Fill in alert info
        reason = self.reason.replace('LOC_NAME', loc_name)
        reason = reason.replace('LOC_PARENT', loc_parent)
        recommendation = self.recommendation.replace('LOC_PARENT', loc_parent)
        alert_dict = {alert.ALERT_ATTR_SEVERITY:self.severity,
                      alert.ALERT_ATTR_URGENCY:'I',
                      alert.ALERT_ATTR_EVENT_LOC_OBJECT:loc_parent_object,
                      alert.ALERT_ATTR_RECOMMENDATION:recommendation,
                      alert.ALERT_ATTR_REASON:reason,
                      alert.ALERT_ATTR_RAW_DATA:'No raw data',
                      alert.ALERT_ATTR_SRC_NAME:self.get_name(),
                      alert.ALERT_ATTR_CONDITION_EVENTS:set((event,))
                      }

        # Get the alert manager to create/allocate/commit the alert
        alertMgr = registry.get_service(registry.SERVICE_ALERT_MGR)
        bg_alert = alertMgr.allocate(self.alertId, in_dict=alert_dict)
        alertMgr.commit(bg_alert, disable_dup=False)

        # Now the alert is created, need to put it in the queue so that it can be analyzed
        # by alert analyzer (instead of sending it - send_alert, which will get reported 
        # through the pipeline right away)
        registry.get_logger().info("Put alertId = " + self.alertId + "  with event recid = " + str(event.get_rec_id()) + " on the alert analyzer queue")
        registry.get_service(SERVICE_ALERT_ANALYZER_Q).put(bg_alert) 
        #self.send_alert(bg_alert)
        return    


    def has_common_location(self, loc, query, cursor):
        ''' Query alerts for the common location to indicate whether or not to send an alert.
        '''
        locParent = self.get_loc_parent(loc)

        # Query for the number of alerts for the same parent's location
        query = query.replace('LOCATION',loc.get_location())
        query = query.replace('PLOC',locParent)
        query = query.replace('WINDOW', self.window_time)
        cursor.execute(query)

        # Send a common alert if ther are alerts with common location
        # Start counting from the current alert (include current alert)
        count = 1
        while cursor.fetchone():
            count += 1
            if count == self.threshold:
                return True

        return False


    def has_duplicate(self, loc, query, cursor):
        ''' Query alerts for the same location.
        '''
        # Query for the number of alerts for the same location
        query = query.replace('LOC',loc)
        query = query.replace('WINDOW', self.window_time)
        cursor.execute(query)

        # Return True if find a match
        row = cursor.fetchone()
        if row:
            return True

        return False
    

    def handle_control_msg(self, control_msg):
        '''Handle any control messages that have been sent. No special action required.
        '''
        registry.get_logger().debug('...Control message received: {0}'.format(control_msg))

