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
# (C) Copyright IBM Corp.  2011, 2012                              
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

import sys
from xml.sax.handler import ContentHandler
import xml.sax

from ibm.teal import registry

class ConfigRasEnvFilterHandler(ContentHandler):
    '''ConfigRasEnvFilterHandler helps to parse the ras_environment_filter.xml file.
    '''
	
    def __init__(self,repository):
        ''' The constructor.
        '''
	self.ras_env = ''
	self.repository = repository
	return
	
    def startElement(self, name, attrs):
        '''Start parsing each element in the ras_environment_filter.xml file.
        '''
	if name == 'RasEventChangeSpec' and self.ras_env == 'PROD':
	    ras_id = attrs.getValue('id')
	    key = attrs.getValue('key')
	    value = attrs.getValue('value')
	    spec = list()
	    spec.extend([ras_id, key, value])
	    specs = self.repository.get(name,list())
	    specs.append(spec)
	    self.repository[name] = specs
        elif name == "BgRasEnvironment":
            self.ras_env = attrs.getValue('environment')
	return

    def endElement(self, name):
        '''End parsing element in the ras_environment_filter.xml file.
        '''
	return


class ConfigService:

    def __init__(self):
        ''' The constructor.
        '''
	# Parse ras environment filter file (ras_environment_filter.xml)
	self.repository = dict()
	p = xml.sax.make_parser()
        ras_filter = registry.get_service('BGQ_RAS_FILTER')
	if ras_filter != None:
	    filter_handler = ConfigRasEnvFilterHandler(self.repository)
	    p.setContentHandler(filter_handler)
	    p.parse(ras_filter)
			
	return

    def get(self,name,value=''):
        ''' Get the value of the configuration property.
        '''
	return self.repository.get(name,value)
	
    def put(self,name,value):
        ''' Put the configuration property in the repository.
        '''
	self.repository[name] = value
	return
