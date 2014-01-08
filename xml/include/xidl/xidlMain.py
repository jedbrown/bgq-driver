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

import os, string, sys, getopt, pprint, xml.parsers.expat
import xidlParser, xidlConfig


# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
def usage():
    print sys.argv[0], '[-s][-h] filename'
    print '   where -s is "simple" or no XML'
    print '         -j is "java"'
    print '         -h for "help"'


# ----------------------------------------------------------------------------
# main
# ----------------------------------------------------------------------------
def main():

    try:
        optlist, args = getopt.getopt(sys.argv[1:], "sjech", \
                                      ["simple", "java", "cheader", "cimpl", "help"])
    except getopt.GetoptError:
        usage()
        sys.exit(1)
        
    files = args
    for o, a in optlist:
        if o in ("-s", "--simple"):
            xidlConfig.generateXML = False

        if o in ("-j", "--java"):
            xidlConfig.generateJava = True
            
        if o in ("-e", "--cheader"):
            xidlConfig.generateCHeader = True

        if o in ("-c", "--cimpl"):
            xidlConfig.generateCImpl = True

        if o in ("-h", "--help"):
            usage()
            sys.exit(0)

    if files == None or len(files) == 0:
        usage()
        sys.exit(1)

    for fileName in files:
        xmlParser = xml.parsers.expat.ParserCreate()
        xmlParser.StartElementHandler = xidlParser.startElementHandler
        xmlParser.CharacterDataHandler = xidlParser.charDataHandler
        xmlParser.EndElementHandler = xidlParser.endElementHandler

        fd = open(fileName, 'r')
        xmlParser.ParseFile(fd)
        fd.close()


# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
main()

        
