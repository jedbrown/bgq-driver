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


def getSpaces(level):
    result = ""
    for num in xrange(level):
        result += "  "
    return result 

def getSpacesString(level):
    return "\"" + getSpaces(level) + "\""
    

class Class:
    # -------------------- initialization ------------------
    def __init__(self, name):
        # these three data members can appear as attributes in the
        # <Class > tag (name is required, commend and parentClass are
        # optional)
        self._name         = name
        self._comment      = None

        if xidlConfig.generateXML:
            self._parentClass = "XML::Serializable" # default parent
        else:
            self._parentClass = None
            
        self._isSuperClass = 0              # not a superclass

        self._extraMethods = []             # custom methods

        self._valueSets = []                # value set members (ENUMS)
        self._dataMembers  = []             # all the fields in this class
        # (including ones that we inherit from a parent class; will not
        # be redefined, but will be marshalled/unmarshalled

        # data members to keep track of a nesting hierarchy of classes
        self._nestingClass  = None          # not Null if this is a subclass
        self._nestingLevel  = 0             # for indentation
        self._nestedClasses = []            # list of subclasses
	
	self._file          = None          # The file handle used for this Java class.

    # -------------------- setters -------------------------
    def setComment(self, comment):
        self._comment = comment.capitalize()
        if self._comment[-1] != ".":
            self._comment += "."
    
    def setParent(self, parentClass):
        self._parentClass = parentClass

    def setIsSuperClass(self, isSuperClass):
        self._isSuperClass = isSuperClass

    def addNestedClass(self, nestedClass):
        nestedClass._nestingClass = self
        nestedClass._nestingLevel = self._nestingLevel + 1
        self._nestedClasses.append(nestedClass)

    def addExtraMethod(self, method):
        self._extraMethods.append(method)    

    def addDataMember(self, dataMember):
        dataMember.setIsValueSet(self.isValueSet(dataMember))
        self._dataMembers.append(dataMember)

    def addValueSet(self, valueSet):
        self._valueSets.append(valueSet)

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
        return getSpaces(self._nestingLevel)

    def getNestingLevel (self):
        return(self._nestingLevel);

    # check if we need an attribute constructor
    # loop through all the data members: we need to find data members
    # with occurrences="one" and 
    def needsAttributeConstructor(self):
        for d in self._dataMembers:
            if d.isInConstructor():
                return 1
        return 0
        
    # check if we need a destructor
    # loop through all the data members: we need to find data members
    # with occurrences="many" and 
    def needsDestructor(self):
        for d in self._dataMembers:
            if d.isMultiple():
                return 1
        return 0
        
    #
    # check to see if this is a value set element.
    #
    def isValueSet (self, d):
        for n in self._valueSets:
            if (d._type == n._name):
                return 1
        return 0

    # --- generator methods for the class declaration -------
    # write a class declaration
    def writeDeclaration_C(self):
        self.writeHeader_C()

        # first all the nested classes
        for c in self._nestedClasses:
            c.writeDeclaration_C()

        self.writeValueSets_C()

        # write the default (null) constructor    
        self.writeDefaultConstructorDecl_C()

        # do we need a constructor with attributes(?)
        if self.needsAttributeConstructor():
            self.writeAttributeConstructorDecl_C()
            
        # do we need a destructor(?)
        if self.needsDestructor():
            self.writeDestructorDecl_C()

        # write the class name (reflection)
        self.writeClassNameDecl_C();

        # write the methods (just a declaration for now
        # for the standard three ones)
        if xidlConfig.generateXML:
            self.writeStandardMethodDeclarationsDecl_C()

        for m in self._extraMethods:
            print "// Extra method!"
            m.writeImplementation()

        self.writeDataMembers_C()
        self.writeClosingBrackets_C()

    # --- generator methods for the class declaration -------
    # write a class declaration
    def writeImpl_C(self):
        # first all the nested classes
        for c in self._nestedClasses:
            c.writeImpl_C()

        self.writeImplHeader_C()

        # write the default (null) constructor    
        self.writeDefaultConstructorImpl_C()

        # do we need a constructor with attributes(?)
        if self.needsAttributeConstructor():
            self.writeAttributeConstructorImpl_C()
            
        # do we need a destructor(?)
        if self.needsDestructor():
            self.writeDestructorImpl_C()

        # write the class name (reflection)
        self.writeClassNameImpl_C();

        for m in self._extraMethods:
            print "//!!! Extra method"
            m.writeImplementation()
            
        self.writeStandardMethodsImpl_C();

    # starting to write a new class
    def writeHeader_C(self):
        print self.getIndent() + "/*!"
        print self.getIndent() + " * Class " + self.fullyQualifiedName_C()

        if self._comment:
            print self.getIndent() + " * " + self._comment
        
        print self.getIndent() + " *"
        print self.getIndent() + " */"

        temp = self.getIndent() + "class " + self._name
        if self._parentClass != None:
            temp = temp + " : public " + self._parentClass
        temp = temp + " {"        
        print temp
        
        print self.getIndent() + "public: "

    def writeImplHeader_C(self):
        print
        print
        print "/////////////////////////////////////////////////////////////////////////////////////////////////////////////////"
        print "// Class " + self.fullyQualifiedName_C()
        if self._comment:
          print "// " + self._comment
        print "/////////////////////////////////////////////////////////////////////////////////////////////////////////////////"

    # we finish the current class
    def writeClosingBrackets_C(self):
        print self.getIndent() + "};"
        print

    # write the null (default) constructor
    def writeDefaultConstructorDecl_C(self):
        print
        print self.getIndent() + "  /*!"
        print self.getIndent() + "   * Default constructor."
        print self.getIndent() + "   *"
        print self.getIndent() + "   */"
        print self.getIndent() + "  " + self._name + "();"

    # write the null (default) constructor
    def writeDefaultConstructorImpl_C(self):
        print
        print "/*!"
        print " * Default constructor."
        print " *"
        print " */"
        print self.fullyQualifiedName_C() + "::" + self._name + "()"
        print "{}"

    # write the attribute constructors, calling the copy constructor
    # of all single instance attributes (occurrences=="one") that are
    # not optional (inConstructor==1)
    def writeAttributeConstructorDecl_C(self):
        attributeList   = ""
        assignementList = ""
        doxygen = ""
        count           = 0
        
        for d in self._dataMembers:
            if d.isInConstructor():
                if count > 0:
                    attributeList = attributeList + ", "
                    assignementList = assignementList + ", "
                doxygen += self.getIndent() + "   * param " + d._name + "\n"
                if d._comment != None:
                  doxygen += self.getIndent() + "   *         " + d._comment + "\n"
                else:
                  doxygen += self.getIndent() + "   *         No description found for this parameter.\n"
                attributeList = attributeList + d.getConstructorArgument_C()
                assignementList = assignementList + d.getCopyConstructor()
                count = count + 1
                
        print
        print self.getIndent() + "  /*!"
        print self.getIndent() + "   * Attribute constructor. The copy constructors of all attributes are used to initialize the class attributes."
        print self.getIndent() + "   *"
        print doxygen,
        print self.getIndent() + "   */"
        print self.getIndent() + "  " + self._name + "(" + attributeList + ");"

    # write the attribute constructors, calling the copy constructor
    # of all single instance attributes (occurrences=="one") that are
    # not optional (inConstructor==1)
    def writeAttributeConstructorImpl_C(self):
        attributeList   = ""
        assignementList = ""
        doxygen = ""
        count           = 0
        
        for d in self._dataMembers:
            if d.isInConstructor():
                if count > 0:
                    attributeList = attributeList + ", "
                    assignementList = assignementList + ", "
                doxygen += " * param " + d._name + "\n"
                if d._comment != None:
                  doxygen += " *         " + d._comment + "\n"
                else:
                  doxygen += " *         No description found for this parameter.\n"
                attributeList = attributeList + d.getConstructorArgument_C()
                assignementList = assignementList + d.getCopyConstructor()
                count = count + 1
                
        print
        print "/*!"
        print " * Attribute constructor. The copy constructors of all attributes are used to initialize the class attributes."
        print " *"
        print doxygen,
        print " */"
        print self.fullyQualifiedName_C() + "::" + self._name + "(" + attributeList + ")"
        print "    : " + assignementList
        print "{}"

    # write destructor for attributes which are not primatives
    def writeDestructorDecl_C(self):
        print
        print self.getIndent() + "  /*!"
        print self.getIndent() + "   * Default destructor."
        print self.getIndent() + "   *"
        print self.getIndent() + "   */"
        print self.getIndent() + "  ~" + self._name + "();"

    # write destructor for attributes which are not primatives
    def writeDestructorImpl_C(self):
        print
        print "/*!"
        print " * Default destructor."
        print " *"
        print " */"
        print self.fullyQualifiedName_C() + "::~" + self._name + "() {"
        for d in self._dataMembers:
            if d.isMultiple():
                print "  _" + d._name + ".clear();"
        print "}"

    # every XML class could have these three methods:
    # -setAttributes: for input "one", simple (native) data members
    # -addSubEntity: for input multiple (vector), or complex (non-native), data members
    # -write: always, for output
    def writeStandardMethodDeclarationsDecl_C(self):
        print
        print self.getIndent() + "  /*!"
        print self.getIndent() + "   * Set the XML class attributes."
        print self.getIndent() + "   *"
        print self.getIndent() + "   * param attrs"
        print self.getIndent() + "   *         the XML class attributes to set."
        print self.getIndent() + "   * return the index of the read data."
        print self.getIndent() + "   * throws XML::Exception"
        print self.getIndent() + "   *          if there was a problem setting the new XML attributes."
        print self.getIndent() + "   */"
        print self.getIndent() + "  virtual unsigned setAttributes(const char **attrs) throw(XML::Exception);"
        print
        print self.getIndent() + "  /*!"
        print self.getIndent() + "   * Adds an XML subentity."
        print self.getIndent() + "   *"
        print self.getIndent() + "   * param name"
        print self.getIndent() + "   *         the name for the XML subentity to add."
        print self.getIndent() + "   * param attrs"
        print self.getIndent() + "   *         the XML class attributes to add for the subentity."
        print self.getIndent() + "   * return a pointer to the serializable object."
        print self.getIndent() + "   * throws XML::Exception"
        print self.getIndent() + "   *          if there was a problem adding the new XML subentity."
        print self.getIndent() + "   */"
        print self.getIndent() + "  XML::Serializable* addSubEntity(const char *name, const char **attrs) throw(XML::Exception);"
        print
        print self.getIndent() + "  /*!"
        print self.getIndent() + "   * Writes the XML to the output stream."
        print self.getIndent() + "   *"
        print self.getIndent() + "   * param os"
        print self.getIndent() + "   *         the output stream to dump the XML to."
        print self.getIndent() + "   */"
        print self.getIndent() + "  virtual void write(std::ostream& os) const;"
        print
        print self.getIndent() + "  /*!"
        print self.getIndent() + "   * Writes the XML members to an output stream."
        print self.getIndent() + "   *"
        print self.getIndent() + "   * param os"
        print self.getIndent() + "   *         the output stream to dump the XML to."
        print self.getIndent() + "   */"
        print self.getIndent() + "  virtual void writeMembers(std::ostream& os) const;"
        print
        print self.getIndent() + "  /*!"
        print self.getIndent() + "   * Writes the XML subentities to an output stream."
        print self.getIndent() + "   *"
        print self.getIndent() + "   * param os"
        print self.getIndent() + "   *         the output stream to dump the XML to."
        print self.getIndent() + "   */"
        print self.getIndent() + "  virtual void writeSubentities(std::ostream& os) const;"

    # if this class has a tag, then write the method
    def writeClassNameDecl_C(self):
        print
        print self.getIndent() + "  /*!"
        print self.getIndent() + "   * Retrieves the class name. This is used for XML tag matching in client-server programs."
        print self.getIndent() + "   *"
        print self.getIndent() + "   * return a string containing the name of the class."
        print self.getIndent() + "   */"
        print self.getIndent() + "  static std::string getClassName();"

    # if this class has a tag, then write the method
    def writeClassNameImpl_C(self):
        print
        print "/*!"
        print " * Retrieves the class name. This is used for XML tag matching in client-server programs."
        print " *"
        print " * return a string containing the name of the class."
        print " */"
        print "std::string " + self.fullyQualifiedName_C() + "::getClassName() {"
        print "  return \"" + self.fullyQualifiedName_C() + "\";"
        print "}"
            
    # write the declaration of all the data members (but skip the ones
    # that are inherited from a parent class)
    def writeDataMembers_C(self):
        for d in self._dataMembers:
            if d._isInherited == 0:
                print
                for line in d.getDeclaration_C().split("\n"):
                  print self.getIndent() + line

    def writeValueSets_C(self):
        for v in self._valueSets:
            v.writeDeclaration_C()



    # --- generator methods for the inline methods -------

    # write the inline part of the three standard methods
    def writeStandardMethods_C(self):
        #if self.hasAttributes():
        self.writeSetAttributesMethod_C();
        #if self.hasSubEntities():
        self.writeAddSubEntityMethod_C();
        self.writeWriteSubentitiesMethod_C();
        self.writeWriteMembersMethod_C();
        self.writeWriteMethod_C();

        for c in self._nestedClasses:
            c.writeStandardMethods_C()    


    # write the inline part of the three standard methods
    def writeStandardMethodsImpl_C(self):
        self.writeSetAttributesMethod_C();
        self.writeAddSubEntityMethod_C();
        self.writeWriteSubentitiesMethod_C();
        self.writeWriteMembersMethod_C();
        self.writeWriteMethod_C();
