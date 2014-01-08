
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
import time

class XmlFile:
    """
    An xmlFile object.
    """
    _header = ""
    
    def __init__(self, name, attrs, header):
        """
        The constructor.
        
        \param name
                 the name of the file.
        \param attrs
                 the XML attributes for the file.
        """

        self._typeName = name
        self._name = attrs['filename']
        self._version = None
        self._type = None
        self._comment = None
        self._namespace = None
        self._apiHeader = None
        self._apiImplementation = None
        self._apiConsole = None
        self._header = header
        
        if 'version' in attrs.keys():
            self._version = attrs['version']
            
        if 'type' in attrs.keys():
            self._type = attrs['type']
            
        if 'comment' in attrs.keys():
            self._comment = attrs['comment'].capitalize()
            if self._comment[-1] != '.':
                self._comment += '.'

        if 'namespace' in attrs.keys():
            self._namespace = attrs['namespace']
        
        # A list of the top-level classes for this file.
        self._classes = []
        
        # A list of the top-level value sets for this file.
        self._valueSets = []
        
        # A list of API methods.
        self._apiMethods = []

        # A list of required includes.
        self._includes = []

    def addClass(self, classObject):
        """
        Adds an XmlClass object to the list of subordinate classes for the file.
        
        \param classObject
                 the XmlClass object to add.
        """

        self._classes.append(classObject)
        
    def addValueSet(self, valueSet):
        """
        Adds an XmlValueSet object to the list of subordinate value sets for the file.
        
        \param valueSet
                 the XmlValueSet object to add.
        """

        self._valueSets.append(valueSet)
        
    def addInclude(self, include):
        """
        Adds an XmlInclude object to the list of includes for the file.
        
        \param method
                 the XmlInclude object to add.
        """

        self._includes.append(include)
        
    def addMethod(self, method):
        """
        Adds an XmlMethod object to the list of methods for the file.
        
        \param method
                 the XmlMethod object to add.
        """

        self._apiMethods.append(method)
        
    def getHeader(self, hFile, language):
        """
        Generates a header for source files.
        
        \param hFile
                 a flag indicating if this header is for a C++ .h file. This is ignored for Java source.
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        \return a string containing a header for API source files.
        """
        text = ""
        text += "// begin_generated_IBM_copyright_prolog                             \n"
        text += "//                                                                  \n"
        text += "// This is an automatically generated copyright prolog.             \n"
        text += "// After initializing,  DO NOT MODIFY OR MOVE                       \n"
        text += "// ---------------------------------------------------------------- \n"
        text += "// Licensed Materials - Property of IBM                             \n"
        text += "//                                                                  \n"
        text += "// Blue Gene/Q                                                      \n"
        text += "//                                                                  \n"
        text += "// (C) Copyright IBM Corp.  2010, 2011                              \n"
        text += "//                                                                  \n"
        text += "// US Government Users Restricted Rights -                          \n"
        text += "// Use, duplication or disclosure restricted                        \n"
        text += "// by GSA ADP Schedule Contract with IBM Corp.                      \n"
        text += "//                                                                  \n"
        text += "// This software is available to you under the                      \n"
        text += "// Eclipse Public License (EPL).                                    \n"
        text += "//                                                                  \n"
        text += "// ---------------------------------------------------------------- \n"
        text += "//                                                                  \n"
        text += "// end_generated_IBM_copyright_prolog                               \n"
        
        if language.lower() == "c++":
            text += "// ========================================================================\n"
            text += "//          WARNING               WARNING                 WARNING          \n"
            text += "// ========================================================================\n"
            text += "//\n"
            text += "//          You are looking at a generated file. Do not edit!!!            \n"
            text += "//\n"                                                                         
            text += "// Generated on " + time.asctime() + "\n"
            text += "// ========================================================================\n"
            text += "//          WARNING               WARNING                 WARNING          \n"
            text += "// ========================================================================\n"
            text += "\n"
            
            if hFile:
                text += "#ifndef _" + self._name.upper() + "_H\n"
                text += "#define _" + self._name.upper() + "_H\n"
                text += "\n"
            text += "#include <cstring>\n"
            text += "#include <string>\n"
            text += "#include <vector>\n"
            text += "#include \"xml/include/library/XML.h\"\n"
            if not hFile:
                if len(self._header) != 0:
                    text += "#include \"" + self._header + "/" + self._name + ".h\"\n"
                else:
                    text += "#include \"xml/include/c_api/" + self._name + ".h\"\n"

            text += "\n"
        
            
            for i in self._includes:
                text += "#include \"" + i + ".h\"\n"

            if self._type == "boostXML":
                text += "#include <boost/archive/xml_oarchive.hpp>\n"
                text += "#include <boost/archive/xml_iarchive.hpp>\n"
            if self._type == "boostBinary":
                text += "#include <boost/archive/binary_oarchive.hpp>\n"
                text += "#include <boost/archive/binary_iarchive.hpp>\n"
            if self._type == "boostText":
                text += "#include <boost/archive/text_oarchive.hpp>\n"
                text += "#include <boost/archive/text_iarchive.hpp>\n"
            if self._type == "boostXML" or self._type == "boostBinary" or self._type == "boostText":
                text += "#include <boost/serialization/string.hpp>\n"
                text += "#include <boost/serialization/vector.hpp>\n"
                text += "#include <boost/serialization/base_object.hpp>\n"
                text += "#include <boost/serialization/binary_object.hpp>\n"
                text += "#include <boost/serialization/export.hpp>\n"
                text += "#include <boost/serialization/tracking.hpp>\n"

            
            indent = ""
            
            if self._namespace != None:
                text += "\n"
                text += "namespace " + self._namespace + "\n"
                text += "{\n"
                indent = "  "
            
            if ((hFile) and (self._version != None)):
                if self._namespace != None:
                    text += "\n"
                    text += indent + "/*! The " + self._namespace + " version. */\n"
                else:
                    text += "\n"
                    text += indent + "/*! The API version. */\n"
                text += indent + "const std::string VERSION = \"" + self._version + "\";\n"
                
        elif language.lower() == "java":
            text += "// ========================================================================\n"
            text += "//          WARNING               WARNING                 WARNING          \n"
            text += "// ========================================================================\n"
            text += "//\n"
            text += "//          You are looking at a generated file. Do not edit!!!            \n"
            text += "//\n"
            text += "// Generated on " + time.asctime() + "\n"
            text += "// ========================================================================\n"
            text += "//          WARNING               WARNING                 WARNING          \n"
            text += "// ========================================================================\n"
            text += "\n"
            text += "package " + self.getPackageName() + ";\n"
            text += "\n"
            text += "import java.io.*;\n"
            text += "import java.util.*;\n"
            text += "import java.lang.*;\n"
            text += "import java.math.*;\n"
            text += "import org.xml.sax.*;\n"
            text += "import org.apache.xerces.impl.dv.util.Base64;\n"
            text += "import mcserver.*;\n"
            
        else:
            print "Unsupported language: " + language
        
        return text


    def getFooter(self, hFile, language):
        """
        Generates a footer for source files.
        
        \param hFile
                 a flag indicating if this header is for a C++ .h file. This is ignored for Java source.
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        \return a string containing a footer for API source files.
        """
        
        text = ""
        
        if language.lower() == "c++":
            if self._namespace != None:
                text += "}\n"
                text += "\n"
        
            if hFile:
                text += "#endif\n"
                
        elif language.lower() == "java":
            text = ""
            
        else:
            print "Unsupported language: " + language
            
        return text
        
    def getPackageName(self):
        """
        Retrieves the Java package name.
        """
        
        if self._namespace != None:
            return self._namespace.lower()
        else:
            return self._name.lower()

    def generateManifestFile(self):
        """
        Creates a basic Java manifest file for the API JAR. Note: this routine assumes it's already in the 
        JAR's root directory. The manifest file will end up in whatever the current working directory is.
        """
        
        f = open("manifest", 'w')
        f.write("Manifest-Version: 1.0\n")
        f.write("Class-path: ./\n")
        if self._comment != None:
            f.write("Specification-Title: \"" + self._comment + "\"\n")
        if self._version != None:
            f.write("Specification-Version: \"" + self._version + "\"\n")
        f.write("Specification-Vendor: IBM Corp.\n")
        f.write("\n")
        f.close()
        
    def generateApiHeader(self):
        """
        Generates a C++ API header file.
        """
        text = ""
        text += "// begin_generated_IBM_copyright_prolog                             \n"
        text += "//                                                                  \n"
        text += "// This is an automatically generated copyright prolog.             \n"
        text += "// After initializing,  DO NOT MODIFY OR MOVE                       \n"
        text += "// ---------------------------------------------------------------- \n"
        text += "// Licensed Materials - Property of IBM                             \n"
        text += "//                                                                  \n"
        text += "// Blue Gene/Q                                                      \n"
        text += "//                                                                  \n"
        text += "// (C) Copyright IBM Corp.  2010, 2011                              \n"
        text += "//                                                                  \n"
        text += "// US Government Users Restricted Rights -                          \n"
        text += "// Use, duplication or disclosure restricted                        \n"
        text += "// by GSA ADP Schedule Contract with IBM Corp.                      \n"
        text += "//                                                                  \n"
        text += "// This software is available to you under the                      \n"
        text += "// Eclipse Public License (EPL).                                    \n"
        text += "//                                                                  \n"
        text += "//  --------------------------------------------------------------- \n"
        text += "//                                                                  \n"
        text += "// end_generated_IBM_copyright_prolog                               \n"
        text += "// ========================================================================\n"        
        text += "//          WARNING               WARNING                 WARNING          \n"
        text += "// ========================================================================\n"
        text += "//\n"
        text += "//          You are looking at a generated file. Do not edit!!!            \n"
        text += "//\n"                                                                         
        text += "// Generated on " + time.asctime() + "\n"
        text += "// ========================================================================\n"
        text += "//          WARNING               WARNING                 WARNING          \n"
        text += "// ========================================================================\n"
        text += "\n"
        text += "#ifndef " + self._apiHeader.upper() + "_H\n"
        text += "#define " + self._apiHeader.upper() + "_H\n"
        text += "\n"
        if len(self._header) != 0:
            text += "#include \"" + self._header + "/" + self._name + ".h\"\n"
        else:
            text += "#include \"xml/include/c_api/" + self._name + ".h\"\n"
        text += "\n"
        text += "class " + self._apiHeader + "\n" 
        text += "{\n"
        text += "public:\n"
        text += "\n"
        text += "  virtual ~" + self._apiHeader + "() {}\n"
        text += "\n"
        text += "  static " + self._apiHeader + " *Instance()\n"
        text += "  {\n"
        text += "    assert(_instance != 0);\n"
        text += "    return _instance;\n"
        text += "  }\n"
        text += "\n"
  
        for m in self._apiMethods:
            if m._type == "RequestReply":
                text += m.getDecl("  ", self._namespace)
                text += "\n"
        
        text += "protected:\n"
        text += "  static " + self._apiHeader + "* _instance;\n"
        text += "};\n"
        text += "\n"
        text += "// access the singleton object\n"
        text += "inline " + self._apiHeader + " *get" + self._apiHeader + "()\n"
        text += "{\n"
        text += "  return " + self._apiHeader + "::Instance();\n"
        text += "}\n"
        text += "\n"
        text += "#endif\n"

        print "Generating " + self._apiHeader + ".h"
        file = open(self._apiHeader + ".h", 'w')
        file.write(text)
        file.close()

    def generateApiImplementation(self):
        """
        Generates a C++ API implemenation file.
        """
     
        text = ""
        text += "// begin_generated_IBM_copyright_prolog                             \n"
        text += "//                                                                  \n"
        text += "// This is an automatically generated copyright prolog.             \n"
        text += "// After initializing,  DO NOT MODIFY OR MOVE                       \n"
        text += "// ---------------------------------------------------------------- \n"
        text += "// Licensed Materials - Property of IBM                             \n"
        text += "//                                                                  \n"
        text += "// Blue Gene/Q                                                      \n"
        text += "//                                                                  \n"
        text += "// (C) Copyright IBM Corp.  2010, 2011                              \n"
        text += "//                                                                  \n"
        text += "// US Government Users Restricted Rights -                          \n"
        text += "// Use, duplication or disclosure restricted                        \n"
        text += "// by GSA ADP Schedule Contract with IBM Corp.                      \n"
        text += "//                                                                  \n"
        text += "// This software is available to you under the                      \n"
        text += "// Eclipse Public License (EPL).                                    \n"
        text += "//  --------------------------------------------------------------- \n"
        text += "//                                                                  \n"
        text += "// end_generated_IBM_copyright_prolog                               \n"
        text += "// ========================================================================\n"
        text += "//          WARNING               WARNING                 WARNING          \n"
        text += "// ========================================================================\n"
        text += "//\n"
        text += "//          You are looking at a generated file. Do not edit!!!            \n"
        text += "//\n"                                                                         
        text += "// Generated on " + time.asctime() + "\n"
        text += "// ========================================================================\n"
        text += "//          WARNING               WARNING                 WARNING          \n"
        text += "// ========================================================================\n"
        text += "\n"
        text += "#ifndef " + self._apiImplementation.upper() + "_H\n"
        text += "#define " + self._apiImplementation.upper() + "_H\n"
        text += "\n"
        if len(self._header) != 0:
            text += "#include \"" + self._header + "/" + self._name + ".h\"\n"
        else:
            text += "#include \"xml/include/c_api/" + self._name + ".h\"\n"
        text +=  "\n"
        text += "class " + self._apiImplementation + ": public " + self._apiHeader + "\n"
        text += "{\n" 
        text += "public:\n"
        text += "\n"
        text += "  virtual ~" + self._apiImplementation + "() {}\n"
        text += "\n"

        for m in self._apiMethods:
            if m._type == "RequestReply":
                text += m.getImpl("  ", self._namespace)
                text += "\n"
        
        text += "}\n"
        text += "\n"
        text += "#endif\n"
        
        print "Generating " + self._apiImplementation + ".cc"
        file = open(self._apiImplementation + ".cc", 'w')
        file.write(text)
        file.close()
        
    def generateConsoleHeader(self):
        """
        Generates a C++ console header file.
        """

        text = ""
        text += "// begin_generated_IBM_copyright_prolog                             \n"
        text += "//                                                                  \n"
        text += "// This is an automatically generated copyright prolog.             \n"
        text += "// After initializing,  DO NOT MODIFY OR MOVE                       \n"
        text += "// ---------------------------------------------------------------- \n"
        text += "// Licensed Materials - Property of IBM                             \n"
        text += "//                                                                  \n"
        text += "// Blue Gene/Q                                                      \n"
        text += "//                                                                  \n"
        text += "// (C) Copyright IBM Corp.  2010, 2011                              \n"
        text += "//                                                                  \n"
        text += "// US Government Users Restricted Rights -                          \n"
        text += "// Use, duplication or disclosure restricted                        \n"
        text += "// by GSA ADP Schedule Contract with IBM Corp.                      \n"
        text += "//                                                                  \n"
        text += "// This software is available to you under the                      \n"
        text += "// Eclipse Public License (EPL).                                    \n"
        text += "//  --------------------------------------------------------------- \n"
        text += "//                                                                  \n"
        text += "// end_generated_IBM_copyright_prolog                               \n"
        text += "// ========================================================================\n"     
        text += "//          WARNING               WARNING                 WARNING          \n"
        text += "// ========================================================================\n"
        text += "//\n"
        text += "//          You are looking at a generated file. Do not edit!!!            \n"
        text += "//\n"                                                                         
        text += "// Generated on " + time.asctime() + "\n"
        text += "// ========================================================================\n"
        text += "//          WARNING               WARNING                 WARNING          \n"
        text += "// ========================================================================\n"
        text += "\n"
        text += "#ifndef " + self._apiConsole.upper() + "_H\n"
        text += "#define " + self._apiConsole.upper() + "_H\n"
        text += "\n"
        text += "#include <string>\n"
        text += "#include <vector>\n"
        if len(self._header) != 0:
            text += "#include \"" + self._header + "/" + self._name + ".h\"n"
        else:
            text += "#include \"xml/include/c_api/" + self._name + ".h\"\n"
        text += "\n"
        text += "class " + self._apiConsole + "\n"
        text += "{\n" 
        text += "public:\n"
        text += "  " + self._apiConsole + "( " + self._apiHeader + " *server);\n"
        text += "  std::vector<std::string> readCommand();\n"
        text += "  bool dispatch(const std::vector<std::string> &argv);\n"
        text += "  void help();\n"
        text += "\n"
        text += "protected:\n"
        text += "  " + self._apiHeader + " *_theServer;\n"
        text += "\n"
        
        for m in self._apiMethods:
            if m._type == "RequestReply":
                text += "  bool " + m._name + " (const std::vector<std::string> &argv);\n"
                
        text += "\n"
        text += "private:\n"
        text += "  " + self._apiConsole + "() {};\n"
        text += "};\n"
        text += "\n"
        text += "#endif\n"

        print "Generating " + self._apiConsole + ".h"
        file = open(self._apiConsole + ".h", 'w')
        file.write(text)
        file.close()

    def generateConsoleImplementation(self):
        """
        Generates a C++ console implementation file.
        """
        
        text = ""
        text += "// begin_generated_IBM_copyright_prolog                             \n"
        text += "//                                                                  \n"
        text += "// This is an automatically generated copyright prolog.             \n"
        text += "// After initializing,  DO NOT MODIFY OR MOVE                       \n"
        text += "// ---------------------------------------------------------------- \n"
        text += "// Licensed Materials - Property of IBM                             \n"
        text += "//                                                                  \n"
        text += "// Blue Gene/Q                                                      \n"
        text += "//                                                                  \n"
        text += "// (C) Copyright IBM Corp.  2010, 2011                              \n"
        text += "//                                                                  \n"
        text += "// US Government Users Restricted Rights -                          \n"
        text += "// Use, duplication or disclosure restricted                        \n"
        text += "// by GSA ADP Schedule Contract with IBM Corp.                      \n"
        text += "//                                                                  \n"
        text += "// This software is available to you under the                      \n"
        text += "// Eclipse Public License (EPL).                                    \n"
        text += "//  --------------------------------------------------------------- \n"
        text += "//                                                                  \n"
        text += "// end_generated_IBM_copyright_prolog                               \n"
        text += "// ========================================================================\n"
        text += "//          WARNING               WARNING                 WARNING          \n"
        text += "// ========================================================================\n"
        text += "//\n"
        text += "//          You are looking at a generated file. Do not edit!!!            \n"
        text += "//\n"                                                                         
        text += "// Generated on " + time.asctime() + "\n"
        text += "// ========================================================================\n"
        text += "//          WARNING               WARNING                 WARNING          \n"
        text += "// ========================================================================\n"
        text += "\n"
        text += "#include \"" + self._apiConsole + ".h\"\n"
        text += "#include <stdio.h>"
        text += "using namespace " + self._namespace + ";\n"
        text += "\n"
        
        text += "void " + self._apiConsole + "::help()\n"
        text += "{\n"
        text += "  printf(\"usage:\\n\"\n"
        for m in self._apiMethods:
            if m._type == "RequestReply":
                text += "    \"" + m._name + " - " + m._comment + "\\n\"\n"
                
        text += "  );\n"
        text += "}\n"
        text +=  "\n"
        text += "std::vector<std::string> " + self._apiConsole + "::readCommand()\n"
        text += "{\n"
        text += "  std::vector<std::string> result;\n"
        text += "  // parse command here\n"
        text += "  return result;\n"
        text += "}\n"
        text += "\n"
        text += "bool " + self._apiConsole + "::dispatch(const std::vector<std::string> &argv)\n"
        text += "{\n"
        text += "  assert(argv.size() > 0);\n"
        text += "  bool error = false;\n"
        text += "  try\n"
        text += "  {\n"
        
        prefix = "if"
        for m in self._apiMethods:
            if m._type == "RequestReply":
                text += "    " + prefix + "(argv[0] == \"" + m._name + "\")\n"
                text += "    {\n"
                text += "      error = " + m._name + "(argv);\n"
                text += "    }\n"
                prefix = "else if"
        text += "    else\n"
        text += "    {\n"
        text += "      printf(\"Unrecognized command\");\n"
        text += "      help();\n"
        text += "    }\n"
        text += "  }\n"
        text += "  catch(invalid_argument &ia)\n"
        text += "  {\n"
        text += "    printf(std::string(\"!!!Exception from server: \").append(ia.what()));\n"
        text += "  }\n"
        text += "\n"
        text += "  return error;\n"
        text += "}\n"
        text += "\n"
        
        for m in self._apiMethods:
            if m._type == "RequestReply":
                text += "bool " + self._apiConsole + "::" + m._name + "(const std::vector<std::string> &argv)\n"
                text += "{\n"
                text += "  if(argv.size() < 1)\n"
                text += "  {\n"
                text += "    stdout << \"args?\";\n"
                text += "    return false;\n"
                text += "  }\n"
                text += "  else\n"
                text += "  {\n"
                text += "    " + m.getNameCapFirst() + "Request request();\n"
                text += "    " + m.getNameCapFirst() + "Reply   reply;\n"
                text += "    _theServer->" + m._name + "(request, reply);\n"
                text += "\n"
                text += "    stdout << replyMsg( reply );\n"
                text += "  }\n"
                text += "\n"
                text += "  return true;\n"
                text += "}\n"
                text += "\n"

        print "Generating " + self._apiConsole + ".cc"
        file = open(self._apiConsole + ".cc", 'w')
        file.write(text)
        file.close()
