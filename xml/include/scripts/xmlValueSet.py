
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
class XmlValueSet:
    """
    An XmlValueSet object.
    """
    
    def __init__(self, name, attrs, version):
        """
        The constructor.
        
        \param name
                 the name of the value set.
        \param attrs
                 the XML attributes for the value set.
        """

        self._typeName = name
        self._name = attrs['name']
        self._comment = None 
        self._version = version

        if 'comment' in attrs.keys():
            self._comment = attrs['comment'].capitalize()
            if self._comment[-1] != '.':
                self._comment += '.'

        # A list of the value members in this value set.
        self._valueMembers = []
        
        # The longest value member name length. Used for code formatting.
        self._longestMemberNameLength = 0
        
        # A variable to track what the value assigned to the last value member was. 
        self._lastValueMemberValue = -1

    def addValueMember(self, valueMember):
        """
        Adds an XmlValueMember object to the list of value members for this value set.
        
        \param valueMember
                 the XmlValueMember object to add.
        """

        if valueMember._value == None:
            self._lastValueMemberValue += 1
            valueMember._value = str(self._lastValueMemberValue)
        else:
            self._lastValueMemberValue = int(valueMember._value)
            
        self._valueMembers.append(valueMember)
        self._longestMemberNameLength = max([self._longestMemberNameLength, len(valueMember._name)])

    def getDecl(self, indent, parentQualName, language):
        """
        Generates a value set declaration.
        
        \param indent
                 the spaces to indent the declaration.
        \param parentQualName
                 the fully qualified name of the parent classes. None if there are no parents.
        \param language
                 the language to generate the declaration for. Valid values are "C++" or "Java".
        \return a string containing the value set declaration.
        """

        text = ""

        if language.lower() == "c++":
            text  = indent + "/*!\n"
            if self._comment != None:
                text += indent + " * " + self._comment + "\n"
            else:
                text += indent + " * The " + self._name + " enumeration.\n"
            text += indent + " * \n"
            text += indent + " */\n"
            text += indent + "typedef enum\n"
            text += indent + "{\n"
            
            for m in self._valueMembers:
                if m._comment != None:
                    text += indent + "  /*! " + m._comment + " */\n"
                text += indent + "  " + m._name
                if m._value != None:
                    text += ((self._longestMemberNameLength - len(m._name)) * " ") + " = " + m._value
                text += ",\n"
                if m._comment != None:
                    text += "\n"
            
            if m._comment != None:
                text = text[0:-3] + "\n"
            else:
                text = text[0:-2] + "\n"
            text += indent + "} " + self._name + ";\n"
            
        elif language.lower() == "java":
            text += indent + "/**\n"
            text += indent + " * "
            if self._comment:
                text += self._comment + "\n"
            else:
                text += "Enumeration " + self._name + ".\n"
            text += indent + " *\n"
            text += indent + " */\n"
            text += indent + "public enum " + self._name + "\n"
            text += indent + "{\n"
            
            i = 0
            for v in self._valueMembers:

                if v._comment != None:
                    text += indent + "  /** " + v._comment + " */\n"
                
                text += indent + "  " + v._name + "("
                    
                if v._value != None:
                    text += v._value
                    i = int(v._value)
                else:
                    text += str(i)
                    i += 1
                        
                text += "),\n\n"
            text = text[:-3] + ";\n"
            
            text += "\n"
            text += indent + "  /** The numeric value for the enumeration. */\n"
            text += indent + "  private Integer value = null;\n"
            text += "\n"
            text += indent + "  /**\n"
            text += indent + "   * Constructs a " + self._name + ".\n"
            text += indent + "   *\n"
            text += indent + "   * @param value\n"
            text += indent + "   *          the integer value for the enumeration.\n"
            text += indent + "   */\n"
            text += indent + "  private " + self._name + "(Integer value)\n"
            text += indent + "  {\n"
            text += indent + "    this.value = value;\n"
            text += indent + "  }\n"
            text += "\n"
            text += indent + "  /**\n"
            text += indent + "   * Retrieves the value for the " + self._name + "enumeration object.\n"
            text += indent + "   *\n"
            text += indent + "   * @return the corresponding integer value for the given " + self._name + ".\n"
            text += indent + "   */\n"
            text += indent + "  public Integer getValue()\n"
            text += indent + "  {\n"
            text += indent + "    return this.value;\n"
            text += indent + "  }\n"
            text += "\n"
            text += indent + "  /**\n"
            text += indent + "   * Translates an integer value to an " + self._name + ".\n"
            text += indent + "   *\n"
            text += indent + "   * @param value\n"
            text += indent + "   *          the integer value to translate.\n"
            text += indent + "   * @return the corresponding " + self._name + " value for the given integer or null if there is no match.\n"
            text += indent + "   */\n"
            text += indent + "  public static " + self._name + " translate(Integer value)\n"
            text += indent + "  {\n"
            text += indent + "    " + self._name + " enumValue = null;\n"
            text += "\n"
            text += indent + "    switch(value)\n"
            text += indent + "    {\n"
            i = 0
            for v in self._valueMembers:
                if(v._value == None):
                   text += indent + "      case " + str(i) + ":\n"
                else:
                  text += indent + "      case " + v._value + ":\n"
                  i = int(v._value)
                text += indent + "         enumValue = " + v._name + ";\n"
                text += indent + "         break;\n"
                i += 1
            text += indent + "      default:\n"
            text += indent + "        enumValue = null;\n"
            text += indent + "        break;\n"
            text += indent + "    }\n"
            text += indent + "\n"
            text += indent + "    return enumValue;\n"
            text += indent + "  }\n"
            text += indent + "\n"
            text += indent + "  /**\n"
            text += indent + "   * Translates a " + self._name + " value to an integer.\n"
            text += indent + "   *\n"
            text += indent + "   * @param value\n"
            text += indent + "   *          the " + self._name + " value to translate.\n"
            text += indent + "   * @return the corresponding integer value for the given " + self._name + ".\n"
            text += indent + "   */\n"
            text += indent + "  public static Integer translate(" + self._name + " value)\n"
            text += indent + "  {\n"
            text += indent + "    return value.value;\n"
            text += indent + "  }\n"
            text += indent + "}\n"
            
        else:
            print "Unsupported language: " + language
        
        return text
    
