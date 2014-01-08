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

import os, string, sys, getopt, datetime, fnmatch
from os import path
from stat import *
from time import *
import xml.parsers.expat
from xml.sax.handler import ContentHandler
import xml.sax

class ElementParser:
    '''ElementParser is responsible for parsing an XML 
    element string.  The attributes are extracted and 
    returned in a dictionary.'''
    
    def start_element(self, name, attrs):
        # print 'Start element:', name, attrs
        self.name = name
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

class FileScanner:
	
    '''The FileScanner is a base class for classes that 
    scan a file looking for an element using a begging 
    pattern and end pattern string.  The element is passed
    to a Parser which parses the element into a dictionary
    of attribute/value pairs.   The FileScanner caches the 
    dictionary returned by the Parser in a list which can 
    be accessed via the getElements method.'''
	
    def __init__(self, parser, el_start, el_end):
        self._parser = parser
        self._el_start = el_start
        self._el_end = el_end
        self._elements = []
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
            if line.find(self._el_start) >= 0:
                inElement = True
                start_line = line_cnt
                # print "found ", self._el_start, " in ", filename, ", at line ", start_line
            if inElement:
                endex = line.find(self._el_end) 
                if endex >= 0:
                    element += line[0:endex+len(self._el_end)]
                    inElement = False
                    # print filename
                    try:
                        attrs = self._parser.parseElement(element)
                        attrs['file_name'] = filename
                        attrs['line_num'] = start_line

                        # If the id starts with 0x or 0X, remove it (i.e.: 0x00001234 -> 00001234)
                        id = attrs['id']
                        if id.startswith("0x"):
                            attrs['id'] = id.replace("0x", "")
                        elif id.startswith("0X"):
                            attrs['id'] = id.replace("0X", "")
                            
                        self._elements.append(attrs)
                        name = '<' + self._parser.name
                        if name != self._el_start:
                            print "WARNING: the RAS event metadata has unexpected extra characters on its start element:", name, "(it must be:", self._el_start +  "), file name:", filename + ", line number:", start_line
                    except Exception:
                        if element.find('//') == -1:
                            # ignore commented out metadata
                            print "ERROR: rasevent element did not parse"
                            print "file: ", filename
                            print "line number: ", start_line
                            print element
                    element = ""
                else:                                        
                    element += line
        f.close
        return
        	
    def getElements(self):
        return self._elements

    def resetElements(self):
        self._elements = []

class FileSystemWalker:

    '''The FileSystemWalker will traverse the file system
    beginning at the roots of the tree looking for files that 
    match the pattern and time criteria.   The FileScanner 
    is called for each matching file.  The root directories
    are ';' seperated'''
    
    def __init__(self, scanner, patterns='*'):
        '''param scanner = scanner that is notified as file 
        matches are found param patterns = ';' seperated 
        list of patterns that file names must match param''' 
        self._scanner = scanner
        self._patterns = patterns.split(';')
        # print self.patterns

		
    def walk(self, roots):
        '''recursively descend the file system starting at each  
        root that is specified.   Call the scanner for each file 
        matching the criteria.'''
        root_list = roots.split(';')
        for root in root_list:
            # print 'tree traversal starting at ', root
            # handle case where root does not exist
            mode = os.stat(root)[ST_MODE]
            if not S_ISDIR(mode):
                continue
            # process the files in the directory
            for f in os.listdir(root):
                if f == '.svn':
                    continue
                name = os.path.join(root, f)
                # print 'process file %s' % name
                try:
                    mode = os.stat(name)[ST_MODE]
                    if S_ISREG(mode):
                        # It's a file, check if it has been modified after the mtime

                        # print 'check file %s for match' % f
                        for pattern in self._patterns:
                            if fnmatch.fnmatch(f,pattern):
                                # print 'file match on %s' % f
                                self._scanner.scan(name)
                                break
                    if S_ISDIR(mode):
                        # It's a directory, recurse into it
                        self.walk(name)
                except Exception,e:
                    print e
                    pass
        return


