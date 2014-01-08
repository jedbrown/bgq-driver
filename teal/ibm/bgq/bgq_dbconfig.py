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
import os
import pwd
import ConfigParser

from ibm.teal.database import db_interface
from ibm.teal.database.db_interface_pyodbc import Configuration, SQLGeneratorDB2
from ibm.teal import registry
from ibm.bgq.bgq_configService import ConfigService

BG_PROPERTIES_FILE = '/bgsys/local/etc/bg.properties'

class BgqPropertiesFile(object):
    ''' This class makes the bg.properties file palatable for the ConfigParser
    by filtering out comment lines that can cause syntax errors in normal parsing
    '''
    def __init__(self, filename):
        ''' Constructor
        '''
        self.fp = open(filename)
        self.name = filename

    def readline(self):
        ''' Only return lines that don't start with a comment to the config parser
        '''
        tmp_line = '#'
        while tmp_line.startswith('#'):
            line = self.fp.readline()
            tmp_line = line.lstrip()
        return line    

class BgqConfiguration(Configuration):
    ''' This class retrieves the BGQ database configuration and returns it to the interface
    for creating the connection to the database. The
    '''
    def __init__(self):
        ''' Constructor
        '''
        pass
        
    def get_generator(self, config_dict):
        ''' Return the appropriate SQL generator based on the configuration information
        retrieved from the bg.properties
        '''
        # 1) see if person calling has it specified
        properties_file = config_dict.get('bgproperties', None)
        
        # 2) go to env variable PROPERTIES_FILE
        if not properties_file:
            properties_file = os.environ.get('PROPERTIES_FILE',None)
        if not properties_file:
            properties_file = os.environ.get('BG_PROPERTIES_FILE',None)

        # 3) then look in /bgsys/local/etc
        if not properties_file:
            properties_file = BG_PROPERTIES_FILE
            
        cfg = ConfigParser.ConfigParser()
        cfg.readfp(BgqPropertiesFile(properties_file))

        # get ras filter info
        try:
            rasFilter = cfg.get('ras','filter')
        except ConfigParser.NoOptionError:
            rasFilter = '/bgsys/drivers/ppcfloor/ras/etc/ras_environment_filter.xml'
        registry.get_logger().debug('RAS Environment filter file: ' + rasFilter)
        registry.register_service('BGQ_RAS_FILTER', rasFilter)
        config_service = ConfigService()
        registry.register_service('BGQ_CONFIG_SERVICE', config_service)

        # get database info
        db = cfg.get('database','name')
        try:
            usr_schema = cfg.get('database','schema_name') + '.'
        except ConfigParser.NoOptionError:
            usr_schema = ''

        pwless = False
        try:
            usr = cfg.get('database','user')
            pw = cfg.get('database','password')
        except ConfigParser.NoOptionError:
            registry.get_logger().debug('Database user and/or password is not specified.')
            pwless = True

        # Set the table names
        db_interface.TABLE_EVENT_LOG = usr_schema + 'x_tealeventlog'
        db_interface.TABLE_EVENT_LOG_EXT = usr_schema + 'x_tealeventlogext'
        db_interface.TABLE_BG_EVENT_LOG = usr_schema + 'tbgqeventlog'
        db_interface.TABLE_CHECKPOINT = usr_schema + 'x_tealcheckpoint'
        db_interface.TABLE_ALERT_LOG = usr_schema + 'x_tealalertlog'
        db_interface.TABLE_ALERT2ALERT = usr_schema + 'x_tealalert2alert'
        db_interface.TABLE_ALERT2EVENT = usr_schema + 'x_tealalert2event'
        db_interface.TABLE_TEMPLATE = usr_schema + 'x_{0}'

        if pwless:
            return SQLGeneratorDB2({'dsn':db})
        else:
            return SQLGeneratorDB2({'dsn':db, 'uid':usr, 'pwd':pw})
        
        
if __name__ == '__main__':   

    bgq_conf = BgqConfiguration()
    print bgq_conf.get_generator({}).gen_connect()
    print db_interface.TABLE_EVENT_LOG
    print db_interface.TABLE_TEMPLATE
    

