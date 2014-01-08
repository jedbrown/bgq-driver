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

import os, sys, getopt, fnmatch
from rasDocGen import RasDocGen
from rasTableGen import RasTableGen
from os import path
from stat import *
import xml.parsers.expat


#===============================

class ElementParser:
	'''ElementParser is responsible for parsing an XML 
        element string.  The attributes are extracted and 
        returned in a dictionary.'''
	
	def start_element(self, name, attrs):
		# print 'Start element:', name, attrs
                for a in attrs:
		        self.attrs.update(attrs)
		return
	
	def parseElement(self, element):
		# print element
		self.attrs = {}
		p = xml.parsers.expat.ParserCreate()
		p.StartElementHandler = self.start_element
		p.Parse(element,1)
		return self.attrs

#============================

class FileScanner:
	
	'''The FileScanner is a base class for classes that 
        scan a file looking for an element using a begging 
        pattern and end pattern string.  The element is passed
        to a Parser which parses the element into a dictionary
        of attribute/value pairs.   The FileScanner caches the 
        dictionary returned by the Parser in a list which can 
        be accessed via the getElements method.'''
	
	def __init__(self, parser, el_start, el_end):
		self.parser = parser
                self.el_start = el_start
                self.el_end = el_end
		self.elements = []
		return
	
	def scan(self,filename):
		# print 'opening ' + filename + ' for reading'
		f = open(filename, 'r')
		inElement = False
		element = ""
		line_cnt = 0
		start_line = 0
		for line in f:
                        # print line
			line_cnt += 1
			if line.find(self.el_start) >= 0:
				inElement = True
				start_line = line_cnt
				# print "found ", self.el_start, " in ", filename, ", at line ", start_line
			if inElement:
                                endex = line.find(self.el_end) 
				if endex >= 0:
                                        element += line[0:endex+len(self.el_end)]
					inElement = False
					# print filename
					try:
						attrs = self.parser.parseElement(element)
						# check if the element provides
						# its own filename and line num
						if not attrs.has_key('file_name'):
							attrs['file_name'] = filename
							attrs['line_num'] = start_line
						self.elements.append(attrs)
					except Exception , e:
						if element.find('//') == -1:
							# ignore commented out metadata
							print "ERROR: RAS_Event element did not parse"
							print "file: ", filename
							print "line number: ", start_line
							print element
							print e
					element = ""
                                else:                                        
				        element += line
		f.close
		return
        	
	def getElements(self):
		return self.elements

	def resetElements(self):
		self.elements = []


#=================================================

class RasEventSorter:

	'''The RasEventSorter is responsible for converting
	a list of Ras Events into a structured set of 
	dictionaries.'''

	 
	def __init__(self):
		self.events = {}
		self.errorStart = '*********** RAS Metatdata Issue Start ******** '
		self.errorEnd   = '*********** RAS Metatdata Issue End ********** '
		return
	
	def addEvent(self, id, category, comp, sev, msg, desc, act, decoder, ctl, thrs_cnt, thrs_per, diags, fn, ln):
		# get the id dictionary
		id_dict = self.events.get(id,{})
		if id_dict != {}:
			if id_dict['id'] == id and id_dict['category'] == category and id_dict['component'] == comp and id_dict['severity'] == sev and id_dict['message'] == msg and id_dict['decoder'] == decoder and id_dict['description'] == desc and id_dict['service_action'] == act and id_dict['control_action'] == ctl:
				#print warning for duplicate - exact match
				print self.errorStart 
				print 'Warning: Duplicate RAS Event definitions detected!  Metadata appears the same.'
				print '   Def (1): id= ', id_dict['id'], " category=", id_dict['category'], " comp= ", id_dict['component'], " file=", id_dict['file_name'], " line=", id_dict['line_num']
				print '----------------------------------------------- '
				print '   Def (2): id= ', id, " category=", category, " comp= ", comp, " file=", fn, " line=", ln
				print self.errorEnd
			else:
				#print error for duplicate - don't match 
				print self.errorStart 
				print 'Error: Duplicate RAS Event definitions detected!  Metadata details vary. '
				print '   Def (1): id= ', id_dict['id'], " category=", id_dict['category'], " comp= ", ec_dict['component'], " file=", id_dict['file_name'], " line=", id_dict['line_num']
				print '----------------------------------------------- '
				print '   Def (2): id= ', id, " category=", category, " comp= ", comp, " file=", fn, " line=", ln
				print self.errorEnd
		id_dict['id'] = id
		id_dict['severity'] = sev
		id_dict['category'] = category
		id_dict['component'] = comp
		id_dict['message'] = msg
		id_dict['decoder'] = decoder
		id_dict['description'] = desc
		id_dict['service_action'] = act
		id_dict['control_action'] = ctl
		id_dict['threshold_count'] = thrs_cnt
		id_dict['threshold_period'] = thrs_per
		id_dict['relevant_diags'] = diags
		id_dict['file_name'] = fn
		id_dict['line_num'] = ln
		self.events[id] = id_dict
		return

	def sort(self, events):
		# add events to the current list of components, subcomponents, and error codes
		for e in events:
                        try: 
                                # print e
				id = e['id']
			        category = e['category']
			        comp = e['component']
				sev = e['severity']
			        msg = e['message']
				decoder = e.get('decoder', "")
			        desc = e['description']
			        act = e['service_action']
				ctl = e.get('control_action', "")
				thrs_cnt = e.get('threshold_count', "")
				thrs_per = e.get('threshold_period', "")
				diags = e.get('relevant_diags', "")
			        fn = e['file_name']
			        ln = e['line_num']
			        self.addEvent(id, category, comp, sev, msg, desc, act, decoder, ctl, thrs_cnt, thrs_per, diags, fn, ln)
                        except KeyError, k:
				print self.errorStart
				print 'KeyError: ', k
				print '   The key was not found in the set of RAS enum files.'
				print '   Action: define the key.'
				print '   RasEventSorter rejecting event: ', e
                                print '   Check file ', e['file_name'], ", line ", e['line_num']
				print self.errorEnd
		return self.events

