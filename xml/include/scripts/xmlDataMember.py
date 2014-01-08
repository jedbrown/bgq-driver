
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
class XmlDataMember:
    """
    An XmlDataMember object.
    """
    
    def __init__(self, name, attrs):
        """
        The constructor.
        
        \param name
                 the name of the data member.
        \param attrs
                 the XML attributes for the data member.
        """

        self._typeName = name
        self._type = attrs['type']
        self._name = attrs['name']
        self._isValueSet = False
        self._isOptional = False
        self._isVector = False
        self._isArray = False
        self._comment = None
        self._size = ""
        
        if 'isOptional' in attrs.keys():
            self._isOptional = bool(attrs['isOptional'].upper() == 'TRUE')
        
        if 'occurrences' in attrs.keys():
            self._isVector = bool(attrs['occurrences'].upper() == 'MANY')
            self._isArray = not self._isVector
            if self._isArray:
                self._size = attrs['occurrences']
            
        if 'comment' in attrs.keys():
            self._comment = attrs['comment'].capitalize()
            if self._comment[-1] != '.':
                self._comment += '.'

    def getRealType(self, language):
        """
        Determines the type of the data member specific to the specified language.
        
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        \return a string containing the language-specific type for the data member. If the data member
                is not a real type, then the existing type name is returned unmodified.
        """
        
        if language.lower() == "c++":
            if self._type == "text":
                return "std::string"
            elif self._type == "binary":
                return "std::string"
            elif self._type == "int32":
                return "int"
            elif self._type == "uint32":
                return "unsigned"
            elif self._type == "uint64":
                return "unsigned long long"
            elif self._type == "uchar":
                return "unsigned char"
            elif self._type == "float":
                return "double"
            elif self._type == "bool":
                return "bool"
            else:
                return self._type
            
        elif language.lower() == "java":
            if self._type == "text":
                return "String"
            elif self._type == "binary":
                return "byte[]"
            elif self._type == "int32":
                return "Integer"
            # Note: Java does not have "unsigned" integers, so need to go larger to accommodate all bits
            elif self._type == "uint32":
                return "Long"
            elif self._type == "uint64":
                return "BigInteger"
            elif self._type == "uchar":
                return "Short"
            elif self._type == "float":
                return "BigDecimal"
            elif self._type == "bool":
                return "Boolean"
            else:
                return self._type
            
        else:
            return self._type

    def isRealType(self):
        """
        Determines if the type for the data member is a real type.
        
        \return true if the data member's type is a real type, false otherwise.
        """
        
        if self._type == "text" or \
           self._type == "binary" or  \
           self._type == "int32" or \
           self._type == "uint32" or \
           self._type == "uint64" or \
           self._type == "uchar" or \
           self._type == "float" or \
           self._type == "bool":
            return True
        else:
            return False
        
    def getXmlReadMethod(self):
        """
        Retrieves the XML object read method to use for this data member.
        
        \return a string containing the XML read method that should be used for this data member. If this data
                member is not a real type then an empty string is returned.
        """
        
        text = None
        if self._type == "text":
            text = "XML::read_text"
        elif self._type == "binary":
            text = "XML::read_binary"
        elif self._type == "int32":
            text = "XML::read_int32"
        elif self._type == "uint32": 
            text = "XML::read_uint32"
        elif self._type == "uint64": 
            text = "XML::read_uint64"
        elif self._type == "uchar":
            text = "XML::read_uchar"
        elif self._type == "float":
            text = "XML::read_float"
        elif self._type == "bool":
            text = "XML::read_bool"
        elif self._isValueSet:
            text = "XML::read_int32"

        return text
    
    def getXmlWriteMethod(self):
        """
        Retrieves the XML object write method to use for this data member.
        
        \return a string containing the XML write method that should be used for this data member. If this data
                member is not a real type then an empty string is returned.
        """
        
        text = None
        if self._type == "text":
            text = "XML::write_text"
        elif self._type == "binary":
            text = "XML::write_binary"
        elif self._type == "int32":
            text = "XML::write_int32"
        elif self._type == "uint32": 
            text = "XML::write_uint32"
        elif self._type == "uint64": 
            text = "XML::write_uint64"
        elif self._type == "uchar":
            text = "XML::write_uchar"
        elif self._type == "float":
            text = "XML::write_float"
        elif self._type == "bool":
            text = "XML::write_bool"
        elif self._isValueSet:
            text = "XML::write_int32"

        return text
    
    def getConstructorArgument(self, language):
        """
        Retrieves a class constructor argument for this data member.
        
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        """

        text = ""

        if language.lower() == "c++":
            realType = self.getRealType(language)
            
            if self._type == "text" or self._type == "binary":
                text += "const " + realType + "& " + self._name
            elif self._type == "int32" or \
                 self._type == "uint32" or \
                 self._type == "uint64" or \
                 self._type == "uchar" or \
                 self._type == "float" or \
                 self._type == "bool":
                text += "const " + realType
                if self._isArray:
                    text += "*"
                text += " " + self._name
            else:
                text += self._type + " " + self._name
        
        elif language.lower() == "java":
            if self.isRealType():
                text += self.getRealType(language) + " " + self._name
            else:
                text += self._type + " " + self._name
            if self._isArray:
                text += "[]"
        

            
        else:
            print "Unsupported language: " + language
    
        return text
    
    def isAttribute(self):
        """
        Determines if this data member is a class attribute (a non-vector, real or value set type).
        
        \return true if this data member is not a vector and it a real or value set type, false otherwise.
        """
        
        if not self._isVector:
            if self._isValueSet or \
               self._type == "text" or \
               self._type == "binary" or \
               self._type == "int32" or \
               self._type == "uint32" or \
               self._type == "uint64" or \
               self._type == "uchar" or \
               self._type == "float" or \
               self._type == "bool":
                return True
            else:
                return False
        else:
            return False
        
    def isSubEntity(self):
        """
        Determines if this data member is a sub-entity (a vector or a non-real or non-value set type).
        
        \return true if this data member is a vector or is a non-real and non-value set type, false otherwise.
        """
        
        return not self.isAttribute()

    def getSetAttributeText(self, indent, fullyQualifiedName, language):
        """
        Retrieves a code statement that sets an attribute for the setAttribute() method.
        
        \param indent
                 the spaces to indent the declaration.
        \param fullyQualifiedName
                 the fully qualified name of the parent class.
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        """
        
        text = ""

        if language.lower() == "c++":
            if self.isAttribute():
                if self._isValueSet:
                    text += indent + "  _" + self._name + " = (" + self._type + ")" + self.getXmlReadMethod() + "(\"" + \
                            fullyQualifiedName + "\", \"" + self._name + "\", attrs[i], attrs[i+1], &i, " + \
                            str(int(self._isOptional)) + ");\n"
                else:
                    text += indent + "  _" + self._name + " = " + self.getXmlReadMethod() + "(\"" + \
                            fullyQualifiedName + "\", \"" + self._name + "\", attrs[i], attrs[i+1], &i, " + \
                            str(int(self._isOptional)) + ");\n"

        elif language.lower() == "java":
            if self.isAttribute():
                text += indent + "if (qualifiedName.equals(\"" + self._name + "\"))\n"
                text += indent + "{\n"
                
                if self._isValueSet:
                    text += indent + "  _" + self._name + " = " + self._type + ".translate(Integer.decode(value));\n"
                    text += indent + "  i += 2;\n"
                    
                else:
                    if self._type == "text":
                        text += indent + "  _" + self._name + " = value;\n"
                    elif (self._type == "binary"):
                        text += indent + "  _" + self._name + " = Base64.decode(value);\n"
                    elif (self._type == "int32"):
                        text += indent + "  _" + self._name + " = Integer.decode(value);\n"
                    elif (self._type == "uint32"):
                        text += indent + "  _" + self._name + " = Long.decode(value);\n"
                    elif (self._type == "uint64"):
                        text += indent + "  _" + self._name + " = new BigInteger(value);\n"
                    elif (self._type == "uchar"):
                        text += indent + "  _" + self._name + " = Short.decode(value);\n"
                    elif (self._type == "float"):
                        text += indent + "  _" + self._name + " = new BigDecimal(value);\n"
                    elif (self._type == "bool"):
                        text += indent + "  _" + self._name + " = Boolean.parseBoolean(value);\n"
                            
                text += indent + "}\n"

        return text

    def getDecl(self, indent, language):
        """
        Retrieves a declaration for this data member. The declaration comes in several parts so code
        formatting can be applied more easily.
        
        \param indent
                 the spaces to indent the declaration.
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        \return a 4-tuple containing the comment, type declaration, variable name, and initialization code.
                The comment and type declaration include indentation spacing. The initialization code does
                not include the assignment operator or any end-of-statement character (typically a semicolon).
        """
        
        comment = None
        typeText = ""
        varText = ""
        initText = ""
        
        if language.lower() == "c++":
            if (self._comment != None):
                comment = indent + "/*! " + self._comment + " */"
                    
            if self._isVector:
                typeText = indent + "std::vector<" + self.getRealType("c++") + ">"
            else:
                typeText = indent + self.getRealType("c++")
    
            varText = "_" + self._name
            if self._isArray:
                varText += "[" + self._size + "]"
            initText = ""
        
        elif language.lower() == "java":
            if (self._comment != None):
                comment = indent + "/** " + self._comment + " */"
            
            if self._isVector:
                typeText += indent + "public ArrayList<" + self.getRealType("java") + ">"
            else:
                typeText += indent + "public " + self.getRealType("java")
                if self._isArray:
                    typeText += "[]"
    
            varText += "_" + self._name
            initText = "null"
            
        else:
            print "Unsupported language: " + language

        return (comment, typeText, varText, initText)
            

