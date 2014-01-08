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

import os, string, sys, pprint, time, xidlFile

def underscoreAux(name):
    return "_" + name

def quotedAux(name):
    return "\"" + name + "\""


def realTypeAux_C(type):
    if type == "text":
        return "std::string"
    elif type == "binary":
        return "std::string"
    elif type == "int32":
        return "int"
    elif type == "uint32":
        return "unsigned"
    elif type == "uint64":
        return "unsigned long long"
    elif type == "uchar":
        return "unsigned char"
    elif type == "float":
        return "double"
    elif type == "bool":
        return "bool"
    else:
        return type


def realTypeAux_Java(type):
    if type == "text":
        return "String"
    elif type == "binary":
        return "byte[]"
    elif type == "int32":
        return "Integer"
    # Note: Java does not have "unsigned" integers, so need to go larger to accomodate all bits
    elif type == "uint32":
        return "Long"
    elif type == "uint64":
        return "BigInteger"
    elif type == "uchar":
        return "Short"
    elif type == "float":
        return "BigDecimal"
    elif type == "bool":
        return "Boolean"
    else:
        return type


def isRealType(type):
    if type == "text" or type == "binary" or  type == "int32" or type == "uint32" or type == "uint64"  or type == "uchar" or type == "float" or type == "bool":
        return 1
    else:
        return 0

def readAttributeAux_C(fullyQualifiedClass, type, name, isOptional):
    result = underscoreAux(name)
    if type == "text":
        result = result + " = XML::read_text("
    elif type == "binary":
        result = result + " = XML::read_binary("
    elif type == "uint32": 
        result = result + " = XML::read_uint32("
    elif type == "uint64": 
        result = result + " = XML::read_uint64("
    elif type == "int32": 
        result = result + " = XML::read_int32("
    elif type == "uchar":
        result = result + " = XML::read_uchar("
    elif type == "float":
        result = result + " = XML::read_float("
    elif type == "bool":
        result = result + " = XML::read_bool("

    result = result + quotedAux(fullyQualifiedClass) + ", " + quotedAux(name) + ", attrs[i], attrs[i+1], &i, " + isOptional + "); "
 
    return result

def readAttributeEnumAux_C(fullyQualifiedClass, type, name, isOptional):
    result = underscoreAux(name)
    result = result + " = (" + type + ")XML::read_int32("
    result = result + quotedAux(fullyQualifiedClass) + ", " + quotedAux(name) + ", attrs[i], attrs[i+1], &i, " + isOptional + "); "
    return result

def readAttributeEnumAux2_C(fullyQualifiedClass, type, name, isOptional):
    result = realTypeAux_C(type) + " " + name
    result = result + " = (" + type + ")XML::read_int32("
    result = result + quotedAux(fullyQualifiedClass) + ", " + quotedAux(name) + ", attrs[i], attrs[i+1], &i, " + isOptional + "); " 
    return result

## JGC
def readAttributeAux2(fullyQualifiedClass, type, name, isOptional):
    result = realTypeAux_C(type) + " " + name
    if type == "text":
        result = result + " = XML::read_text("
    elif type == "binary":
        result = result + " = XML::read_binary("
    elif type == "int32": 
        result = result + " = XML::read_int32("
    elif type == "uint32": 
        result = result + " = XML::read_uint32("
    elif type == "uint64": 
        result = result + " = XML::read_uint64("
    elif type == "uchar":
        result = result + " = XML::read_uchar("
    elif type == "float":
        result = result + " = XML::read_float("
    elif type == "bool":
        result = result + " = XML::read_bool("

    result = result + quotedAux(fullyQualifiedClass) + ", "+ quotedAux("value") + ", attrs[i], attrs[i+1], &i, " + isOptional + "); " 
 
    return result

