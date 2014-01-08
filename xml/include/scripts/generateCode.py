#!/usr/bin/python
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

import sys
import xml.parsers.expat
import apiWriter
from xmlFile import XmlFile
from xmlClass import XmlClass
from xmlDataMember import XmlDataMember
from xmlValueSet import XmlValueSet
from xmlValueMember import XmlValueMember
from xmlApiMethod import XmlApiMethod

# The xmlDepth of the XML element being processed.
xmlDepth = 0

# The file object for the current XML file being processed.
fileObject = None

# A stack of objects used to create and manage XML object generation.
objectStack = []

# A list of all top level objects.
topLevelObjects = []

# A list of all the value set objects.
valueSets = []

# A list of all the class objects.
classes = []

# A list of all data member objects.
dataMembers = []

def usage():
    """
    Displays the the script usage.
    """
    
    print sys.argv[0], "[OPTIONS] file1 file2 file3..."
    print "[OPTIONS]"
    print "  -c, --cimpl      Generates C++ implemenation code."
    print "  -H, --cheader    Generates C++ header code."
    print "  -d, --dir        Header include dir."
    print "  -j, --java       Generates Java code."
    print "  -h               This help text."

def startElementHandler(name, attrs):
    """
    A handler called when the XML parser encounters the beginning of an XML element.
    \param name
             the name of the XML element.
    \param attrs
             a dictionary of the XML element's attribute names and values.
    """
    global xmlDepth, fileObject
    
    try:
        if name == 'File':
            fileObject = XmlFile(name, attrs, header)
            objectStack.append(fileObject)
        elif name == "Class":
            c = XmlClass(name, attrs, fileObject._version, fileObject._type)
            objectStack[-1].addClass(c)
            objectStack.append(c)
            classes.append(c)
        elif name == "DataMember":
            d = XmlDataMember(name, attrs)
            objectStack[-1].addDataMember(d)
            objectStack.append(d)
            dataMembers.append(d)
        elif name == "MemberFunction":
            d = attrs['type']
            objectStack[-1].addMemberFunction(d)
        elif name == "ValueSet":
            v = XmlValueSet(name, attrs, fileObject._version)
            objectStack[-1].addValueSet(v)
            objectStack.append(v)
            valueSets.append(v._name)
        elif name == "ValueMember":
            v = XmlValueMember(name, attrs)
            objectStack[-1].addValueMember(v)
            objectStack.append(v)
        elif name == "Method":
            m = XmlApiMethod(name, attrs)
            fileObject.addMethod(m)
        elif name == "APIHeader":
            fileObject._apiHeader = attrs['filename']
        elif name == "APIImplementation":
            fileObject._apiImplementation = attrs['filename']
        elif name == "APIConsole":
            fileObject._apiConsole = attrs['filename']
        elif name == "Include":
            fileObject.addInclude( attrs['filename'] )


    except KeyError, e:
        print "XML element " + str(name) + " is missing attribute " + str(e) + "."
        print "Current XML element attributes are: " + str(attrs)
        print "Fix " + fileObject._name + " and retry."
        sys.exit(1)
    except AttributeError, e:
        print "A " + str(name) + " is not a valid child for a " + str(objectStack[-1]._typeName) + " type of object."
        print "Fix " + str(objectStack[-1]._typeName) + " " + str(objectStack[-1]._name) + " in " + fileObject._name + " and retry."
        sys.exit(2)
        
    xmlDepth += 1
    
def endElementHandler(name):
    """
    A handler called when the XML parser encounters the end of an XML element.
    \param name
             the name of the XML element.
    """
    global xmlDepth
    xmlDepth -= 1
    
    if name == "File" or \
       name == "Class" or \
       name == "DataMember" or \
       name == "ValueSet" or \
       name == "ValueMember":
        o = objectStack.pop()
        
        if len(objectStack) == 1:
            topLevelObjects.append(o)


#####################################################################
#
# Main routine.
#
#####################################################################

generate = "--cimpl"
header = ""
files = []
flag = 0

for o in sys.argv[1:]:
    if o[0] == '-':
        if o in ("-h", "--help"):
            usage()
            sys.exit(0)
        elif o in ("-c", "--cimpl"):
            generate = "--cimpl"
        elif o in ("-d", "--dir"):
            flag = 1
        elif o in ("-H", "--cheader"):
            generate = "--cheader"
        elif o in ("-j", "--java"):
            generate = "--java"
    elif flag == 1:
        header = o
        flag = 0
    else:
        if flag != 1:
            files.append(o)

# Set up the XML parser.
xmlParser = xml.parsers.expat.ParserCreate()
xmlParser.StartElementHandler = startElementHandler
xmlParser.EndElementHandler = endElementHandler

# Process all files.
for fileName in files:
    # Initialize globals for processing the XML file.
    xmlDepth = 0
    fileObject = None
    objectStack = []
    topLevelObjects = []
    valueSets = []
    dataMembers = []

    # Open and parse the XML file.
    fd = open(fileName, 'r')
    xmlParser.ParseFile(fd)
    fd.close()
    
    # Update all data member objects to ensure they have their isValueSet flags set properly.
    for d in dataMembers:
        if d._type in valueSets:
            d._isValueSet = True
            
    # Point each class to their parent class objects so they can build appropriate constructors, etc.
    for c in classes:
        if (c._parent != None) and (c._parent != "XML::Serializable"):
            for p in classes:
                if p._name == c._parent:
                    c._parentObject = p

    if generate == "--cheader":
        apiWriter.writeApiCHeader(fileObject, topLevelObjects)
    elif generate == "--cimpl":
        apiWriter.writeApiCImpl(fileObject, topLevelObjects)
    elif generate == "--java":
        apiWriter.writeApiJava(fileObject, topLevelObjects)