#==============================

#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
def usage():
    print sys.argv[0], '[-h] components src_dirs gen_dir'
    print '   where -h for help'
    print '         src_dirs - list of top level install directories to read the metadata from'
    print '         gen_dir - directory to write the output file'
    return


def readExtract(f_name):
    print 'extract events from ', f_name
    try:
        p = ElementParser()
        s = FileScanner(p,'<rasevent ', '/>')
        s.scan(f_name)
        return s.getElements()
    except IOError, e:
        print e
        return []

# ----------------------------------------------------------------------------
# main
# ----------------------------------------------------------------------------
def main():

    try:
        optlist, args = getopt.getopt(sys.argv[1:], "h", ["help"])
        
    except getopt.GetoptError:
        usage()
        sys.exit(1)
    
    for o, a in optlist:    
        if o in ("-h", "--help"):
            usage()
            sys.exit(0)

    print args
    if args == None or len(args) < 2 or len(args) > 2:
        usage()
        sys.exit(1)

    events = []
    src_dirs = args[0].split(';')
    for dir in src_dirs:
        try:
            mode = os.stat(dir)[ST_MODE]
            if not S_ISDIR(mode):
                continue
            
            # walk through the components in the directory 
            for d in os.listdir(dir):
                if d == '.svn':
                    continue
                comp = os.path.join(dir, d)
                if path.exists(comp) == True:
                    mode = os.stat(comp)[ST_MODE]
                    if S_ISDIR(mode):
                        # It's a directory 
                        subdir = comp + '/ras/metadata/'
                        try:
                            mode = os.stat(subdir)[ST_MODE]
                            if S_ISDIR(mode):
                                for f in os.listdir(subdir):
                                    f_md = os.path.join(subdir, f)
                                    mode = os.stat(f_md)[ST_MODE]
                                    if S_ISREG(mode):
                                        #It's a file, check if RAS metadata file
                                        if fnmatch.fnmatch(f, '*_ras.xml'):
                                            t_events = readExtract(f_md)
                                            events.extend(t_events)
                  
                        except OSError, e:
                           continue   
                else:
                    print 'Warning: No such file or directory: ', comp
                    
        except OSError, e:
            print 'Error traversing components in the source directory: ', e
            continue


    sorter = RasEventSorter()
    sorted_events = sorter.sort(events)
    
    doc_gen = RasDocGen(args[1])
    doc_gen.gen(sorted_events)
    
    tab_gen = RasTableGen(args[1])
    tab_gen.gen(sorted_events)   

    
# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
main()
