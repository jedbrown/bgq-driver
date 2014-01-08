
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
class XmlClass:
    """
    An XmlClass object.
    """

    def __init__(self, name, attrs, version, protocolType):
        """
        The constructor.
        
        \param name
                 the name of the class.
        \param attrs
                 the XML attributes for the class.
        \version
                 the version of the API.
        """
        
        self._typeName = name
        self._protocolType = protocolType
        self._name = attrs['name']
        self._parent = "XML::Serializable"
        self._parentObject = None
        self._superclass = False
        self._comment = None
        self._version = version

        if 'parent' in attrs.keys():
            self._parent = attrs['parent']
        if 'superclass' in attrs.keys():
            self._superclass = bool(attrs['superclass'].upper() == 'TRUE')
        if 'comment' in attrs.keys():
            self._comment = attrs['comment'].capitalize()
            if self._comment[-1] != '.':
                self._comment += '.'
        
        # A list of the child classes.
        self._classes = []
        
        # A list of the child value sets.
        self._valueSets = []
        
        # A list of the data members.
        self._dataMembers = []

        # A list of the additional member functions.
        self._memberFunctions = []

    def addClass(self, classObject):
        """
        Adds an XmlClass object to the list of subordinate classes for the class.
        
        \param classObject
                 the XmlClass object to add.
        """

        self._classes.append(classObject)

    def addValueSet(self, valueSet):
        """
        Adds an XmlValueSet object to the list of subordinate value sets for the class.
        
        \param valueSet
                 the XmlValueSet object to add.
        """

        self._valueSets.append(valueSet)
        
    def addDataMember(self, dataMember):
        """
        Adds an XmlDataMember object to the list of data members for the class.
        
        \param dataMember
                 the XmlDataMember object to add.
        """

        self._dataMembers.append(dataMember)
        
        
    def addMemberFunction(self, function):
        """
        Adds an XmlDataMember object to the list of data members for the class.
        
        \param dataMember
                 the XmlDataMember object to add.
        """

        self._memberFunctions.append(function)
        
        
    def getDefaultConstructor(self, indent, fullyQualifiedName, language):
        """
        Generates a default constructor.
        
        \param indent
                 a string of spaces to prepend to each line of text generated.
        \param fullyQualifiedName
                 the fully qualified name of the class.
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        \return a string containing the code for the method. It is terminated with a newline.
        """
        
        text = ""
        
        if language.lower() == "c++":
            assignmentList = []
            text += indent + "/*!\n"
            text += indent + " * Constructs a " + self._name + " object.\n"
            text += indent + " *\n"
            text += indent + " */\n"
            text += indent + fullyQualifiedName + "::" + self._name + "()\n"
            if self._parent != None:
                if self._parent != "XML::Serializable":
                    assignmentList.append(self._parent + "()")
                
            for d in self._dataMembers:
                if d._isVector:
                    assignmentList.append("_" + d._name + "()")
                elif d._type == "text":
                    assignmentList.append("_" + d._name + "(\"\")")
                elif d._type == "binary":
                    assignmentList.append("_" + d._name + "(\"\")")
            if len(assignmentList) > 0:
                text += indent + "    : " + ", ".join(assignmentList) + "\n"
            
            text += indent + "{}\n"
            
        elif language.lower() == "java":
            text += indent + "/**\n"
            text += indent + " * Constructs a " + self._name + " object.\n"
            text += indent + " *\n"
            text += indent + " */\n"
            text += indent + "public " + self._name + "()\n"
            text += indent + "{"
            
            impl = ""
            
            for d in self._dataMembers:
                if d._isVector == False:
                    subscript = ""
                    if d._isArray:
                        subscript = "[i]"
                        impl += indent + "  _" + d._name + " = new " + d.getRealType(language) + "[" + d._size + "];\n"
                        impl += indent + "  for( int i = 0; i < " + d._size + "; i++ ) {\n"
                    if (d._type == "int32" or d._type == "uchar"):
                        impl += indent + "  _" + d._name + subscript + " = 0;\n"
                    elif (d._type == "uint32"):
                        impl += indent + "  _" + d._name + subscript + " = new Long(0);\n"            
                    elif (d._type == "uint64"):
                        impl += indent + "  _" + d._name + subscript + " = BigInteger.valueOf(0);\n"
                    elif (d._type == "float"):
                        impl += indent + "  _" + d._name + subscript + " = BigDecimal.valueOf(0);\n"
                    elif (d._type == "bool"):
                        impl += indent + "  _" + d._name + subscript + " = false;\n"
                    elif (d._type == "text"):
                        impl += indent + "  _" + d._name + subscript + " = new String();\n"
                    elif (d._type == "binary"):
                        impl += indent + "  _" + d._name + subscript + " = null;\n"
                    elif (d._isValueSet):
                        impl += indent + "  _" + d._name + subscript + " = null;\n"
                    else:
                        impl += indent + "  _" + d._name + subscript + " = new " + d._type + "();\n"
                    if d._isArray:
                        impl += indent + "  }\n"

            for d in self._dataMembers:
                 if d._isVector:
                     impl += indent + "  _" + d._name + " = new ArrayList<" + d.getRealType(language) + ">();\n"
            
            if impl != "":
                text += "\n" + impl
                text += indent + "}\n"
            else:
                text += "}\n"
            
        else:
            print "Unsupported language: " + language

        return text

    def getAttributeConstructor(self, indent, fullyQualifiedName, language):
        """
        Generates a constructor that initializes all attributes.
        
        \param indent
                 a string of spaces to prepend to each line of text generated.
        \param fullyQualifiedName
                 the fully qualified name of the class.
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        \return a string containing the code for the method. It is terminated with a newline.
        """
        
        text = ""
        
        if language.lower() == "c++":
            
            (attList, parentList, doxygenList) = self.getAttributeList(indent, language);

            if len(attList) > 0:
                assignmentList = []
                body = ""

                # Check to see if we need to also have a superclass constructor call for initialization.
                # If so, add it to the beginning of the assignment list.
                if len(parentList) > 0:
                    inheritedMemberList = []
                    for pAtt in parentList:
                        member = pAtt[pAtt.rindex(" ") + 1:]
                        inheritedMemberList.append(member)
                    assignmentList.append(self._parent + "(" + ", ".join(inheritedMemberList) + ")")

                # Populate the assignment list.
                for d in self._dataMembers:
                    if d._isVector:
                        assignmentList.append("_" + d._name + "()")
                    elif d._isArray:
                        body += indent + "  for (uint u=0; u < " + d._size + "; ++u)\n"
                        body += indent + "    _" + d._name + "[u] = " + d._name + "[u];\n"
                    else:
                        assignmentList.append("_" + d._name + "(" + d._name + ")")
            
                text += indent + "/*!\n"
                text += indent + " * Constructs a " + self._name + " object. The copy constructors of all attributes are\n"
                text += indent + " * used to initialize the class attributes.\n"
                text += indent + " *\n"
                text += "".join(doxygenList)
                text += indent + " */\n"
                text += indent + fullyQualifiedName + "::" + self._name + "(" + ", ".join(attList) + ")\n"
                if len(assignmentList) > 0:
                    text += indent + "    : " + ", ".join(assignmentList) + "\n"
                text += indent + "{\n"
                text += body
                text += indent + "}\n"
            
        elif language.lower() == "java":
            
            (attList, parentList, doxygenList) = self.getAttributeList(indent, language);
            
            if len(attList) > 0:
                text += indent + "/**\n"
                text += indent + " * Constructs a " + self._name + " object. All attributes are initialized.\n"
                text += indent + " *\n"
                text += "".join(doxygenList)
                text += indent + " */\n"
                text += indent + "public " + self._name + "(" + ", ".join(attList) + ")\n"
                text += indent + "{\n"
                
                if len(parentList) > 0:
                    inheritedMemberList = []
                    
                    for pAtt in parentList:
                        member = pAtt[pAtt.rindex(" ") + 1:]
                        inheritedMemberList.append(member)

                    text += indent + "  super(" + ", ".join(inheritedMemberList) + ");\n"
                    text += "\n"
                    
                for d in self._dataMembers:
                    if d._isVector:
                        text += indent + "  _" + d._name + " = new ArrayList<" + d.getRealType(language) + ">();\n"
                    else:
                        text += indent + "  _" + d._name + " = " + d._name + ";\n"
                
                text += indent + "}\n"
            
        else:
            print "Unsupported language: " + language

        return text
    
    def getCopyConstructor(self, indent, fullyQualifiedName, language):
        """
        Generates a copy constructor
        
        \param indent
                 a string of spaces to prepend to each line of text generated.
        \param fullyQualifiedName
                 the fully qualified name of the class.
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        \return a string containing the code for the method. It is terminated with a newline.
        """
        
        text = ""
        
        if language.lower() == "c++":
            
            text += indent + "/*!\n"
            text += indent + " * Constructs a " + self._name + " object. The copy constructors of all attributes are\n"
            text += indent + " * used to initialize the class attributes.\n"
            text += indent + " */\n"
            text += indent + fullyQualifiedName + "::" + self._name + "( const " + self._name + "& orig)\n"
            text += indent + "{ // copy constructor\n"
            text += indent + "  *this = orig;\n"
            text += indent + "} // copy constructor\n"
            
        else:
            print "Unsupported language: " + language
            
        return text

    def getAssignmentOperator(self, indent, fullyQualifiedName, language):
        """
        Generates an assignment operator
        
        \param indent
                 a string of spaces to prepend to each line of text generated.
        \param fullyQualifiedName
                 the fully qualified name of the class.
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        \return a string containing the code for the method. It is terminated with a newline.
        """
        
        text = ""
        
        if language.lower() == "c++":
            
            (attList, parentList, doxygenList) = self.getAttributeList(indent, language);

            if len(attList) > 0:
                assignmentList = []

            
                text += indent + "/*!\n"
                text += indent + " * Assignment operator.\n"
                text += indent + " */\n"
                text += indent + self._name + "& operator=(const " + self._name + "& orig)\n"                   
                text += indent + "{\n"
                # Check to see if we need to also have a superclass constructor call for initialization.
                # If so, add it to the beginning of the assignment list.
                if len(parentList) > 0:
                    inheritedMemberList = []
                    for pAtt in parentList:
                        member = pAtt[pAtt.rindex(" ") + 1:]
                        text += indent + "  _" + member + " = orig._" + member + ";\n"
 
                # Copy every data member.
                for d in self._dataMembers:
                    if d._isArray:
                        text += indent + "  for (uint u=0; u < " + d._size + "; ++u)\n"
                        text += indent + "    _" + d._name + "[u] = orig._" + d._name + "[u];\n"
                    else:
                        text += indent + "  _" + d._name + " = orig._" + d._name + ";\n"
            
                text += indent + "  return *this;\n"
                text += indent + "}\n"
            
        else:
            print "Unsupported language: " + language

        return text
    
    def getDestructor(self, indent, fullyQualifiedName):
        """
        Generates a default C++ destructor.
        
        \param indent
                 a string of spaces to prepend to each line of text generated.
        \param fullyQualifiedName
                 the fully qualified name of the class.
        \return a string containing the code for the method. It is terminated with a newline.
        """
        
        text  = indent + "/*!\n"
        text += indent + " * Default destructor.\n"
        text += indent + " *\n"
        text += indent + " */\n"
        text += indent + fullyQualifiedName + "::~" + self._name + "()\n"
        text += indent + "{\n"
        for d in self._dataMembers:
            if d._isVector:
                text += indent + "  _" + d._name + ".clear();\n"
        text += indent + "}\n"
        
        return text
    
    def getReset(self, indent, fullyQualifiedName, language):
        """
        Generates a reset function.
        
        \param indent
                 a string of spaces to prepend to each line of text generated.
        \param fullyQualifiedName
                 the fully qualified name of the class.
        \return a string containing the code for the method. It is terminated with a newline.
        """
        
        text = ""
        
        if language.lower() == "c++":
            text  = indent + "/*!\n"
            text += indent + " * Reset all member values.\n"
            text += indent + " *\n"
            text += indent + " */\n"
            text += indent + " void " + fullyQualifiedName + "::reset()\n"
            text += indent + "{ // reset method\n"
            
            for d in self._dataMembers:
                memberClear = ""
                if d._isVector:
                    memberClear += indent + "  _" + d._name + ".clear();\n"
                else:
                    if d._isArray:
                        memberClear += indent + "  for (uint u=0; u < " + d._size + "; ++u)\n  "
                    memberClear += indent + "  _" + d._name
                    if d._isArray:
                        memberClear += "[u]"
                    if d._type == "text" or d._type == "binary":
                        memberClear += " = \"\";\n"
                    elif d._type == "int32" or \
                         d._type == "uint32" or \
                         d._type == "uint64" or \
                         d._type == "uchar" or \
                         d._type == "float":
                        memberClear += " = 0;\n"
                    elif d._type == "bool":
                        memberClear += " = false;\n"
                    else:
                        # This is an unknown type, need helper method to reset
                        memberClear = indent + "  resethelpers::reset( _" + d._name + " );\n"
                text += memberClear

            text += indent + "} // reset method\n"
        
        else:
            print "Unsupported language: " + language

        return text
    
    def getGetClassName(self, indent, fullyQualifiedName, language):
        """
        Generates a method that returns the class name.
        
        \param indent
                 a string of spaces to prepend to each line of text generated.
        \param fullyQualifiedName
                 the fully qualified name of the class.
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        \return a string containing the code for the method. It is terminated with a newline.
        """
        
        text = ""
        
        if language.lower() == "c++":
            text += indent + "/*!\n"
            text += indent + " * Retrieves the class name. This is used for XML tag matching in client-server programs.\n"
            text += indent + " *\n"
            text += indent + " * @return a string containing the name of the class.\n"
            text += indent + " */\n"
            text += indent + "std::string " + fullyQualifiedName + "::getClassName()\n"
            text += indent + "{\n"
            text += indent + "  return \"" + fullyQualifiedName + "\";\n"
            text += indent + "}\n"
        
        elif language.lower() == "java":
            text += indent + "/**\n"
            text += indent + " * Retrieves the class name. This is used for XML tag matching in client-server programs.\n"
            text += indent + " *\n"
            text += indent + " * @return a string containing the name of the class.\n"
            text += indent + " */\n"
            text += indent + "public String getClassName()\n"
            text += indent + "{\n"
            text += indent + "  return \"" + fullyQualifiedName + "\";\n"
            text += indent + "}\n"

        else:
            print "Unsupported language: " + language

        return text
    
    def getSetAttributes(self, indent, fullyQualifiedName, language):
        """
        Generates a method that sets the attributes of the object.
        
        \param indent
                 a string of spaces to prepend to each line of text generated.
        \param fullyQualifiedName
                 the fully qualified name of the class.
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        \return a string containing the code for the method. It is terminated with a newline.
        """
        
        text = ""
        
        if language.lower() == "c++":
            text += indent + "/*!\n"
            text += indent + " * Set the XML class attributes.\n"
            text += indent + " *\n"
            text += indent + " * @param attrs\n"
            text += indent + " *         the XML class attributes to set.\n"
            text += indent + " * @return the index of the read data.\n"
            text += indent + " * @throws XML::Exception\n"
            text += indent + " *          if there was a problem setting the new XML attributes.\n"
            text += indent + " */\n"
            text += indent + "unsigned " + fullyQualifiedName + "::setAttributes(const char **attrs) throw(XML::Exception)\n"
            text += indent + "{\n"
            text += indent + "  unsigned i = 0;\n"
            # need to set parent class attributes, if any
            if self._parent != None:
                if self._parent != "XML::Serializable":
                    text += indent + "  i = " + self._parent + "::setAttributes(attrs);\n"
                
            for d in self._dataMembers:
                text += d.getSetAttributeText(indent, fullyQualifiedName, language)
            
            # If this is a superclass, there could be more attributes in the subclass
            if self._superclass == False:        
                text += indent + "  XML::verify_last_attr(\"" + self._name + "\", attrs[i]);\n"
                
            text += indent + "  return i;\n"
            text += indent + "}\n"
        
        elif language.lower() == "java":
            text += indent + "/**\n"
            text += indent + " * Set the XML class attributes.\n"
            text += indent + " *\n"
            text += indent + " * @param attrs\n"
            text += indent + " *         the XML class attributes to set.\n"
            text += indent + " * @return the index of the read data.\n"
            text += indent + " */\n"
            text += indent + "public int setAttributes(Attributes attrs)\n"
            text += indent + "{\n"

            if (self._parent != None) and (self._parent != "XMLSerializable"):
                impl  = indent + "  int numSuperAttrs = super.setAttributes(attrs);\n"
            else:
                impl  = indent + "  int numSuperAttrs = 0;\n"
            impl += indent + "  for (int i = numSuperAttrs; i < attrs.getLength(); i++)\n"
            impl += indent + "  {\n"
            impl += indent + "    String qualifiedName = attrs.getQName(i);\n"
            impl += indent + "    String value = attrs.getValue(i);\n"
            impl += "\n"
            
            printElse = False
            numDataMembersToIncrementBy = 0
            for d in self._dataMembers:
                saText = d.getSetAttributeText(indent + "    ", fullyQualifiedName, language)
                
                if saText != "":
                    if printElse:
                        saText = saText.replace("if", "else if", 1)
                    else:
                        printElse = True
                    
                    impl += saText
                    numDataMembersToIncrementBy += 1

            impl += indent + "  }\n"
            impl += "\n"
            impl += indent + "  return (numSuperAttrs + " + str(numDataMembersToIncrementBy) + ");\n"
            
            if printElse:
                text += impl
            else:
                if (self._parent != None) and (self._parent != "XMLSerializable"):
                    text += indent + "  return super.setAttributes(attrs);\n"
                else:
                    text += indent + "  return 0;\n"
                
            text += indent + "}\n"
        
        else:
            print "Unsupported language: " + language

        return text
    
    def getAddSubEntity(self, indent, fullyQualifiedName, language):
        """
        Generates a method that adds a subentity to the object.
        
        \param indent
                 a string of spaces to prepend to each line of text generated.
        \param fullyQualifiedName
                 the fully qualified name of the class.
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        \return a string containing the code for the method. It is terminated with a newline.
        """
        
        text = ""
        
        if language.lower() == "c++":
            text += indent + "/*!\n"
            text += indent + " * Adds an XML subentity.\n"
            text += indent + " *\n"
            text += indent + " * @param name\n"
            text += indent + " *          the name for the XML subentity to add.\n"
            text += indent + " * @param attrs\n"
            text += indent + " *          the XML class attributes to add for the subentity.\n"
            text += indent + " * @return a pointer to the serializable object.\n"
            text += indent + " * @throws XML::Exception\n"
            text += indent + " *           if there was a problem adding the new XML subentity.\n"
            text += indent + " */\n"
            text += indent + "XML::Serializable *" + fullyQualifiedName + "::addSubEntity(const char *name, const char **attrs) throw(XML::Exception)\n"
            text += indent + "{\n"
            
            # Need to set parent class attributes, if any.
            if self._parent != None:
                if self._parent != "XML::Serializable":
                    text += indent + "  try\n"
                    text += indent + "  {\n"
                    text += indent + "    return " + self._parent + "::addSubEntity(name, attrs);\n"
                    text += indent + "  }\n"
                    text += indent + "  catch(...)\n"
                    text += indent + "  {\n"
                    text += indent + "    // If this subentity is not from the base class then catch the exception and ignore it.\n"
                    text += indent + "  }\n"
                    text += "\n"
    
            subEntityAdded = False
            for d in self._dataMembers:
                if d.isSubEntity():
                    if d._isVector:
                        if d.isRealType():
                            text += indent + "  " + ("else " * subEntityAdded) + "if(strcmp(name, \"" + d._name +"\") == 0)\n"
                            text += indent + "  {\n"
                            text += indent + "    unsigned i = 0;\n"
                            text += indent + "    " + d.getRealType("c++") + " temp = " + d.getXmlReadMethod() + \
                                    "(\"" + d._name + "\", \"value\", attrs[i], attrs[i+1], &i, " + \
                                    str(int(d._isOptional)) + ");\n" 
                            text += indent + "    _" + d._name + ".push_back(temp);\n"
                            text += indent + "    XML::verify_last_attr(\"" + d._name + "\", attrs[i]);\n"
                            text += indent + "    return this;\n"
                            text += indent + "  }\n"
    
                        elif d._isValueSet:
                            text += indent + "  " + ("else " * subEntityAdded) + "if(strcmp(name, \"" + d._name +"\") == 0)\n"
                            text += indent + "  {\n"
                            text += indent + "    unsigned i = 0;\n"
                            text += indent + "    " + d.getRealType(language) + " temp = (" + d._type + \
                                    ")XML::read_int32(\"" + d._type + "\", \"temp\", attrs[i], attrs[i+1], &i, " + \
                                    str(int(d._isOptional)) + ");\n" 
                            text += indent + "    _" + d._name + ".push_back(temp);\n"
                            text += indent + "    XML::verify_last_attr(\"" + d._name + "\", attrs[i]);\n"
                            text += indent + "    return this;\n"
                            text += indent + "  }\n"
                            
                        else:
                            text += indent + "  " + ("else " * subEntityAdded) + "if(strcmp(name, \"" + d._type +"\") == 0)\n"
                            text += indent + "  {\n"
                            text += indent + "    " + d._type + " temp;\n"
                            text += indent + "    temp.setAttributes(attrs);\n"
                            text += indent + "    _" + d._name + ".push_back(temp);\n"
                            text += indent + "    return &_" + d._name + ".back();\n"
                            text += indent + "  }\n"
                            
                    else:
                        text += indent + "  " + ("else " * subEntityAdded) + "if(strcmp(name, \"" + d._type +"\") == 0)\n"
                        text += indent + "  {\n"
                        text += indent + "    _" + d._name + ".setAttributes(attrs);\n"
                        text += indent + "    return &_" + d._name +";\n"
                        text += indent + "  }\n"
                        
                    subEntityAdded = True
    
            if subEntityAdded:
                text += indent + "  else\n"
                text += indent + "  {\n"
                text += indent + "    // Could not find a subentity by the given name for this class. Throw an exception.\n"
                text += indent + "    XMLLIB_THROW(\"XML %s: invalid subentity %s\", \"" + self._name + "\", name);\n"
                text += indent + "  }\n"
                text += indent + "}\n"
                
            else:
                text += indent + "  // There are no subentities for this class. Throw an exception if this is called.\n"
                text += indent + "  XMLLIB_THROW(\"XML %s: invalid subentity %s\", \"" + self._name + "\", name);\n"
                text += indent + "}\n"
        
        elif language.lower() == "java":
            text += indent + "/**\n"
            text += indent + " * Adds an XML subentity.\n"
            text += indent + " *\n"
            text += indent + " * @param name\n"
            text += indent + " *          the name for the XML subentity to add.\n"
            text += indent + " * @param attrs\n"
            text += indent + " *          the XML class attributes to add for the subentity.\n"
            text += indent + " * @return a pointer to the serializable object that was added as the subentity.\n"
            text += indent + " */\n"
            text += indent + "public XMLSerializable addSubEntity(String name, Attributes attrs)\n"
            text += indent + "{\n"

            
            if (self._parent != None) and(self._parent != "XMLSerializable"):
                text += indent + "  XMLSerializable xmlObj = super.addSubEntity(name, attrs);\n"
                text += indent + "\n"
                text += indent + "  if(xmlObj == null)\n"
                text += indent + "  {\n"
                indent2 = "  "
            else:
                text += indent + "  XMLSerializable xmlObj = null;\n"
                text += "\n"
                indent2 = ""
            
            printElse = False
            
            for d in self._dataMembers:
                if d.isSubEntity():
                    if d._isVector:
                        if d.isRealType() or d._isValueSet:
                            text += indent + indent2 + "  " + ("else " * printElse) + "if(name.equals(\"" + d._name +"\"))\n"
                            text += indent + indent2 + "  {\n"
                            
                            if d._type == "text":
                                text += indent + indent2 + "    _" + d._name + ".add(attrs.getValue(0));\n"
                            elif d._type == "binary":
                                text += indent + indent2 + "    _" + d._name + ".add(Base64.decode(attrs.getValue(0)));\n"
                            elif d._type == "int32":
                                text += indent + indent2 + "    _" + d._name + ".add(Integer.decode(attrs.getValue(0)));\n"
                            elif d._type == "uint32":
                                text += indent + indent2 + "    _" + d._name + ".add(Long.decode(attrs.getValue(0)));\n"
                            elif d._type == "uint64":
                                text += indent + indent2 + "    _" + d._name + ".add(new BigInteger(attrs.getValue(0)));\n"
                            elif d._type == "uchar":
                                text += indent + indent2 + "    _" + d._name + ".add(Short.decode(attrs.getValue(0)));\n"
                            elif d._type == "float":
                                text += indent + indent2 + "    _" + d._name + ".add(new BigDecimal(attrs.getValue(0)));\n"
                            elif d._type == "bool":
                                text += indent + indent2 + "    _" + d._name + ".add(Boolean.parseBoolean(attrs.getValue(0)));\n"
                            elif d._isValueSet:
                                text += indent + indent2 + "    _" + d._name + ".add(" + d._type + ".translate(Integer.decode(attrs.getValue(0))));\n"
                            text += indent + indent2 + "    xmlObj = this;\n"
                            
                        else:
                            text += indent + indent2 + "  " + ("else " * printElse) + "if(name.equals(\"" + d._type +"\"))\n"
                            text += indent + indent2 + "  {\n"
                            text += indent + indent2 + "    xmlObj = new " + d._type + "();\n"
                            text += indent + indent2 + "    xmlObj.setAttributes(attrs);\n"
                            text += indent + indent2 + "    _" + d._name + ".add((" + d._type + ")xmlObj);\n"
                            
                    else:
                        text += indent + indent2 + "  " + ("else " * printElse) + "if(name.equals(\"" + d._type +"\"))\n"
                        text += indent + indent2 + "  {\n"
                        text += indent + indent2 + "    _" + d._name + ".setAttributes(attrs);\n"
                        text += indent + indent2 + "    xmlObj = _" + d._name +";\n"
                        
                    text += indent + indent2 + "  }\n"
                    printElse = True

            if (self._parent != None) and(self._parent != "XMLSerializable"):
                text += indent + "  }\n"
                
            text += "\n"
            text += indent + "  return xmlObj;\n"
            text += indent + "}\n"
            
        else:
            print "Unsupported language: " + language

        return text

    def getWriteMembers(self, indent, fullyQualifiedName, language):
        """
        Generates a method that writes members to an output stream.
        
        \param indent
                 a string of spaces to prepend to each line of text generated.
        \param fullyQualifiedName
                 the fully qualified name of the class.
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        \return a string containing the code for the method. It is terminated with a newline.
        """
        
        text = ""
        
        if language.lower() == "c++":
            text += indent + "/*!\n"
            text += indent + " * Writes the XML members to an output stream.\n"
            text += indent + " *\n"
            text += indent + " * @param os\n"
            text += indent + " *          the output stream to dump the XML to.\n"
            text += indent + " */\n"
            text += indent + "void " + fullyQualifiedName + "::writeMembers(std::ostream& os) const\n"
            text += indent + "{\n"

            for d in self._dataMembers:
                if d.isAttribute():
                    text += indent + "  " + d.getXmlWriteMethod() + "(os, \"" + fullyQualifiedName + \
                            "\", \"" + d._name + "\", _" + d._name + ");\n"

            text += indent + "}\n"
            
        elif language.lower() == "java":
            text += indent + "/**\n"
            text += indent + " * Generates an XML representation of the object's attributes.\n"
            text += indent + " *\n"
            text += indent + " * @return a StringBuilder object containing the XML representation.\n"
            text += indent + " *         A StringBuilder is used in lieu of a String for performance reasons.\n"
            text += indent + " */\n"
            text += indent + "public StringBuilder getAttributeString()\n"
            text += indent + "{\n"
            text += indent + "  StringBuilder command = new StringBuilder();\n"
            text += "\n"

            if self._parent != None:
                if self._parent != "XMLSerializable":
                    text += indent + "  command.append(super.getAttributeString());\n"
                    text += "\n"
                    
            for d in self._dataMembers:
                if d.isAttribute():
                    if d._type == "binary":
                        text += indent + "  command.append(\" " + d._name + "=\\\"\");\n"
                        text += indent + "  command.append(Base64.encode(_" + d._name + "));\n"
                        text += indent + "  command.append(\"\\\"\");\n"
                        text += "\n"
                    elif d._isValueSet:
                        text += indent + "  command.append(\" " + d._name + "=\\\"\");\n"
                        text += indent + "  command.append(_" + d._name + ".getValue());\n"
                        text += indent + "  command.append(\"\\\"\");\n"
                        text += "\n"
                    else:
                        text += indent + "  command.append(\" " + d._name + "=\\\"\");\n"
                        text += indent + "  command.append(_" + d._name + ");\n"
                        text += indent + "  command.append(\"\\\"\");\n"
                        text += "\n"
                    
            text += indent + "  return command;\n"
            text += indent + "}\n"
            
        else:
            print "Unsupported language: " + language

        return text

    def getWriteSubentities(self, indent, fullyQualifiedName, language):
        """
        Generates a method that writes subentities to an output stream.
        
        \param indent
                 a string of spaces to prepend to each line of text generated.
        \param fullyQualifiedName
                 the fully qualified name of the class.
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        \return a string containing the code for the method. It is terminated with a newline.
        """
        
        text = ""
        
        if language.lower() == "c++":
            text += indent + "/*!\n"
            text += indent + " * Writes the XML subentities to an output stream.\n"
            text += indent + " *\n"
            text += indent + " * @param os\n"
            text += indent + " *          the output stream to dump the XML to.\n"
            text += indent + " */\n"
            text += indent + "void " + fullyQualifiedName + "::writeSubentities(std::ostream& os) const\n"
            text += indent + "{\n"
            for d in self._dataMembers:
                if d.isSubEntity():
                    if d._isVector:
                        text += indent + "  for (std::vector<" + d.getRealType("c++") +">::const_iterator iter = _" + \
                                d._name + ".begin(); iter != _" + d._name + ".end(); ++iter)\n"
                        text += indent + "  {\n"
                        
                        if d.isRealType():
                            text += indent + "    os << " + " \"<" + d._name + "\";\n"
                            text += indent + "    " + d.getXmlWriteMethod() + "(os, \"" + d._name + \
                                    "\", \"value\", *iter);\n"
                            text += indent + "    os << \"/>\" << std::endl;\n"
                        else:
                            text += indent + "    os << *iter;\n"
                        
                        text += indent + "  }\n"
                        
                    else:
                        text += indent + "  os << _" + d._name + " << std::endl;\n"
            text += indent + "}\n"
            
            
        elif language.lower() == "java":
            text += indent + "/**\n"
            text += indent + " * Generates an XML representation of the object's subentities.\n"
            text += indent + " *\n"
            text += indent + " * @return a StringBuilder object containing the XML representation.\n"
            text += indent + " *         A StringBuilder is used in lieu of a String for performance reasons.\n"
            text += indent + " */\n"
            text += indent + "public StringBuilder getSubentitiesString()\n"
            text += indent + "{\n"
            text += indent + "  StringBuilder command = new StringBuilder();\n"
            text += "\n"

            if self._parent != None:
                if self._parent != "XMLSerializable":
                    text += indent + "  command.append(super.getSubentitiesString());\n"
                    text += "\n"
            
            for d in self._dataMembers:
                if d.isSubEntity():
                    if d._isVector:
                        text += indent + "  if(_" + d._name + " != null)\n"
                        text += indent + "  {\n"
                        text += indent + "    for(" + d.getRealType(language) + " temp : _" + d._name + ")\n"
                        text += indent + "    {\n"
                        if d.isRealType() or d._isValueSet:
                            text += indent + "      command.append(\"<" + d._name + " value=\\\"\");\n"
                            text += indent + "      command.append(temp);\n"
                            text += indent + "      command.append(\"\\\" />\");\n"
                        else:
                            text += indent + "      command.append(temp.getXMLString());\n"
                        text += indent + "    }\n"
                        text += indent + "  }\n"
                        text += "\n"
                        
                    else:
                        text += indent + "  command.append(_" + d._name + ".getXMLString());\n"
    
            text += indent + "  return command;\n"
            text += indent + "}\n"
            
        else:
            print "Unsupported language: " + language

        return text

    def getWrite(self, indent, fullyQualifiedName, language):
        """
        Generates a method that writes the object to an output stream.
        
        \param indent
                 a string of spaces to prepend to each line of text generated.
        \param fullyQualifiedName
                 the fully qualified name of the class.
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        \return a string containing the code for the method. It is terminated with a newline.
        """
        
        text = ""
        timerstats = "false"
        
        if language.lower() == "c++":
            text += indent + "/*!\n"
            text += indent + " * Writes the XML to the output stream.\n"
            text += indent + " *\n"
            text += indent + " * @param os\n"
            text += indent + " *          the output stream to dump the XML to.\n"
            text += indent + " */\n"
            text += indent + "void " + fullyQualifiedName + "::write(std::ostream& os) const\n"
            text += indent + "{\n"
            
            if timerstats == "true":
                text += indent + "    struct timespec starttime, endtime, temp;\n"
                text += indent + "    clock_gettime(CLOCK_MONOTONIC, &starttime);\n"
                
            text += indent + "  os << \"<" + self._name + "\";\n"
            
            # Need to write parent class members, if any.
            if self._parent != None:
                if self._parent != "XML::Serializable":
                    text += indent + "  " + self._parent + "::writeMembers(os);\n"

            text += indent + "  writeMembers(os);\n" 
            text += indent + "  os << \">\" << std::endl;\n"
            
            # Need to write parent class subentities, if any.
            if self._parent != None:
                if self._parent != "XML::Serializable":
                    text += indent + "  " + self._parent + "::writeSubentities(os);\n"
                
            text += indent + "  writeSubentities(os);\n" 
            text += indent + "  os << \"</" + self._name + ">\" << std::endl;\n"
            
            if timerstats == "true":
                text += indent + "    clock_gettime(CLOCK_MONOTONIC, &endtime);\n"
                text += indent + "    if ((endtime.tv_nsec - starttime.tv_nsec)<0) {\n"
                text += indent + "        temp.tv_sec = endtime.tv_sec-starttime.tv_sec-1;\n"
                text += indent + "        temp.tv_nsec = 1000000000+endtime.tv_nsec-starttime.tv_nsec;\n"
                text += indent + "    } else {\n"
                text += indent + "        temp.tv_sec = endtime.tv_sec-starttime.tv_sec;\n"
                text += indent + "        temp.tv_nsec = endtime.tv_nsec-starttime.tv_nsec;\n"
                text += indent + "    }\n"
                text += indent + "    std::ostringstream nsecs;\n"
                text += indent + "    nsecs << temp.tv_nsec;\n"
                text += indent + "    std::string nanos = \"\";\n"
                text += indent + "    size_t nine = 9;\n"
                text += indent + "    if(nsecs.str().length() < nine) {\n"
                text += indent + "        for(size_t c = 0; c < (nine - nsecs.str().length()); ++c) {\n"
                text += indent + "            nanos += \"0\";\n"
                text += indent + "        }\n"
                text += indent + "    }\n"
                text += indent + "    nanos += nsecs.str();\n"
                text += indent + "    std::cout << \"ser time=\" << temp.tv_sec << \".\" << nanos << std::endl;\n"
                
            text += indent + "}\n"
        
        elif language.lower() == "java":
            text += indent + "/**\n"
            text += indent + " * Generates an XML string for the object.\n"
            text += indent + " *\n"
            text += indent + " * @return a string containing an XML representation of the object.\n"
            text += indent + " */\n"
            text += indent + "public String getXMLString()\n"
            text += indent + "{\n"
            text += indent + "  StringBuilder command = new StringBuilder(\"<" + self._name + "\");\n"
            text += "\n"
            if self.needGetAttributesString():
                text += indent + "  command.append(getAttributeString());\n"
            text += indent + "  command.append(\">\");\n"
            if self.needGetSubentitiesString():
                text += indent + "  command.append(getSubentitiesString());\n"
            text += indent + "  command.append(\"</" + self._name + ">\");\n"
            text += "\n"
            text += indent + "  return command.toString();\n"
            text += indent + "}\n"
            text += "\n" 
            text += indent + "/**\n"
            text += indent + " * Writes the object's XML to the given print stream.\n"
            text += indent + " *\n"
            text += indent + " * @param ps\n"
            text += indent + " *          the print stream to dump the XML to.\n"
            text += indent + " */\n"
            text += indent + "public void write(PrintStream ps)\n"
            text += indent + "{\n"
            text += indent + "  try\n"
            text += indent + "  {\n"
            text += indent + "    if(sslConnection)\n"
            text += indent + "    {\n"
            text += indent + "      // Send class name length.\n"
            text += indent + "      String command = \"" + self._name + "\";\n"
            text += indent + "      int size = command.length();\n"
            text += indent + "      byte[] array = XMLSerializable.intToByteArray(size);\n"
            text += indent + "      ps.write(array);\n"
            text += "\n"
            text += indent + "      // Send class name.\n"
            text += indent + "      ps.print(command);\n"
            text += "\n"
            text += indent + "      // Send data length plus 1 byte for new line.\n"
            text += indent + "      command = getXMLString();\n"
            text += indent + "      size = command.length() + 1;\n"
            text += indent + "      array = XMLSerializable.intToByteArray(size);\n"
            text += indent + "      ps.write(array);\n"
            text += "\n"
            text += indent + "      // Send data.\n"
            text += indent + "      ps.println(command);\n"
            text += indent + "    }\n"
            text += indent + "    else\n"
            text += indent + "    {\n"
            text += indent + "      DataOutputStream dataOut = new DataOutputStream(ps);\n"
            text += indent + "      String command = \"" + self._name + " \" + getXMLString();\n"
            text += "\n"
            text += indent + "      // Send data length plus 1 byte for new line.\n"
            text += indent + "      dataOut.writeInt(command.length() + 1);\n"
            text += "\n"
            text += indent + "      // Send data.\n"
            text += indent + "      ps.println(command);\n"
            text += indent + "    }\n"
            text += indent + "  }\n"
            text += indent + "  catch (IOException e)\n"
            text += indent + "  {\n"
            text += indent + "    System.err.println(e);\n"
            text += indent + "    e.printStackTrace(System.err);\n"
            text += indent + "  }\n"
            text += indent + "}\n"
        
        else:
            print "Unsupported language: " + language

        return text

    def getSerialize(self, indent, fullyQualifiedName, language):
        """
        Generates a method for boost serialization.
        
        \param indent
                 a string of spaces to prepend to each line of text generated.
        \param fullyQualifiedName
                 the fully qualified name of the class.
        \param language
                 the language to generate the source for. Valid values are "C++" 
        \return a string containing the code for the method. It is terminated with a newline.
        """
        
        text = ""
        
        if language.lower() == "c++":
            text += indent + "template<class Archive>\n"
            text += indent + "void serialize(Archive& ar, const unsigned int version) {\n"

            if self._parent != "XML::Serializable":
                if self._protocolType == "boostXML":
                    text += indent + "  ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(" + self._parent + ");\n"
                if self._protocolType == "boostBinary" or self._protocolType == "boostText":
                    text += indent + "  ar & boost::serialization::base_object<" + self._parent + ">(*this);\n"

            for d in self._dataMembers:
                if self._protocolType == "boostXML":
                    if d._isArray:
                        text += indent + "  ar & boost::serialization::make_nvp(\"_" + d._name + "\",boost::serialization::make_binary_object(_" + d._name + "," + d._size + " * sizeof(" + d.getRealType("c++") + ")));\n"
                    else:
                        text += indent + "  ar & BOOST_SERIALIZATION_NVP(_" + d._name + ");\n"
                if self._protocolType == "boostBinary" or self._protocolType == "boostText":
                    if d._isArray:
                        text += indent + "  ar & boost::serialization::make_binary_object(_" + d._name + "," + d._size + " * sizeof(" + d.getRealType("c++") + "));\n"
                    else:
                        text += indent + "  ar & _" + d._name + ";\n"
    
            text += indent + "}\n"
            
        else:
            print "Unsupported language: " + language
       
        return text
     
    def getReadMessage(self, indent, fullyQualifiedName, language):
        """
        Generates a method for reading using boost serialization.
        
        \param indent
                 a string of spaces to prepend to each line of text generated.
        \param fullyQualifiedName
                 the fully qualified name of the class.
        \param language
                 the language to generate the source for. Valid values are "C++" 
        \return a string containing the code for the method. It is terminated with a newline.
        """
        
        text = ""
        
        if language.lower() == "c++":

            timerstats = "false"
            text += indent + "void " + fullyQualifiedName + "::readMessage(std::istream &is) {\n"
            if timerstats == "true":
                text += indent + "    struct timespec starttime, endtime;\n"
                text += indent + "    clock_gettime(CLOCK_MONOTONIC, &starttime);\n"
            if self._protocolType == "boostXML":
                text += indent + "    boost::archive::xml_iarchive ba(is);\n"
                text += indent + "    ba >> BOOST_SERIALIZATION_NVP(*this);\n"
            if self._protocolType == "boostBinary":
                text += indent + "    boost::archive::binary_iarchive ba(is);\n"
                text += indent + "    ba >> *this;\n"
            if self._protocolType == "boostText":
                text += indent + "    boost::archive::text_iarchive ba(is);\n"
                text += indent + "    ba >> *this;\n"
            if timerstats == "true":
                text += indent + "    clock_gettime(CLOCK_MONOTONIC, &endtime);\n"
                text += indent + "    double stime = (double)starttime.tv_sec + (double)starttime.tv_nsec * 1.0E-9f;\n"
                text += indent + "    double etime = (double)endtime.tv_sec + (double)endtime.tv_nsec * 1.0E-9f;\n"
                text += indent + "    std::cout << \"deser time=\" << etime - stime << std::endl;\n"
            text += indent + "    return;\n"
            text += indent + "}\n"
            
        elif language.lower() == "java":
            text += indent + "/**\n"
            text += indent + " * Reads the object's Boost text representation from the given stream.\n"
            text += indent + " *\n"
            text += indent + " * @param is\n"
            text += indent + " *          the input stream to read from.\n"
            text += indent + " */\n"
            text += indent + "public void readMessage(InputStream is)\n"
            text += indent + "throws java.io.IOException\n"
            text += indent + "{\n"
            text += indent + "  DataInputStream dis = new DataInputStream(is);\n"
            text += indent + "\n"
            text += indent + "  // Skip over some boost header info\n"
            text += indent + "  Scanner scanner = new Scanner(dis);\n"
            text += indent + "  int boostNum = scanner.nextInt();\n"
            text += indent + "  scanner.skip( \"serialization::archive\");\n"
            text += indent + "  int version = scanner.nextInt();\n\n"
            text += indent + "  readMembers(dis);\n"
            text += indent + "}\n"
            
            text += indent + "/**\n"
            text += indent + " * Reads the object's members from the given stream.\n"
            text += indent + " *\n"
            text += indent + " * @param dis\n"
            text += indent + " *          the input stream to read from.\n"
            text += indent + " */\n"
            text += indent + "public void readMembers(DataInputStream dis)\n"
            text += indent + "throws java.io.IOException\n"
            text += indent + "{\n"
            if (self._parent != None) and(self._parent != "XMLSerializable"):
                text += indent + "  super.readMembers(dis);\n"
                text += indent + "\n"
            
            text += indent + "  Scanner scanner = new Scanner(dis);\n"
            text += indent + "  Reader reader = new InputStreamReader(dis);\n"
            text += indent + "\n"
            
            for d in self._dataMembers:
                if d._isVector:
                    text += indent + "  { int vectorSize = scanner.nextInt();\n"
                    text += indent + "    for( int i = 0; i < vectorSize; i++ ) {\n"
                    
                    if d.isRealType() or d._isValueSet:
                        if d._type == "text":
                            text += indent + "    int length = scanner.nextInt();\n"
                            text += indent + "    char[] charBuff = new char[length];\n"
                            text += indent + "    reader.read( charBuff );\n"
                            text += indent + "    _" + d._name + ".add(new String(charBuff));\n"
                        elif d._type == "binary":
                            text += indent + "    int length = scanner.nextInt();\n"
                            text += indent + "    char[] charBuff = new char[length];\n"
                            text += indent + "    reader.read( charBuff );\n"
                            text += indent + "    _" + d._name + ".add(Base64.decode(new String(charBuff)));\n"
                        elif d._type == "uchar":
                            text += indent + "    _" + d._name + ".add(scanner.nextShort());\n"
                        elif d._type == "int32":
                            text += indent + "    _" + d._name + ".add(scanner.nextInt());\n"
                        elif d._type == "uint32":
                            text += indent + "    _" + d._name + ".add(scanner.nextLong());\n"
                        elif d._type == "uint64":
                            text += indent + "    _" + d._name + ".add(scanner.nextBigInteger());\n"
                        elif d._type == "float":
                            text += indent + "    _" + d._name + ".add(scanner.nextBigDecimal());\n"
                        elif d._type == "bool":
                            text += indent + "    _" + d._name + ".add(scanner.nextBoolean());\n"
                        elif d._isValueSet:
                            text += indent + "    _" + d._name + ".add(" + d._type + ".translate(scanner.nextInt()));\n"
                        
                    else: 
                        text += indent + "    " + d._type + " value = new " + d._type + "();\n"
                        text += indent + "    value.readMembers(dis);\n"
                        text += indent + "    _" + d._name + ".add(value);\n"
                        

                    text += indent + "    }\n"
                    text += indent + "  }\n"
                         
                else:
                    subscript = ""
                    if d._isArray:
                        subscript = "[i]"
                        text += indent + "for( int i = 0; i < " + d._size + "; i++ ) {\n"
                    if d._type == "text":
                        text += indent + "    { int length = scanner.nextInt();\n"
                        text += indent + "    char[] charBuff = new char[length];\n"
                        text += indent + "    reader.read( charBuff );\n"
                        text += indent + "    _" + d._name + subscript + " = new String(charBuff); }\n"
                    elif d._type == "binary":
                        text += indent + "    { int length = scanner.nextInt();\n"
                        text += indent + "    char[] charBuff = new char[length];\n"
                        text += indent + "    reader.read( charBuff );\n"
                        text += indent + "    _" + d._name + subscript + " = Base64.decode(new String(charBuff)); }\n"
                    elif d._type == "uchar":
                        text += indent + "    _" + d._name + subscript + " = scanner.nextShort();\n"
                    elif d._type == "int32":
                        text += indent + "    _" + d._name + subscript + " = scanner.nextInt();\n"
                    elif d._type == "uint32":
                        text += indent + "    _" + d._name + subscript + " = scanner.nextLong();\n"
                    elif d._type == "uint64":
                        text += indent + "    _" + d._name + subscript + " = scanner.nextBigInteger();\n"
                    elif d._type == "float":
                        text += indent + "    _" + d._name + subscript + " = scanner.nextBigDecimal();\n"
                    elif d._type == "bool":
                        text += indent + "    _" + d._name + subscript + " = scanner.nextBoolean();\n"
                    elif d._isValueSet:
                        text += indent + "    _" + d._name + subscript + " = " + d._type + ".translate(scanner.nextInt());\n"
                    else:
                        text += indent + "    _" + d._name + subscript + " = new " + d._type + "();\n"
                        text += indent + "    _" + d._name + subscript + ".readMembers(dis);\n"
                    if d._isArray:
                        text += indent + "    }\n"
                                                                       
            text += "\n"
            text += indent + "}\n"
            
        return text
        
    # write inline method for self-serialization
    def getSerializeMessage(self, indent, fullyQualifiedName, language):
        """
        Generates a method for boost serialization to xml stream.
        
        \param indent
                 a string of spaces to prepend to each line of text generated.
        \param fullyQualifiedName
                 the fully qualified name of the class.
        \param language
                 the language to generate the source for. Valid values are "C++" 
        \return a string containing the code for the method. It is terminated with a newline.
        """
        
        text = ""
        
        if language.lower() == "c++":
            timerstats = "false"
            text += indent + "void " + fullyQualifiedName + "::serializeMessage(std::ostream& os) const {\n"
            if timerstats == "true":
                text += indent + "    struct timespec starttime, endtime, temp;\n"
                text += indent + "    clock_gettime(CLOCK_MONOTONIC, &starttime);\n"
            # not sure why you have to use this for xml archive, but *this for binary
            if self._protocolType == "boostXML":
                text += indent + "    boost::archive::xml_oarchive oa(os);\n"
                text += indent + "    oa << BOOST_SERIALIZATION_NVP(this);\n"
            if self._protocolType == "boostBinary":
                text += indent + "    boost::archive::binary_oarchive oa(os);\n"
                text += indent + "    oa << *this;\n"
            if self._protocolType == "boostText":
                text += indent + "    boost::archive::text_oarchive oa(os);\n"
                text += indent + "    oa << *this;\n"
            if timerstats == "true":
                text += indent + "    clock_gettime(CLOCK_MONOTONIC, &endtime);\n"
                text += indent + "    double stime = (double)starttime.tv_sec + (double)starttime.tv_nsec * 1.0E-9f;\n"
                text += indent + "    double etime = (double)endtime.tv_sec + (double)endtime.tv_nsec * 1.0E-9f;\n"
                text += indent + "    std::cout << \"ser time=\" << etime - stime << std::endl;\n"
            text += indent + "    return;\n"
            text += indent + "}\n"

        elif language.lower() == "java":
            text += indent + "/**\n"
            text += indent + " * Generates a Boost text string for the object.\n"
            text += indent + " *\n"
            text += indent + " * @return a string containing an Boost text representation of the object.\n"
            text += indent + " */\n"
            text += indent + "public String getString()\n"
            text += indent + "{\n"
            text += indent + "  StringBuilder command = new StringBuilder(\"\");\n"
            text += "\n"
            if (self._parent != None) and (self._parent != "XMLSerializable"):
                text += indent + "  command.append( super.getString() );\n"
                text += "\n"
            for d in self._dataMembers:
                subscript = ""
                if d._isVector:
                    text += indent + "  command.append(_" + d._name + ".size() );\n"
                    text += indent + "  command.append(\" \");\n"
                    text += indent + "  for( int i = 0; i < _" + d._name + ".size(); i++){\n"
                    subscript = ".get(i)"
                if d._type == "binary":
                    text += indent + "  command.append(_" + d._name + subscript + ".length);\n"
                    text += indent + "  command.append(\" \");\n"
                    text += indent + "  command.append(Base64.encode(_" + d._name + subscript + "));\n"
                elif d._isValueSet:
                    text += indent + "  command.append(_" + d._name + subscript + ".getValue());\n"
                elif d._type == "text":
                    text += indent + "  command.append(_" + d._name + subscript + ".length());\n"
                    text += indent + "  command.append(\" \");\n"
                    text += indent + "  command.append(_" + d._name + subscript + ");\n" 
                elif d._isArray:
                    text += indent + "  command.append(_" + d._name + subscript + ".length);\n"
                    text += indent + "  command.append(\" \");\n"
                    text += indent + "  command.append(_" + d._name + subscript + ");\n" 
                else:
                    text += indent + "  command.append(_" + d._name + subscript + ");\n"
                text += indent + "  command.append(\" \");\n"
                if d._isVector:
                    text += indent + "  }\n"
                text += "\n"        
            text += "\n"
            text += indent + "  return command.toString();\n"
            text += indent + "}\n"
            text += "\n" 
            
            text += indent + "/**\n"
            text += indent + " * Writes the object's Boost text representation to the given print stream.\n"
            text += indent + " *\n"
            text += indent + " * @param ps\n"
            text += indent + " *          the print stream to dump the text to.\n"
            text += indent + " */\n"
            text += indent + "public void write(PrintStream ps)\n"
            text += indent + "{\n"
            text += indent + "  DataOutputStream dataOut = new DataOutputStream(ps);\n"
            text += indent + "  String command = \"22 serialization::archive 5  \" + getString();\n"
            text += "\n"
            text += indent + "  try\n"
            text += indent + "  {\n"
            text += indent + "    // Send data length plus 1 byte for new line.\n"
            text += indent + "    dataOut.writeInt(command.length() + 1);\n"
            text += indent + "  }\n"
            text += indent + "  catch (IOException e)\n"
            text += indent + "  {\n"
            text += indent + "    System.err.println(e);\n"
            text += indent + "    e.printStackTrace(System.err);\n"
            text += indent + "  }\n"
            text += "\n"
            text += indent + "  ps.println(command);\n"
            text += indent + "}\n"
        
			
        else:
            print "Unsupported language: " + language
       
        return text
     
    def needGetAttributesString(self):
        """
        Determines if there is a need to write and/or call a getAttributesString() routine.
        
        \return true if the class needs to call a getAttributesString() method, false otherwise.
        """
        
        needGetAttributes = False
        
        foundAttribute = False
        for d in self._dataMembers:
            if d.isAttribute():
                foundAttribute = True
                break
            
        if ((self._parent != None) and (self._parent != "XMLSerializable")) or \
           (foundAttribute):
        
            # Need a getAttributes() call if this class defines any attributes itself
            # or there is a parent classes and the parent class is not XMLSerializable.
            needGetAttributes = True
        else:
          
            needGetAttributes = False
            
        return needGetAttributes
        
    def needGetSubentitiesString(self):
        """
        Determines if there is a need to write and/or call a getSubentitiesString() routine.
        
        \return true if the class needs to call a getSubentitiesString() method, false otherwise.
        """
        
        needGetSubentities = False
        
        foundSubentity = False
        for d in self._dataMembers:
            if d.isSubEntity():
                foundSubentity = True
                break
            
        if ((self._parent != None) and (self._parent != "XMLSerializable")) or \
           (foundSubentity):
            
            # Need a getSubentities() call if this class has any subentities itself
            # or there is a parent class and the parent class is not XMLSerializable.
            needGetSubentities = True
        else:

            needGetSubentities = False
            
        return needGetSubentities
        
    def getAttributeList(self, indent, language):
        """
        Retrieve lists of text strings for class attribute declarations, parameters, and doxygen.
        
        \param indent
                 the spaces to indent the declaration.
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        \return a three-tuple of lists. The first element is the list of attributes. The second is the list
                of attributes that were inherited from the parent class. The third is the list of doxygen
                @param lines for use in doxygen function comments.
        """
        
        parentList = []
        doxygenList = []
        attList = []
        
        if self._parentObject != None:
            (parentList, ppList, doxygenList) = self._parentObject.getAttributeList(indent, language)

        for d in self._dataMembers:
            if not d._isVector:
                doxygenList.append(indent + "   * @param " + d._name + "\n")

                if d._comment != None:
                    doxygenList.append(indent + "   *          " + d._comment + "\n")
                else:
                    doxygenList.append(indent + "   *          No description found for this parameter.\n")
                
                attList.append(d.getConstructorArgument(language))
                

        return (parentList + attList, parentList, doxygenList)
    
    def getDecl(self, indent, parentQualName, language):
        """
        Generates a class declaration. For Java, this will also include the implementation source.
        
        \param indent
                 the spaces to indent the declaration.
        \param parentQualName
                 the fully qualified name of the parent classes. None if there are no parents.
        \param language
                 the language to generate the source for. Valid values are "C++" and "Java".
        \return a string containing the class declaration.
        """
        
        text = ""
        
        if language.lower() == "c++":
            fullyQualifiedName = self._name
            if parentQualName != None:
                fullyQualifiedName = parentQualName + "::" + fullyQualifiedName
            
            text += indent + "/*!\n"
            text += indent + " * Class " + fullyQualifiedName + ".\n"
            if self._comment:
                text += indent + " * " + self._comment + "\n"
            text += indent + " *\n"
            text += indent + " */\n"
            text += indent + "class " + self._name
            if self._parent != None:
                text += " : public " + self._parent
            text += "\n"
            text += indent + "{\n"    
            text += indent + "public:\n\n"
        
            for v in self._valueSets:
                text += v.getDecl(indent + "  ", fullyQualifiedName, language)
                text += "\n"
        
            for c in self._classes:
                text += c.getDecl(indent + "  ", fullyQualifiedName, language)
                text += "\n"
        
            text += indent + "  /*!\n"
            text += indent + "   * Default constructor.\n"
            text += indent + "   *\n"
            text += indent + "   */\n"
            text += indent + "  " + self._name + "();\n"

            (attList, parentList, doxygenList) = self.getAttributeList(indent, language);
            
            if len(attList) > 0:
                text += "\n"
                text += indent + "  /*!\n"
                text += indent + "   * Attribute constructor. The copy constructors of all attributes are used to initialize the class attributes.\n"
                text += indent + "   *\n"
                text += "".join(doxygenList)
                text += indent + "   */\n"
                text += indent + "  " + self._name + "(" + ", ".join(attList) + ");\n"
                text += "\n"
                
            for f in self._memberFunctions:
                if f == "copy":
                    text += indent + "  /*!\n"
                    text += indent + "   * Copy constructor.\n"
                    text += indent + "   */\n"  
                    text += indent + "  " + self._name + "(const " + self._name + "& orig);\n"                  
                    text += "\n"
                    text += self.getAssignmentOperator(indent + "  ", fullyQualifiedName, language)
                    text += "\n"
                if f == "reset":
                    text += indent + "  /*!\n"
                    text += indent + "   * Reset the fields.\n"
                    text += indent + "   */\n"  
                    text += indent + "  void reset();\n"                  
                    text += "\n"
                    
            text += indent + "  /*!\n"
            text += indent + "   * Default destructor.\n"
            text += indent + "   *\n"
            text += indent + "   */\n"
            text += indent + "  ~" + self._name + "();\n"
            text += "\n"
            text += indent + "  /*!\n"
            text += indent + "   * Retrieves the class name. This is used for XML tag matching in client-server programs.\n"
            text += indent + "   *\n"
            text += indent + "   * @return a string containing the name of the class.\n"
            text += indent + "   */\n"
            text += indent + "  static std::string getClassName();\n"
            text += "\n"
            
            if self._protocolType == "boostXML" or self._protocolType == "boostBinary" or self._protocolType == "boostText":
                text += indent + "  /*!\n"
                text += indent + "   * Boost serialization method.\n"
                text += indent + "   *\n"
                text += indent + "   * @param ar\n"
                text += indent + "   *          boost archive.\n"
                text += indent + "   * @param version\n"
                text += indent + "   *          version number.\n"
                text += indent + "   */\n"
                text += self.getSerialize(indent + "  ", fullyQualifiedName, "c++")
                text += "\n"
                text += indent + "  /*!\n"
                text += indent + "   * Writes the message to a stream.\n"
                text += indent + "   *\n"
                text += indent + "   * @param os\n"
                text += indent + "   *          the output stream to dump the message to.\n"
                text += indent + "   */\n"
                text += indent + "  virtual void serializeMessage(std::ostream& os) const;\n"
                text += "\n"
                text += indent + "  /*!\n"
                text += indent + "   * Reads the message from a stream.\n"
                text += indent + "   *\n"
                text += indent + "   * @param os\n"
                text += indent + "   *          the input stream to read the message from.\n"
                text += indent + "   */\n"
                text += indent + "  virtual void readMessage(std::istream &is);\n"
                text += "\n"
            else:
                text += indent + "  /*!\n"
                text += indent + "   * Set the XML class attributes.\n"
                text += indent + "   *\n"
                text += indent + "   * @param attrs\n"
                text += indent + "   *          the XML class attributes to set.\n"
                text += indent + "   * @return the index of the read data.\n"
                text += indent + "   * @throws XML::Exception\n"
                text += indent + "   *           if there was a problem setting the new XML attributes.\n"
                text += indent + "   */\n"
                text += indent + "  virtual unsigned setAttributes(const char **attrs) throw(XML::Exception);\n"
                text += "\n"
                text += indent + "  /*!\n"
                text += indent + "   * Adds an XML subentity.\n"
                text += indent + "   *\n"
                text += indent + "   * @param name\n"
                text += indent + "   *          the name for the XML subentity to add.\n"
                text += indent + "   * @param attrs\n"
                text += indent + "   *          the XML class attributes to add for the subentity.\n"
                text += indent + "   * @return a pointer to the serializable object.\n"
                text += indent + "   * @throws XML::Exception\n"
                text += indent + "   *           if there was a problem adding the new XML subentity.\n"
                text += indent + "   */\n"
                text += indent + "  XML::Serializable* addSubEntity(const char *name, const char **attrs) throw(XML::Exception);\n"
                text += "\n"
                text += indent + "  /*!\n"
                text += indent + "   * Writes the XML members to an output stream.\n"
                text += indent + "   *\n"
                text += indent + "   * @param os\n"
                text += indent + "   *          the output stream to dump the XML to.\n"
                text += indent + "   */\n"
                text += indent + "  virtual void writeMembers(std::ostream& os) const;\n"
                text += "\n"
                text += indent + "  /*!\n"
                text += indent + "   * Writes the XML subentities to an output stream.\n"
                text += indent + "   *\n"
                text += indent + "   * @param os\n"
                text += indent + "   *          the output stream to dump the XML to.\n"
                text += indent + "   */\n"
                text += indent + "  virtual void writeSubentities(std::ostream& os) const;\n"
                text += "\n"
                text += indent + "  /*!\n"
                text += indent + "   * Writes the XML to the output stream.\n"
                text += indent + "   *\n"
                text += indent + "   * @param os\n"
                text += indent + "   *          the output stream to dump the XML to.\n"
                text += indent + "   */\n"
                text += indent + "  virtual void write(std::ostream& os) const;\n"
            
            dList = []

            for d in self._dataMembers:
                dList.append(d.getDecl(indent + "  ", "c++"))

            longestType = 0
            longestVar = 0

            for (commentText, typeText, varText, initText) in dList:
                if len(typeText) > longestType:
                    longestType = len(typeText)

            for (commentText, typeText, varText, initText) in dList:
                text += "\n"
                if commentText != None:
                    text += commentText + "\n"
                text += typeText.ljust(longestType) + " " + varText + ";\n"
                
            text += indent + "};\n"

        elif language.lower() == "java":
            fullyQualifiedName = self._name
            if parentQualName != None:
                fullyQualifiedName = parentQualName + "." + fullyQualifiedName
            
            text += indent + "/**\n"
            text += indent + " * "
            
            if self._comment != None:
                text += self._comment
            else:
                text += "Class " + self._name + ".\n"
                
            text += indent + " *\n"
            text += indent + " */\n"
            text += indent + "public class " + self._name
            
            if self._parent != None:
                if self._parent == "XML::Serializable":
                    self._parent = "XMLSerializable"
                text += indent + " extends " + self._parent
            text += "\n"
            
            text += indent + "{\n"
            
            for v in self._valueSets:
                text += v.getDecl(indent + "  ", fullyQualifiedName, language)
                text += "\n"
        
            for c in self._classes:
                text += c.getDecl(indent + "  ", fullyQualifiedName, language)
                text += "\n"
            
            dList = []

            if self._version != None:
                dList.append((indent + "  /** The API version of this class implementation. */",
                              indent + "  public static final String",
                              "VERSION",
                              "\"" + self._version + "\""))

            for d in self._dataMembers:
                dList.append(d.getDecl(indent + "  ", "java"))

            longestType = 0
            longestVar = 0

            for (commentText, typeText, varText, initText) in dList:
                if len(typeText) > longestType:
                    longestType = len(typeText)
                    
                if len(varText) > longestVar:
                    longestVar = len(varText)
            
            for (commentText, typeText, varText, initText) in dList:
                if commentText != None:
                    text += commentText + "\n"
                else:
                    text += indent + "  /** The data member for the " + varText[1:].strip() + " XML. */\n"
                text += typeText.ljust(longestType) + " " + varText.ljust(longestVar) + " = " + initText + ";\n"
                text += "\n"

            text += self.getDefaultConstructor(indent + "  ", fullyQualifiedName, "java")
            text += "\n"
            text += self.getAttributeConstructor(indent + "  ", fullyQualifiedName, "java")
            text += "\n"
            text += self.getGetClassName(indent + "  ", fullyQualifiedName, "java")
            text += "\n"
            if self._protocolType == "boostXML" or self._protocolType == "boostBinary" or self._protocolType == "boostText":
                text += self.getSerializeMessage(indent + "  ", fullyQualifiedName, "java")
                text += "\n"
                text += self.getReadMessage(indent + "  ", fullyQualifiedName, "java")
                text += "\n"
            else:
				text += self.getSetAttributes(indent + "  ", fullyQualifiedName, "java")
				text += "\n"
				text += self.getAddSubEntity(indent + "  ", fullyQualifiedName, "java")
				text += "\n"
				text += self.getWriteMembers(indent + "  ", fullyQualifiedName, "java")
				text += "\n"
				text += self.getWriteSubentities(indent + "  ", fullyQualifiedName, "java")
				text += "\n"
				text += self.getWrite(indent + "  ", fullyQualifiedName, "java")
            text += indent + "}\n"
            
        else:
            print "Unsupported language: " + language
        
        return text
        
    def getImpl(self, indent, parentQualName):
        """
        Generates a C++ class implementation.
        
        \param indent
                 the spaces to indent the implementation.
        \param parentQualName
                 the fully qualified name of the parent classes. None if there are no parents.
        \return a string containing the C++ class implementation.
        """
        
        fullyQualifiedName = self._name
        if parentQualName != None:
            fullyQualifiedName = parentQualName + "::" + fullyQualifiedName
        
        text = ""
        
        for c in self._classes:
            text += c.getImpl(indent, fullyQualifiedName)
        
        text += indent + "/////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n"
        text += indent + "// Class " + fullyQualifiedName + "\n"
        if self._comment:
          text += indent + "// " + self._comment + "\n"
        text += indent + "/////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n"

        text += self.getDefaultConstructor(indent, fullyQualifiedName, "c++")
        text += "\n"        
        text += self.getAttributeConstructor(indent, fullyQualifiedName, "c++")
        text += "\n"
        
        for f in self._memberFunctions:
            if f == "copy":
                text += self.getCopyConstructor(indent, fullyQualifiedName, "c++")
                text += "\n"
            if f == "reset":
                text += self.getReset(indent, fullyQualifiedName, "c++")
                text += "\n"
                
        text += self.getDestructor(indent, fullyQualifiedName)
        text += "\n"
        text += self.getGetClassName(indent, fullyQualifiedName, "c++")
        text += "\n"
        
        if self._protocolType == "boostXML" or self._protocolType == "boostBinary" or self._protocolType == "boostText":
            text += self.getSerializeMessage(indent, fullyQualifiedName, "c++")
            text += "\n"
            text += self.getReadMessage(indent, fullyQualifiedName, "c++")
        else:
            text += self.getSetAttributes(indent, fullyQualifiedName, "c++")
            text += "\n"
            text += self.getAddSubEntity(indent, fullyQualifiedName, "c++")
            text += "\n"
            text += self.getWriteMembers(indent, fullyQualifiedName, "c++")
            text += "\n"
            text += self.getWriteSubentities(indent, fullyQualifiedName, "c++")
            text += "\n"
            text += self.getWrite(indent, fullyQualifiedName, "c++")
            
        text += "\n"

        return text


    