class ConfigFileParser(ContentHandler):
    '''ConfigFileParser is responsible for parsing ras.xml file
    The attributes are extracted and returned in a dictionary.'''

    def __init__(self, repository):
        self.name = ''
        self.repository = repository
        return
    
    def startElement(self, name, attrs):
        '''Parse each element in the ras.xml file.
        '''
        if name == 'category':
            self.name = attrs.getValue('name')
            categories = self.repository.get('ras_categories', list())
            categories.append(self.name)
            self.repository['ras_categories'] = categories
        elif name == 'component':
            self.name = attrs.getValue('name')
            components = self.repository.get('ras_components', list())
            components.append(self.name)
            self.repository['ras_components'] = components
            startId = self.name + '.start_id'
            endId = self.name + '.end_id'
            self.repository[startId] = attrs.getValue('start_id')
            self.repository[endId] = attrs.getValue('end_id')
        elif name == 'severity':
            self.name = attrs.getValue('name')
            severities = self.repository.get('ras_severities', list())
            severities.append(self.name)
            self.repository['ras_severities'] = severities
        elif name == 'diagnostic':
            self.name = attrs.getValue('name')
            diagnostics = self.repository.get('ras_diagnostics', list())
            diagnostics.append(self.name)
            self.repository['ras_diagnostics'] = diagnostics
        elif name == 'control_action':
            self.name = attrs.getValue('name')
            ctlActions = self.repository.get('ras_control_actions', list())
            ctlActions.append(self.name)
            self.repository['ras_control_actions'] = ctlActions
        return
	
    def endElement(self, name):
        '''End parsing element
        '''
        return