def readAttributeAux_Java(type, name):
    if (type == "text"):
        return underscoreAux(name) + " = attrs.getValue(i);"
    elif (type == "binary"):
        return underscoreAux(name) + " = Base64.decode(attrs.getValue(i));"
    elif (type == "int32"):
        return underscoreAux(name) + " = Integer.decode(attrs.getValue(i));"
    elif (type == "uint32"):
        return underscoreAux(name) + " = Long.decode(attrs.getValue(i));"
    elif (type == "uint64"):
        return underscoreAux(name) + " = new BigInteger(attrs.getValue(i));"
    elif (type == "uchar"):
        return underscoreAux(name) + " = Short.decode(attrs.getValue(i));"
    elif (type == "float"):
        return underscoreAux(name) + " = new BigDecimal(attrs.getValue(i));"
    elif (type == "bool"):
        return underscoreAux(name) + " = Boolean.parseBoolean(attrs.getValue(i));"

def readAttributeEnumAux_Java(fullyQualifiedClass, type, name):
    result = underscoreAux(name)
    result = result + " = " + type + ".translate(Integer.decode(attrs.getValue(i))); i += 2;"
    return result

def writeAttributeAux_C(fullyQualifiedClass, type, name):
    if type == "text":
        return "XML::write_text(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux(name) + ", " + underscoreAux(name) + ");"
    elif type == "binary":
        return "XML::write_binary(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux(name) + ", " + underscoreAux(name) + ");"
    elif type == "int32": 
        return "XML::write_int32(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux(name) + ", " + underscoreAux(name) + ");"
    elif type == "uint32": 
        return "XML::write_uint32(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux(name) + ", " + underscoreAux(name) + ");"
    elif type == "uint64": 
        return "XML::write_uint64(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux(name) + ", " + underscoreAux(name) + ");"
    elif type == "uchar":
        return "XML::write_uchar(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux(name) + ", " + underscoreAux(name) + ");"
    elif type == "float":
        return "XML::write_float(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux(name) + ", " + underscoreAux(name) + ");"
    elif type == "bool":
        return "XML::write_bool(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux(name) + ", " + underscoreAux(name) + ");"
    elif type == "enum":
        return "XML::write_int32(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux(name) + ", (int)" + underscoreAux(name) + ");"



def readAttributeAux2_Java(type, name, isValueSet):
    if type == "text":
        return underscoreAux(name) + ".add(attrs.getValue(0));"
    elif type == "binary":
        return underscoreAux(name) + ".add(Base64.decode(attrs.getValue(0)));"
    elif type == "int32":
        return underscoreAux(name) + ".add(Integer.decode(attrs.getValue(0)));"
    elif type == "uint32":
        return underscoreAux(name) + ".add(Long.decode(attrs.getValue(0)));"
    elif type == "uint64":
        return underscoreAux(name) + ".add(new BigInteger(attrs.getValue(0)));"
    elif type == "uchar":
        return underscoreAux(name) + ".add(Short.decode(attrs.getValue(0)));"
    elif type == "float":
        return underscoreAux(name) + ".add(new BigDecimal(attrs.getValue(0)));"
    elif type == "bool":
        return underscoreAux(name) + ".add(Boolean.parseBoolean(attrs.getValue(0)));"
    elif isValueSet:
        return underscoreAux(name) + ".add(" + type + ".translate(Integer.decode(attrs.getValue(0))));"
    else:
        return "ERROR: type=" + type + ", name=" + name



# JGC: used in attribute
def writeAttributeAux2(fullyQualifiedClass, type, name):
    if type == "text":
        return "XML::write_text(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux("value") + ", " + name + ");"
    elif type == "binary":
        return "XML::write_binary(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux("value") + ", " + name + ");"
    elif type == "int32": 
        return "XML::write_int32(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux("value") + ", " + name + ");"
    elif type == "uint32": 
        return "XML::write_uint32(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux("value") + ", " + name + ");"
    elif type == "uint64": 
        return "XML::write_uint64(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux("value") + ", " + name + ");"
    elif type == "uchar":
        return "XML::write_uchar(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux("value") + ", " + name + ");"
    elif type == "float":
        return "XML::write_float(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux("value") + ", " + name + ");"
    elif type == "bool":
        return "XML::write_bool(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux("value") + ", " + name + ");"
    elif type == "enum": 
        return "XML::write_int32(os, " + quotedAux(fullyQualifiedClass) + ", " + quotedAux("value") + ", " + name + ");"



