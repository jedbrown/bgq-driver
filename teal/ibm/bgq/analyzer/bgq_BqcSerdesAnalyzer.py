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
from datetime import datetime,timedelta
import time
import re
import subprocess


BGQ_TEAL_BQC_SERDES_ANALYZER = 'event_analyzer.bgqBqcSerdesAnalyzer'
BGQ_TEAL_BQC_SERDES_SCOPE = 'serdesScope'
BGQ_TEAL_BQC_SERDES_BACKLOG_WINDOW = 'serdesBacklogWindow'
BGQ_TEAL_BQC_SERDES_LINK_CHIP_EVENT_PERIOD = 'linkChipEventPeriod'
BGQ_TEAL_BQC_SERDES_LINK_CHIP_EVENT_LIMIT = 'linkChipEventLimit'


def get_eventList():
    '''Get the list of RAS events for serdes & prbs training failures.
       msgid 0006000A - boot failure
       msgid 00080014 - ras storm
    '''
    return ['0006000A','00080014']

def get_serdesScope():
    '''Get the scope at which to mark nodes in error for serdes errors.  
       Valid values are none, midplane, system
    '''
    cfg = registry.get_service(registry.SERVICE_CONFIGURATION)
    serdesScope = 'allLinks'
    try: 
        serdesScope = cfg.get(BGQ_TEAL_BQC_SERDES_ANALYZER, BGQ_TEAL_BQC_SERDES_SCOPE)
    except Exception, e:
        registry.get_logger().debug(e)

    if serdesScope != 'copperLinks' and serdesScope != 'allLinks':
        registry.get_logger().error(BGQ_TEAL_BQC_SERDES_ANALYZER + BGQ_TEAL_BQC_SERDES_SCOPE + ' is not set to a valid value of copperLinks or allLinks.  Setting to allLinks')
        serdesScope = 'allLinks'

    registry.get_logger().info(BGQ_TEAL_BQC_SERDES_ANALYZER + ' analyzer serdes scope (' + BGQ_TEAL_BQC_SERDES_SCOPE + ') = ' + serdesScope)
    return serdesScope

def get_serdesBacklogWindow():
    '''Get the backlog window - only analyze event if it happened prior to 
       the current time minus the backlog window.  Time is in minutes.
       The default is window 30 minutes.
    '''
    cfg = registry.get_service(registry.SERVICE_CONFIGURATION)
    serdesBacklogWindow = 30
    try: 
        serdesBacklogWindowStr = cfg.get(BGQ_TEAL_BQC_SERDES_ANALYZER, BGQ_TEAL_BQC_SERDES_BACKLOG_WINDOW)
        serdesBacklogWindow = int(serdesBacklogWindowStr)
    except Exception, e:
        registry.get_logger().debug(e)
    registry.get_logger().info(BGQ_TEAL_BQC_SERDES_ANALYZER + ' analyzer serdes backlog window (' + BGQ_TEAL_BQC_SERDES_BACKLOG_WINDOW + ') = ' + str(serdesBacklogWindow))
    return serdesBacklogWindow

def get_linkChipEventPeriod():
    '''Get the link chip event period - for optical link related events.  
       The analyzer will look back (for the period) from the time of the 
       event being analyzed and count the link chip events. 
       Default is 60 minutes. 
    '''
    cfg = registry.get_service(registry.SERVICE_CONFIGURATION)
    linkChipEventPeriod = 60
    try: 
        linkChipEventPeriodStr = cfg.get(BGQ_TEAL_BQC_SERDES_ANALYZER, BGQ_TEAL_BQC_SERDES_LINK_CHIP_EVENT_PERIOD)
        linkChipEventPeriod = int(linkChipEventPeriodStr)
    except Exception, e:
        registry.get_logger().debug(e)
    registry.get_logger().info(BGQ_TEAL_BQC_SERDES_ANALYZER + ' analyzer serdes backlog window (' + BGQ_TEAL_BQC_SERDES_LINK_CHIP_EVENT_PERIOD + ') = ' + str(linkChipEventPeriod))
    return linkChipEventPeriod

def get_linkChipEventLimit():
    '''Get the link chip event limit - for optical link related events.
       Check if the count of BQL events exceeds the limit.  If it does, 
       this is likely an optical link issue versus a copper link issue.
       The default is 2 events for a link chip location. 
    '''
    cfg = registry.get_service(registry.SERVICE_CONFIGURATION)
    linkChipEventLimit = 2
    try: 
        linkChipEventLimitStr = cfg.get(BGQ_TEAL_BQC_SERDES_ANALYZER, BGQ_TEAL_BQC_SERDES_LINK_CHIP_EVENT_LIMIT)
        linkChipEventLimit = int(linkChipEventLimitStr)
    except Exception, e:
        registry.get_logger().debug(e)
    registry.get_logger().info(BGQ_TEAL_BQC_SERDES_ANALYZER + ' analyzer serdes backlog window (' + BGQ_TEAL_BQC_SERDES_LINK_CHIP_EVENT_LIMIT + ') = ' + str(linkChipEventLimit))
    return linkChipEventLimit

class NDLinkEvent:
    '''The NDLInkEvent class encapsulates the information about a ND link error'''
    def __init__(self, event_time, msg_id, recid, location, severity, serialnumber, ecid, block, jobid, message, rawdata, qualifier):
        self.time_occurred = event_time
        self.msg_id = msg_id
        self.recid = recid
        self.location = location
        self.raw_data = dict()
        self.raw_data['severity'] = severity
        self.raw_data['serialnumber'] = serialnumber
        self.raw_data['ecid'] = ecid
        self.raw_data['block'] = block
        self.raw_data['jobid'] = jobid
        self.raw_data['message'] = message
        self.raw_data['rawdata'] = rawdata
        self.raw_data['qualifier'] = qualifier
        return

    def get_time_occurred(self):
        return self.time_occurred

    def get_event_id(self):
        return self.msg_id

    def get_rec_id(self):
        return self.recid

    def get_src_loc(self):
        # TEAL events have a header in front of the BG location 
        # simulating that here
        return 'xxx' + self.location