class RasEventValidator:

    '''The RasEventValidator is responsible for validating a list of
    RAS Events.   It returns the RAS Events that pass validation in
    a dictionary with Ras Events ids as the key.'''
	 
    def __init__(self, config_file):
        self._events = {}
        self._errors = 0
        self._required_attributes = ['id', 'category', 'component', 'severity', 'message', 'description']
        self._optional_attributes = ['service_action', 'control_action', 'decoder', 'relavant_diags', 'threshold_count', 'threshold_period', 'frus', 'file_name', 'line_num']
        self._periods = ['YEAR', 'YEARS', 'MONTH', 'MONTHS', 'DAY', 'DAYS', 'HOUR', 'HOURS', 'MINUTE', 'MINUTES', 'SECOND', 'SECONDS', 'MICROSECOND', 'MICROSECONDS']
        self._db_fields = {'id': 8, 'category': 16, 'component': 16, 'severity': 8, 'message': 1024, 'description': 1024, 'service_action': 1024, 'control_action': 256, 'decoder': 64, 'thresholdperiod': 16, 'relevant_diags': 256}
        self._errorStart = '*********** RAS Metatdata Issue Start ******** '
        self._errorEnd   = '*********** RAS Metatdata Issue End ********** '
        self._repositories = dict()
        self._configFile = config_file
        cfg_handler = ConfigFileParser(self._repositories)
        p = xml.sax.make_parser()
        p.setContentHandler(cfg_handler)
        p.parse(config_file)
        return

    def get(self, name, value):
        return self._repositories.get(name, value)

    def events(self):
        return self._events

    def validateRequiredAttributes(self,event):
        # throw exception if the event does not have required attributes
        for a in self._required_attributes:
            try:
                test = event[a]
            except KeyError, k:
                msg = 'event missing required key: %s' %  k
                raise Exception(msg)
        return 

    def validateDBFields(self,event):
        # throw exception if the field exceeds the field width in the database
        for d in self._db_fields:
            try:
                value = event[d]
                limit = self._db_fields[d]
                if len(value) > limit:
                    msg = "the event '%s' attribute field width, %d, exceeds the database field witdth of %d.  %s = '%s'"  % (d, len(value), limit, d, value)
                    raise Exception(msg)
            except KeyError, k:
                continue
        return 

    def validateCategories(self,event):
        # throw exception if the category is not valid
        categories = self._repositories['ras_categories']
        found = False
        category = event['category']
        for c in categories:
            if c == category:
                found = True
                break
        if not found:
            msg = "the event category '%s' is not a valid value. Please see %s for valid categories."  % (category, self._configFile)
            raise Exception(msg)
        return

    def validateComponents(self,event):
        # throw exception if the component is not valid
        components = self._repositories['ras_components']
        found = False
        inRange = False
        component = event['component'].upper()
        for c in components:
            if c == component:
                found = True
                startId = c + '.start_id'
                endId = c + '.end_id'
                id = int(event['id'], 16)
                if id >= int(self._repositories[startId], 16) and id <= int(self._repositories[endId], 16):
                    inRange = True
                break
        if not found:
            msg = "the event component '%s' is not a valid value, Please see %s for valid components."  % (component, self._configFile)
            raise Exception(msg)
        if not inRange:
            msg = "the event id '%s' for component '%s' is not a valid.  The rannge must be '%s' - '%s'"  % (event['id'], component, self._repositories[startId], self._repositories[endId])
            raise Exception(msg)
        return
    
    def validateSeverity(self,event):
        # throw exception if the severity is not a valid level
        sevs = self._repositories['ras_severities']
        found = False
        severity = event['severity'].upper()
        for s in sevs:
            if s == severity:
                found = True
                break
        if not found:
            msg = "the event severity '%s' is not a valid value.  It must be one of %s"  % (severity, sevs)
            raise Exception(msg)
        return 

    def validateControlActions(self,event):
        # throw exception if the control action is not in the valid list
        ctlActions_list = self._repositories['ras_control_actions']
        ctlActions = event.get('control_action', "")
        if not ctlActions:
            return
        items = ctlActions.split(',')
        for ca in items:
            if ca == '':
                print "Empty"
                continue
            if ca not in ctlActions_list:
                msg = "the event control_action '%s' is not a valid value.  It must be one of %s"  % (ca, ctlActions_list)
                raise Exception(msg)
        return 

    def validateDiagnostics(self,event):
        # throw exception if the diagnostic is not in the valid list
        diags = self._repositories['ras_diagnostics']
        diagnostic = event.get('relevant_diags', "")
        if not diagnostic:
            return
        items = diagnostic.split(',')
        for d in items:
            if d == '':
                continue
            if d not in diags:
                print self._errorStart
                print "WARNING: the rasevent metadata has an invalid diagnostic bucket name in the relevant_diags attribute: ", d, ". Please see ", self._configFile, " for valid diagnostics buckets. "
                print event
                print self._errorEnd
        return

    def validatePeriod(self,event):
        # throw exception if the threshold period is not valid
        found = False
        items = event.get('threshold_period',"").split()
        if len(items) >= 2:
            period = items[len(items)-1].upper()
            for p in self._periods:
                if p == period:
                    found = True
                    break
        else:
            return
        if not found:
            msg = "the event threshold_period '%s' is not a valid value.  It must be one of %s"  % (period, self._periods)
            raise Exception(msg)
        return 

    def validateNoDuplicate(self,event):
        # throw exception if the id has already been defined
        if event['id'] in self._events:
            msg = "the event id 0x" + event['id'] + " was declared multiple times."
            raise Exception(msg)
        return 

    def error(self, msg, event):
        self._errors += 1
        print self._errorStart
        print 'ERROR: Rejecting RAS Event because', msg
        print event
        print self._errorEnd
        return

    def validate(self, events):
        # add events to the current dictionary of events
        self._errors = 0
        for e in events:
            try:
                # print e
                self.validateRequiredAttributes(e)
                self.validateDBFields(e)
                self.validateCategories(e)
                self.validateComponents(e)
                self.validateSeverity(e)
                self.validateControlActions(e)
                self.validateDiagnostics(e)
                self.validatePeriod(e)
                self.validateNoDuplicate(e)
                self._events[e['id']] = e
            except Exception,x:
                self.error(x,e)
                continue
        return self._errors
    
# from RasEventWriter import RasEventWriter

