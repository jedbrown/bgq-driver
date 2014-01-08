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

# class to create enumerated types for C and java.

import os, string, sys, pprint, time
import xidlConfig

class ValueSet:
    # -------------------- initialization ------------------
    def __init__(self, name):
        # these three data members can appear as attributes in the
        # <Class > tag (name is required, commend and parentClass are
        # optional)
        self._name         = name
        self._comment      = None
        self._nestingLevel = 0

        self._valueMembers  = []             # all the fields in this class
        # (including ones that we inherit from a parent class; will not
        # be redefined, but will be marshalled/unmarshalled
        
        self._packageHeader = ""

    def getSpaces(self, level):
        result = ""
        for num in xrange(level):
            result += "  "
        return result 

    # -------------------- setters -------------------------
    def setComment(self, comment):
        self._comment = comment
    
    def setParent(self, parentClass):
        self._parentClass = parentClass

    def addValueMember(self, valueMember):
        self._valueMembers.append(valueMember)

    def setNestingLevel (self, level):
        self._nestingLevel = level

    # -------------------- auxiliary methods ----------------
    def fullyQualifiedName_C(self):
        if self._nestingClass == None:
            return self._name
        else:
            return self._nestingClass.fullyQualifiedName_C() + "::" + self._name

    def fullyQualifiedName_Java(self):
        if self._nestingClass == None:
            return self._name
        else:
            return self._nestingClass.fullyQualifiedName_Java() + "::" + self._name

    def getIndent(self):
        return self.getSpaces(self._nestingLevel+1)


    # starting to write a new class
    def writeHeader_C(self):
        print self.getIndent() + "// ======================================================================== "
        print self.getIndent() + "// ValueSet " + self._name
        print self.getIndent() + "// ========================================================================"
        print
        if self._comment:
            print self.getIndent() + "/// " + self._comment
            print

        temp = self.getIndent() + "typedef enum " + " {"        
        print temp


    # we finish the current class
    def writeClosingBrackets_C(self):
        print self.getIndent() + "} " + self._name + ";"
        print

    def writeDeclaration_C(self):
        self.writeHeader_C()
        self.writeValueMembers_C()
        self.writeClosingBrackets_C()

    def writeValueMembers_C (self):
        count = 0
        for v in self._valueMembers:
            if (count > 0) :
                print self.getIndent() + "  , " + v.getDeclaration_C()
            else:
                print self.getIndent() + "    " + v.getDeclaration_C()
            count = count + 1



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
                    
                    
    def writeDeclaration_Java(self):
        self.writeHeader_Java()
        self.writeValueMembers_Java()
        self.writeEnumFunctions()
        self.writeClosingBrackets_Java()

    def writeValueSet_Java(self, packageName, packageHeader):
        print "Creating " + packageName + "." + self._name
        
        # Open a Java file for the class.
        self._file = open(self._name + ".java", 'w')
        
        # Redirect stdout
        sys.stdout = self._file
            
        # Write the package header.
        print packageHeader
        
        self.writeDeclaration_Java()
        
        # Close the Java file for the class.
        self._file.close()
    
        # Restore stdout
        sys.stdout = sys.__stdout__
        


    def writeHeader_Java (self):
        print self.getIndent() + "// ========================================================================"
        print self.getIndent() + "// ValueSet " + self._name
        print self.getIndent() + "// ========================================================================"
        print
        if self._comment:
            print self.getIndent() + "/// " + self._comment
            print

        temp = self.getIndent() + "public enum " + self._name + " {"        
        print temp

    def writeValueMembers_Java(self):
        for v in self._valueMembers:
            print self.getIndent() + "    " + v.getDeclaration_Java(",")
        print self.getIndent() + "    ;"

    def writeEnumFunctions(self):
        print
        print self.getIndent() + "    public static " + self._name + " translate(Integer value) {"
        print self.getIndent() + "        " + self._name + " temp = " + self._name + "." + self._valueMembers[0]._name + ";"
        print self.getIndent() + "        switch(value) {"
        i = 0
        for v in self._valueMembers:
            if(v._value == None):
               print self.getIndent() + "            case " + str(i) + ":"
            else:
              print self.getIndent() + "            case " + v._value + ":"
            print self.getIndent() + "                temp = " + v._name + ";"
            print self.getIndent() + "                break;"
            i += 1
        print self.getIndent() + "        }"
        print self.getIndent() + "        return temp;"
        print self.getIndent() + "    }"
        print
        print self.getIndent() + "    public static Integer translate(" + self._name + " value) {"
        if(self._valueMembers[0]._value == None):
          print self.getIndent() + "        Integer temp = 0;"
        else:        
          print self.getIndent() + "        Integer temp = " + self._valueMembers[0]._value + ";"
        i = 0
        for v in self._valueMembers:
          print self.getIndent() + "        if(value == " + v._name + ") {"
          if(v._value == None):
              print self.getIndent() + "            temp = " + str(i) + ";"
          else:
              print self.getIndent() + "            temp = " + v._value + ";"
          print self.getIndent() + "        }"
          i += 1
        print self.getIndent() + "        return temp;"
        print self.getIndent() + "    }"
    
    def writeClosingBrackets_Java(self):
        print self.getIndent() + "};"
        print

#    def writeDeclaration_Java ():
#        result = "  "
#
#        result = self._name
#        if (self._value != None):
#            result = result + "(" + self._value + ")"
#        if (self._comment):
#            result = result + " // " + self._comment
#
#        return result


class ValueMember:
    def __init__(self, name):
        # these three data members can appear as attributes in the
        # <Class > tag (name is required, commend and parentClass are
        # optional)
        self._name         = name
        self._comment      = None
        self._value        = None

    # -------------------- setters -------------------------
    def setComment(self, comment):
        self._comment = comment

    def setValue(self, value):
        self._value = value

    # return the a string to appear at the declaration of the class
    def getDeclaration_C(self):
        result = "  "

        result = self._name
        if (self._value != None):
            result = result + " = " + self._value
        if (self._comment):
            result = result + " // " + self._comment

        return result


    def getDeclaration_Java(self):
        result = self._name

        if (self._comment):
            result = result + " // " + self._comment

        return result

    def getDeclaration_Java(self, terminatorChar):
        result = self._name

        if (terminatorChar):
            result = result + terminatorChar
        
        if (self._comment):
            result = result + " // " + self._comment

        return result