#        for c in self._nestedClasses:
#            c.writeStandardMethods_C()    


    # write an inline method to read all attributes
    def writeSetAttributesMethod_C(self):
        print
        print "/*!"
        print " * Set the XML class attributes."
        print " *"
        print " * param attrs"
        print " *         the XML class attributes to set."
        print " * return the index of the read data."
        print " * throws XML::Exception"
        print " *          if there was a problem setting the new XML attributes."
        print " */"
        print "unsigned " + self.fullyQualifiedName_C() + "::setAttributes(const char **attrs) throw(XML::Exception) {"
        print "  unsigned i = 0;"
        # need to set parent class attributes, if any
        if self._parentClass != None:
            if self._parentClass != "XML::Serializable":
                print "  i = " + self._parentClass + "::setAttributes(attrs);"
            
        for d in self._dataMembers:
            if d.isAttribute():
                d.writeReadAttribute_C(self.fullyQualifiedName_C())
        
        # If this is a superclass, there could be more attributes in the subclass
        if self._isSuperClass == 0:        
            print "  XML::verify_last_attr(\"" + self._name + "\", attrs[i]);"
            
        print "  return i;"
        print "}"


    # write an inline method to read all subentities
    def writeAddSubEntityMethod_C(self):
        print
        print "/*!"
        print " * Adds an XML subentity."
        print " *"
        print " * param name"
        print " *         the name for the XML subentity to add."
        print " * param attrs"
        print " *         the XML class attributes to add for the subentity."
        print " * return a pointer to the serializable object."
        print " * throws XML::Exception"
        print " *          if there was a problem adding the new XML subentity."
        print " */"
        print "XML::Serializable *" + self.fullyQualifiedName_C() + "::addSubEntity(const char *name, const char **attrs) throw(XML::Exception) {"
        # need to set parent class attributes, if any
        if self._parentClass != None:
            if self._parentClass != "XML::Serializable":
                print "  try {"
                print "  return " + self._parentClass + "::addSubEntity(name, attrs);"
                print "  } catch (...) {}"
            
        print "  if (0) {"
        for d in self._dataMembers:
            if d.isSubEntity():
                d.writeAddSubEntity_C()
                
        print "  } else {"
        print "    XMLLIB_THROW(\"XML %s: invalid subentity %s\", \"" + self._name + "\", name);"
        print "  }"
        print "}"


    # write an inline method for output of the class
    def writeWriteMethod_C(self):
        print
        print "/*!"
        print " * Writes the XML to the output stream."
        print " *"
        print " * param os"
        print " *         the output stream to dump the XML to."
        print " */"
        print "void " + self.fullyQualifiedName_C() + "::write(std::ostream& os) const {"
        print "  os << " + getSpacesString(self._nestingLevel) + " << \"<" + self._name + "\";"
        
        # need to write parent class members, if any
        if self._parentClass != None:
            if self._parentClass != "XML::Serializable":
                print "  " + self._parentClass + "::writeMembers(os);"
            
        print "  writeMembers(os);" 
        
        print "  os << \" >\" << std::endl;"

        # need to write parent class subentities, if any
        if self._parentClass != None:
            if self._parentClass != "XML::Serializable":
                print "  " + self._parentClass + "::writeSubentities(os);"
            
        print "  writeSubentities(os);" 

        print "  os << " + getSpacesString(self._nestingLevel) + " << \"</" + self._name + " >\" << std::endl;"
        print "}"


    def writeWriteMembersMethod_C(self):
        print
        print "/*!"
        print " * Writes the XML members to an output stream."
        print " *"
        print " * param os"
        print " *         the output stream to dump the XML to."
        print " */"
        print "void " + self.fullyQualifiedName_C() + "::writeMembers(std::ostream& os) const {"
        # first do the attribute data members
        for d in self._dataMembers:
            if d.isAttribute():
                d.writeAttribute_C(self.fullyQualifiedName_C())
        print "}"


    def writeWriteSubentitiesMethod_C(self):
        print
        print "/*!"
        print " * Writes the XML subentities to an output stream."
        print " *"
        print " * param os"
        print " *         the output stream to dump the XML to."
        print " */"
        print "void " + self.fullyQualifiedName_C() + "::writeSubentities(std::ostream& os) const {"
        # then do the subentitity data members     
        if self.hasSubEntities():
            for d in self._dataMembers:
                if d.isSubEntity():
                    d.writeSubEntity_C()
                    
        print "}"


    # -------------------- auxiliary methods ----------------

    # has a data member that is also an attribute (occurences
    # == "one" and native)
    def hasAttributes(self):
        for d in self._dataMembers:
            if d.isAttribute():
                return 1
        return 0

    # has a data member that is also a subEntity (occurences
    # != "one" and complex (struct, or nested class)
    def hasSubEntities(self):
        for d in self._dataMembers:
            if d.isSubEntity():
                return 1
        return 0


    # -------------------- java stuff ----------------

    # write a class declaration
    def writeClass_Java(self, packageName, packageHeader):
	print "Creating " + packageName + "." + self._name
	
	# Open a Java file for the class.
	self._file = open(self._name + ".java", 'w')
	
	# Redirect stdout
	sys.stdout = self._file
        
	# Write the package header.
	print packageHeader
	
	# start writing this class
        self.writeClassHeader_Java()

        # Write all the nested classes
        for c in self._nestedClasses:
            c.writeInnerClass_Java(packageName + "." + self._name)

	# Write enums.
        self.writeValueSets_Java()
        
        # write the default (null) constructor    
        self.writeDefaultConstructor_Java()

        # do we need a constructor with attributes(?)
        if self.needsAttributeConstructor():
            self.writeAttributeConstructor_Java()

        # write the methods 
        if xidlConfig.generateXML:
            self.writeStandardMethods_Java()

        self.writeDataMembers_Java()
        self.writeClosingBrackets_Java()
	
	# Close the Java file for the class.
        self._file.close()
	
	# Restore stdout
	sys.stdout = sys.__stdout__


    # write an inner class declaration
    def writeInnerClass_Java(self, parentClass):
	
	# start writing this class
        self.writeInnerClassHeader_Java()

        # Write all the nested classes
        for c in self._nestedClasses:
            c.writeInnerClass_Java(self._name)

	# Write enums.
        self.writeValueSets_Java()
        
        # write the default (null) constructor    
        self.writeDefaultConstructor_Java()

        # do we need a constructor with attributes(?)
        if self.needsAttributeConstructor():
            self.writeAttributeConstructor_Java()

        # write the methods 
        if xidlConfig.generateXML:
            self.writeStandardMethods_Java()

        self.writeDataMembers_Java()
        self.writeClosingBrackets_Java()
	

    # starting to write a new class
    def writeClassHeader_Java(self):
        print self.getIndent() + "// ======================================================================== "
        print self.getIndent() + "// CLASS " + self._name
        print self.getIndent() + "// ========================================================================"
        print
        if self._comment:
            print self.getIndent() + "/* " + self._comment +" */"
            print

        print self.getIndent() + "public class " + self._name
        if self._parentClass != None:
            if self._parentClass == "XML::Serializable":
                self._parentClass = "XMLSerializable"
            print " extends " + self._parentClass 
        print "{"
        if self._version != "":
            print self.getIndent() + "public static final String VERSION = \"" + self._version + "\";"

    # starting to write a new class
    def writeInnerClassHeader_Java(self):
        print self.getIndent() + "// ======================================================================== "
        print self.getIndent() + "// CLASS " + self._name
        print self.getIndent() + "// ========================================================================"
        print
        if self._comment:
            print self.getIndent() + "/* " + self._comment +" */"
            print

        print self.getIndent() + "public static class " + self._name + " extends XMLSerializable {"
        self._parentClass = "XMLSerializable"

    # write the null (default) constructor
    def writeDefaultConstructor_Java(self):
        print self.getIndent() + "  public " + self._name + "() {"

        for d in self._dataMembers:
             if d.isSingle():
                 print self.getIndent() + "    " + d.getDefaultSingleConstructor_Java()

        for d in self._dataMembers:
             if d.isMultiple():
                 print self.getIndent() + "    " + d.getDefaultMultipleConstructor_Java()
                 
        print self.getIndent() + "  }"
        print


    # write the attribute constructors, calling the copy constructor
    # of all single instance attributes (occurrences=="one") that are
    # not optional (inConstructor==1)
    def writeAttributeConstructor_Java(self):
        attributeList   = ""
        assignementList = ""
        count           = 0
        
        for d in self._dataMembers:
            if d.isInConstructor():
                if count > 0:
                    attributeList = attributeList + ", "
                attributeList = attributeList + d.getConstructorArgument_Java()
                count = count + 1
                
        print self.getIndent() + "  public " + self._name + "(" + attributeList + ") {"

        for d in self._dataMembers:
            if d.isInConstructor():
                print self.getIndent() + "    " + d.getAttributeConstructor_Java()

        print self.getIndent() + "  }"
        print    


    # write an inline method to read all attributes
    def writeSetAttributesMethod_Java(self): 
        print self.getIndent() + "  public int setAttributes(Attributes attrs) {"
        print self.getIndent() + "    int i = 0;"
        # need to set parent class attributes, if any
        if self._parentClass != None:
            if self._parentClass != "XMLSerializable":
                print self.getIndent() + "    i = super.setAttributes(attrs);"
                
	    print self.getIndent() + "    for (; i < attrs.getLength(); i++) {"
        print self.getIndent() + "      if (false) {"
        for d in self._dataMembers:
            if d.isAttribute():
                print self.getIndent(),
                d.writeReadAttribute_Java(self.fullyQualifiedName_Java());
        print self.getIndent() + "      } else {"
        print self.getIndent() + "        // check for errors"
        print self.getIndent() + "      }"
        print self.getIndent() + "    }"
        print self.getIndent() + "    return i;"
        print self.getIndent() + "  }"
        print
        

    # write an inline method to read all subentities
    def writeAddSubEntityMethod_Java(self):
        print self.getIndent() + "  public XMLSerializable addSubEntity(String name, Attributes attrs) {"
        # need to set parent class attributes, if any
        if self._parentClass != None:
            if self._parentClass != "XMLSerializable":
                print self.getIndent() + "  XMLSerializable retval = super.addSubEntity(name, attrs);"
                print self.getIndent() + "  if ( retval != null ) {"
                print self.getIndent() + "    return retval;"
                print self.getIndent() + "  }"
                print
            
        print self.getIndent() + "    if (false) {"
        for d in self._dataMembers:
            if d.isSubEntity():
                print self.getIndent(),
                d.writeAddSubEntity_Java()

        print self.getIndent() + "    } else {"
        print self.getIndent() + "      System.err.println(\"found no entity\");"
        print self.getIndent() + "      return null;"
        print self.getIndent() + "    }"
        print self.getIndent() + "    return null;" # TODO: should never be reached
        print self.getIndent() + "  }"
        print

    # write an inline method for output of the class
    def writeWriteMembersMethod_Java(self):
        print self.getIndent() + "  public String getAttributeString() {"
        print self.getIndent() + "    String command = \"\";"
        # first do the attribute data members
        for d in self._dataMembers:
            if d.isAttribute():
                print self.getIndent(),
                d.writeAttribute_Java(self.fullyQualifiedName_Java())
        print self.getIndent() + "    return command;"
        print self.getIndent() + "  }"
        print


    # write an inline method for output of the class
    def writeWriteSubentitiesMethod_Java(self):
        print self.getIndent() + "  public String getSubentitiesString() {"
        print self.getIndent() + "    String command = \"\";"
        # first do the attribute data members
        for d in self._dataMembers:
            if d.isSubEntity():
                print self.getIndent(),
                d.writeSubEntity_Java()

        print self.getIndent() + "    return command;"
        print self.getIndent() + "  }"
        print


    # write an inline method for output of the class
    def writeWriteMethod_Java(self):
        print self.getIndent() + "  public String getXMLString() {"
        # Example: String command = "<DemoRequest ";
        print self.getIndent() + "    String command = \"<" + self._name + " \";"

        # need to set parent class attributes, if any
        if self._parentClass != None:
            if self._parentClass != "XMLSerializable":
                print self.getIndent() + "    command += super.getAttributeString();"
                
        print self.getIndent() + "    command += getAttributeString();"

        # Example: command += ">";
        print self.getIndent() + "    command += \">\";"

        # then do the subentitity data members     
        # need to set parent class attributes, if any
        if self._parentClass != None:
            if self._parentClass != "XMLSerializable":
                print self.getIndent() + "    command += super.getSubentitiesString();"
                
        if self.hasSubEntities():
            print self.getIndent() + "    command += getSubentitiesString();"

        # Example: command += "</DemoRequest>";
        print self.getIndent() + "    command += \"</" + self._name + ">\";"
        print self.getIndent() + "    return command;"
        print self.getIndent() + "  }"
        print 
        print self.getIndent() + "  public void write(PrintStream os) {"
        print self.getIndent() + "    DataOutputStream dataOut = new DataOutputStream(os);"
        print self.getIndent() + "    String command = \"" + self._name + " \" + getXMLString();"
        print
        print self.getIndent() + "    try {"
        print self.getIndent() + "      // Send data length plus 1 byte for new line."
        print self.getIndent() + "      dataOut.writeInt(command.length() + 1);"
        print self.getIndent() + "    } catch (IOException e) {"
        print self.getIndent() + "      System.err.println(e);"
        print self.getIndent() + "      e.printStackTrace(System.err);"
        print self.getIndent() + "    }"
        print
        print self.getIndent() + "    os.println(command);"
        print self.getIndent() + "  }"
        print



    # write the inline part of the three standard methods
    def writeStandardMethods_Java(self):
        self.writeSetAttributesMethod_Java()
        self.writeAddSubEntityMethod_Java()
        self.writeWriteMembersMethod_Java();
        self.writeWriteSubentitiesMethod_Java();
        self.writeWriteMethod_Java();


    # write the declaration of all the data members (but skip the ones
    # that are inherited from a parent class)
    def writeDataMembers_Java(self):
        for d in self._dataMembers:
            if d._isInherited == 0:                                      
                print self.getIndent() + d.getDeclaration_Java()


    def writeValueSets_Java(self):
        for v in self._valueSets:
            v.writeDeclaration_Java()

    # we finish the current class
    def writeClosingBrackets_Java(self):
        print self.getIndent() + "};"
        print


