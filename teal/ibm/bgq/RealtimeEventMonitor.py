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


from threading import Thread
from Queue import Queue
import time 
import ServiceRegistry
from Event import Event
from EventMonitor import EventMonitor

import pyrealtime

msgidList = list()

def rt_callback(recid,msgid):
    '''Realtime callback function.
    '''
    e = Event('ras',recid,msgid)
    eventQueue = ServiceRegistry.getService('event_queue')
    if msgid in RealtimeEventMonitor.msgidList:
        try:
            eventQueue.put(e)
        except BaseException, e:
            logger = ServiceRegistry.getService('log_service')
            logger.error('rt_callback exception: ' + e)
    return

def rt_term_callback():
    '''Realtime termination callback function 
    '''
    eventQueue = ServiceRegistry.getService('event_queue')
    try:
        eventQueue.put(None)
    except BaseException, e:
        logger = ServiceRegistry.getService('log_service')
        logger.error('rt_term_callback exception: ' + e)
    return

class RealtimeEventMonitor(EventMonitor):

    '''The RealtimeEventMonitor class is a client of the BlueGene
    Real-time server.  The Real-time server sends RAS event notifications
    back to this class.
    '''
    
    def __init__(self):
        '''The constructor.
        '''
        self.running = False
        return;

    def start(self, msgidList):
        '''Start running the event monitor for RealtimeEventMonitor.
        '''
        self.running = True
        
        RealtimeEventMonitor.msgidList = msgidList

        # create filter of msgids
        filter = ''
        first = True 
        for m in msgidList:
            if first:
                filter += '(' + m + ')'
                first = False
            else:
                filter += '|(' + m + ')'

        # pyrealtime.ras_init(self.rt_callback)
        self.t = Thread(None, pyrealtime.ras_init, 'pyrealtime', (filter,rt_callback,rt_term_callback))
        self.t.setDaemon(True)
        self.t.start()
        return

    def stop(self):
        '''Stop running the event monitor for RealtimeEventMonitor.
        '''
        self.running = False