class ServiceTransformer:
    '''The ServiceTransformer is responsible for transforming
    service action variables into actual service action text.'''
	 
    def __init__(self):
        self._checkLevelsVar = '$(CheckLevels)'
        self._checkLevelsText = 'This condition may have been caused by a software error. Check the software levels and make sure the system fix level is up to date. '
        self._checkFacilities = '$(CheckFacilities)'
        self._checkFacilitiesText = 'Check the system facilities such as circuit breakers, air handlers, and air cooling syistems. '
        self._checkConnections = '$(CheckConnections)'
        self._checkConnectionsText = 'Check the cables and connections that run external to the system.'
        self._checkEnvVar = '$(CheckEnvironmentals)'
        self._checkEnvText = 'Check the environmental monitor data. '
        self._noServiceVar = '$(NoService)'
        self._noServiceText = 'A service action is not required for this event.'
        self._diagsVar = '$(Diagnostics)'
        self._diagsText = 'Run diagnostics. '
        self._testText = 'The relevant diagnostic bucket(s) to run are: ' 
        self._threshVar = '$(ThresholdPolicy)'
        self._threshText1 = 'The part at this location has threshold count policy of $(threshold_count) or more errors'
        self._threshText2 = ' in a period of $(threshold_period) or less. '
        self._errorStart = '*********** RAS Metatdata Issue Start ******** '
        self._errorEnd   = '*********** RAS Metatdata Issue End ********** '
        return
	
    def transform(self, service_action, thresh_count, thresh_period, diags, event):
        # if the event as a service action variable
        # transform it to the expected service text
        if service_action.find('$') < 0:
            return service_action
        if service_action.find(self._checkLevelsVar) >= 0:
            service_action = service_action.replace(self._checkLevelsVar, self._checkLevelsText);
        if service_action.find(self._checkEnvVar) >= 0:
            service_action = service_action.replace(self._checkEnvVar, self._checkEnvText);
        if service_action.find(self._checkFacilities) >= 0:
            service_action = service_action.replace(self._checkFacilities, self._checkFacilitiesText);
        if service_action.find(self._checkConnections) >= 0:
            service_action = service_action.replace(self._checkConnections, self._checkConnectionsText);
        if service_action.find(self._noServiceVar) >= 0:
            service_action = service_action.replace(self._noServiceVar, self._noServiceText);
        if service_action.find(self._diagsVar) >= 0:
            diagsText = self._diagsText
            if diags != "":
                diagsText += self._testText
                diagsText += diags
                diagsText += ". "
            service_action = service_action.replace(self._diagsVar, diagsText);
        if service_action.find(self._threshVar) >= 0:
            threshText = ""
            if thresh_count != "":
                threshText = self._threshText1.replace('$(threshold_count)',thresh_count)
                if thresh_period != "":
                    threshText += self._threshText2.replace('$(threshold_period)',thresh_period)
                else:
                    threshText += ". "
            else:
		print self._errorStart
                print "WARNING: the rasevent metadata has threshold policy in the service action but the threshold count is not specified."
                print event
                print self._errorEnd
            service_action = service_action.replace(self._threshVar,threshText)
        return service_action

