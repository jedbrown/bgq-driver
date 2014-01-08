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
# (C) Copyright IBM Corp.  2010, 2011                              
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

import os, time

class RasDocGen:

	'''The RasDocGen is responsible for generating the 
	Ras Event documentation.   The documentation is in
	html format.   The format may change to DocBook in 
	the future.'''
	 
	def __init__(self, path):
		self.filename = path + '/RasEventBook.htm'
		print 'Ras event book file name: ', self.filename
		self.preamble = '''<!-- begin_generated_IBM_copyright_prolog                             -->
<!--                                                                  -->
<!-- This is an automatically generated copyright prolog.             -->
<!-- After initializing,  DO NOT MODIFY OR MOVE                       -->
<!--  --------------------------------------------------------------- -->
<!-- Product(s):                                                      -->
<!--     Blue Gene/Q Licensed Machine Code                            -->
<!--                                                                  -->
<!-- (C) Copyright IBM Corp.  2010, 2010                              -->
<!-- All rights reserved.                                             -->
<!-- US Government Users Restricted Rights -                          -->
<!-- Use, duplication or disclosure restricted                        -->
<!-- by GSA ADP Schedule Contract with IBM Corp.                      -->
<!--                                                                  -->
<!-- Licensed Materials-Property of IBM                               -->
<!--  --------------------------------------------------------------- -->
<!--                                                                  -->
<!-- end_generated_IBM_copyright_prolog                               -->
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
  <HEAD>
    <TITLE>
       BlueGene/Q RAS Events
    </TITLE>
  </HEAD>
  <BODY bgcolor="#66FFFF">
     <H1>
       BlueGene/Q RAS Events
     </H1>
        <P>This document includes the list of possible RAS Events along with link to the event details.</P>
        <P>RAS Events are uniquely identified by the message id.   
        The Component is the software component detecting and reporting the event.  The list of components include:</P>
        <UL>
            <LI>
                 <A href="#CNK">CNK</A> - Compute Node Kernel.
            </LI>
            <LI>
                 <A href="#DIAGS">DIAGS</A> - Diagnostic functions running on a Compute or an I/O or the sevice node.
            </LI>
            <LI>
                 <A href="#BGMASTER">BGMASTER</A> - The process running on the service node that monitors control system processes.
            </LI>
            <LI>
                 <A href="#MC">MC</A> - The machine controller running on the sevice node.
            </LI>
            <LI>
                 <A href="#MCSERVER">MCSERVER</A> - The process for machine controller.
            </LI>
            <LI>
                 <A href="#MMCS">MMCS</A> - The control system running on the sevice node.
            </LI>
            <LI>
                 <A href="#BAREMETAL">BAREMETAL</A> - Service related facilities.
            </LI>
            <LI>
                 <A href="#FIRMWARE">FIRMWARE</A> - Firmware.
            </LI>
            <LI>
                 <A href="#CTRLNET">CTRLNET</A> - Control Net.
            </LI>
            <LI>
                 <A href="#LINUX">LINUX</A> - Linux.
            </LI>
            <LI>
                 <A href="#CIOS">CIOS</A> - CIOS.
            </LI>
            <LI>
                 <A href="#MUDM">MUDM</A> - Memory Unit.
            </LI>
            <LI>
                 <A href="#SPI">SPI</A> - SPI.
            </LI>
        </UL>

        <P>RAS Events can have one of the following severities:
	<UL>
            <LI>
                 FATAL - designates severe error events that will presumably lead the application to fail or prevent a block from booting.
            </LI>
            <LI>
                 WARN - designates situations like a soft error threshold being exceeded or a redundant component failed.
	    </LI>
            <LI>
	         INFO - designates informational messages that highlight the progress of system software.
            </LI>
        </UL>
'''
		self.postamble = '''  </BODY>
</HTML>
'''
                self.sumPreamble = '''    <TABLE>
	<COLGROUP>
		<COL WIDTH=6%>
		<COL WIDTH=5%>
		<COL WIDTH=10%>
		<COL WIDTH=19%>
		<COL WIDTH=5%>
		<COL WIDTH=5%>
		<COL WIDTH=50%>
	</COLGROUP>
      <THEAD>
        <TR>
          <TD>
            MSG ID
          </TD>
          <TD>
            SEV
          </TD>
          <TD>            CATEGORY
          </TD>
          <TD>
            CTRL_ACTION
          </TD>
          <TD>
            COUNT 
          </TD>
          <TD>
            PERIOD
          </TD>
          <TD>
            MESSAGE 
          </TD>
        </TR>
      </THEAD>
      <TBODY>
'''             
                self.sumPostamble = '''      </TBODY>
    </TABLE>
'''
		return
                

	def gen(self, events):
		self.writeFile(events)
		return

	def writeFile(self, events):
		f = open(self.filename, 'w')
		f.write(self.preamble)
                self.writeTime(f)
                self.writeSummaryTable(f,events)
                self.writeDetails(f,events)
		f.write(self.postamble)
		f.close()
		return                
        
        def writeTime(self, f):
                f.write('      <P>')
                f.write('      	 Generated ' + time.asctime(time.localtime()))
                f.write('      </P>')
                f.write('    <HR>')
                return			

        def writeSummaryTable(self,f,events):
                # write summary events
		id_keys = events.keys()
                id_keys.sort()
                prevComp = ''
                num = 0
		for i_key in id_keys:
 			id = i_key;                        
			details = events[i_key]
                        comp = details['component']
                        if comp != prevComp:
                                if num != 0:
 					f.write(self.sumPostamble)
                        	f.write('    <P>')
                        	f.write('    <HR>')
                        	f.write('    <H2>')
                        	f.write('      <A name="' + comp + '"></A>Component: ' + comp)
				f.write('    </H2>')
				f.write(self.sumPreamble)
 				prevComp = comp
			category = details['category']
			sev = details['severity']
			ca = details['control_action']
			msg = details['message']
			t_count = details['threshold_count']
			t_period = details['threshold_period']
			self.writeSummaryRow(f, id, sev, category, ca, t_count, t_period, msg)
                        num = num + 1
                f.write(self.sumPostamble)      
                f.write('    <HR>') 
                return              
		
        def writeSummaryRow(self, f, id, sev, category, ctlr_act, count, period, msg):
                f.write('        <TR>')
                f.write('          <TD>')
                f.write('            <A href="#' + id + '">' + id + '</A>')
                f.write('          </TD>')
                f.write('          <TD>')
                f.write('            ' + sev)
                f.write('          </TD>')
                f.write('          <TD>')
                f.write('            ' + category)
                f.write('          </TD>')
                f.write('          <TD>')
                f.write('            ' + ctlr_act)
                f.write('          </TD>')
                f.write('          <TD>')
                f.write('            ' + count)
                f.write('          </TD>')
                f.write('          <TD>')
                f.write('            ' + period)
                f.write('          </TD>')
                f.write('          <TD>')
                f.write('            ' + msg)
                f.write('          </TD>')
                f.write('        </TR>')
                return

	def writeDetails(self, f, events):
		# write details events
		id_keys = events.keys()
                id_keys.sort()
                f.write('    <HR>')
		for i_key in id_keys:
			id = i_key;                        
			details = events[i_key]
                        comp = details['component']
			self.writeSection(f, id, comp, details)
                        f.write('    <HR>')
                f.write('    <HR>')
		return
			

        def writeSection(self, f, id, comp, details):        
                f.write('    <H2>')
                f.write('      <A name="' + id + '"></A>Message Id: ' + id)
                f.write('    </H2>')
		f.write('<UL>\n')
		f.write('<LI>Component - ' + comp + '</LI>\n')
                category = details['category']
		f.write('<LI>Category - ' + category + '</LI>\n')
		sev = details['severity']
		f.write('<LI>Severity - ' + sev + '</LI>\n')
		msg = details['message']
		f.write('<LI>Message - ' + msg + '</LI>\n')
		dec = details['decoder']
		if len(dec):
			f.write('<LI>Decoder - ' + dec + '</LI>\n')
		desc = details['description']		
		f.write('<LI>Description - ' + desc + '</LI>\n')
		act = details['service_action']
		f.write('<LI>Service Action - ' + act + '</LI>\n')
		ctl = details['control_action']
		if len(ctl):
			f.write('<LI>Control Action - ' + ctl + '</LI>\n')
		t_cnt = details['threshold_count']
		if len(t_cnt):
			f.write('<LI>Threshold Count - ' + t_cnt + '</LI>\n')
		t_per = details['threshold_period']
		if len(t_per):
			f.write('<LI>Threshold Period - ' + t_per + '</LI>\n')
		diags = details['relevant_diags']
		if len(diags):
			f.write('<LI>Relevant diagnostic suites - ' + diags + '</LI>\n')
		#frus = details['frus']
		#if len(frus):
		#	f.write('<LI>FRUs - ' + frus + '</LI>\n')
		fn = details['file_name']
		f.write('<LI>Source File - ' + self.fileStr(fn) + '</LI>\n')
		ln = details['line_num']
		f.write('<LI>Line Number - ' + ln + '</LI>\n')
		f.write('</UL>\n')
                #f.write('    <HR>')
                return

	def fileStr(self, file):
		index = file.find('/bgq/')
 		if (index >= 0):
			return file[index:]
		else:
			return file
		
import getopt, sys

# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
def usage():
    print sys.argv[0], '[-h] root'
    print '   where -h for help'

# ----------------------------------------------------------------------------
# main
# ----------------------------------------------------------------------------
def test():
    '''Self test of RasDocGen'''
    try:
        optlist, args = getopt.getopt(sys.argv[1:], "h", ["help"])

    except getopt.GetoptError:
        usage()
        sys.exit(1)
    
    for o, a in optlist:    
        if o in ("-h", "--help"):
            usage()
            sys.exit(0)
    
    events = {}
    dg = RasDocGen()
    dg.gen(events)

if __name__=='__main__':
	test()
