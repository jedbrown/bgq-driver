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


class File:

    # -------------------- initialization ------------------
    def __init__(self, filename):
        self._filename      = filename  # name of the class (only used for the #ifdef);
                                        # output goes to stdout
        self._apiMethods    = []        # list of API methods
        self._classes       = []        # list of "top" classes (not nested ones)
        self._valueSets     = []        # value set members (ENUMS)
        self._functions     = []        # list of functions
        self._includes      = []        # list of files to include
        self._namespace     = ""        # define a common nampespace
        self._version       = ""        # version of the API
        self._packageName   = ""        # the Java package name
        self._packageHeader = ""        # the Java package header
        self._apiHeader     = ""        # header file name
        self._apiImplementation = ""    # api implementation file
        self._apiConsole    = ""        # stub implementation for console

    #
    # check to see if this is a value set element (an ENUM type).
    #
    def isValueSet (self, type):
        for n in self._valueSets:
            if (type == n._name):
                return 1
        return 0


    # -------------------- setters -------------------------
    def addApiMethod(self, apiMethod):
        self._apiMethods.append(apiMethod)

    def addTopClass(self, topClass):
        self._classes.append(topClass)

    def addValueSet(self, valueSet):
        self._valueSets.append(valueSet)

    def addFunction(self, func):
        self._functions.append(func)

    def addInclude(self, filename):
        self._includes.append(filename)
        
    def setNamespace(self, namespace):
        self._namespace = namespace

    def setVersion(self, version):
        self._version = version
        
    def setAPIHeader(self, filename):
        self._apiHeader = filename

    def setAPIImplementation(self, filename):
        self._apiImplementation = filename

    def setAPIConsole(self, filename):
        self._apiConsole = filename

    # ------------------- generator methods for C++ ---------
    # write the current file to standard output
    def generate_C(self):
        # write the begining of the generated .h file
        self.writeHeader_C()

        # write the enum declaration
        for v in self._valueSets:
            v.writeDeclaration_C()

        # write the class declaration
        for c in self._classes:
            c.writeDeclaration_C()

        # write the inline methods    
        if xidlConfig.generateXML:
            self.writeInlineHeader_C()
            for c in self._classes:
                c.writeStandardMethods_C()

        # write the extra functions
        for f in self._functions:
            f.implementation()

        # close the end of the generated .h file    
        self.writeFooter_C()   
        
        if self._apiHeader != None:
            self.generateAPIHeader_C()
            
        if self._apiImplementation != None:
            self.generateAPIImplementation_C()
            
        if self._apiConsole != None:
            self.generateConsoleHeader_C()
            self.generateConsoleImplementation_C()
         
    # ------------------- generator methods for C++ ---------
    # write the current file to standard output
    def generate_CHeader(self):
        # write the begining of the generated .h file
        self.writeHeader_CHeader()

        # write the enum declaration
        for v in self._valueSets:
            v.writeDeclaration_C()

        # write the class declaration
        for c in self._classes:
            c.writeDeclaration_C()

        # close the end of the generated .h file    
        self.writeFooter_CHeader()   
        
         
    # ------------------- generator methods for C++ ---------
    # write the current file to standard output
    def generate_CImpl(self):
        # write the begining of the generated .h file
        self.writeHeader_CImpl()

        # write the enum declaration
#        for v in self._valueSets:
#            print "//!!! Value set"
#            v.writeDeclaration_C()

        # write the class declaration
        for c in self._classes:
            c.writeImpl_C()

