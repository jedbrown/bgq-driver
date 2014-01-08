#!/usr/bin/env python
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
# (C) Copyright IBM Corp.  2011, 2011                              
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

import time
import signal
import threading
import optparse
import socket
from datetime import datetime
from threading import Thread

from ibm.teal import teal
from ibm.teal import registry
from ibm.teal.util import command
from ibm.teal.database import db_interface
from ibm.teal.monitor import teal_semaphore
from ibm.bgq import pyrealtime
from ibm.bgq.analyzer import bgq_HardwareInErrorAnalyzer
from ibm.bgq.analyzer import bgq_JobFatalRasAnalyzer
from ibm.bgq.analyzer import bgq_ThresholdExceededAnalyzer
from ibm.bgq.analyzer import bgq_BqlEventAnalyzer
from ibm.bgq.analyzer import bgq_BqcSerdesAnalyzer

COMMIT_LIMIT = 10000
BGQ_RAW_DATA_FMT =  0x4247510080010001

BGQ_TEAL_CONFIG = 'connector.bgq'
BGQ_TEAL_CONFIG_POLL_INTERVAL = 'poll_interval'
BGQ_DEFAULT_POLL_INTERVAL = 60
        
def next_row(cursor):
    ''' Helper function to iterate through query results 
    '''
    return cursor # pyodbc-ism

class BgqConnector(threading.Thread):
    def __init__(self):
        ''' Constructor
        '''
        self.running = True
        self.last_row_processed = 0
        self.first_realtime_event = False
        self.notifier = teal_semaphore.Semaphore() 
        self._configure()
        self._getEventList()
        threading.Thread.__init__(self)

    def rt_callback(self,recid,msgid):
        '''Realtime callback function.
        '''
        # In case some RAS events occurred before the real-time server could be started ...
        if self.first_realtime_event:
            # Not sure this is needed?  This should be current from the previous call to query_and_log_event