class BqcSerdesEvent:
    '''The BqcSerdesEvent class encapsulates the information about a 
       BQC serdes event.    '''    
    def __init__(self, event, queryEngine, rcvrLinkMap, xmitLinkMap, ioRcvrLinkMap, ioXmitLinkMap):
        self.event = event
        self.queryEngine = queryEngine
        self.rcvrLinkMap = rcvrLinkMap
        self.xmitLinkMap = xmitLinkMap
        self.ioRcvrLinkMap = ioRcvrLinkMap
        self.ioXmitLinkMap = ioXmitLinkMap
        self.msg_id = event.get_event_id()
        self.rec_id = event.get_rec_id()
        self.dim_dir = None
        self.dim_swap = None
        self.mark_hw_in_error = False
        self.replacementRecommended = False
        self.nodeAction = None
        self.boardAction = None
        self.linkboardAction = None
        self.cableAction = None
        self.midplaneAction = None

        self.neighborAction = None
        self.neighborboardAction = None
        self.neighborlinkboardAction = None
        self.neighborcableAction = None

        # Link type is 'receiver' or 'transmitter'.  
        # Serdes errors are always the receiver, ND errors could be either
        self.link_type = None
        self.link = None

        # Primary information associated with the location of the error
        self.primaryLocationValid = False
        self.primaryRasEnabled = True
        self.locationIsCompute = None
        self.location = None
        self.jxxloc = None
        self.serialnumber = None
        self.nodestatus = None
        self.board = None
        self.board_sn = None
        self.board_productid = None
        self.ecid = None
        self.block = None
        self.jobid = None
        self.message = None
        self.linkchip = None
        self.linkboard = None
        self.linkboard_sn = None
        self.linkboard_productid = None
        self.cableport = None

        # New primary if swapping 
        self.newlocation = None
        self.newserialnumber = None
        self.newboard = None
        self.newboard_sn = None
        self.newlinkboard = None
        self.newlinkboard_sn = None


        # Neighbor information associated with the location of the error 
        self.neighborLocationValid = False
        self.neighborRasEnabled = True
        self.neighborIsCompute = None
        self.neighbor = None
        self.jxxneighbor = None
        self.neighbor_sn = None
        self.neighborstatus = None
        self.neighborboard = None
        self.neighborboard_sn = None
        self.neighborboard_productid = None
        self.neighborlinkchip = None
        self.neighborlinkboard = None
        self.neighborlinkboard_sn = None
        self.neighborlinkboard_productid = None
        self.neighborcableport = None

        self.extraIoNeighbor = None
        self.jxxextraIoNeighbor = None
        self.extraIoNeighbor_sn = None        
        self.ioCable = None
        self.ioCable2 = None

        # New neighbor if swapping 
        self.newneighbor = None
        self.newneighbor_sn = None
        self.newneighborboard = None
        self.newneighborboard_sn = None
        self.newneighborlinkboard = None
        self.newneighborlinkboard_sn = None

        self.trainingErrors = ['000900A0', '000900A1', '000900B0', '000900B1', '000900C0', '000900C1', '000900D0', '000900D1', '000900E0', '000900E1', '000900F0']
        self.prbsErrors = ['000901A0', '000901A1', '000901B0', '000901B1', '000901C0', '000901C1', '000901D0', '000901D1', '000901E0', '000901E1', '000901F0']

        self.linkMap = {'000900A0': 'A-', '000900A1': 'A+', '000900B0': 'B-', '000900B1': 'B+', '000900C0': 'C-', '000900C1': 'C+', '000900D0': 'D-', '000900D1': 'D+', '000900E0': 'E-', '000900E1': 'E+', '000900F0': 'IO','000901A0': 'A-', '000901A1': 'A+', '000901B0': 'B-', '000901B1': 'B+', '000901C0': 'C-', '000901C1': 'C+', '000901D0': 'D-', '000901D1': 'D+', '000901E0': 'E-', '000901E1': 'E+', '000901F0': 'IO'}
        self.linkIndexMap = {'E-': 8, 'D-': 6, 'C-': 4, 'B-': 2, 'A-': 0, 'A+': 1, 'B+': 3, 'C+': 5, 'E+': 9, 'D+': 7}
        self.reverseLink = {'A+': 'A-', 'A-': 'A+','B+': 'B-', 'B-': 'B+', 'C+': 'C-', 'C-': 'C+','D+': 'D-', 'D-': 'D+','E+': 'E-', 'E-': 'E+'}

        # map a node board to its A, B, C, and D neighbor on the same midplane
        self.nodeBoardDimMap = { 'N00': ['N04', 'N08', 'N01', 'N02'], 'N01': ['N05', 'N09', 'N00', 'N03'], 'N02': ['N06', 'N10', 'N03', 'N00'], 'N03': ['N07', 'N11', 'N02', 'N01'], 'N04': ['N00', 'N12', 'N05', 'N06'], 'N05': ['N01', 'N13', 'N04', 'N07'], 'N06': ['N02', 'N14', 'N07', 'N04'], 'N07': ['N03', 'N15', 'N06', 'N05'], 'N08': ['N12', 'N00', 'N09', 'N10'], 'N09': ['N13', 'N01', 'N08', 'N11'], 'N10': ['N14', 'N02', 'N11', 'N08'], 'N11': ['N15', 'N03', 'N10', 'N09'], 'N12': ['N08', 'N04', 'N13', 'N14'], 'N13': ['N09', 'N05', 'N12', 'N15'], 'N14': ['N10', 'N06', 'N15', 'N12'], 'N15': ['N11', 'N07', 'N14', 'N13'] }

        self.nodesWithIoLinks = ['J04', 'J05', 'J06', 'J07', 'J08', 'J09', 'J10', 'J11']
        self.nodeBoardConnectorMap = {'T00': ['J06','J11'],'T01': ['J07','J10'],'T02': ['J08','J05'],'T03': ['J09','J04']}
        self.ioBoardConnectorMap = {'T19': ['J05','J07'],'T23': ['J04','J06'],'T18': ['J00','J02'],'T22': ['J01','J03'],'T04': ['J02','J03'],'T06': ['J00','J01'],'T12': ['J06','J07'],'T14': ['J04','J05']}
        self.ioBoardReverseConnectorMap = {'J00': ['T06','T18'],'J01': ['T06','T22'],'J02': ['T04','T18'],'J03': ['T04','T22'],'J04': ['T14','T23'],'J05': ['T14','T19'],'J06': ['T12','T23'],'J07': ['T12','T19']}
 
        return

    def extractInfoFromRasEvent(self):
        # get the dim_dir and dim_swap from the rawdata
        self.rawdata = self.event.raw_data['rawdata'].strip()
        if self.rawdata == None:
            registry.get_logger().error('skipping analysis of Serdes RAS event because the raw data=' + str(self.rawdata))
            return

        rindex = self.rawdata.find('DIM_DIR=')
        if rindex >= 0:
            rindex += len('DIM_DIR=')
            self.dim_dir = self.rawdata[rindex:rindex+2]
        rindex = self.rawdata.find('DIM_SWAP=')
        if rindex >= 0:
            rindex += len('DIM_SWAP=')
            self.dim_swap = self.rawdata[rindex:rindex+1]

        # Serdes errors are always the receiver, ND errors could be either
        if (self.msg_id in self.trainingErrors) or (self.msg_id in self.prbsErrors): 
            self.link_type = 'receiver'
            self.link = self.linkMap[self.msg_id]
        elif (self.msg_id == '00080037') or (self.msg_id == '00080039'): 
            self.link_type = 'receiver'
        elif (self.msg_id == '00080038'):
            self.link_type = 'transmitter'
        else:
            registry.get_logger().error('skipping analysis of Serdes RAS event because msg id is not valid for this analyzer.   msg_id=' + self.msg_id +  " and recid=" + str(self.rec_id) + ' and raw data=' + self.rawdata)
            return

        if self.link == None and self.dim_dir in self.linkIndexMap.keys():
            if self.dim_swap and self.dim_swap == '0':
                self.link = self.dim_dir
            elif self.dim_swap and self.dim_swap == '1':
                self.link = self.reverseLink[self.dim_dir]
                
        if self.link == None:
            registry.get_logger().error('skipping analysis of Serdes RAS event because neighbor information is missing the DIM_SWAP for msg_id=' + self.msg_id +  " and recid=" + str(self.rec_id) + ' and raw data=' + self.rawdata)
            return

        # Extract the information about the primary location
        self.location = str(self.event.get_src_loc())
        self.location = self.location[3:].strip()

        registry.get_logger().debug('analyzing msg_id=' + self.msg_id +  " and recid=" + str(self.rec_id) + ' and location=' + self.location)

        if re.match("R[0-9A-V][0-9A-V]-M[01]-N(?:0[0-9]|1[0-5])", self.location):
            self.locationIsCompute = True
        elif (re.match("R[0-9A-V][0-9A-V]-I[C-F]", self.location) or re.match("Q[0-9A-V][0-9A-V]-I", self.location)):
            self.locationIsCompute = False

        if self.locationIsCompute == None:
            registry.get_logger().error('skipping analyzing msg_id=' + self.msg_id +  " and recid=" + str(self.rec_id) + ' and location=' + self.location + ' because the location is not a compute or I/O node type.')
            return
        elif self.locationIsCompute == True:
            self.jxxloc = self.location[11:14]
            self.board = self.location[0:10]
        else:
            self.jxxloc = self.location[7:10]
            self.board = self.location[0:6]
        self.serialnumber = self.event.raw_data['serialnumber']
        self.ecid = str(self.event.raw_data['ecid'])
        self.block = self.event.raw_data['block'].rstrip()
        self.jobid = self.event.raw_data['jobid']
        # other ras event fields available in the event raw_data inlude:
        # 'message', 'category', 'severity', 'cpu', 
        # 'ctlaction', 'rawdata', and 'diags'

        # get the node status (already got the sn from the ras event)
        self.nodestatus, serialnumber = self.queryEngine.nodeStatusAndSnQuery(self.location, self.locationIsCompute)
        self.primaryLocationValid = True

        # get the node board serial number and product id
        if self.locationIsCompute:
            status, self.board_sn, self.board_productid =  self.queryEngine.nodeBoardQuery(self.board)
        else: 
            status, self.board_sn, self.board_productid =  self.queryEngine.ioBoardQuery(self.board)

        # Get the neighbor info from the rawdata 
        # format: NEIGHBOR=RB3-M1-N01-J12 or RB3-ID-J00
        rindex = self.rawdata.find('NEIGHBOR=')
        if rindex >= 0:
            rindex = rindex + len('NEIGHBOR=')
            sindex = self.rawdata.find(' ',rindex)
            s2index = self.rawdata.find(';',rindex)
            if sindex == -1 and s2index == -1:
                self.neighbor = self.rawdata[rindex:].rstrip()
            else:
                if s2index != -1 and s2index < sindex:
                    sindex = s2index
                self.neighbor = self.rawdata[rindex:sindex].rstrip()
            if re.match("R[0-9A-V][0-9A-V]-M[01]-N(?:0[0-9]|1[0-5])", self.neighbor):
                self.neighborIsCompute = True
            elif (re.match("R[0-9A-V][0-9A-V]-I[C-F]", self.neighbor) or re.match("Q[0-9A-V][0-9A-V]-I", self.neighbor)):
                self.neighborIsCompute = False
            if self.neighborIsCompute == None:
                registry.get_logger().error('skipping analyzing msg_id=' + self.msg_id +  " and recid=" + str(self.rec_id) + ' and neighbor=' + self.neighbor + ' because the neighbor is not a compute or I/O node type.  Raw data= ' + self.rawdata.strip())
                return
            elif self.neighborIsCompute == True:
                self.jxxneighbor = self.neighbor[11:14]
                self.neighborboard = self.neighbor[0:10]
            else:
                self.jxxneighbor = self.neighbor[7:10]
                self.neighborboard = self.neighbor[0:6]
            self.neighborLocationValid = True

            # get the link board and cable ports 
            # Sample data: Location R00-M1-N04-J25  NEIGHBOR=R01-M1-N05-J25 Connects through link chips R01-M1-N05-U03 and R00-M1-N05-U03 using cable from port R00-M1-N05-T08 to port R01-M1-N05-T10
            # Sample data: Location R00-M1-N00-J06 NEIGHBOR=Q01-I0-J05 Connects through link chips R00-M1-N00-U04 and Q01-I0-U04 using cable from port R00-M1-N00-T00 to Q01-I0-T14

            # get the link board - only one side will have a link board that 
            # is different from the boards that have already been extracted 
            rindex = self.rawdata.find('Connects through link chips ')
            if rindex >= 0:
                rindex += len('Connects through link chips ')
                sindex = self.rawdata.find(' and ',rindex)
                if sindex >= 0: 
                    bql = self.rawdata[rindex:sindex]
                    # check if the 1st bql is on the same midplane as the primary board
                    if self.board and (bql[0:6] == self.board[0:6]):
                        self.linkchip = bql
                        if self.locationIsCompute:
                            if (bql[0:10] != self.board):
                                self.linkboard = bql[0:10]
                        elif (bql[0:6] != self.board):
                            self.linkboard = bql[0:6]
                    # check if the 1st bql is on the same midplane as the neighbor board
                    elif self.neighborboard and (bql[0:6] == self.neighborboard[0:6]):
                        self.neighborlinkchip = bql
                        if self.neighborIsCompute:    
                            if (bql[0:10] != self.neighborboard):
                                self.neighborlinkboard = bql[0:10]
                        elif (bql[0:6] != self.neighborboard):
                            self.neighborlinkboard = bql[0:6]
                    # check the 2nd bql
                    rindex = sindex + len(' and ')
                    sindex = self.rawdata.find(' using cable from port ',rindex)  
                    if sindex >= 0:
                        bql = self.rawdata[rindex:sindex]
                        # check if the 2nd bql is on the same midplane as the primary board
                        if self.board and (bql[0:6] == self.board[0:6]):
                            self.linkchip = bql
                            if self.locationIsCompute:
                                if (bql[0:10] != self.board):
                                    self.linkboard = bql[0:10]
                            elif (bql[0:6] != self.board):
                                self.linkboard = bql[0:6]
                        # check if the 2nd bql is on the same midplane as the neighbor board
                        elif self.neighborboard and (bql[0:6] == self.neighborboard[0:6]):
                            self.neighborlinkchip = bql
                            if self.neighborIsCompute:    
                                if (bql[0:10] != self.neighborboard):
                                    self.neighborlinkboard = bql[0:10]
                            elif (bql[0:6] != self.neighborboard):
                                self.neighborlinkboard = bql[0:6]

            # get the link board serial number and product id
            if self.linkboard:
                status, self.linkboard_sn, self.linkboard_productid =  self.queryEngine.nodeBoardQuery(self.linkboard)

            if self.neighborlinkboard:
                status, self.neighborlinkboard_sn, self.neighborlinkboard_productid =  self.queryEngine.nodeBoardQuery(self.neighborlinkboard)
                
            # get the cable ports
            rindex = self.rawdata.find(' using cable from port ')
            if rindex >= 0:
                rindex += len(' using cable from port ')
                sindex = self.rawdata.find(' to port ', rindex)
                slen = len(' to port ')
                if sindex < 0:
                    sindex = self.rawdata.find(' to ', rindex)
                    slen = len(' to ')
                if sindex >= 0:
                    cable = self.rawdata[rindex:sindex]
                    # check if this cable is on the same midplane as the primary board
                    if self.board and (cable[0:6] == self.board[0:6]):
                        self.cableport = cable
                    # check if this cable is on the same midplane as the primary board
                    elif self.neighborboard and (cable[0:6] == self.neighborboard[0:6]):
                        self.neighborcableport = cable
                    # get the other cable port 
                    rindex = sindex + slen
                    cable = self.rawdata[rindex:].strip()
                    if len(cable) > 14:
                        sindex = cable.find(' ')
                        if sindex:
                            cable = cable[0:sindex]
                    # check if this cable is on the same midplane as the primary board
                    if self.board and cable[0:6] == self.board[0:6]:
                        self.cableport = cable
                    # check if this cable is on the same midplane as the primary board
                    elif self.neighborboard and (cable[0:6] == self.neighborboard[0:6]):
                        self.neighborcableport = cable
        else:
            # set the neighbor based on the cable connector
            # Raw data format for serdes training errors detected by the I/O node (over the I/O cable): 
            # 'using cable from R00-M1-N00-T00 to Q01-I0-T14 and cable from R00-M1-N08-T00 to Q01-I0-T23'
            rindex = self.rawdata.find('using cable from ')
            if rindex >= 0:
                # get the I/O cable from/to
                self.ioCable = self.rawdata[rindex + len('using cable from '):]
                rindex = self.ioCable.find(' and cable from ')
                if rindex >=0:
                    self.ioCable2 = self.ioCable[rindex + len(' and cable from '):]
                    self.ioCable = self.ioCable[:rindex]
                self.neighborboard = self.ioCable[0:10]
                # validate the format of the node board
                if not re.match("R[0-9A-V][0-9A-V]-M[01]-N(?:0[0-9]|1[0-5])", self.neighborboard):
                    registry.get_logger().error('skipping analysis of Serdes RAS event because neighbor information is not valid for msg_id=' + self.msg_id +  " and recid=" + str(self.rec_id) + ' and raw data: ' + self.rawdata.strip())
                    return
                self.neighborIsCompute = True
                # extract the T0x port from the node board cable
                nodeBoardConnector = self.ioCable[11:14]
                # lookup the list of two nodes associated with the connector
                computes = self.nodeBoardConnectorMap[nodeBoardConnector]
                self.jxxneighbor = computes[0]
                self.neighbor = self.neighborboard + '-' + self.jxxneighbor
                self.jxxextraIoNeighbor = computes[1]
                self.extraIoNeighbor = self.neighborboard + '-' + self.jxxextraIoNeighbor
                self.neighborLocationValid = True
            else:
                registry.get_logger().error('skipping analysis of Serdes RAS event because neighbor information is not available for msg_id=' + self.msg_id +  " and recid=" + str(self.rec_id) + ' and raw data: ' + self.rawdata.strip())
                return

        # get the neighbor status and serial number
        self.neighborstatus, self.neighbor_sn = self.queryEngine.nodeStatusAndSnQuery(self.neighbor, self.neighborIsCompute)

        # get the neighbor node board serial number and product id
        if self.neighborboard:
            if self.neighborIsCompute:
                status, self.neighborboard_sn, self.neighborboard_productid =  self.queryEngine.nodeBoardQuery(self.neighborboard)
            else:
                status, self.neighborboard_sn, self.neighborboard_productid =  self.queryEngine.ioBoardQuery(self.neighborboard)

        # Verify the extra neighbor node is active
        if self.extraIoNeighbor:
            # if the extra neighbor is not in an active status, return
            self.extraNeighborstatus, self.extraIoNeighbor_sn = self.queryEngine.nodeStatusAndSnQuery(self.extraIoNeighbor, self.neighborIsCompute)
            
        registry.get_logger().debug(self.formatResults())

        return

    def formatResults(self):
        # dump the extraction results for the primary
        results = 'Extraction results: '
        results += 'location=' + str(self.location)
        results += ', board=' + str(self.board)
        results += ', board prodid=' + str(self.board_productid)
        results += ', linkchip=' + str(self.linkchip)
        results += ', linkboard=' + str(self.linkboard)
        results += ', linkboard prodid=' + str(self.linkboard_productid)
        results += ', cableport=' + str(self.cableport)
        # dump the extraction results for the neighbor
        results += ', neighbor=' + str(self.neighbor)
        results += ', neighborboard=' + str(self.neighborboard)
        results += ', neighborboard prodid=' + str(self.neighborboard_productid)
        results += ', neighborlinkchip=' + str(self.neighborlinkchip)
        results += ', neighborlinkboard=' + str(self.neighborlinkboard)
        results += ', neighborlinkboard_productid=' + str(self.neighborlinkboard_productid)
        results += ', neighborcableport=' + str(self.neighborcableport)
        results += ', extraIoNeighbor=' + str(self.extraIoNeighbor)
        results += ', ioCable=' + str(self.ioCable)
        results += ', ioCable2=' + str(self.ioCable2)
        # dump the dim dir and swap
        results += ', dim_dir=' + str(self.dim_dir)
        results += ', dim_swap=' + str(self.dim_swap)
        results += ', link_type=' + str(self.link_type)
        results += ', link=' + str(self.link)
        return results

    def linkIndex(self, link):
        if link in self.linkIndexMap:
            return self.linkIndexMap[link]
        return -1

    def getBoardAtRelativeOffset(self, board, linkboard, newboard):
        # find a newlinkboard at the same offset that 
        # separates the board and linkboard
        bxx = int(board[8:10])
        lxx = int(linkboard[8:10])
        nxx = int(newboard[8:10])
        rxx = 0
        if bxx > lxx:
            rxx = (nxx + bxx -lxx) % 16
        else:
            rxx = (nxx + lxx - bxx) % 16
        newlinkboard = linkboard[0:8] + ('%02d' % rxx)
        return newlinkboard

    def determineCableActions(self):
        # clean the cables and ports
        if self.cableport:
            self.cableAction = 'Clean cable and port at ' + self.cableport
            if self.linkchip:
                # check if the count of ras events for this link chip exceeds the limit
                if self.queryEngine.isLinkChipEventCountOverLimit(self.linkchip, self.block, self.event.get_time_occurred()):
                    self.cableAction = "Likely optical link issue due to associated link chip events.  Clean cable and port at " + self.cableport + ' and use the check_opt_health.sh script to isolate the problem'
                elif not self.locationIsCompute:
                    self.cableAction = "Likely optical link issue since it is an I/O link.  Clean cable and port at " + self.cableport + ' and use the check_opt_health.sh script to isolate the problem'
        elif self.ioCable:
            self.cableAction = 'Likely optical link issue since it is an I/O link.  Clean cable and port for ' + self.ioCable
            if self.ioCable2:
                self.cableAction += ', and clean cable and port for ' + self.ioCable2
        if self.neighborcableport:
            self.neighborcableAction = 'Clean cable and port at ' + self.neighborcableport
            if self.neighborlinkchip:
                # check if the count of ras events for this link chip exceeds the limit
                if self.queryEngine.isLinkChipEventCountOverLimit(self.neighborlinkchip, self.block, self.event.get_time_occurred()):
                    self.neighborcableAction = "Likely optical link issue due to associated link chip events.  Clean cable and port at " + self.neighborcableport + ' and use the check_opt_health.sh script to isolate the problem'
                elif not self.neighborIsCompute:
                    self.neighborcableAction = "Likely optical link issue since it is an I/O link.  Clean cable and port at " + self.neighborcableport + ' and use the check_opt_health.sh script to isolate the problem'
        return

    def determineComputeComputeSwapsForBoards(self):
        # scenario 1 - two nodes on the same board, no board action is required
        if self.board == self.neighborboard:
            registry.get_logger().debug('msg_id=' + self.msg_id +  " and recid=" + str(self.rec_id) + ', nodes on same board, ' + str(self.board) + ', no swapping required')
            return

        # get a map of node board to serial number in the same midplane 
        # as the primary with the same product id
        midplane = self.board[0:6]
        boards = self.queryEngine.getNodeBoardsByProductId(midplane, self.board_productid)

        if len(boards) == 0:
            registry.get_logger().debug('msg_id=' + self.msg_id +  " and recid=" + str(self.rec_id) + ', no boards for available on midplane ' + midplane + ' for swapping with board ' + self.board)
            self.boardAction = 'Consider swapping node board ' + self.board + ' (S/N ' + self.board_sn + ') with a node board on a different midplane.'
            return

        bkeys = boards.keys()
        if self.board in bkeys:
            bkeys.remove(self.board)

        # scenario 2 - two nodes on different boards in the same midplane
        if midplane == self.neighborboard[0:6]:
            # remove the neighbor board (if same productid)
            if self.neighborboard in bkeys:
                bkeys.remove(self.neighborboard)

            # select one of the remaining boards to swap for the primary
            if len(bkeys) > 0:
                self.newboard = bkeys[0]
                bkeys.remove(self.newboard)
                self.newboard_sn = boards[self.newboard]
                self.boardAction = 'Swap node boards ' + self.board + ' (S/N ' + self.board_sn + ') <-> ' + self.newboard + ' (S/N ' + self.newboard_sn + ')'
                registry.get_logger().debug(self.boardAction)
                # if the neighbor is the same productid, use the same query results
                # as the primary
                if self.neighborboard_productid == self.board_productid:
                    # make sure the new neighbor board is not connected in the 
                    # same dimension that the primary and neighboring boards 
                    # were connected
                    # 1) determine the index of the neighbor in the node board 
                    #    dim map
                    nxx = self.board[7:]
                    if nxx in self.nodeBoardDimMap:
                        nodeList = self.nodeBoardDimMap[nxx]
                        neighbor_nxx = self.neighborboard[7:]
                        if neighbor_nxx in nodeList:
                            neighbor_index = nodeList.index(neighbor_nxx)
                            # 2) remove the new board's neighbor at this index
                            #    from the query results
                            newnodeList = self.nodeBoardDimMap[self.newboard[7:]]
                            nb = newnodeList[neighbor_index]
                            nb = self.newboard[0:7] + newnodeList[neighbor_index]
                            if nb in bkeys:
                                bkeys.remove(nb)
                                if len(bkeys) > 0:
                                    self.newneighborboard = bkeys[0]
                                    self.newneighborboard_sn = boards[self.newneighborboard]
                                    self.neighborboardAction = 'Swap node boards ' + self.neighborboard + ' (S/N ' + self.neighborboard_sn + ') <-> ' + self.newneighborboard + ' (S/N ' + self.newneighborboard_sn + ')'
                                    registry.get_logger().debug(self.neighborboardAction)
                            else:
                                registry.get_logger().debug('new board, ' + self.newboard[7:] + ', with new neighbor, ' + nb + ', at index= '+ str(neighbor_index) + ' not in the remaining list of boards: ' + str(bkeys))
                        else: 
                            registry.get_logger().debug('neighbor board, ' + neighbor_nxx + ', not in primary board list: ' + nxx + ' list ' + str(nodeList))
                            
                else:
                    boards = self.queryEngine.getNodeBoardsByProductId(midplane, self.neighborboard_productid)
                    bkeys = boards.keys()
                    # remove the neighbor board 
                    if self.neighborboard in bkeys:
                        bkeys.remove(self.neighborboard)
                    # select one of the remaining boards to swap for the neighbor
                    if len(bkeys) > 0:
                        # make sure the new neighbor board is not associates with the primary board 
                        rboard = self.getBoardAtRelativeOffset(self.board, self.neighborboard, self.newboard)
                        if rboard in bkeys:
                            bkeys.remove(rboard)
                        if len(bkeys) > 0:
                            self.newneighborboard = bkeys[0]
                            self.newneighborboard_sn = boards[self.newneighborboard]
                            self.neighborboardAction = 'Swap node boards ' + self.neighborboard + ' (S/N ' + self.neighborboard_sn + ') <-> ' + self.newneighborboard + ' (S/N ' + self.newneighborboard_sn + ')'
                            registry.get_logger().debug(self.neighborboardAction)
                    
        # scenario 3 - different boards and different midplanes
        else: 
            # remove the linkboard if it is defined
            if self.linkboard and (self.linkboard in bkeys):
                bkeys.remove(self.linkboard)
            # select one of the remaining boards to swap for the primary
            if len(bkeys) > 0:
                self.newboard = bkeys[0]
                bkeys.remove(self.newboard)
                self.newboard_sn = boards[self.newboard]
                self.boardAction = 'Swap node boards ' + self.board + ' (S/N ' + self.board_sn + ') <-> ' + self.newboard + ' (S/N ' + self.newboard_sn + ')'
                registry.get_logger().debug(self.boardAction)
                # determine the swap for the linkboard if it exist
                if self.linkboard:
                    # if the linkboard is a different productid 
                    if self.linkboard_productid != self.board_productid:
                        boards = self.queryEngine.getNodeBoardsByProductId(midplane, self.linkboard_productid)
                        bkeys = boards.keys()
                        if self.linkboard in bkeys:
                            bkeys.remove(self.linkboard)
                    if len(bkeys) > 0:
                        # make sure the delta between the newboard and newlinkboard is not 
                        # the same as it was between the board and linkboard
                        linkboard = self.getBoardAtRelativeOffset(self.board, self.linkboard, self.newboard)
                        if linkboard in bkeys:
                            bkeys.remove(linkboard)
                        if len(bkeys) > 0:
                            self.newlinkboard = bkeys[0]
                            self.newlinkboard_sn = boards[self.newlinkboard]
                            self.linkboardAction = 'Swap node boards ' + self.linkboard + ' (S/N ' + self.linkboard_sn + ') <-> ' + self.newlinkboard + ' (S/N ' + self.newlinkboard_sn + ')'              
                            registry.get_logger().debug(self.linkboardAction)
                        
            # determine the swap for the neighbor on the other midplane 
            if self.neighborboard:
                midplane = self.neighborboard[0:6]
                boards = self.queryEngine.getNodeBoardsByProductId(midplane, self.neighborboard_productid)
                bkeys = boards.keys()
                # remove the neighbor board 
                if self.neighborboard in bkeys:
                    bkeys.remove(self.neighborboard)
                # remove the neighbor link board
                if self.neighborlinkboard:
                    if self.neighborlinkboard in bkeys:
                        bkeys.remove(self.neighborlinkboard)
                if len(bkeys) > 0:
                    # select one of the remaining boards to swap for the neighbor
                    # make sure the new neighbor board is not associated
                    # with the primary board or link board
                    if self.board:
                        # remove the primary board offset from the options
                        tempboard = midplane + self.board[6:10]
                        if tempboard in bkeys:
                            bkeys.remove(tempboard)
                    if self.newboard:
                        # remove the new primary board offset from the options
                        tempboard = midplane + self.newboard[6:10]
                        if tempboard in bkeys:
                            bkeys.remove(tempboard)
                    if self.linkboard:
                        # remove the linkboard board offset from the options
                        tempboard = midplane + self.linkboard[6:10]
                        if tempboard in bkeys:
                            bkeys.remove(tempboard)
                    if self.newlinkboard:
                        # remove the new link board offset from the options
                        tempboard = midplane + self.newlinkboard[6:10]
                        if tempboard in bkeys:
                            bkeys.remove(tempboard)
                    if len(bkeys) > 0:
                        self.newneighborboard = bkeys[0]
                        bkeys.remove(self.newneighborboard)
                        self.newneighborboard_sn = boards[self.newneighborboard]
                        self.neighborboardAction = 'Swap node boards ' + self.neighborboard + ' (S/N ' + self.neighborboard_sn + ') <-> ' + self.newneighborboard + ' (S/N ' + self.newneighborboard_sn + ')'
                        registry.get_logger().debug(self.neighborboardAction)

                        # determine the swap for the neighbor linkboard if it exist
                        if self.neighborlinkboard:
                            # if the linkboard is a different productid 
                            if self.neighborlinkboard_productid != self.neighborboard_productid:
                                boards = self.queryEngine.getNodeBoardsByProductId(midplane, self.neighborlinkboard_productid)
                                bkeys = boards.keys()
                                # remove self 
                                if self.neighborlinkboard in bkeys:
                                    bkeys.remove(self.neighborlinkboard)
                                # remove the other board on this midplane
                                if self.neighborboard in bkeys:
                                    bkeys.remove(self.neighborboard)
                                # remove the new other board on this midplane
                                if self.newneighborboard in bkeys:
                                    bkeys.remove(self.newneighborboard)
                                # remove the other midplane's board 
                                if self.board:
                                    tempboard = midplane + self.board[6:10]
                                    if tempboard in bkeys:
                                        bkeys.remove(tempboard)
                                # remove the other miplane's new board
                                if self.newboard:
                                    tempboard = midplane + self.newboard[6:10]
                                    if tempboard in bkeys:
                                        bkeys.remove(tempboard)
                                # remove the other miplane's link board
                                if self.linkboard:
                                    tempboard = midplane + self.linkboard[6:10]
                                    if tempboard in bkeys:
                                        bkeys.remove(tempboard)
                                # remove the other miplane's new link board
                                if self.newlinkboard:
                                    tempboard = midplane + self.newlinkboard[6:10]
                                    if tempboard in bkeys:
                                        bkeys.remove(tempboard)
                            if len(bkeys) > 0:
                                # make sure the new neighbor link board is not used 
                                # by the new neighbor board
                                linkboard = self.getBoardAtRelativeOffset(self.neighborboard, self.neighborlinkboard, self.newneighborboard)
                                if linkboard in bkeys:
                                    bkeys.remove(linkboard)
                                if len(bkeys) > 0:
                                    self.newneighborlinkboard = bkeys[0]
                                    self.newneighborlinkboard_sn = boards[self.newneighborlinkboard]
                                    self.neighborlinkboardAction = 'Swap node boards ' + self.neighborlinkboard + ' (S/N ' + self.neighborlinkboard_sn + ') <-> ' + self.newneighborlinkboard + ' (S/N ' + self.newneighborlinkboard_sn + ')'
                                    registry.get_logger().debug(self.neighborlinkboardAction)


        # if the board action is not set, set it to the default
        if self.board and self.boardAction == None:
            self.boardAction = 'Consider swapping node board ' + self.board + ' (S/N ' + self.board_sn + ') with a node board on a different midplane.'
        if self.linkboard and self.linkboardAction == None:
            self.linkboardAction = 'Consider swapping node board ' + self.linkboard + ' (S/N ' + self.linkboard_sn + ') with a node board on a different midplane.'
        if self.neighborboard and self.neighborboardAction == None:
            self.neighborboardAction = 'Consider swapping node board ' + self.neighborboard + ' (S/N ' + self.neighborboard_sn + ') with a node board on a different midplane.'
        if self.neighborlinkboard and self.neighborlinkboardAction == None:
            self.neighborlinkboardAction = 'Consider swapping node board ' + self.neighborlinkboard + ' (S/N ' + self.neighborlinkboard_sn + ') with a node board on a different midplane.'
            
        return


    def determineComputeComputeSwapsForNodes(self):
        index = self.linkIndex(self.link)
        if index < 0 or index > 9:
            raise Exception("Error - invalid index " + str(index) + " for location " + self.location + " and link " + self.link )
        if (self.link_type == 'receiver'):
            # move the node so that failing link remains on-board
            keys = self.rcvrLinkMap.keys()
            # remove this location from the list of keys
            keys.remove(self.jxxloc)
            # if the neighbor is on the same board, remove its key too
            if self.board == self.neighborboard:
                keys.remove(self.jxxneighbor)
            found = False
            for key in keys:
                if self.rcvrLinkMap[key][index][0] == 'J':
                    # swap this location with the node at the key
                    self.newlocation = self.location[0:11] + key
                    status, self.newserialnumber = self.queryEngine.nodeStatusAndSnQuery(self.newlocation, self.locationIsCompute)
                    self.nodeAction = 'Swap nodes ' + self.location + ' (S/N ' + self.serialnumber + ') <-> ' + self.newlocation + ' (S/N ' + self.newserialnumber + ')'
                    registry.get_logger().debug('receiver key=' + key + ', index=' + str(index) + ', ' + self.nodeAction)
                    keys.remove(key)
                    found = True
                    break
            if not found:
                raise Exception("Error - cound not find a swap at index " + index + " for location " + self.location + " and link " + self.link )
            # neighbor is transmitting in the opposite direction
            nlink = self.reverseLink[self.link]
            nindex = self.linkIndex(nlink)
            if nindex < 0 or nindex > 9:
                raise Exception("Error - invalid index " + nindex + " for location " + self.neighbor + " and link " + nlink )
            found = False
            for key in keys:
                if self.xmitLinkMap[key][nindex][0] == 'J':
                    self.newneighbor = self.neighbor[0:11] + key
                    status, self.newneighbor_sn = self.queryEngine.nodeStatusAndSnQuery(self.newneighbor, self.neighborIsCompute)
                    # if the neighbor node is on the same board as the primary, 
                    # then append the swap details to the nodeAction
                    self.neighborAction = 'Swap nodes ' + self.neighbor + ' (S/N ' + self.neighbor_sn + ') <-> ' + self.newneighbor + ' (S/N ' + self.newneighbor_sn + ')'
                    registry.get_logger().debug('transmitter key=' + key + ', index=' + str(index) + ', ' + self.neighborAction)
                    found = True
                    break
            if not found:
                raise Exception("Error - cound not find a swap at index " + nindex + " for location " + self.neighbor + " and link " + nlink )
            return
        elif (self.link_type == 'transmitter'):
            # move the node so that failing link remains on-board
            keys = self.xmitLinkMap.keys()
            # remove this location from the list of keys
            keys.remove(self.jxxloc)
            # if the neighbor is on the same board, remove its key too
            if self.board == self.neighborboard:
                keys.remove(self.jxxneighbor)
            found = False
            for key in keys:
                if self.xmitLinkMap[key][index][0] == 'J':
                    # swap this location with the node at the key
                    self.newlocation = self.location[0:11] + key
                    status, self.newserialnumber = self.queryEngine.nodeStatusAndSnQuery(self.newlocation, self.locationIsCompute)
                    self.nodeAction = 'Swap nodes ' + self.location + ' (S/N ' + self.serialnumber + ') <-> ' + self.newlocation + ' (S/N ' + self.newserialnumber + ')'
                    registry.get_logger().debug('transmitter key=' + key + ', index=' + str(index) + ', ' + self.nodeAction)
                    keys.remove(key)
                    found = True
                    break
            if not found:
                raise Exception("Error - cound not find a swap at index " + index + " for location " + self.location + " and link " + self.link )
            # neighbor is transmitting in the opposite direction
            nlink = self.reverseLink[self.link]
            nindex = self.linkIndex(nlink)
            if nindex < 0 or nindex > 9:
                raise Exception("Error - invalid index " + nindex + " for location " + self.neighbor + " and link " + nlink )
            found = False
            for key in keys:
                if self.rcvrLinkMap[key][nindex][0] == 'J':
                    self.newneighbor = self.neighbor[0:11] + key
                    status, self.newneighbor_sn = self.queryEngine.nodeStatusAndSnQuery(self.newneighbor, self.neighborIsCompute)
                    # if the neighbor node is on the same board as the primary, 
                    # then append the swap details to the nodeAction
                    self.neighborAction = 'Swap nodes ' + self.neighbor + ' (S/N ' + self.neighbor_sn + ') <-> ' + self.newneighbor + ' (S/N ' + self.newneighbor_sn + ')'
                    registry.get_logger().debug('receiver key=' + key + ', index=' + str(index) + ', ' + self.neighborAction)
                    found = True
                    break
            if not found:
                raise Exception("Error - cound not find a swap at index " + nindex + " for location " + self.neighbor + " and link " + nlink )
            return
        else:
            raise Exception("Error - invalid link_type")

    def determineReplacementForNodes(self):
        self.replacementRecommended = False
        if self.location:
            # if the primary node S/N has been associated with a previous error
            # at a different location - then the problem followed the node
            recid = self.queryEngine.isNodeAssociatedWithPreviousError(self.location, self.serialnumber, self.dim_dir)
            if recid:
                self.replacementRecommended = True
                self.nodeAction = 'Replace node ' + self.location + ' (S/N ' + self.serialnumber + ') since it was associated with a previous error (recid=' + str(recid) + ') at a different location.'
                registry.get_logger().debug(self.nodeAction)
                # disable the ras for the neighbor
                self.neighborRasEnabled = False
                return
        # if the neighbor node S/N has been associated with a previous error
        # at a different location - then the problem followed the node
        if self.neighbor:
            recid = self.queryEngine.isNodeAssociatedWithPreviousError(self.neighbor, self.neighbor_sn, self.dim_dir)
            if recid:
                self.replacementRecommended = True
                self.neighborAction = 'Replace node ' + self.neighbor + ' (S/N ' + self.neighbor_sn + ') since it was associated with a previous error (recid=' + str(recid) + ') at a different location.'
                registry.get_logger().debug(self.nodeAction)
                # disable the ras for the primary
                self.primaryRasEnabled = False
                return
        return

    def determineReplacementForBoards(self):
        self.replacementRecommended = False
        # if the primary board S/N has been associated with a previous error
        # at a different location - then the problem followed the board
        if self.board:
            recid = self.queryEngine.isBoardAssociatedWithPreviousError(self.board, self.board_sn, self.dim_dir)
            if recid:
                self.replacementRecommended = True
                self.boardAction = 'Replace board ' + self.board + ' (S/N ' + self.board_sn + ') since it was associated with a previous error (recid=' + str(recid) + ') at a different location.'
                registry.get_logger().debug(self.boardAction)
                # disable the ras for the neighbor
                self.neighborRasEnabled = False
                return
            # handle the case where the node errors were on the same board
            if (self.board == self.neighborboard) and self.primaryLocationValid and self.neighborLocationValid:
                recid = self.queryEngine.isSameNodeAndBoardAssociatedWithPreviousError(self.location, self.serialnumber, self.neighbor, self.neighbor_sn, self.dim_dir)
                if recid:
                    self.replacementRecommended = True
                    self.boardAction = 'Replace board ' + self.board + ' (S/N ' + self.board_sn + ') since it was associated with a previous error (recid=' + str(recid) + ') with the same node locations but the nodes have been moved.'
                    registry.get_logger().debug(self.boardAction)
                    # disable the ras for the neighbor
                    self.neighborRasEnabled = False
                    return

        # if the link board S/N has been associated with a previous error
        # at a different location - then the problem followed the board
        if self.linkboard:
            recid = self.queryEngine.isBoardAssociatedWithPreviousError(self.linkboard, self.linkboard_sn, self.dim_dir)
            if recid:
                self.replacementRecommended = True
                self.linkboardAction = 'Replace board ' + self.linkboard + ' (S/N ' + self.linkboard_sn + ') since it was associated with a previous error (recid=' + str(recid) + ') at a different location.'
                registry.get_logger().debug(self.linkboardAction)
                # disable the ras for the neighbor
                self.neighborRasEnabled = False
                return
            
        # if the neighbor node S/N has been associated with a previous error
        # at a different location - then the problem followed the node
        if self.neighborboard:
            recid = self.queryEngine.isBoardAssociatedWithPreviousError(self.neighborboard, self.neighborboard_sn, self.dim_dir)
            if recid:
                self.replacementRecommended = True
                self.neighborboardAction = 'Replace board ' + self.neighborboard + ' (S/N ' + self.neighborboard_sn + ') since it was associated with a previous error (recid=' + str(recid) + ') at a different location.'
                registry.get_logger().debug(self.neighborboardAction)
                # disable the ras for the primary
                self.primaryRasEnabled = False
                return

        # if the neighbor link node S/N has been associated with a previous 
        # error at a different location - then the problem followed the node
        if self.neighborlinkboard:
            recid = self.queryEngine.isBoardAssociatedWithPreviousError(self.neighborlinkboard, self.neighborlinkboard_sn, self.dim_dir)
            if recid:
                self.replacementRecommended = True
                self.neighborlinkboardAction = 'Replace board ' + self.neighborlinkboard + ' (S/N ' + self.neighborlinkboard_sn + ') since it was associated with a previous error (recid=' + str(recid) + ') at a different location.'
                registry.get_logger().debug(self.neighborlinkboardAction)
                # disable the ras for the primary
                self.primaryRasEnabled = False
                return

        return

    def determineReplacementForMidplane(self):
        self.replacementRecommended = False
        # if the boards are on the same midplane and the locations 
        # match a previous serdes error but the S/Ns are different
        # then the problem stayed with the midplane
        if self.board and self.neighborboard and self.board[0:6] == self.neighborboard[0:6]:
            recid_primary = self.queryEngine.recidForSameLocationDifferentSerialNumbers(self.location, self.serialnumber, self.board, self.board_sn, self.dim_dir)
            if recid_primary:
                recid_neighbor = self.queryEngine.recidForSameLocationDifferentSerialNumbers(self.neighbor, self.neighbor_sn, self.neighborboard, self.neighborboard_sn, self.dim_dir)
                if recid_neighbor:
                    self.replacementRecommended = True
                    self.midplaneAction = 'Replace midplane ' + self.board[0:6] + ' since it was associated with previous errors in the same locations but the serial numbers have all changed.  Record Ids (recid) of the previous errors are ' + str(recid_primary) + ' and ' + str(recid_neighbor)
                    registry.get_logger().debug(self.neighborlinkboardAction)
                    # disable the ras for the primary
                    self.neighborRasEnabled = False

    def determineComputeIoSwaps(self):
        # move the node to a non-Io link position on the node board
        keys = self.rcvrLinkMap.keys()
        iokeys = self.ioXmitLinkMap.keys()
        if (self.link_type == 'transmitter'):
            keys = self.xmitLinkMap.keys()
            iokeys = self.ioRcvrLinkMap.keys()
        # remove nodes I/O link locations from the keys
        for ioLinkNode in self.nodesWithIoLinks:
            keys.remove(ioLinkNode)
        # select one of the remaining keys
        self.newlocation = self.location[0:11] + keys[0]
        status, self.newserialnumber = self.queryEngine.nodeStatusAndSnQuery(self.newlocation, self.locationIsCompute)
        self.nodeAction = 'Swap nodes ' + self.location + ' (S/N ' + self.serialnumber + ') <-> ' + self.newlocation + ' (S/N ' + self.newserialnumber + ')'
        registry.get_logger().debug(self.nodeAction)

        # neighbor is an I/O node, swap it with any of the other I/O nodes
        iokeys.remove(self.jxxneighbor)
        self.newneighbor = self.neighbor[0:7] + iokeys[0]
        status, self.newneighbor_sn = self.queryEngine.nodeStatusAndSnQuery(self.newneighbor, self.neighborIsCompute)
        self.neighborAction = 'Swap nodes ' + self.neighbor + ' (S/N ' + self.neighbor_sn + ') <-> ' + self.newneighbor + ' (S/N ' + self.newneighbor_sn + ')'
        registry.get_logger().debug(self.neighborAction)
        return

    def determineIoComputeSwaps(self):
        # exchange the I/O node so it is no longer connected to  
        # either of the two compute nodes
        tconns = self.ioBoardReverseConnectorMap[self.jxxloc]
        tkeys = self.ioBoardConnectorMap.keys()
        tkeys.remove(tconns[0])
        tkeys.remove(tconns[1])
        tkey = tkeys[0]
        nodes = self.ioBoardConnectorMap[tkey]
        self.newlocation = self.location[0:7] + nodes[0]
        status, self.newserialnumber = self.queryEngine.nodeStatusAndSnQuery(self.newlocation, self.locationIsCompute)
        self.nodeAction = 'Swap nodes ' + self.location + ' (S/N ' + self.serialnumber + ') <-> ' + self.newlocation + ' (S/N ' + self.newserialnumber + ')'
        registry.get_logger().debug(self.nodeAction)

        # neighbors are compute nodes, swap them with non-I/O linked nodes
        keys = self.xmitLinkMap.keys()
        if (self.link_type == 'transmitter'):
            keys = self.ioRcvrLinkMap.keys()
        # remove nodes I/O link locations from the keys
        for ioLinkNode in self.nodesWithIoLinks:
            keys.remove(ioLinkNode)
        # select one of the remaining keys
        jxx = keys[0]
        self.newneighbor = self.neighbor[0:11] + jxx
        keys.remove(jxx)
        extra_newneighbor = self.neighbor[0:11] + keys[0]
        status, self.newneighbor_sn = self.queryEngine.nodeStatusAndSnQuery(self.newneighbor, self.neighborIsCompute)
        self.neighborAction = 'Swap nodes ' + self.neighbor + ' (S/N ' + self.neighbor_sn + ') <-> ' + self.newneighbor + ' (S/N ' + self.newneighbor_sn + ')'
        status, sn = self.queryEngine.nodeStatusAndSnQuery(extra_newneighbor, self.neighborIsCompute)
        self.neighborAction += ', and Swap nodes ' + self.extraIoNeighbor + ' (S/N ' + self.extraIoNeighbor_sn + ') <-> ' + extra_newneighbor + ' (S/N ' + sn + ')' 

        # handle the second neighbors on the other I/O cable
        if self.ioCable2:
            nodeBoard = self.ioCable2[0:10]
            # validate the format of the node board
            if not re.match("R[0-9A-V][0-9A-V]-M[01]-N(?:0[0-9]|1[0-5])", nodeBoard):                
                registry.get_logger().error('Format of second I/O cable is not valid for msg_id=' + self.msg_id +  " and recid=" + str(self.rec_id) + ' and ioCable2=' + self.ioCable2)
                return
            # extract the T0x port from the node board cable
            nodeBoardConnector = self.ioCable2[11:14]
            # lookup the list of two nodes associated with the connector
            computes = self.nodeBoardConnectorMap[nodeBoardConnector]
            jxxneighbor = computes[0]
            neighbor = nodeBoard + '-' + jxxneighbor
            status, neighbor_sn = self.queryEngine.nodeStatusAndSnQuery(neighbor, self.neighborIsCompute)
            jxxextraIoNeighbor = computes[1]
            extraIoNeighbor = nodeBoard + '-' + jxxextraIoNeighbor
            status, extraIoNeighbor_sn = self.queryEngine.nodeStatusAndSnQuery(extraIoNeighbor, self.neighborIsCompute)

            # neighbors are compute nodes, swap them with non-I/O linked nodes
            keys = self.xmitLinkMap.keys()
            if (self.link_type == 'transmitter'):
                keys = self.ioRcvrLinkMap.keys()
            # remove nodes I/O link locations from the keys
            for ioLinkNode in self.nodesWithIoLinks:
                keys.remove(ioLinkNode)
            # select one of the remaining keys
            jxx = keys[0]
            newneighbor = nodeBoard + '-' + jxx
            keys.remove(jxx)
            extra_newneighbor = nodeBoard + '-' + keys[0]
            status, sn = self.queryEngine.nodeStatusAndSnQuery(newneighbor, self.neighborIsCompute)
            self.neighborAction += ', and Swap nodes ' + neighbor + ' (S/N ' + neighbor_sn + ') <-> ' + newneighbor + ' (S/N ' + sn + ')'
            status, sn = self.queryEngine.nodeStatusAndSnQuery(extra_newneighbor, self.neighborIsCompute)
            self.neighborAction += ', and Swap nodes ' + extraIoNeighbor + ' (S/N ' + extraIoNeighbor_sn + ') <-> ' + extra_newneighbor + ' (S/N ' + sn + ')' 

        registry.get_logger().debug(self.neighborAction)
        return

    def determineIoIoSwaps(self):
        # handle primary node
        nodes = self.ioBoardReverseConnectorMap.keys()
        nodes.remove(self.jxxloc)
        self.newlocation = self.location[0:7] + nodes[0]
        status, self.newserialnumber = self.queryEngine.nodeStatusAndSnQuery(self.newlocation, self.locationIsCompute)
        self.nodeAction = 'Swap nodes ' + self.location + ' (S/N ' + self.serialnumber + ') <-> ' + self.newlocation + ' (S/N ' + self.newserialnumber + ')'
        registry.get_logger().debug(self.nodeAction)
        # handle neighbor node
        nodes = self.ioBoardReverseConnectorMap.keys()
        nodes.remove(self.jxxneighbor)
        self.newneighbor = self.neighbor[0:7] + nodes[0]
        status, self.newneighbor_sn = self.queryEngine.nodeStatusAndSnQuery(self.newneighbor, self.locationIsCompute)
        self.neighborAction = 'Swap nodes ' + self.neighbor + ' (S/N ' + self.neighbor_sn + ') <-> ' + self.newneighbor + ' (S/N ' + self.newneighbor_sn + ')'
        registry.get_logger().debug(self.neighborAction)
        return

    def determineActions(self):
        # check if a part replacement can be recommended
        self.determineReplacementForNodes()
        if self.replacementRecommended:
            return # no need to fill in swap information
        self.determineReplacementForBoards()
        if self.replacementRecommended:
            return # no need to fill in swap information
        self.determineReplacementForMidplane()
        if self.replacementRecommended:
            return # no need to fill in swap information
        if self.locationIsCompute and self.neighborIsCompute:
            self.determineComputeComputeSwapsForNodes()
            self.determineComputeComputeSwapsForBoards()
            self.determineCableActions()
        elif self.locationIsCompute and not self.neighborIsCompute:
            self.determineComputeIoSwaps()
            self.determineCableActions()
        elif not self.locationIsCompute and self.neighborIsCompute:
            self.determineIoComputeSwaps()
            self.determineCableActions()
        elif not self.locationIsCompute and not self.neighborIsCompute:
            self.determineIoIoSwaps()
            self.determineCableActions()

    def sendRas(self):
        # set the message id - assume the link includes optics
        msgId = '00090216'
        # refine the message id to copper only event if nodes on same midplane
        if self.location[0:6] == self.neighbor[0:6]:
            msgId = '00090213'
        # refine the message id to part replace ras
        if self.replacementRecommended:
            msgId = '00090217'
        if self.primaryLocationValid and self.primaryRasEnabled:
            self.sendPrimaryNodeRas(msgId)
        if self.neighborLocationValid and self.neighborRasEnabled:
            self.sendNeighborNodeRas(msgId)
        
    # consider making the parameters part of the object
    def sendPrimaryNodeRas(self, new_msg_id):
        # log a ras event for the location
        command = list()
        command.append('/bgsys/drivers/ppcfloor/sbin/mc_server_log_ras')
        command.append('--location')
        # if replacement is recommended, scope the location to the replacement part 
        if self.replacementRecommended:
            if self.nodeAction:
                command.append(self.location)
            elif self.boardAction:
                command.append(self.board)
            elif self.midplaneAction:
                # the mc_server_log_ras command does not accept a 
                # midplane location so use the compute location
                command.append(self.location)
        else:
            command.append(self.location)            
        command.append('--message-id')
        command.append(new_msg_id)
        command.append('--detail')
        d = 'BG_SN=' + self.serialnumber
        command.append(d)
        #command.append('--detail')
        #d = 'BG_ECID=' + self.ecid
        #command.append(d)
        command.append('--detail')
        d = 'BG_BLOCKID=' + self.block
        command.append(d)
        if self.jobid:
            command.append('--detail')
            d = 'BG_JOBID=' + str(self.jobid)
            command.append(d)
        command.append('--detail')
        d = 'NEIGHBOR=' + self.neighbor
        command.append(d)
        command.append('--detail')
        d = 'NEIGHBOR_SERIAL_NUMBER=' + self.neighbor_sn
        command.append(d)
        rindex = self.rawdata.find('Connects ')
        if rindex != -1:
            command.append('--detail')
            d = 'CONNECTION=' + self.rawdata[rindex:]
            command.append(d)
        if self.dim_dir:
            command.append('--detail')
            d = 'DIM_DIR=' + self.dim_dir
            command.append(d)
        if self.mark_hw_in_error == True:
            command.append('--detail')
            if self.replacementRecommended:
                if self.nodeAction:
                    command.append('BG_CTL_ACT=COMPUTE_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK')
                elif self.boardAction:
                    command.append('BG_CTL_ACT=BOARD_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK')
                elif self.midplaneAction:
                    command.append('BG_CTL_ACT=COMPUTE_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK')
            else:
                command.append('BG_CTL_ACT=COMPUTE_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK')
        else:
            command.append('--detail')
            command.append('BG_CTL_ACT=None')
        if self.nodeAction:
            command.append('--detail')
            d = 'Actions="'
            if self.cableAction:
                d += self.cableAction + ', '
            d += self.nodeAction
            if self.boardAction:
                d += ', ' + self.boardAction
            if self.linkboardAction:
                d += ', ' + self.linkboardAction
            if self.midplaneAction:
                d += ', ' + self.midplaneAction
            d += '"'
            command.append(d)
        elif self.boardAction:
            command.append('--detail')
            d = 'Actions="'
            d += self.boardAction
            command.append(d)
        elif self.midplaneAction:
            command.append('--detail')
            d = 'Actions="'
            d += self.midplaneAction
            command.append(d)
        command.append('--detail')
        command.append('Submitter=TEAL')
        command.append('--detail')
        d = 'Associated_Rec_Id=' + str(self.rec_id)
        command.append(d)
        registry.get_logger().debug('Calling: ' + str(command)) 
        subprocess.call(command)
        return

    def sendNeighborNodeRas(self, new_msg_id):
        # log a ras event for the neighbor
        command = list()
        command.append('/bgsys/drivers/ppcfloor/sbin/mc_server_log_ras')
        command.append('--location')
        # if replacement is recommended, scope the location to the replacement part 
        if self.replacementRecommended:
            if self.neighborAction:
                command.append(self.neighbor)
            elif self.neighborboardAction:
                command.append(self.neighborboard)
        else:
            command.append(self.neighbor)
        command.append('--message-id')
        command.append(new_msg_id)
        command.append('--detail')
        d = 'BG_SN=' + self.neighbor_sn
        command.append(d)
        command.append('--detail')
        d = 'BG_BLOCKID=' + self.block
        command.append(d)
        if self.jobid:
            command.append('--detail')
            d = 'BG_JOBID=' + str(self.jobid)
            command.append(d)
        command.append('--detail')
        d = 'NEIGHBOR=' + self.location
        command.append(d)
        command.append('--detail')
        d = 'NEIGHBOR_SERIAL_NUMBER=' + self.serialnumber
        command.append(d)
        rindex = self.rawdata.find('Connects ')
        if rindex != -1:
            command.append('--detail')
            d = 'CONNECTION=' + self.rawdata[rindex:]
            command.append(d)
        if self.dim_dir:
            command.append('--detail')
            d = 'DIM_DIR=' + self.dim_dir
            command.append(d)
        if self.mark_hw_in_error == True:
            command.append('--detail')
            if self.replacementRecommended:
                if self.neighborAction:
                    command.append('BG_CTL_ACT=COMPUTE_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK')
                elif self.neighborboardAction:
                    command.append('BG_CTL_ACT=BOARD_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK')
                else: # miplane action is handled by the primary
                    command.append('BG_CTL_ACT=COMPUTE_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK')
            else:
                command.append('BG_CTL_ACT=COMPUTE_IN_ERROR,END_JOB,FREE_COMPUTE_BLOCK')
        else:
            command.append('--detail')
            command.append('BG_CTL_ACT=None')
        if self.neighborAction:
            command.append('--detail')
            d = 'Actions="'
            if self.neighborcableAction:
                d += self.neighborcableAction + ', '
            d += self.neighborAction
            if self.neighborboardAction:
                d += ', ' + self.neighborboardAction
            if self.neighborlinkboardAction:
                d += ', ' + self.neighborlinkboardAction
            d += '"'
            command.append(d)
        elif self.neighborboardAction:
            command.append('--detail')
            d = 'Actions="'
            d += ', ' + self.neighborboardAction
            command.append(d)
        command.append('--detail')
        command.append('Submitter=TEAL')
        command.append('--detail')
        d = 'Associated_Rec_Id=' + str(self.rec_id)
        command.append(d)
        registry.get_logger().debug('Calling: ' + str(command)) 
        subprocess.call(command)
        return
    