#        # write the inline methods    
#        if xidlConfig.generateXML:
#            for c in self._classes:
#                c.writeStandardMethodsImpl_C()

        # write the extra functions
        for f in self._functions:
            f.implementation()

        # close the end of the generated .h file    
        self.writeFooter_CImpl()   
        
        if self._apiHeader != None:
            self.generateAPIHeader_C()
            
        if self._apiImplementation != None:
            self.generateAPIImplementation_C()
            
        if self._apiConsole != None:
            self.generateConsoleHeader_C()
            self.generateConsoleImplementation_C()
         
    def generateAPIHeader_C(self):
        file = open(self._apiHeader + ".h", 'w')
        sys.stdout = file
        
        print "#ifndef " + string.upper(self._apiHeader) + "_HEADER"
        print "#define " + string.upper(self._apiHeader) + "_HEADER"
        print
        print "#include <xml/include/c_api/" + self._filename + ".h>"
        print "class " + self._apiHeader + " {" 
        print "public:"
        print
        print "virtual ~" + self._apiHeader + "() {}"
        print
        print "static " + self._apiHeader + " *Instance() {"
        print "assert(_instance != 0);"
        print "return _instance;"
        print "}"
  
        for m in self._apiMethods:
            if m._type == "RequestReply":
                m.writeAPI(self._namespace)
                print " = 0;" # make these pure virtual
                print
        
        print
        print "protected:"
        print "static " + self._apiHeader + "* _instance; // for a singleton object"
        print "};"
        print
        print "// access the singleton object"
        print "inline " + self._apiHeader + " *get" + self._apiHeader + "() {"
        print "return " + self._apiHeader + "::Instance();"
        print "}"
        print
        print "#endif"

        file.close()

    def generateAPIImplementation_C(self):
        file = open(self._apiImplementation + ".h", 'w')
        sys.stdout = file
        
        print "#ifndef " + string.upper(self._apiImplementation) + "_HEADER"
        print "#define " + string.upper(self._apiImplementation) + "_HEADER"
        print
        print "#include \"xml/include/c_api/" + self._filename + ".h\""
        print 
        print "class " + self._apiImplementation + ": public " + self._apiHeader + " {" 
        print "public:"
        print
        print "virtual ~" + self._apiImplementation + "() {}"
        print

        for m in self._apiMethods:
            if m._type == "RequestReply":
                m.writeAPI(self._namespace)
                m.writeAPIImplementation(self._namespace) 
        
        print "}"

        print "#endif"
        file.close()
        
    def generateConsoleHeader_C(self):
        file = open(self._apiConsole + ".h", 'w')
        sys.stdout = file
        
        print "#ifndef " + string.upper(self._apiConsole) + "_HEADER"
        print "#define " + string.upper(self._apiConsole) + "_HEADER"
        print
        print
        print "#include <string>"
        print "#include <vector>"
        print 
        print "#include \"xml/include/c_api/" + self._apiHeader + ".h\""
        print
        print "class " + self._apiConsole + " {" 
        print "public:"
        print "  " + self._apiConsole + " ( " + self._apiHeader + " *server);"
        print 
        print "  std::vector<std::string> readCommand();"
        print "  bool dispatch(const std::vector<std::string>& argv);"
        print
        print "  void help();"
        print
        print "protected:"
        print "  " + self._apiHeader + " *_theServer;"
        print
        for m in self._apiMethods:
            if m._type == "RequestReply":
                print "  " + m.consoleMethodDecl(self._apiConsole) + ";"
        print
        print "private:"
        print "  " + self._apiConsole + "(){};"
        print "};"
        print "#endif // " + string.upper(self._apiConsole) + "_HEADER"
        
        file.close()

    def generateConsoleImplementation_C(self):
        file = open(self._apiConsole + ".cc", 'w')
        sys.stdout = file
        
        print "#include \"" + self._apiConsole + ".h\""
        print "#include <stdio.h>"
        print "using namespace " + self._namespace + ";"
        print
        
        print "void " + self._apiConsole + "::help() {"
        print "  printf(\"usage:\\n\""
        print
        for m in self._apiMethods:
            if m._type == "RequestReply":
                print "    " + m.consoleHelp()
        print "  );"
        print "}"
        print 
        print "std::vector<std::string> " + self._apiConsole + "::readCommand() {"
        print "  std::vector<std::string> result;"
        print "  // parse command here"
        print "  return result;"
        print "}"
        print
        print "bool " + self._apiConsole + "::dispatch(const std::vector<std::string>& argv) {"
        print 
        print "  assert(argv.size() > 0);"
        print "  bool error = false;"
        print "  try {"
        prefix = "if"
        for m in self._apiMethods:
            if m._type == "RequestReply":
                print "    " + prefix + " " + m.dispatchString()
                prefix = "else if"
        print
        print "    else"
        print "    {"
        print "      printf(\"Unrecognized command\");"
        print "      help();"
        print "     }"
        print "  }"
        print "  catch ( invalid_argument &ia )"
        print "  {"
        print "    printf( std::string(\"!!!Exception from server: \").append(ia.what()) );"
        print "  }"
        print
        print "  return error;"
        print "}"
        print
        for m in self._apiMethods:
            if m._type == "RequestReply":
                m.writeConsoleImplementation(self._apiConsole, self._namespace)
        
        file.close()

    def writeHeader_C(self):        
        print "#ifndef _" + string.upper(self._filename) + "_H"
        print "#define _" + string.upper(self._filename) + "_H"
        print
        print "// ========================================================================"
        print "//          WARNING               WARNING                 WARNING          "
        print "// ========================================================================"
        print "//"                                                                         
        print "//          You are looking at a generated file. Do not edit!!!            "
        print "//"                                                                         
        print "// Generated on " + time.asctime()
        print "// ========================================================================"
        print "//          WARNING               WARNING                 WARNING          "
        print "// ========================================================================"
        print
        print "#include <string>"
        print "#include <vector>"

        if xidlConfig.generateXML:
            print "#include \"xml/include/library/XML.h\""
            
        print
        for i in self._includes:
            print "#include \"xml/include/c_api/" + i + "\""
        print    

        if self._namespace != "":
            print
            print "namespace " + self._namespace + " {"
            print
    
        if self._version != "":
            print
            print "  const std::string VERSION = \"" + self._version + "\";"
            print
            
    def writeHeader_CHeader(self):        
        print "#ifndef _" + string.upper(self._filename) + "_H"
        print "#define _" + string.upper(self._filename) + "_H"
        print
        print "// ========================================================================"
        print "//          WARNING               WARNING                 WARNING          "
        print "// ========================================================================"
        print "//"                                                                         
        print "//          You are looking at a generated file. Do not edit!!!            "
        print "//"                                                                         
        print "// Generated on " + time.asctime()
        print "// ========================================================================"
        print "//          WARNING               WARNING                 WARNING          "
        print "// ========================================================================"
        print
        print "#include <string>"
        print "#include <vector>"

        if xidlConfig.generateXML:
            print "#include \"xml/include/library/XML.h\""
            
        print

        for i in self._includes:
            print "#include \"xml/include/c_api/" + i + "\""

        print

        if self._namespace != "":
            print "namespace " + self._namespace + " {"
            print
    
        if self._version != "":
            print "const std::string VERSION = \"" + self._version + "\";"
            print

    def writeHeader_CImpl(self):        
        print "// ========================================================================"
        print "//          WARNING               WARNING                 WARNING          "
        print "// ========================================================================"
        print "//"                                                                         
        print "//          You are looking at a generated file. Do not edit!!!            "
        print "//"                                                                         
        print "// Generated on " + time.asctime()
        print "// ========================================================================"
        print "//          WARNING               WARNING                 WARNING          "
        print "// ========================================================================"
        print
        print "#include <string>"
        print "#include <vector>"

        if xidlConfig.generateXML:
            print "#include \"xml/include/library/XML.h\""
            
        for i in self._includes:
            print "#include \"xml/include/c_api/" + i + "\""
        print "#include \"xml/include/c_api/" + self._filename + ".h\""
        print

        if self._namespace != "":
            print
            print "namespace " + self._namespace + " {"
            print
    
    def writeInlineHeader_C(self):
        print "// ========================================================================"
        print "// Inline methods for sending and receving previous classes using XML      "
        print "// ========================================================================"
        print 


    def writeFooter_C(self):
        if self._namespace != "":
            print
            print "} // namespace " + self._namespace
            print 

        print "#endif // " + string.upper(self._filename) + "_HEADER"

    def writeFooter_CHeader(self):
        if self._namespace != "":
            print "} // namespace " + self._namespace
            print 

        print "#endif // " + string.upper(self._filename) + "_HEADER"

    def writeFooter_CImpl(self):
        if self._namespace != "":
            print "} // namespace " + self._namespace
            print 


    # ------------------- generator methods for Java --------     
    def generate_Java(self):
        # Create the directory structure for the package.
        self._packageName = self._filename.lower().replace(".java","")
        print "Generating package " + self._packageName
        if(not os.path.exists(self._packageName)):
          os.mkdir(self._packageName)
          
        os.chdir(self._packageName)
        
        # Generate the JAR manifest file.
        self.generateManifestFile_Java()
        
        # Generate the package class file header
        self.generatePackageHeader_Java()

        # write the class 
        for c in self._classes:
            c._version = self._version
            c.writeClass_Java(self._packageName, self._packageHeader)
            
        for v in self._valueSets:
            v._version = self._version
            v.writeValueSet_Java(self._packageName, self._packageHeader)
            
        os.chdir("..")


    def generatePackageHeader_Java(self):
        self._packageHeader =  "// ========================================================================\n"
        self._packageHeader += "//          WARNING               WARNING                 WARNING          \n"
        self._packageHeader += "// ========================================================================\n"
        self._packageHeader += "//\n"
        self._packageHeader += "//          You are looking at a generated file. Do not edit!!!            \n"
        self._packageHeader += "//\n"
        self._packageHeader += "// Generated on " + time.asctime() + "\n"
        self._packageHeader += "// ========================================================================\n"
        self._packageHeader += "//          WARNING               WARNING                 WARNING          \n"
        self._packageHeader += "// ========================================================================\n"
        self._packageHeader += "\n"
        self._packageHeader += "package " + self._packageName + ";"
        self._packageHeader += "\n"
        self._packageHeader += "import java.io.*;\n"
        self._packageHeader += "import java.util.*;\n"
        self._packageHeader += "import java.lang.*;\n"
        self._packageHeader += "import java.math.*;\n"
        self._packageHeader += "import org.xml.sax.*;\n"
        self._packageHeader += "import org.apache.xerces.impl.dv.util.Base64;\n"
        self._packageHeader += "import mcserver.*;\n"
                    

    def generateManifestFile_Java(self):
        print "Creating " + self._packageName + ".manifest"
        f = open("manifest", 'w')
        f.write("Manifest-Version: 1.0\n")
        f.write("Class-path: ./\n")
        f.write("\n")
        f.close()