class DataMember:
    # -------------------- initialization ------------------    
    def __init__(self, type, name, file):
        # required attributes
        self._type        = type     # type of the data member
        self._name        = name     # name of the data member

        # optional attributes
        self._occurrences   = "one"  # is it a vector ("many")?
        self._inConstructor = 1      # does not need to appear in attribute constructor
                                     # (only used when occurrences = "one")
        self._isOptional    = 0      # does not need to appear in the XML stream, default to false
        self._isInherited = 0        # it is a data member of a superclass
                                     # should not be declared again, but
                                     # should be marshaled/unmarshaled
        self._isValueSet = file.isValueSet(self._type) # set to 1 if this is a file-scoped value set, 
                                     # value sets get marshaled as integers
        self._comment     = None
        
    # -------------------- setters -------------------------
    def setComment(self, comment):
        self._comment = comment.capitalize()
        if self._comment[-1] != ".":
            self._comment += "."

    def setInConstructor(self, inConstructor):
        self._inConstructor = inConstructor
   
    def setOccurrences(self, occurrences):
        self._occurrences = occurrences

    def setIsOptional(self, isOptional):
        self._isOptional = isOptional

    def setIsInherited(self, isInherited):
        self._isInherited = isInherited

    def setIsValueSet(self, isValueSet):
        if self._isValueSet == 0: # originally set if there is a file-scoped enum, don't reset
            self._isValueSet = isValueSet

    # -------------------- auxiliary methods ----------------

    # we can only marshal data members as an xml attribute
    # (which is a pair name='value') if it is one of the primitive data
    # types and we only need one instance (not a vector>
    # otherwise, we need to treat it as an xml subentity, with its own
    # <entity> </subentity> tags (note that we can "optmize" vectors of
    # primitive data types as a sequence of tags with one attribute
    # as in <_name value='value' />)
    def isAttribute(self):
        if self._occurrences == "one":
            if self._isValueSet or self._type == "text" or self._type == "binary" or self._type == "int32" or self._type == "uint32" or self._type == "uint64" or self._type == "uchar" or self._type == "float" or self._type == "bool":
                return 1
            else:
                return 0
        else:    
            return 0    
        
    def isSubEntity(self):
        return not self.isAttribute()

    # return true if needs to appear in constructor of class
    def isInConstructor(self):
        return self._occurrences == "one" and self._inConstructor == 1

    # return true if it a single occurrences
    def isSingle(self):
        return self._occurrences == "one"

    # return true if it is multiple occurrences
    def isMultiple(self):
        return self._occurrences != "one"
    
    # return string value of boolean
    def getIsOptional(self):
        if self._isOptional == 0:
            return "false"
        else:
            return "true"
    


    # for the arguement list of the constructor of the class
    def getConstructorArgument_C(self):
        if self._type == "text" or self._type == "binary":
            return "const " + realTypeAux_C(self._type) + "& " + self._name
        elif self._type == "int32" or self._type == "uint32" or self._type == "uint64" or self._type == "uchar" or self._type == "float" or self._type == "bool":
            return realTypeAux_C(self._type) + " " + self._name
        else:
            return self._type + " " + self._name


    # for the arguement list of the constructor of the class
    def getConstructorArgument_Java(self):
        if self._type == "text" or self._type == "binary":
            return realTypeAux_Java(self._type) + " " + self._name
        elif self._type == "int32" or self._type == "uint32" or self._type == "uint64" or self._type == "uchar" or self._type == "float" or self._type == "bool":
            return realTypeAux_Java(self._type) + " " + self._name
        else:
            return self._type + " " + self._name


    # for the constructor of the class (after the : )
    def getCopyConstructor(self):
        return underscoreAux(self._name) + "(" + self._name + ")"


    # return the a string to appear at the declaration of the class
    def getDeclaration_C(self):
        result = "  "

        if (self._comment):
            result += "/*! " + self._comment + " */\n  "
                
        if self._occurrences == "many":
            result += "std::vector<" + realTypeAux_C(self._type) + ">"
        else:    
            result += realTypeAux_C(self._type)

        result += " " + underscoreAux(self._name) + ";"
        
        return result


    def getDeclaration_Java(self):
        result = "  public "

        if self._occurrences == "many":
            result = result + "ArrayList<" + realTypeAux_Java(self._type) + ">"
        else:    
            result = result + realTypeAux_Java(self._type)

        result = result + " " + underscoreAux(self._name) + ";"
        
        if (self._comment):
            result = result + " // " + self._comment
                
        return result


    # write a read attribute line    
    def writeReadAttribute_C(self, fullyQualifiedClass):
        if self._isValueSet:
            print "  " + readAttributeEnumAux_C(fullyQualifiedClass, self._type, self._name, self.getIsOptional())
        else:
            print "  " + readAttributeAux_C(fullyQualifiedClass, self._type, self._name, self.getIsOptional())

    def writeAttribute_C(self, fullyQualifiedClass):
        if self._isValueSet:
            print "  " + writeAttributeAux_C(fullyQualifiedClass, "enum", self._name)
        else:
            print "  " + writeAttributeAux_C(fullyQualifiedClass, self._type, self._name)

    def writeReadAttribute_Java(self, fullyQualifiedClass):
        print "      } else if (attrs.getQName(i).equals(\"" + self._name + "\")) {"
        if self._isValueSet:
            print "        " + readAttributeEnumAux_Java(fullyQualifiedClass, self._type, self._name)
        else:
            if(readAttributeAux_Java(self._type, self._name) == None):
                print "        readAttributeAux_Java() failed on a " + self._type + " type with " + self._name + " name"
            print "        " + readAttributeAux_Java(self._type, self._name)

    def writeAttribute_Java(self, fullyQualifiedClass):
        # Example: command += "demoattribute=\"" + _demoattribute + "\" ";
        if (self._type == "binary"):
            print "   command += \"" + self._name + "=\\\"\" + Base64.encode(" + underscoreAux(self._name) + ") + \"\\\" \";"
        elif self._isValueSet:
            print "   command += \"" + self._name + "=\\\"\" + " + self._type + ".translate(" + underscoreAux(self._name) + ") + \"\\\" \";"
        else:
            print "   command += \"" + self._name + "=\\\"\" + " + underscoreAux(self._name) + " + \"\\\" \";"
 

    # write an addSubentity line    
    def writeAddSubEntity_C(self):
        if self._occurrences == "many":
            if isRealType(self._type) or self._isValueSet:
                print "  } else if (!strcmp(name, \"" + underscoreAux(self._name) +"\")) {"
                print "    unsigned i = 0;"
                if self._isValueSet:
                    print "    " + readAttributeEnumAux2_C(self._type, self._type, "temp", self.getIsOptional())
                else:
                    print "    " + readAttributeAux2(underscoreAux(self._name), self._type, "temp", self.getIsOptional())
                print "    " + underscoreAux(self._name) + ".push_back(temp);"
                print "    XML::verify_last_attr(\"" + underscoreAux(self._name) + "\", attrs[i]);"
                print "    return this;"
                
            else:    
                print "  } else if (!strcmp(name, \"" + self._type +"\")) {"
                print "    " + self._type + " temp;"
                print "    temp.setAttributes(attrs);"
                print "    " + underscoreAux(self._name) + ".push_back(temp);"
                print "    return &" + underscoreAux(self._name) + ".back();"
        else:
            print "  } else if (!strcmp(name, \"" + self._type +"\")) {"
            print "    " + underscoreAux(self._name) + ".setAttributes(attrs);"
            print "    return &" + underscoreAux(self._name) +";"
            

    def writeSubEntity_C(self):
        if self._occurrences == "many":
            print "  for (std::vector<" + realTypeAux_C(self._type) +">::const_iterator iter = " + underscoreAux(self._name) + ".begin(); iter != " + underscoreAux(self._name) + ".end(); ++iter) {"
            
            if isRealType(self._type):
                print "    os << " + " \"<" + underscoreAux(self._name) + "\";"
                print "    " + writeAttributeAux2(underscoreAux(self._name), self._type, "*iter")
                print "    os << " + " \"/>\" << std::endl;"
            else:
                print "    os << *iter;"
                
            print "  }"

        else:
            print "  os << " + underscoreAux(self._name) + " << std::endl;"



    
    def writeAddSubEntity_Java(self):
        if self._occurrences == "many":
            if isRealType(self._type) or self._isValueSet:
                print "    } else if (name.equals(\"" + underscoreAux(self._name) +"\")) {"