#            self._get_last_processed_event()
            self._query_and_log_event(">", self.last_processed_event, recid)
            self.first_realtime_event = False
            
        registry.get_logger().debug("in rt_callback " + str(recid) + " " + msgid)
        self._query_and_log_event("=", recid)
        return

    def rt_term_callback(self):
        '''Realtime callback function.
        '''
        registry.get_logger().debug("in rt_term_callback")
        return
    
    def _configure(self):
        # Set the polling time based on the BGQ Connector conf file
        cfg = registry.get_service(registry.SERVICE_CONFIGURATION)
        try:
            value = cfg.get(BGQ_TEAL_CONFIG, BGQ_TEAL_CONFIG_POLL_INTERVAL)
            self.poll_interval = int(value)
            if self.poll_interval <= 0:
                registry.get_logger().error('The value ' + str(self.poll_interval) + ' specified in the poll interval is not valid. The value must be greater than zero.')    
                raise 
        except:
            registry.get_logger().warn('Configuring poll interval to default {0} seconds'.format(BGQ_DEFAULT_POLL_INTERVAL))            
            self.poll_interval = BGQ_DEFAULT_POLL_INTERVAL

    def _getEventList(self):
        ''' Get the list of RAS events to be analyzed
        '''
        self.msgIDs, msgidService = bgq_HardwareInErrorAnalyzer.get_eventList()
        msgIDsEndJob, msgidService = bgq_JobFatalRasAnalyzer.get_eventList()
        self.msgIDs.extend(msgIDsEndJob)
        msgIDsThreshold, msgidService, msgidCount, msgidPeriod = bgq_ThresholdExceededAnalyzer.get_eventList()
        self.msgIDs.extend(msgIDsThreshold)
        self.msgIDs.extend(bgq_BqlEventAnalyzer.get_eventList())
        self.msgIDs.extend(bgq_BqcSerdesAnalyzer.get_eventList())
        self.msgIDs = list(set(self.msgIDs))
        self.msgIDs.sort()

        self.filter = ''
        first = True
        for msgid in self.msgIDs:
            if first:
                self.filter += '(' + msgid + ')'
                first = False
            else:
                self.filter += '|(' + msgid + ')'

    def stop(self):
        ''' Tell the thread to shutdown cleanly
        '''
        self.running = False

    def _log_event(self, bgq_info, teal_cursor):
        ''' Create the TEAL event log entry
        '''
        rec_id = bgq_info[0]

        if bgq_info[1]:
            category = bgq_info[1].strip()
        else:
            category = None

        if bgq_info[2]:
            component = bgq_info[2].strip()
        else:
            component = None

        if bgq_info[3]:
            jobid = bgq_info[3]
        else:
            jobid = None

        if bgq_info[4]:
            block = bgq_info[4].strip()
        else:
            block = None

        if bgq_info[5]:
            location = bgq_info[5].strip()
        else:
            location = None

        if bgq_info[6]:
            msgid = bgq_info[6]
        else:
            msgid = None

        # Location overrides everything. If the location was specified and was not the empty string -- use it
        if location:
            # I/O Rack
            if location.startswith('Q'):
                loc_type = 'I' # I/O Rack Location Type
            # Compute Rack
            elif location.startswith('R'):
                loc_type = 'C' # Compute Rack Location Type
            else:
                # Oops -- No idea
                registry.get_logger().warn('Event {0} not logged. Invalid location specified: {1}.'.format(rec_id, location))
                return
        else:
            # No location -- will have to guess based on other parms
            if category == 'Job':
                loc_type = 'J'

                # The block the job was running on may be important so tack it
                # on to the job location code if it exists
                if jobid:
                    if block:
                        location = '{0}##{1}'.format(jobid,block)
                    else:
                        location = jobid
                else:
                    # Job category but with no job id specified
                    registry.get_logger().warn('Event {0} not logged. No location specified.'.format(rec_id))
                    return
            else:
                # Not a job so assume that it was reported by the management server that we are running on
                loc_type = 'A'
                location = socket.gethostname()
       
        # Insert the additional data for TEAL 
        registry.get_logger().info('Logging event with recid {0}  msgid {1}'.format(rec_id, msgid))
        teal_insert = "INSERT INTO " + db_interface.TABLE_EVENT_LOG_EXT + "(REC_ID, RAW_DATA_FMT, SRC_LOC_TYPE, SRC_LOC) VALUES(?, ?, ?, ?)"
        teal_cursor.execute(teal_insert, rec_id, BGQ_RAW_DATA_FMT, loc_type, location)

    def _get_last_processed_event(self):
        ''' Log events that have occurred prior to starting the monitor
        '''
        dbi = registry.get_service(registry.SERVICE_DB_INTERFACE)
        cnxn = dbi.get_connection()
        cursor = cnxn.cursor()

        # Find the last event injected into TEAL and then inject
        # all the events that have occurred since then
        maxEvent_query = "SELECT MAX(REC_ID) FROM " + db_interface.TABLE_EVENT_LOG_EXT
        cursor.execute(maxEvent_query);
        
        max_id = cursor.fetchone()[0]
        if max_id is None:
            self.last_processed_event = 0
        else:
            self.last_processed_event = max_id

        registry.get_logger().info('Last Processed Event = ' + str(self.last_processed_event))
        cnxn.close()

    def _query_and_log_event(self, query_sign, recid, max_recid=0):
        ''' Query the BG event log for new events and log into TEAL
        '''
        registry.get_logger().debug("in _query_and_log_event")
        event_logged = False
        db = registry.get_service(registry.SERVICE_DB_INTERFACE)
        cnxn = db.get_connection()
        bgq_cursor = cnxn.cursor()
        teal_cursor = cnxn.cursor()
            
        # Query the BG event log for new events
        bgEvent_query = "SELECT RECID, CATEGORY, COMPONENT, JOBID, BLOCK, LOCATION, MSG_ID FROM " + db_interface.TABLE_BG_EVENT_LOG + " WHERE RECID " + query_sign + " ? ORDER BY RECID ASC"
        bgq_cursor.execute(bgEvent_query, recid)
        commit_count = 0
        for bg_event in next_row(bgq_cursor):
            
            # Don't process events with recids >= max_recid, if it is nonzero
            if max_recid > 0 and bg_event[0] >= max_recid:
                break
                
            # Log only events we are interested in
            if bg_event[6] in self.msgIDs:
                event_logged = True
                
                # Log the event into TEAL
                self._log_event(bg_event, teal_cursor)
                      
                # Commit every so often to limit the transaction size
                commit_count += 1
                if commit_count == COMMIT_LIMIT:           
                    cnxn.commit()
                    commit_count = 0
            else:
                registry.get_logger().debug('ignore msgid ' + bg_event[6])

            # Update the 'cursor' into the BGQ database
            self.last_processed_event = bg_event[0]
                    
        # Notify TEAL that events have been inserted
        if (event_logged):
            registry.get_logger().debug("event to log " + str(event_logged))
            cnxn.commit()
                
            if self.notifier:
                self.notifier.post()
            else:
                registry.get_logger().warn('TEAL notifier not configured.')
        
        cnxn.close()
        registry.get_logger().debug("exit _query_and_log_event")
        
    def _periodic_monitor(self):
        ''' Runs the monitor thread waiting for new events to occur
        '''
        registry.get_logger().debug("in periodic monitor " + str(self.last_processed_event))

        # Wait for the next polling iteration
        time.sleep(self.poll_interval)
        self._get_last_processed_event()
        self._query_and_log_event(">", self.last_processed_event)

    def run(self):
        ''' Runs the monitor thread waiting for new events to occur
        '''
        self._get_last_processed_event()
        self._query_and_log_event(">", self.last_processed_event)

        while (self.running):
            # Start real-time client
            registry.get_logger().info("starting real-time monitor")
            registry.get_logger().debug("RAS event ids to filter: " + self.filter)
            self.first_realtime_event = True
            self.t = Thread(None, pyrealtime.ras_init, 'pyrealtime', (self.filter,self.rt_callback,self.rt_term_callback))
            self.t.start()

            # If the real-time client thread terminate for whatever reason, start the periodic monitor
            self.t.join()
            registry.get_logger().info("real_time server is ended, starting periodic monitor")
            self._periodic_monitor()

        return


