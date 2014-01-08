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

import os, string, sys
import xidlFile, xidlApiMethod, xidlClass, xidlDataMember, xidlFunction, xidlMethod, xidlConfig, xidlValueSet

# ----------------------------------------------------------------------------
# A simple stack to keep track of the tree of nested classes
# ------------------------------------------h----------------------------------
class Stack:
    def __init__(self):
        self._classes = []

    def push(self, myclass):
        self._classes.append(myclass)

    def pop(self):
        if len(self._classes) == 0:
            print "Error in Stack::pop"
            sys.exit(1)
        temp = self._classes[len(self._classes)-1]
        del self._classes[len(self._classes)-1]
        return temp

    def last(self):
        if len(self._classes) == 0:
            print "Error in Stack::last"
            sys.exit(1)            
        return self._classes[len(self._classes)-1]

    def size(self):
        return len(self._classes)
    

currentFile       = None
currentApiMethod  = None
currentClassStack = Stack()
currentDataMember = None
currentFunction   = None
currentMethod     = None
currentValueSet   = None
currentValueMember = None

# ----------------------------------------------------------------------------
# XML handlers
# ----------------------------------------------------------------------------
def startElementHandler(name, attrs):
    global currentFile, currentApiMethod, currentClassStack, currentDataMember, currentFunction, currentMethod
    global currentValueSet, currentValueMember, valueSets

    if name == "File":
        currentFile = xidlFile.File(attrs['filename'])

        if attrs.has_key('namespace'):
            currentFile.setNamespace(attrs['namespace'])

        if attrs.has_key('version'):
            currentFile.setVersion(attrs['version'])
            
    elif name == "APIHeader":
        currentFile.setAPIHeader(attrs['filename'])
        
    elif name == "APIImplementation":
        currentFile.setAPIImplementation(attrs['filename'])

    elif name == "APIConsole":
        currentFile.setAPIConsole(attrs['filename'])

    elif name == "Include":
        currentFile.addInclude(attrs['filename'])

    elif name == "Method":
        currentApiMethod = xidlApiMethod.ApiMethod(attrs['name'])
        if attrs.has_key('type'):
            currentApiMethod.setType(attrs['type'])
        if attrs.has_key('comment'):
            currentApiMethod.setComment(attrs['comment'])

    elif name == "Class":
        currentClassStack.push(xidlClass.Class(attrs['name']))
        if attrs.has_key('comment'):
            currentClassStack.last().setComment(attrs['comment'])
        elif currentApiMethod != None:
            currentClassStack.last().setComment(currentApiMethod.getComment())
            
        if attrs.has_key('parent'):
            currentClassStack.last().setParent(attrs['parent'])
        if attrs.has_key('superclass'):
            currentClassStack.last().setIsSuperClass(attrs['superclass'])

    elif name == "DataMember":
        currentDataMember = xidlDataMember.DataMember(attrs['type'], attrs['name'], currentFile)
        if attrs.has_key('comment'):
            currentDataMember.setComment(attrs['comment'])
        if attrs.has_key('occurrences'):
            currentDataMember.setOccurrences(attrs['occurrences'])
        if attrs.has_key('inConstructor'):
            currentDataMember.setInConstructor(attrs['inConstructor'])
        if attrs.has_key('isOptional'):
            currentDataMember.setIsOptional(attrs['isOptional'])
        if attrs.has_key('isInherited'):
            currentDataMember.setIsInherited(attrs['isInherited'])

    elif name == "Function":
        currentFunction = xidlFunction.Function()

    elif name == "Method":
        currentMethod = xidlMethod.Method()

    elif name == "ValueSet":
        currentValueSet = xidlValueSet.ValueSet(attrs['name']);
        if currentClassStack.size() != 0:
            currentValueSet.setNestingLevel(currentClassStack.last().getNestingLevel())
        if attrs.has_key('comment'):
            currentValueSet.setComment(attrs['comment'])

    elif name == "ValueMember":
        currentValueMember = xidlValueSet.ValueMember(attrs['name']);
        if attrs.has_key('comment'):
            currentValueMember.setComment(attrs['comment'])
        if attrs.has_key('value'):
            currentValueMember.setValue(attrs['value'])

    else:
        print "Error Parsing"
        sys.exit(1)


# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
def charDataHandler(data):
    global currentFuntion, currentMethod

    if currentFunction != None:
        currentFunction.addLine(data)
    elif currentMethod != None:
        currentMethod.addLine(data)
    
# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
def endElementHandler(name):
    global currentFile, currentApiMethod, currentClassStack, currentDataMember, currentFunction, currentMethod
    global currentValueSet, currentValueMember

#    print 'endElementHandler(', name, ')'

    if name == "File":
        if xidlConfig.generateJava:
            currentFile.generate_Java()
        elif xidlConfig.generateCHeader:
            currentFile.generate_CHeader()
        elif xidlConfig.generateCImpl:
            currentFile.generate_CImpl()
        else:
            currentFile.generate_C()
        currentFile = None

    elif name == "Method":
        currentFile.addApiMethod(currentApiMethod)
        currentApiMethod = None

    elif name == "Class":
        currentClass = currentClassStack.pop();
        if currentClassStack.size() == 0:
            currentFile.addTopClass(currentClass)
        else:
            currentClassStack.last().addNestedClass(currentClass)

    elif name == "DataMember":
        currentClassStack.last().addDataMember(currentDataMember)
        currentDataMember = None

    elif name == "Function":
        currentFile.addFunction(currentFunction)
        currentFunction = None

    elif name == "Method":
        currentClassStack.last().addExtraMethod(currentMethod)
        currentMethod = None

    elif name == "ValueSet":
        if currentClassStack.size() == 0:
            currentFile.addValueSet(currentValueSet)
        else:
            currentClassStack.last().addValueSet(currentValueSet)
        currentValueSet = None

    elif name == "ValueMember":
        currentValueSet.addValueMember(currentValueMember)
        currentValueMember = None