#                print "      String temp = attrs.getValue(0);"
#                print "      " + underscoreAux(self._name) + ".add(temp);"

                print "      " + readAttributeAux2_Java(self._type, self._name, self._isValueSet);
                print "      return this;"
                
            else:    
                print "    } else if (name.equals(\"" + self._type +"\")) {"
                print "      " + self._type + " temp = new " + self._type + "();"
                print "      temp.setAttributes(attrs);"
                print "      " + underscoreAux(self._name) + ".add(temp);"
                print "      return temp;"
        else:
            print "  } else if (name.equals(\"" + self._type +"\")) {"
            print "    " + underscoreAux(self._name) + ".setAttributes(attrs);"
            print "    return " + underscoreAux(self._name) +";"

    def writeSubEntity_Java(self):
        if self._occurrences == "many":
            print "   if (" + underscoreAux(self._name) + " != null) {"
            print "      for (" + realTypeAux_Java(self._type) + " temp : " + underscoreAux(self._name) + ") {"
            if isRealType(self._type) or self._isValueSet:
                # Example: command += "<_realTypeEntity value=\"";
                # Example: command += 0;
                # Example: command += "\" />";
                print "        command += \"<" + underscoreAux(self._name) + " value=\\\"\";"
                print "        command += temp;"
                print "        command += \"\\\" />\";"
            else:
                print "        command += temp.getXMLString();"
            print "      }"
            print "    }"
            print
            
        else:
            print "    command += " + underscoreAux(self._name) + ".getXMLString();"

    def getDefaultSingleConstructor_Java(self):
        result = ""
        if (self._type == "int32" or self._type == "uchar"):
            result = underscoreAux(self._name) + " = 0;"
        elif (self._type == "uint32"):
            result = underscoreAux(self._name) + " = new Long(0);"            
        elif (self._type == "uint64"):
            result = underscoreAux(self._name) + " = BigInteger.valueOf(0);"
        elif (self._type == "float"):
            result = underscoreAux(self._name) + " = BigDecimal.valueOf(0);"
        elif (self._type == "bool"):
            result = underscoreAux(self._name) + " = false;"
        elif (self._type == "text"):
            result = underscoreAux(self._name) + " = new String();"
        elif (self._type == "binary"):
            result = underscoreAux(self._name) + " = null;"
        elif (self._isValueSet):
            result = underscoreAux(self._name) + " = null;"
        else:
            result = underscoreAux(self._name) + " = new " + self._type + "();"
        return result;

    def getDefaultMultipleConstructor_Java(self):
        result = ""
        if (self._type == "int32"):
            result = underscoreAux(self._name) + " = new ArrayList<Integer>();"
        elif (self._type == "uint32"):
            result = underscoreAux(self._name) + " = new ArrayList<Long>();"
        elif (self._type == "uint64"):
            result = underscoreAux(self._name) + " = new ArrayList<BigInteger>();"
        elif (self._type == "float"):
            result = underscoreAux(self._name) + " = new ArrayList<BigDecimal>();"
        elif (self._type == "uchar"):
            result = underscoreAux(self._name) + " = new ArrayList<Short>();"
        elif (self._type == "bool"):
            result = underscoreAux(self._name) + " = new ArrayList<Boolean>();"
        elif (self._type == "text"):
            result = underscoreAux(self._name) + " = new ArrayList<String>();"
        elif (self._type == "binary"):
            result = underscoreAux(self._name) + " = new ArrayList<byte[]>();"
        else:
            result = underscoreAux(self._name) + " = new ArrayList<" + self._type + ">();"
        return result;


    def getAttributeConstructor_Java(self):
        return underscoreAux(self._name) + " = " + self._name + ";"
    
