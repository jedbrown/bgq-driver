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

import os, string, sys, pprint, time
import xidlConfig



class ApiMethod:
    # -------------------- initialization ------------------
    def __init__(self, name):
        # these three data members can appear as attributes in the
        # <Class > tag (name is required, commend and parentClass are
        # optional)
        self._name         = name
        self._comment      = None
        self._type         = None

    # -------------------- setters -------------------------
    def setComment(self, comment):
        self._comment = comment

    def setType(self, type):
        self._type = type

    # -------------------- getters -------------------------
    def getComment(self):
        return self._comment
    
    def getNameCapFirst(self):
        return self._name[0].capitalize() + self._name[1:]
    
    # -------------------- writers -------------------------
    def writeAPI(self, namespace):
        print
        print "/*!"
        print "* \\brief " + self._name + " method."
        print "* " + self._comment
        print "*" 
        print "* \\param[in]  " + self.getNameCapFirst() + "Request -- request object"
        print "* \\param[out] " + self.getNameCapFirst() + "Reply -- reply object"
        print "*/"
        print "  virtual void " + self._name + "(const " + namespace + "::" + self.getNameCapFirst() + "Request& request, " + namespace + "::" + self.getNameCapFirst() + "Reply& reply)"
    
    def writeAPIImplementation(self, namespace):
        print "{"
        print "    sendReceive(" + namespace + "::" + self.getNameCapFirst() + "Request::getClassName(), request, " + namespace + "::" + self.getNameCapFirst() + "Reply::getClassName(), reply);"
        print "}"
        print
        
    def consoleMethodDecl(self, classname):
        method = self._name
        return "bool " + method + " (const std::vector<std::string>& argv)"
        
    def consoleHelp(self):
        return "\"" + self._name + " - " + self._comment + "\\n\""
        
    def dispatchString(self):
        return " (argv[0] == \"" + self._name + "\") error = " + self._name + "(argv);"
        
    def writeConsoleImplementation(self, classname, namespace):
        print
        print "bool " + classname + "::" + self._name + " (const std::vector<std::string>& argv)"
        print "{"
        print "  if (argv.size() < 1)"
        print "  {"
        print "    stdout << \"args?\";"
        print "    return false;"
        print "  }"
        print "  else"
        print "  {"
        print "    " + self.getNameCapFirst() + "Request request();"
        print "    " + self.getNameCapFirst() + "Reply   reply;"
        print "    _theServer->" + self._name + "(request, reply);"
        print 
        print "    stdout << replyMsg( reply );"
        print "  }"   
        print "  return true;"
        print "}"
        
