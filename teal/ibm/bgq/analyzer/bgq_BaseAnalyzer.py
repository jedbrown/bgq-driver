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

#import ServiceRegistry
#from Analyzer import Analyzer
#from Alert import Alert

from ibm.teal import registry, alert
from ibm.teal.analyzer.analyzer import EventAnalyzer
from ibm.teal.registry import get_logger, get_service, SERVICE_DB_INTERFACE
from ibm.teal.database import db_interface
import binascii

class bgqBaseAnalyzer(EventAnalyzer):

    '''The bgqBaseAnalyzer class provides utility methods for the
    analyzers that extend it.
    '''
    def rasDetailQuery(self, cursor, recid):
        '''Run a query to retrieve details of a RAS event.
        '''
        # define query for ras event details
        eventlogTable = self.appendSchema('tbgqeventlog')
        details_query = "select location, severity, serialnumber, hex(ecid), event_time, jobid, block, message, diags, count from " + eventlogTable + " where recid = ?"
        cursor.execute(details_query, recid);
        row = list(cursor.fetchone())
        if row:
            if row[0]:
                row[0] = row[0].strip()
            if row[1]:
                row[1] = row[1].strip()
            if row[6]:
                row[6] = row[6].strip()
            if row[7]:
                row[7] = row[7].strip()
                
        return row

    def ecidString(self, ecid):
        ''' Convert the ecid value to a readable string
        '''

        if ecid is None:
            ecidStr = str(None)
        else:
            ecidStr = binascii.hexlify(str(ecid)).upper()
        return ecidStr

    def executeQuery(self, query):
        ''' Run a query.
        '''
        dbi = registry.get_service(SERVICE_DB_INTERFACE)
        dbConn = dbi.get_connection()
        cursor = dbConn.cursor()
        cursor.execute(query)
        return cursor.fetchall()
       

    def appendSchema(self, table):
        ''' Get database schema.
        '''
        schema = str(db_interface.TABLE_TEMPLATE).split('.')
        dbTable = schema[0] + '.' + table
        return dbTable