class bgqBqcSerdesAnalyzer(bgqBaseAnalyzer):
    '''The BqcSerdesAnalyzer class looks at serdes training failures
    to determine the isolation or replacement actions.
    '''
    def __init__(self, name, inEventQueue, outQueue, config_dict=None, number=0, checkpoint=None):
        '''The constructor.
        '''
        EventAnalyzer.__init__(self, name, inEventQueue, outQueue, config_dict, number, checkpoint)

        self.severity = "W"
        self.recommendation = "Examine the ... "
        self.alert_id = 'SERDES01'

        # Get the list of ras events that have threshold counts
        self.serdesMsgIDs = get_eventList()

        # a map of handled jobs to timestamp of when handled
        self.handledJobs = dict()
        # a map of handled qualifiers to timestamp of when handled
        self.handledQualifiers = dict()

        for msgID in self.serdesMsgIDs:
            registry.get_logger().debug('msgId = ' + msgID)
        self.serdesScope = get_serdesScope()
        self.serdesBacklogWindow = get_serdesBacklogWindow()
        self.linkChipEventPeriod = get_linkChipEventPeriod()
        self.linkChipEventLimit = get_linkChipEventLimit()
        self.trainingErrors = ['000900A0', '000900A1', '000900B0', '000900B1', '000900C0', '000900C1', '000900D0', '000900D1', '000900E0', '000900E1', '000900F0']
        self.prbsErrors = ['000901A0', '000901A1', '000901B0', '000901B1', '000901C0', '000901C1', '000901D0', '000901D1', '000901E0', '000901E1', '000901F0']

        self.ndLinkError = ['00080037','00080038','00080039']

        # The dictionaries that followed were created by parsing the 
        # nodeboard_nav output for all nodes
        # maps a node to its receiver links - AM, AP, ..., EM, EP
        self.rcvrLinkMap = {'J27': ['J06', 'Mid', 'Mid', 'J24', 'J26', 'Mid', 'Mid', 'J23', 'J28', 'J28'], 'J26': ['J07', 'Mid', 'Mid', 'J25', 'Mid', 'J27', 'Mid', 'J22', 'J29', 'J29'], 'J30': ['J03', 'Mid', 'J29', 'Mid', 'Mid', 'J31', 'Mid', 'J18', 'J25', 'J25'], 'J31': ['J02', 'Mid', 'J28', 'Mid', 'J30', 'Mid', 'Mid', 'J19', 'J24', 'J24'], 'J18': ['J15', 'Mid', 'J17', 'Mid', 'Mid', 'J19', 'J30', 'Mid', 'J21', 'J21'], 'J19': ['J14', 'Mid', 'J16', 'Mid', 'J18', 'Mid', 'J31', 'Mid', 'J20', 'J20'], 'J16': ['J13', 'Mid', 'Mid', 'J19', 'J17', 'Mid', 'J28', 'Mid', 'J23', 'J23'], 'J17': ['J12', 'Mid', 'Mid', 'J18', 'Mid', 'J16', 'J29', 'Mid', 'J22', 'J22'], 'J14': ['Mid', 'J19', 'J13', 'Mid', 'J15', 'Mid', 'J02', 'Mid', 'J09', 'J09'], 'J15': ['Mid', 'J18', 'J12', 'Mid', 'Mid', 'J14', 'J03', 'Mid', 'J08', 'J08'], 'J12': ['Mid', 'J17', 'Mid', 'J15', 'Mid', 'J13', 'J00', 'Mid', 'J11', 'J11'], 'J13': ['Mid', 'J16', 'Mid', 'J14', 'J12', 'Mid', 'J01', 'Mid', 'J10', 'J10'], 'J10': ['Mid', 'J23', 'Mid', 'J09', 'J11', 'Mid', 'J06', 'Mid', 'J13', 'J13'], 'J11': ['Mid', 'J22', 'Mid', 'J08', 'Mid', 'J10', 'J07', 'Mid', 'J12', 'J12'], 'J29': ['J00', 'Mid', 'Mid', 'J30', 'Mid', 'J28', 'Mid', 'J17', 'J26', 'J26'], 'J28': ['J01', 'Mid', 'Mid', 'J31', 'J29', 'Mid', 'Mid', 'J16', 'J27', 'J27'], 'J09': ['Mid', 'J20', 'J10', 'Mid', 'J08', 'Mid', 'J05', 'Mid', 'J14', 'J14'], 'J08': ['Mid', 'J21', 'J11', 'Mid', 'Mid', 'J09', 'J04', 'Mid', 'J15', 'J15'], 'J25': ['J04', 'Mid', 'J26', 'Mid', 'Mid', 'J24', 'Mid', 'J21', 'J30', 'J30'], 'J24': ['J05', 'Mid', 'J27', 'Mid', 'J25', 'Mid', 'Mid', 'J20', 'J31', 'J31'], 'J23': ['J10', 'Mid', 'Mid', 'J20', 'J22', 'Mid', 'J27', 'Mid', 'J16', 'J16'], 'J22': ['J11', 'Mid', 'Mid', 'J21', 'Mid', 'J23', 'J26', 'Mid', 'J17', 'J17'], 'J21': ['J08', 'Mid', 'J22', 'Mid', 'Mid', 'J20', 'J25', 'Mid', 'J18', 'J18'], 'J20': ['J09', 'Mid', 'J23', 'Mid', 'J21', 'Mid', 'J24', 'Mid', 'J19', 'J19'], 'J01': ['Mid', 'J28', 'Mid', 'J02', 'J00', 'Mid', 'Mid', 'J13', 'J06', 'J06'], 'J00': ['Mid', 'J29', 'Mid', 'J03', 'Mid', 'J01', 'Mid', 'J12', 'J07', 'J07'], 'J03': ['Mid', 'J30', 'J00', 'Mid', 'Mid', 'J02', 'Mid', 'J15', 'J04', 'J04'], 'J02': ['Mid', 'J31', 'J01', 'Mid', 'J03', 'Mid', 'Mid', 'J14', 'J05', 'J05'], 'J05': ['Mid', 'J24', 'J06', 'Mid', 'J04', 'Mid', 'Mid', 'J09', 'J02', 'J02'], 'J04': ['Mid', 'J25', 'J07', 'Mid', 'Mid', 'J05', 'Mid', 'J08', 'J03', 'J03'], 'J07': ['Mid', 'J26', 'Mid', 'J04', 'Mid', 'J06', 'Mid', 'J11', 'J00', 'J00'], 'J06': ['Mid', 'J27', 'Mid', 'J05', 'J07', 'Mid', 'Mid', 'J10', 'J01', 'J01']}

        # this is a dictionary that maps a node's transmitter links - AM, AP, ..., EM, EP
        self.xmitLinkMap = {'J27': ['J06', 'Mid', 'U07', 'J24', 'J26', 'Mid', 'U08', 'J23', 'J28', 'J28'], 'J26': ['J07', 'Mid', 'U06', 'J25', 'U05', 'J27', 'U05', 'J22', 'J29', 'J29'], 'J30': ['J03', 'Mid', 'J29', 'Mid', 'U05', 'J31', 'U05', 'J18', 'J25', 'J25'], 'J31': ['J02', 'Mid', 'J28', 'Mid', 'J30', 'Mid', 'U08', 'J19', 'J24', 'J24'], 'J18': ['J15', 'Mid', 'J17', 'Mid', 'U05', 'J19', 'J30', 'Mid', 'J21', 'J21'], 'J19': ['J14', 'Mid', 'J16', 'Mid', 'J18', 'Mid', 'J31', 'Mid', 'J20', 'J20'], 'J16': ['J13', 'Mid', 'U01', 'J19', 'J17', 'Mid', 'J28', 'Mid', 'J23', 'J23'], 'J17': ['J12', 'Mid', 'U02', 'J18', 'U03', 'J16', 'J29', 'Mid', 'J22', 'J22'], 'J14': ['U06', 'J19', 'J13', 'Mid', 'J15', 'Mid', 'J02', 'Mid', 'J09', 'J09'], 'J15': ['U07', 'J18', 'J12', 'Mid', 'U08', 'J14', 'J03', 'Mid', 'J08', 'J08'], 'J12': ['U01', 'J17', 'U01', 'J15', 'U00', 'J13', 'J00', 'Mid', 'J11', 'J11'], 'J13': ['U02', 'J16', 'U02', 'J14', 'J12', 'Mid', 'J01', 'Mid', 'J10', 'J10'], 'J10': ['U06', 'J23', 'U06', 'J09', 'J11', 'Mid', 'J06', 'Mid', 'J13', 'J13'], 'J11': ['U07', 'J22', 'U07', 'J08', 'U08', 'J10', 'J07', 'Mid', 'J12', 'J12'], 'J29': ['J00', 'Mid', 'U02', 'J30', 'U03', 'J28', 'U03', 'J17', 'J26', 'J26'], 'J28': ['J01', 'Mid', 'U01', 'J31', 'J29', 'Mid', 'U00', 'J16', 'J27', 'J27'], 'J09': ['U02', 'J20', 'J10', 'Mid', 'J08', 'Mid', 'J05', 'Mid', 'J14', 'J14'], 'J08': ['U01', 'J21', 'J11', 'Mid', 'U00', 'J09', 'J04', 'Mid', 'J15', 'J15'], 'J25': ['J04', 'Mid', 'J26', 'Mid', 'U03', 'J24', 'U03', 'J21', 'J30', 'J30'], 'J24': ['J05', 'Mid', 'J27', 'Mid', 'J25', 'Mid', 'U00', 'J20', 'J31', 'J31'], 'J23': ['J10', 'Mid', 'U07', 'J20', 'J22', 'Mid', 'J27', 'Mid', 'J16', 'J16'], 'J22': ['J11', 'Mid', 'U06', 'J21', 'U05', 'J23', 'J26', 'Mid', 'J17', 'J17'], 'J21': ['J08', 'Mid', 'J22', 'Mid', 'U03', 'J20', 'J25', 'Mid', 'J18', 'J18'], 'J20': ['J09', 'Mid', 'J23', 'Mid', 'J21', 'Mid', 'J24', 'Mid', 'J19', 'J19'], 'J01': ['U02', 'J28', 'U02', 'J02', 'J00', 'Mid', 'U03', 'J13', 'J06', 'J06'], 'J00': ['U01', 'J29', 'U01', 'J03', 'U00', 'J01', 'U00', 'J12', 'J07', 'J07'], 'J03': ['U07', 'J30', 'J00', 'Mid', 'U08', 'J02', 'U08', 'J15', 'J04', 'J04'], 'J02': ['U06', 'J31', 'J01', 'Mid', 'J03', 'Mid', 'U05', 'J14', 'J05', 'J05'], 'J05': ['U02', 'J24', 'J06', 'Mid', 'J04', 'Mid', 'U03', 'J09', 'J02', 'J02'], 'J04': ['U01', 'J25', 'J07', 'Mid', 'U00', 'J05', 'U00', 'J08', 'J03', 'J03'], 'J07': ['U07', 'J26', 'U07', 'J04', 'U08', 'J06', 'U08', 'J11', 'J00', 'J00'], 'J06': ['U06', 'J27', 'U06', 'J05', 'J07', 'Mid', 'U05', 'J10', 'J01', 'J01']}
        self.ioRcvrLinkMap = {'J01': ['J00', 'U01', 'U01', 'J05', 'U03', 'J03', 'U05', 'U05', 'U03'], 'J00': ['U01', 'J01', 'U01', 'J04', 'U03', 'J02', 'U05', 'U05', 'U03'], 'J03': ['J02', 'U01', 'U00', 'J07', 'J01', 'U03', 'U05', 'U05', 'U03'], 'J02': ['U01', 'J03', 'U00', 'J06', 'J00', 'U03', 'U05', 'U05', 'U03'], 'J05': ['J04', 'U00', 'J01', 'U01', 'U02', 'J07', 'U04', 'U04', 'U02'], 'J04': ['U00', 'J05', 'J00', 'U01', 'U02', 'J06', 'U04', 'U04', 'U02'], 'J07': ['J06', 'U00', 'J03', 'U00', 'J05', 'U02', 'U04', 'U04', 'U02'], 'J06': ['U00', 'J07', 'J02', 'U00', 'J04', 'U02', 'U04', 'U04', 'U02']}
        self.ioXmitLinkMap = {'J01': ['J00', 'U01', 'U01', 'J05', 'U03', 'J03', 'U05', 'U05', 'U03'], 'J00': ['U01', 'J01', 'U01', 'J04', 'U03', 'J02', 'U05', 'U05', 'U03'], 'J03': ['J02', 'U01', 'U00', 'J07', 'J01', 'U03', 'U05', 'U05', 'U03'], 'J02': ['U01', 'J03', 'U00', 'J06', 'J00', 'U03', 'U05', 'U05', 'U03'], 'J05': ['J04', 'U00', 'J01', 'U01', 'U02', 'J07', 'U04', 'U04', 'U02'], 'J04': ['U00', 'J05', 'J00', 'U01', 'U02', 'J06', 'U04', 'U04', 'U02'], 'J07': ['J06', 'U00', 'J03', 'U00', 'J05', 'U02', 'U04', 'U04', 'U02'], 'J06': ['U00', 'J07', 'J02', 'U00', 'J04', 'U02', 'U04', 'U02']}
        # get a connection at init time, methods that use this connection and 
        # cursor need to handle reestablishing the connection if it times out
        dbi = registry.get_service(SERVICE_DB_INTERFACE)
        self.dbConn = dbi.get_connection()
        self.cursor = self.dbConn.cursor()
        return

    def nodeStatusAndSnQuery(self, location, locationIsCompute):
        '''Run a query to retrieve the status and serial number of the node.
        '''
        # define query for ras event details
        nodeTable = self.appendSchema('bgqnode')
        if locationIsCompute == False:
            nodeTable = self.appendSchema('bgqionode')
        status_query = "select status,serialnumber from " + nodeTable + " where location = ?"
        registry.get_logger().debug(status_query + ', ? = ' + location)
        # retry in case the connection timed out
        for x in range(5):
            try:
                self.cursor.execute(status_query, location)
                row = self.cursor.fetchone()
                # print row
                return row
            except Exception, e:
                registry.get_logger().debug(e)
                if x < 4:
                    dbi = registry.get_service(SERVICE_DB_INTERFACE)
                    self.dbConn = dbi.get_connection()
                    self.cursor = self.dbConn.cursor()
                else:
                    raise Exception('Error: bgq_BqcSerdesAnalyzer could not connect to the database')
        return None

    def nodeBoardQuery(self, board):
        '''Run a query to retrieve the status, serial number, and product id 
           of the node board.
        '''
        nodeBoardTable = self.appendSchema('bgqnodecard')
        query = "select status,serialnumber,productid from " + nodeBoardTable + " where location = ?"
        registry.get_logger().debug(query + ', ? = ' + board)
        # retry in case the connection timed out
        for x in range(5):
            try:
                self.cursor.execute(query, board)
                row = self.cursor.fetchone()
                if row:
                    row[2] = row[2].rstrip()
                # print row
                return row
            except Exception, e:
                registry.get_logger().debug(e)
                if x < 4:
                    dbi = registry.get_service(SERVICE_DB_INTERFACE)
                    self.dbConn = dbi.get_connection()
                    self.cursor = self.dbConn.cursor()
                else:
                    raise Exception('Error: bgq_BqcSerdesAnalyzer could not connect to the database')
        return None

    def ioBoardQuery(self, board):
        '''Run a query to retrieve the status, serial number, and product id 
           of the i/o board.
        '''
        ioBoardTable = self.appendSchema('bgqiodrawer')
        query = "select status,serialnumber,productid from " + ioBoardTable + " where location = ?"
        registry.get_logger().debug(query + ', ? = ' + board)
        # retry in case the connection timed out
        for x in range(5):
            try:
                self.cursor.execute(query, board)
                row = self.cursor.fetchone()
                # print row
                if row:
                    row[2] = row[2].rstrip()
                return row
            except Exception, e:
                registry.get_logger().debug(e)
                if x < 4:
                    dbi = registry.get_service(SERVICE_DB_INTERFACE)
                    self.dbConn = dbi.get_connection()
                    self.cursor = self.dbConn.cursor()
                else:
                    raise Exception('Error: bgq_BqcSerdesAnalyzer could not connect to the database')
        return None

    def getNodeBoardsByProductId(self, midplane, productid):
        '''Run a query to retrieve the serial number and product id of the 
           node boards in the same midplane and same productid.  
           Return a dictionary (map) of node board to serial number for 
           boards that are active ('A") or in service ('S').
        '''
        boards = dict() 
        # define query for ras event details
        nodeBoardTable = self.appendSchema('bgqnodecard')
        likeboards = midplane + '%' 
        query = "select location, serialnumber from " + nodeBoardTable + " where location like ? and productid = ? and (status = 'A' or status = 'M')"
        registry.get_logger().debug(query + ', ? = ' + likeboards + ' and ' + productid)
        # retry in case the connection timed out
        for x in range(5):
            try:
                self.cursor.execute(query, likeboards, productid)
                rows = self.cursor.fetchall()
                for row in rows:
                    boards[row[0]] = row[1]
                # print row
                return boards
            except Exception, e:
                registry.get_logger().debug(e)
                if x < 4:
                    dbi = registry.get_service(SERVICE_DB_INTERFACE)
                    self.dbConn = dbi.get_connection()
                    self.cursor = self.dbConn.cursor()
                else:
                    raise Exception('Error: bgq_BqcSerdesAnalyzer could not connect to the database')
        return boards

    def isNodeAssociatedWithPreviousError(self, location, serialnumber, dim_dir):
        '''Run a query to see if this node serialnumber had a previous 
           serdes error at a different location.  If so, assume the problem
           followed the node and return the associated rec_id.  
           Return None if no association is found.
        '''
        eventlogTable = self.appendSchema('tbgqeventlog')
        query = "select recid,rawdata from " + eventlogTable + " where location != ? and serialnumber = ? and (msg_id='00090213' or msg_id='00090216')"
        registry.get_logger().debug(query + ', ? = ' + location + ' and ' + serialnumber)
        # retry in case the connection timed out
        for x in range(5):
            try:
                self.cursor.execute(query, location, serialnumber)
                rows = self.cursor.fetchall()
                for row in rows:
                    # print row
                    # verify the associated error was in the same dimension
                    rec_id = row[0]
                    rawdata = row[1].strip()
                    # first look to see if the DIM_DIR was included in 
                    # the serdes error logged by this analyzer
                    index = rawdata.find('DIM_DIR=')
                    dimstr = "DIM_DIR=" + dim_dir[0]
                    if index >= 0:
                        index = rawdata.find(dimstr)
                        if index >= 0:
                            return rec_id
                        else:
                            registry.get_logger().info("The node at " + location + " with S/N " + serialnumber + "  was associated with a previous serdes link error (recid=" + str(rec_id) + " but in a different dimension, thus replacement is not recommended at this time.") 
                            return None
                    # Look at the DIM_DIR in the associated recid
                    index = rawdata.find('Associated_Rec_Id=')
                    if index >= 0:
                        index += len('Associated_Rec_Id=')
                        rindex = rawdata.find(';',index)
                        if rindex > index:
                            assoc_rec_id = int(rawdata[index:rindex])
                            rquery = "select location,rawdata from " + eventlogTable + " where recid = ? and (locate('" + dimstr + "',rawdata) > 0)"
                            registry.get_logger().debug(rquery + ', ? = ' + str(assoc_rec_id))
                            self.cursor.execute(rquery, assoc_rec_id)
                            row = self.cursor.fetchone()
                            if row: 
                                return rec_id
                            registry.get_logger().info("The node at " + location + " with S/N " + serialnumber + "  was associated with a previous serdes link error (recid=" + str(rec_id) + " but in a different dimension, thus replacement is not recommended at this time.") 
                        else:
                            registry.get_logger().error("Missing termination character ';' for the associated recid in the raw data of the previous error with recid=" + str(rec_id) + ".  rawdata=" + rawdata)
                    else:
                        registry.get_logger().error("Missing the associated recid in the raw data of the previous error with recid=" + str(rec_id))
                return None
            except Exception, e:
                registry.get_logger().debug(e)
                if x < 4:
                    dbi = registry.get_service(SERVICE_DB_INTERFACE)
                    self.dbConn = dbi.get_connection()
                    self.cursor = self.dbConn.cursor()
                else:
                    raise Exception('Error: bgq_BqcSerdesAnalyzer could not connect to the database')
        return None

    def recidForSameLocationDifferentSerialNumbers(self, location, serialnumber, board, board_sn, dim_dir):
        '''Run a query that determines that there has been an copper link error ('00090213')
           at the same location but the serialnumbers of the node and board are different.
           Return the recid of the event if found, else return None.
        '''
        eventlogTable = self.appendSchema('tbgqeventlog')
        boardloc = "Swap node boards " + board  
        boardsn = boardloc + " (S/N " + board_sn + ") <->"
        query = "select recid,rawdata from " + eventlogTable + " where msg_id='00090213' and (location=? and serialnumber != ?) and (locate('BOARD_LOC',rawdata) > 0 and locate('BOARD_SN',rawdata) = 0) order by event_time"
        query = query.replace('BOARD_LOC',boardloc)
        query = query.replace('BOARD_SN',boardsn)
        registry.get_logger().debug(query + ', ? = location ' + location + ', and serialnumber ' + serialnumber )
        # retry in case the connection timed out
        for x in range(5):
            try:
                self.cursor.execute(query, location, serialnumber)
                rows = self.cursor.fetchall()
                for row in rows:
                    # print row
                    # verify the associated error was in the same dimension
                    rec_id = row[0]
                    rawdata = row[1].strip()
                    # first look to see if the DIM_DIR was included in 
                    # the serdes error logged by this analyzer
                    index = rawdata.find('DIM_DIR=')
                    dimstr = "DIM_DIR=" + dim_dir[0]
                    if index >= 0:
                        index = rawdata.find(dimstr)
                        if index >= 0:
                            return rec_id
                        else:
                            registry.get_logger().info("The node at " + location + " with S/N " + serialnumber + "  was associated with a previous serdes link error (recid=" + str(rec_id) + " but in a different dimension, thus replacement is not recommended at this time.") 
                            return None
                    # Look at the DIM_DIR in the associated recid
                    index = rawdata.find('Associated_Rec_Id=')
                    if index >= 0:
                        index += len('Associated_Rec_Id=')
                        rindex = rawdata.find(';',index)
                        if rindex > index:
                            assoc_rec_id = int(rawdata[index:rindex])
                            rquery = "select location,rawdata from " + eventlogTable + " where recid = ? and (locate('" + dimstr + "',rawdata) > 0)"
                            registry.get_logger().debug(rquery + ', ? = ' + str(assoc_rec_id))
                            self.cursor.execute(rquery, assoc_rec_id)
                            row = self.cursor.fetchone()
                            if row: 
                                return rec_id
                            registry.get_logger().info("The node at " + location + " with S/N " + serialnumber + "  was associated with a previous serdes link error (recid=" + str(rec_id) + " but in a different dimension, thus replacement is not recommended at this time.") 
                        else:
                            registry.get_logger().error("Missing termination character ';' for the associated recid in the raw data of the previous error with recid=" + str(rec_id) + ".  rawdata=" + rawdata)
                    else:
                        registry.get_logger().error("Missing the associated recid in the raw data of the previous error with recid=" + str(rec_id))
                return None
            except Exception, e:
                registry.get_logger().debug(e)
                if x < 4:
                    dbi = registry.get_service(SERVICE_DB_INTERFACE)
                    self.dbConn = dbi.get_connection()
                    self.cursor = self.dbConn.cursor()
                else:
                    raise Exception('Error: bgq_BqcSerdesAnalyzer could not connect to the database')
        return None
        

    def isBoardAssociatedWithPreviousError(self, location, serialnumber, dim_dir):
        '''Run a query to see if this board serialnumber had a previous 
           serdes error at a different location.  If so, assume the problem
           followed the board and return the associated rec_id.  
           Return None if no association is found.
        '''
        boardsn = '(S/N ' + serialnumber + ') <->'
        boardloc = location + ' ' + boardsn
        eventlogTable = self.appendSchema('tbgqeventlog')
        query = "select recid,rawdata from " + eventlogTable + " where (msg_id='00090213' or msg_id='00090216') and (locate(?,rawdata) > 0) and not (locate(?,rawdata) > 0)"
        registry.get_logger().debug(query + ', ? = ' + boardsn + ' and ' + boardloc)
        # retry in case the connection timed out
        for x in range(5):
            try:
                self.cursor.execute(query, boardsn, boardloc)
                rows = self.cursor.fetchall()
                for row in rows:
                    # print row
                    # verify the associated error was in the same dimension
                    rec_id = row[0]
                    rawdata = row[1].strip()
                    # first look to see if the DIM_DIR was included in 
                    # the serdes error logged by this analyzer
                    index = rawdata.find('DIM_DIR=')
                    dimstr = "DIM_DIR=" + dim_dir[0]
                    if index >= 0:
                        index = rawdata.find(dimstr)
                        if index >= 0:
                            return rec_id
                        else:
                            registry.get_logger().info("The board at " + location + " with S/N " + serialnumber + "  was associated with a previous serdes link error (recid=" + str(rec_id) + " but in a different dimension, thus replacement is not recommended at this time.") 
                            return None
                    # Look at the DIM_DIR in the associated recid
                    index = rawdata.find('Associated_Rec_Id=')
                    if index >= 0:
                        index += len('Associated_Rec_Id=')
                        rindex = rawdata.find(';',index)
                        if rindex > index:
                            assoc_rec_id = int(rawdata[index:rindex])
                            rquery = "select location,rawdata from " + eventlogTable + " where recid = ? and (locate('" + dimstr + "',rawdata) > 0)"
                            registry.get_logger().debug(rquery + ', ? = ' + str(assoc_rec_id))
                            self.cursor.execute(rquery, assoc_rec_id)
                            row = self.cursor.fetchone()
                            if row: 
                                return rec_id
                            registry.get_logger().info("The board at " + location + " with S/N " + serialnumber + "  was associated with a previous serdes link error (recid=" + str(rec_id) + " but in a different dimension, thus replacement is not recommended at this time.") 
                        else:
                            registry.get_logger().error("Missing termination character ';' for the associated recid in the raw data of the previous error with recid=" + str(rec_id) + ".  rawdata=" + rawdata)
                    else:
                        registry.get_logger().error("Missing the associated recid in the raw data of the previous error with recid=" + str(rec_id))
                return None
            except Exception, e:
                registry.get_logger().debug(e)
                if x < 4:
                    dbi = registry.get_service(SERVICE_DB_INTERFACE)
                    self.dbConn = dbi.get_connection()
                    self.cursor = self.dbConn.cursor()
                else:
                    raise Exception('Error: bgq_BqcSerdesAnalyzer could not connect to the database')
        return None

    def isSameNodeAndBoardAssociatedWithPreviousError(self, location, serialnumber, neighbor_location, neighbor_serialnumber, dim_dir):
        '''Run a query to see if this node location with a different 
           serialnumber had a 00090213 event and its neighbor location with a 
           different serialnumber had a 00090213 and both had the same 
           qualifier.  If so, assume the problem is the board and return 
           the associated rec_id.  
           Return None if no association is found.
        '''
        eventlogTable = self.appendSchema('tbgqeventlog')
        query = "select recid,rawdata,qualifier from " + eventlogTable + " where location = ? and serialnumber != ? and msg_id='00090213' order by recid"
        registry.get_logger().debug(query + ', ? = ' + location + ' and ' + serialnumber)
        # retry in case the connection timed out
        for x in range(5):
            try:
                self.cursor.execute(query, location, serialnumber)
                rows = self.cursor.fetchall()
                for row in rows:
                    # print row
                    # verify the associated error was in the same dimension
                    rec_id = row[0]
                    rawdata = row[1].strip()
                    qualifier = row[2].strip()
                    # first look to see if the DIM_DIR was included in 
                    # the serdes error logged by this analyzer
                    index = rawdata.find('DIM_DIR=')
                    dimstr = "DIM_DIR=" + dim_dir[0]
                    if index >= 0:
                        index = rawdata.find(dimstr)
                        if index >= 0:
                            # the dimension was the same, now check the neighbor
                            nquery = "select recid,rawdata from " + eventlogTable + " where location = ? and serialnumber != ? and msg_id='00090213' and qualifier = ? order by recid"
                            registry.get_logger().debug(nquery + ', ? = ' + neighbor_location + ' and ' + neighbor_serialnumber + ' and ' + str(qualifier))
                            self.cursor.execute(nquery, neighbor_location, neighbor_serialnumber, qualifier)
                            rows = self.cursor.fetchall()
                            for row in rows:
                                nrec_id = row[0]
                                nrawdata = row[1].strip()
                                index = nrawdata.find('DIM_DIR=')
                                if index >= 0:
                                    index = nrawdata.find(dimstr)
                                    if index >= 0:
                                        return nrec_id
                                    else:
                                        registry.get_logger().info("The neighbor node at " + neighbor_location + " with S/N " + neighbor_serialnumber + "  was associated with a previous serdes link error (recid=" + str(nrec_id) + " but in a different dimension, thus board replacement is not recommended at this time.") 
                                        return None
                        else:
                            registry.get_logger().info("The node at " + location + " with S/N " + serialnumber + "  was associated with a previous serdes link error (recid=" + str(rec_id) + " but in a different dimension, thus board replacement is not recommended at this time.") 
                            return None
                return None
            except Exception, e:
                registry.get_logger().debug(e)
                if x < 4:
                    dbi = registry.get_service(SERVICE_DB_INTERFACE)
                    self.dbConn = dbi.get_connection()
                    self.cursor = self.dbConn.cursor()
                else:
                    raise Exception('Error: bgq_BqcSerdesAnalyzer could not connect to the database')
        return None

    def will_analyze_event(self, event):
        '''Indicate this analyzer handles all events from tbgqmsgtypes table.
        '''
        event_id = event.get_event_id()
        if event_id in self.serdesMsgIDs:
           registry.get_logger().debug('matched event id ' + event_id + ' ' + str(event.get_rec_id()))
           return True
        else:
           registry.get_logger().debug('not matched ' + event_id + ' ' + str(event.get_rec_id()))
           return False

    def getNDLinkEvent(self, location, messageCode, jobid):
        '''Run a query to retrieve details of the ND Link RAS event.'''
        eventlogTable = self.appendSchema('tbgqeventlog')
        query = "select recid, severity, serialnumber, ecid, event_time, block, message, diags, count, qualifier, rawdata from " + eventlogTable + " where location = ? and msg_id = ? and jobid = ?"
        registry.get_logger().debug(query + ', ? => location=' + location + ', msg_id=' + messageCode + ', jobid=' + str(jobid))
        # retry in case the connection timed out
        for x in range(5):
            try:
                self.cursor.execute(query, location, messageCode, jobid)
                recid, severity, serialnumber, ecid, event_time, block, message, diags, count, qualifier, rawdata = self.cursor.fetchone()
                # Create an event for the associated ND link error and return it
                registry.get_logger().debug('getNDLinkEvent creating NDLinkEvent for recid=' + str(recid))
                ndEvent = NDLinkEvent(event_time, messageCode, recid, location, severity, serialnumber, ecid, block.rstrip(), jobid, message.rstrip(), rawdata.rstrip(), qualifier.rstrip())
                return ndEvent
            except Exception as e:
                registry.get_logger().error('getNDLinkEvent exception ' + str(e))
                if x < 4:
                    dbi = registry.get_service(SERVICE_DB_INTERFACE)
                    self.dbConn = dbi.get_connection()
                    self.cursor = self.dbConn.cursor()
                else:
                    raise Exception('Error: bgq_BqcSerdesAnalyzer could not connect to the database')
        return None

    def getNDLinkEventForQualifier(self, location, messageCode, qualifier):
        '''Run a query to retrieve details of the ND Link RAS event.'''
        eventlogTable = self.appendSchema('tbgqeventlog')
        query = "select recid, severity, serialnumber, ecid, event_time, block, message, diags, count, rawdata from " + eventlogTable + " where location = ? and msg_id = ? and qualifier = ?"
        registry.get_logger().debug(query + ', ? => location=' + location + ', msg_id=' + messageCode + ', qualifier=' + str(qualifier))
        # retry in case the connection timed out
        for x in range(5):
            try:
                self.cursor.execute(query, location, messageCode, qualifier)
                recid, severity, serialnumber, ecid, event_time, block, message, diags, count, rawdata = self.cursor.fetchone()
                # Create an event for the associated ND link error and return it
                registry.get_logger().debug('getNDLinkEvent creating NDLinkEvent for recid=' + str(recid))
                ndEvent = NDLinkEvent(event_time, messageCode, recid, location, severity, serialnumber, ecid, block.rstrip(), None, message.rstrip(), rawdata.rstrip(), qualifier)
                return ndEvent
            except Exception as e:
                registry.get_logger().error('getNDLinkEvent exception ' + str(e))
                if x < 4:
                    dbi = registry.get_service(SERVICE_DB_INTERFACE)
                    self.dbConn = dbi.get_connection()
                    self.cursor = self.dbConn.cursor()
                else:
                    raise Exception('Error: bgq_BqcSerdesAnalyzer could not connect to the database')
        return None

    def isEventCountOverLimit(self, msg_id, jobid, limit):
        '''Run a count query (ordered by location) for msg_id and jobid.  
           If the number of rows (locations) exceeds the limit, 
           return True, otherwise return False.'''
        eventlogTable = self.appendSchema('tbgqeventlog')
        query = "select count(*), location from " + eventlogTable + " where msg_id = ? and jobid = ? group by location"
        registry.get_logger().debug(query + ', ? => msg_id=' + msg_id + ', jobid=' + str(jobid))
        # retry in case the connection timed out
        for x in range(5):
            try:
                self.cursor.execute(query, msg_id, jobid)
                count = 0
                while count <= limit:
                    row = self.cursor.fetchone()
                    if row: 
                        count += 1
                    else:
                        break
                if count > limit:
                    return True
                else: 
                    return False
            except Exception as e:
                registry.get_logger().error('isEventCountOverLimit exception ' + str(e))
                if x < 4:
                    dbi = registry.get_service(SERVICE_DB_INTERFACE)
                    self.dbConn = dbi.get_connection()
                    self.cursor = self.dbConn.cursor()
                else:
                    raise Exception('Error: bgq_BqcSerdesAnalyzer could not connect to the database')
        return True

    def isEventCountOverLimitForQualifier(self, msg_id, qualifier, limit):
        '''Run a count query (ordered by location) for msg_id and qualifier.  
           If the number of rows (locations) exceeds the limit, 
           return True, otherwise return False.'''
        eventlogTable = self.appendSchema('tbgqeventlog')
        query = "select count(*), location from " + eventlogTable + " where msg_id = ? and qualifier = ? group by location"
        registry.get_logger().debug(query + ', ? => msg_id=' + msg_id + ', qualifier=' + str(qualifier))
        # retry in case the connection timed out
        for x in range(5):
            try:
                self.cursor.execute(query, msg_id, qualifier)
                count = 0
                while count <= limit:
                    row = self.cursor.fetchone()
                    if row: 
                        count += 1
                    else:
                        break
                if count > limit:
                    return True
                else: 
                    return False
            except Exception as e:
                registry.get_logger().error('isEventCountOverLimitForQualifier exception ' + str(e))
                if x < 4:
                    dbi = registry.get_service(SERVICE_DB_INTERFACE)
                    self.dbConn = dbi.get_connection()
                    self.cursor = self.dbConn.cursor()
                else:
                    raise Exception('Error: bgq_BqcSerdesAnalyzer could not connect to the database')
        return True

    def isLinkChipEventCountOverLimit(self, linkchip, block, event_time):
        '''Run a count query by location, block, bounded by the start_time plus and minus
           the backlog window.  
           If the count exceeds the limit, return True, otherwise return False.'''
        eventlogTable = self.appendSchema('tbgqeventlog')
        query = "select count(*) from " + eventlogTable + " where location = ? and block = ? and event_time > (timestamp('MYTIME') - PERIOD)"
        period = str(self.linkChipEventPeriod) + ' minutes'
        query = query.replace('PERIOD', period)
        query = query.replace('MYTIME', str(event_time))
        registry.get_logger().debug(query + ', ? => location=' + linkchip + ', block=' + block + ', (count limit=' + str(self.linkChipEventLimit) + ')')
        # retry in case the connection timed out
        for x in range(5):
            try:
                self.cursor.execute(query, linkchip, block)
                row = self.cursor.fetchone()
                if row: 
                    if row[0] > self.linkChipEventLimit:
                        return True
                    else:
                        return False
            except Exception as e:
                registry.get_logger().error('isEventCountOverLimit exception ' + str(e))
                if x < 4:
                    dbi = registry.get_service(SERVICE_DB_INTERFACE)
                    self.dbConn = dbi.get_connection()
                    self.cursor = self.dbConn.cursor()
                else:
                    raise Exception('Error: bgq_BqcSerdesAnalyzer could not connect to the database')
        return False

    def pruneHandledJobs(self):
        jobs = self.handledJobs.keys()
        for job in jobs:
            handled_time = self.handledJobs[job]
            if (datetime.now() - timedelta(hours=24)) > handled_time:
                # remove the job from the map
                self.handledJobs.pop(job)
        return

    def addHandledJob(self, jobid):
        if jobid not in self.handledJobs:
            self.handledJobs[jobid] = datetime.now()
            # prune entries older then a day 
            self.pruneHandledJobs()
        return

    def pruneHandledQualifiers(self):
        qualifiers = self.handledQualifiers.keys()
        for qualifier in qualifiers:
            handled_time = self.handledQualifiers[qualifier]
            if (datetime.now() - timedelta(hours=24)) > handled_time:
                # remove the qualifier from the map
                self.handledQualifiers.pop(qualifier)
        return

    def addHandledQualifier(self, qualifier):
        if qualifier not in self.handledQualifiers:
            self.handledQualifiers[qualifier] = datetime.now()
            # prune entries older then a day 
            self.pruneHandledQualifiers()
        return

    def checkForMultiLinkIssue(self, event, messageCode):
        ''' Return None if this ras strom event appears to be a 
            multi-link issue.
            Return the NDLinkEvent if this event appears to be related 
            to a single link.'''
        qualifier = self.getQualifier(event)
        if qualifier in self.handledQualifiers.keys():
            registry.get_logger().debug('skipping analysis of the event since analysis was already completed for this block qualifier=' + str(qualifier))
            return None

        # add this job to map of jobs to timestamp of handled jobs
        self.addHandledQualifier(qualifier)
        
        # check if there are more ras storm event locations then the limit
        msg_id = event.get_event_id()
        limit = 2 
        # add a delay to allow time for a multi-link issue to form
        time.sleep(3) # delay for 3 seconds
        if self.isEventCountOverLimitForQualifier(msg_id, qualifier, limit):
            registry.get_logger().debug('skipping analysis of the event because the block appears to have multiple link issues.  RAS event qualifier = ' + str(qualifier))
            return None
        
        # If the ras storm event has a job id, check if this job id is already 
        # associated with a handled issue, if so return None
        location = str(event.get_src_loc())
        location = location[3:].strip()
        jobid = event.raw_data['jobid']
        if jobid == None: 
            # job id is not available, try using the qualifier
            ndLinkEvent = self.getNDLinkEventForQualifier(location, messageCode, qualifier)
            return ndLinkEvent

        # create an NDLinkEvent for the first event found for this 
        # jobid, location, and messagecode
        ndLinkEvent = self.getNDLinkEvent(location, messageCode, jobid)
        return ndLinkEvent


    def analyzeRasStorm(self, event):
        # Check if this fatal ras storm is associated with an ND link error
        #    '00080037' RX, '00080038' TX, '00080039' RX
        # If it is a ND link error and the problem is not due to a bigger
        # issue like node board power off then return the associated event ND event
        # Else return None
        message = event.raw_data['message']
        messageCode = ''
        rindex = message.find('message code ')
        if rindex >= 0:
            rindex += len('message code ')
            messageCode = message[rindex:rindex+8]
            if messageCode in self.ndLinkError:
                # determine if this is single link issue or multiple links
                # and return the NDLinkEvent for a single link issue
                return self.checkForMultiLinkIssue(event, messageCode)
            else:
                return None
        return None

    def analyzeBootFailure(self, event):
        # should be able to combine this into one query

        # check if this boot failure is due to a training error
        # if so, return that event
        newEvent = None
        newEvent = self.analyzeTrainingErrorBootFailure(event)
        if newEvent:
            return newEvent
        # check if this boot failure is due to a ND link error + machine check
        # if so, return that event
        newEvent = self.analyzeNDLinkErrorBootFailure(event)
        return newEvent

    def getQualifier(self, event):
        qualifier = None
        # attempt to get the qualifier for the teal event raw data
        try: 
            qualifier = event.raw_data['qualifier']
        except KeyError, e:
            registry.get_logger().debug("qualifier key not supported: " + str(e))
        # perform a query to get the qualifier
        if qualifier == None:
            eventlogTable = self.appendSchema('tbgqeventlog')
            query = "select qualifier from " +  eventlogTable + " where recid = ?"
            recid = event.get_rec_id()
            registry.get_logger().debug(query + ', ? => recid=' + str(recid))
            # retry in case the connection timed out
            for x in range(5):
                try:
                    self.cursor.execute(query, recid)
                    row = self.cursor.fetchone()
                    if row:
                        return row[0]
                except Exception, e:
                    registry.get_logger().debug(e)
                    if x < 4:
                        dbi = registry.get_service(SERVICE_DB_INTERFACE)
                        self.dbConn = dbi.get_connection()
                        self.cursor = self.dbConn.cursor()
                    else:
                        raise Exception('Error: bgq_BqcSerdesAnalyzer could not connect to the database')
        return qualifier

    def analyzeTrainingErrorBootFailure(self, event):
        # Check if the boot failure had training errors.
        # Use the qualifier from the original boot error, then look for nodes 
        # that had a training error.
        # Return None if no training errors or there are more than two 
        # locations with errors (a bigger problem).
        qualifier = self.getQualifier(event)
        eventlogTable = self.appendSchema('tbgqeventlog')
        query = "select recid, event_time, location, msg_id, severity, serialnumber, ecid, block, message, diags, count, rawdata from " +  eventlogTable + " where qualifier = ? and ("
        # add the message ids to find
        first = True
        for tmsgid in self.trainingErrors:
            if not first:
                query += " or "
            query += "msg_id = '"
            query += tmsgid
            query += "'"
            first = False
        for pmsgid in self.prbsErrors:
            if not first:
                query += " or "
            query += "msg_id = '"
            query += pmsgid
            query += "'"
            first = False
        query += ")"
        registry.get_logger().debug(query + ', ? => qualifier =' + str(qualifier))
        # add a delay to allow time for a multi-link issue to form
        time.sleep(3) # delay for 3 seconds
        # retry in case the connection timed out
        for x in range(5):
            try:
                trainingEvent = None
                nodes = set()
                self.cursor.execute(query, qualifier)
                rows = self.cursor.fetchall()
                for row in rows:
                    recid = row[0]
                    event_time = row[1]
                    location = row[2].strip()
                    msg_id = row[3]
                    severity = row[4].strip()
                    serialnumber = row[5]
                    ecid = row[6]
                    block = row[7]
                    if block:
                        block = block.strip()
                    message = row[8]
                    if message:
                        message = message.strip()
                    diags = row[9]
                    count = row[10]
                    rawdata = row[11]
                    txampFlag = False
                    if rawdata:
                        rawdata.strip()
                        # training errors come in pairs, skip the ones with
                        # the TXAMP information
                        if rawdata.find('TXAMP=') >= 0:
                            txampFlag = True
                    jobid = None
                    if trainingEvent == None and not txampFlag:
                        # Create an event for the associated training error
                        registry.get_logger().debug('analyzeBootFailure creating NDLinkEvent for recid=' + str(recid))
                        trainingEvent = NDLinkEvent(event_time, msg_id, recid, location, severity, serialnumber, ecid, block, jobid, message, rawdata, qualifier)
                    if location not in nodes:
                        nodes.add(location)
                        if len(nodes) > 2:
                            # more than two node locations with issues 
                            # return None as this is likely a more complex case
                            registry.get_logger().info('analyzeBootFailure returning None for NDLinkEvent because more then two locations had machine checks and ND link issues.  ras event qualifier=' + str(qualifier))
                            return None
                return trainingEvent
            except Exception as e:
                registry.get_logger().error('analyzeBootFailure exception ' + str(e))
                if x < 4:
                    dbi = registry.get_service(SERVICE_DB_INTERFACE)
                    self.dbConn = dbi.get_connection()
                    self.cursor = self.dbConn.cursor()
                else:
                    raise Exception('Error: bgq_BqcSerdesAnalyzer could not connect to the database')
        return None

    def analyzeNDLinkErrorBootFailure(self, event):
        # Check if the boot failure had a machine check on a node or pair 
        # of nodes that had serdes errors.  Use the qualifier from the 
        # original boot error, then look for nodes that had a machine check 
        # and an ND link error.  Return None if the conditions are not 
        # satisfied or there are more than two locations (a bigger problem).
        be_recid = event.get_rec_id()
        eventlogTable = self.appendSchema('tbgqeventlog')
        query = "select nd.recid, nd.event_time, nd.location, nd.msg_id, nd.severity, nd.serialnumber, nd.ecid, nd.block, nd.message, nd.diags, nd.count, nd.rawdata, nd.qualifier from " +  eventlogTable + " be, "  +  eventlogTable + " mc, "  +  eventlogTable + " nd where be.recid = ? and mc.qualifier = be.qualifier and mc.msg_id = '0008001F' and mc.qualifier = nd.qualifier and mc.location = nd.location and (nd.msg_id = '00080037' or nd.msg_id = '00080038' or nd.msg_id = '00080039') "
        registry.get_logger().debug(query + ', ? => recid=' + str(be_recid))
        # retry in case the connection timed out
        for x in range(5):
            try:
                ndEvent = None
                nodes = set()
                self.cursor.execute(query, be_recid)
                rows = self.cursor.fetchall()
                for row in rows:
                    recid = row[0]
                    event_time = row[1]
                    location = row[2].strip()
                    msg_id = row[3]
                    severity = row[4].strip()
                    serialnumber = row[5]
                    ecid = row[6]
                    block = row[7]
                    if block:
                        block = block.strip()
                    message = row[8]
                    if message:
                        message = message.strip()
                    diags = row[9]
                    count = row[10]
                    rawdata = row[11]
                    if rawdata:
                        rawdata.strip()
                    qualifier = row[12]
                    if qualifier:
                        qualifier = qualifier.strip()
                    jobid = None
                    if ndEvent == None:
                        # Create an event for the associated ND link error
                        registry.get_logger().debug('analyzeBootFailure creating NDLinkEvent for recid=' + str(recid))
                        ndEvent = NDLinkEvent(event_time, msg_id, recid, location, severity, serialnumber, ecid, block, jobid, message, rawdata, qualifier)
                    if location not in nodes:
                        nodes.add(location)
                        if len(nodes) > 2:
                            # more than two node locations with issues 
                            # return None as this is likely a more complex case
                            registry.get_logger().info('analyzeBootFailure returning None for NDLinkEvent because more then two locations had machine checks and ND link issues.  ras event qualifier=' + str(qualifier))
                            return None
                return ndEvent
            except Exception as e:
                registry.get_logger().error('analyzeBootFailure exception ' + str(e))
                if x < 4:
                    dbi = registry.get_service(SERVICE_DB_INTERFACE)
                    self.dbConn = dbi.get_connection()
                    self.cursor = self.dbConn.cursor()
                else:
                    raise Exception('Error: bgq_BqcSerdesAnalyzer could not connect to the database')
        return None

    def analyze_event(self, event):
        '''Analyze a RAS event and determine whether threshold has been
        reached or exceeded.
        '''
        msg_id = event.get_event_id()
        rec_id = event.get_rec_id()
        registry.get_logger().info("Analyzing msgid = " + msg_id + " recid = " + str(rec_id))                            

        # Exclude events logged from DIAG run (note: diag ras is no longer logged)
        if event.raw_data['diags'] == 'T':
            registry.get_logger().debug('RAS Event generated by Diagnostics, skip creating an alert')
            return

        if msg_id == '0006000A':
            # check if the boot failure was due to a node or pair of nodes
            # that have a serdes errors
            ndEvent = self.analyzeBootFailure(event)
            if ndEvent:
                event = ndEvent
                msg_id = event.get_event_id()
                rec_id = event.get_rec_id()
                registry.get_logger().info("Switching analysis to msgid = " + msg_id + " recid = " + str(rec_id))
            else:
                registry.get_logger().debug('Skipping serdes analysis since the boot failure does not appear to be due to a serdes error, msg_id=' + msg_id +  " and recid=" + str(rec_id))
                return

        if msg_id == '00080014':
            # check if the ras storm was due to a node or pair of nodes
            # that have a serdes errors
            ndEvent = self.analyzeRasStorm(event)
            if ndEvent:
                event = ndEvent
                msg_id = event.get_event_id()
                rec_id = event.get_rec_id()
                registry.get_logger().info("Switching analysis to msgid = " + msg_id + " recid = " + str(rec_id))                            
            else:
                message = event.raw_data['message']
                messageCode = ''
                rindex = message.find(' message code ')
                if rindex >= 0:
                    messageCode = message[rindex:]
                registry.get_logger().debug('Skipping analysis of RAS Storm Event, msg_id=' + msg_id +  " recid=" + str(rec_id) + messageCode)
                return
            
        # try:                        
        serdesEvent = BqcSerdesEvent(event, self, self.rcvrLinkMap, self.xmitLinkMap, self.ioRcvrLinkMap, self.ioXmitLinkMap)

        # extract information about the primary and neighbor locations from the ras event
        serdesEvent.extractInfoFromRasEvent()
                                   
        # if this event happened prior to the current time minus 
        # the backlog window, then avoid setting the HW in error
        if ((datetime.now() - timedelta(minutes=self.serdesBacklogWindow)) > event.get_time_occurred()):
            registry.get_logger().info('Serdes RAS Event for msg_id=' + msg_id +  ' And recid=' + str(rec_id) + ' the training error happened at ' + str(event.get_time_occurred()) + ' which is over ' + str(self.serdesBacklogWindow) + ' minutes ago, the analysis will not mark the hardware in error')
            serdesEvent.mark_hw_in_error = False
        else:
            serdesEvent.mark_hw_in_error = True
            
        if self.serdesScope == 'copperLinks' and (serdesEvent.location[0:6] != serdesEvent.neighbor[0:6]):
            registry.get_logger().info('skipping analysis of Serdes RAS event recid=' + str(rec_id) + ' because the neighbor node is ' + serdesEvent.neighbor + ' is connected via an optical link and the scope of the BqcSerdesAnalysis is set to copperLinks.')
            return

        # verify the primary node is valid
        if serdesEvent.primaryLocationValid != True or not (serdesEvent.nodestatus == 'A' or serdesEvent.nodestatus == 'E'):
            registry.get_logger().error('skipping analysis of Serdes RAS event recid=' + str(serdesEvent.rec_id) + ' because location ' + serdesEvent.location + ' is not valid.  Status=' + serdesEvent.nodestatus )
            return

        # verify the neighbor node is valid
        if serdesEvent.neighborLocationValid != True or not (serdesEvent.neighborstatus == 'A' or serdesEvent.neighborstatus == 'E'):
            registry.get_logger().error('skipping analysis of Serdes RAS event recid=' + str(serdesEvent.rec_id) + ' because neighbor node ' + serdesEvent.neighbor + ' is not valid.  Status=' + serdesEvent.neighborstatus )
            return

        # verify the extra neighbor node is active
        if serdesEvent.extraIoNeighbor:
            if not (serdesEvent.extraNeighborstatus == 'A' or serdesEvent.extraNeighborstatus == 'E'):
                registry.get_logger().error('skipping analysis of Serdes RAS event recid=' + str(serdesEvent.rec_id) + ' because neighbor node ' + serdesEvent.extraIoNeighbor + ' is not valid. Status=' + serdesEvent.extraNeighborstatus )
                return
        
        # analyze the data to determine the swap and/or replace actions
        serdesEvent.determineActions()

        # send ras events that contain the recommended actions
        serdesEvent.sendRas()

        # except Exception as ex:
        # registry.get_logger().error('BqcSerdesEvent encountered an exception: ' + str(ex))
        return 


    def handle_control_msg(self, control_msg):
        ''' Handle any control messages that have been sent. No special action
        required
        '''
        registry.get_logger().debug('...Control message received: {0}'.format(control_msg))