bgcon = None

def app_terminate(sig, stack_frame):
    ''' Catch the termination signals and shut down cleanly
    '''
    if bgcon:
        bgcon.stop()
    teal.app_terminate(sig, stack_frame)        
        
if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option("-d", 
                      "--daemon",
                      help="run program as a daemon",
                      action="store_true",
                      dest="run_as_daemon",
                      default=False)
    parser.add_option("-m", 
                      "--msglevel",
                      help="set the trace message level [default: %default]",
                      action="store",
                      dest="msg_level",
                      choices=['error','warn','info','debug'],
                      default='info')
    parser.add_option("-l", 
                      "--logfile",
                      help="set the trace message level",
                      action="store",
                      dest="log_file",
                      default=None)
    
    (options, args) = parser.parse_args()
    
    if options.run_as_daemon:
        # Do the necessary processing to spin off as a daemon
        command.daemonize('teal_bgq')
    else:
        # Allow the user to CTRL-C application and shutdown cleanly        
        signal.signal(signal.SIGINT, app_terminate)    # CTRL-C
    
    if options.log_file is None:
        log_file = '$TEAL_LOG_DIR/teal_bg.log'
    else:
        log_file = options.log_file
        
    # Set up the TEAL environment to get at the data required for logging
    t = teal.Teal(None,
                  data_only=True,
                  msgLevel=options.msg_level,
                  logFile=log_file,
                  daemon_mode=options.run_as_daemon)
            
    # Create the connector and start it
    bgcon = BgqConnector()
    bgcon.setDaemon(True)
    bgcon.start()
        
    # Wait for Teal to shutdown before exiting
    shutdown = registry.get_service(registry.SERVICE_SHUTDOWN)
    shutdown.wait()