class RasEventWriter:
    '''The RasEventWriter is responsible writing Ras Event
    metadata to a file.'''

    def __init__(self):
        self.prolog = '''<!-- begin_generated_IBM_copyright_prolog                             -->
<!--                                                                  -->
<!-- This is an automatically generated copyright prolog.             -->
<!-- After initializing,  DO NOT MODIFY OR MOVE                       -->
<!-- ================================================================ -->
<!--                                                                  -->
<!-- Licensed Materials - Property of IBM                             -->
<!--                                                                  -->
<!-- Blue Gene/Q                                                      -->
<!--                                                                  -->
<!-- (C) Copyright IBM Corp.  2011, 2011                              -->
<!--                                                                  -->
<!-- US Government Users Restricted Rights -                          -->
<!-- Use, duplication or disclosure restricted                        -->
<!-- by GSA ADP Schedule Contract with IBM Corp.                      -->
<!--                                                                  -->
<!-- This software is available to you under the                      -->
<!-- Eclipse Public License (EPL).                                    -->
<!--                                                                  -->
<!-- ================================================================ -->
<!--                                                                  -->
<!-- end_generated_IBM_copyright_prolog                               -->
'''
        return
	
    def writeEvents(self, events, f_name):
        print 'write events to ', f_name
        f = open(f_name, 'w')
        f.write(self.prolog)
        now = datetime.datetime.now();
        f.write('<rasevents gen="' + str(now) + '" >\n')
        serviceTransformer = ServiceTransformer()
        id_keys = events.keys()
        id_keys.sort()
        for i_key in id_keys:
            f.write('\t<rasevent ')
            details = events[i_key]
            # note the order of these is important
            id = details['id'].upper()
            f.write('id=' + '\"' + str(id) + '\" ')
            category = details['category']
            f.write('category=' + '\"' + str(category) + '\" ')
            component = details['component']
            f.write('component=' + '\"' + str(component) + '\" ')
            sev = details['severity']
            f.write('severity=' + '\"' + str(sev) + '\" ')
            msg = details['message']
            f.write('message=' + '\"' + str(msg) + '\" ')
            desc = details['description']
            f.write('description=' + '\"' + str(desc) + '\" ')
            sa = details.get('service_action',"")
            tc = details.get('threshold_count',"") 
            tp = details.get('threshold_period',"")
            diags = details.get('relevant_diags',"")
            sa_xform = serviceTransformer.transform(sa,tc,tp,diags, details)
            try: 
                if sa_xform.find('$(') >= 0: 
                    print "WARNING: ras metadata has an unrecognized variable in the service action: ", sa, ", file name: ", details['file_name'], ", line number: ", details['line_num']
            except Exception:
                print "Type error:", sa, tc, tp, diags, details['file_name'], details['line_num']
            f.write('service_action=' + '\"' + str(sa_xform) + '\" ')
            # optional parameters
            decoder = details.get('decoder',"")
            f.write('decoder=' + '\"' + str(decoder) + '\" ')
            ctl = details.get('control_action',"")
            f.write('control_action=' + '\"' + str(ctl) + '\" ')
            f.write('threshold_count=' + '\"' + str(tc) + '\" ')
            f.write('threshold_period=' + '\"' + str(tp) + '\" ')
            f.write('relevant_diags=' + '\"' + str(diags) + '\" ')
            fn = details.get('file_name',"")
            f.write('file_name=' + '\"' + str(fn) + '\" ')
            ln = details.get('line_num',"")
            f.write('line_num=' + '\"' + str(ln) + '\" ')
            f.write('/>\n')
        f.write('</rasevents>\n');
        f.close()
        return

#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
def usage():
    print sys.argv[0], '[-h] src_dirs install_dir file_name'
    print '   where -h for help'
    print '         src_dirs - the list of directories to scan for ras events.  The directories are separated with a ";".'
    print '         install_dir - the directory location used to write file with the extracted ras events.'
    print '         file_name - the name of the file to be written to the install_dir.'
    print '         ras_xml - the location of ras.xml file for fields validation of the ras event.'
    return

    
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

    # print args
    if args == None or len(args) != 4:
        usage()
        sys.exit(1)

    p = ElementParser()
    s = FileScanner(p,'<rasevent', '/>')        
    fsw = FileSystemWalker(s, '*.h;*.c;*.cc;*.java;*.cpp;*.xml')

    # print 'tree traversal starting at time ', asctime(localtime())


    v = RasEventValidator(args[3])
    src_dirs = args[0].split(';')
    # print 'src_dirs = ', src_dirs
    errors = 0
    for dir in src_dirs:
        print 'processing ras metadata in ', dir
        try:
            mode = os.stat(dir)[ST_MODE]
            fsw.walk(dir)
            t_events = s.getElements()
            # events.extend(t_events)
            s.resetElements()
            # validate ras events 
            errors += v.validate(t_events)

            # print 'tree traversal ending at time ', asctime(localtime())
        except OSError, e:
            continue
    # print v.events()

    # write the events to a file that will be 
    # installed on the ras/events in the floor
    installDir = args[1]
    if path.exists(installDir) == False:
        os.makedirs(installDir)
    file_name = installDir + '/' + args[2]  
    w = RasEventWriter()
    w.writeEvents(v.events(), file_name)
    return errors
         
    
    
# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
main()
