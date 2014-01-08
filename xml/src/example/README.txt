These directory contains input files for the xidl compiler. Starting from 
the definition of a set of classes (in xml format, using the schema defined
in this file) located here the xidl compilers in the directory xml/scripts 
generate a .h file with class definitions, and with methods for saving 
objects in a C++ ostream, and for reading them from an istream. 

Please, take a look at the Example.xml file. Every definition starts with
a <File> </File> root entity with one required attribute "filename". Every
<File> can contain a number of <Class>, <Include> and <Function> elements.

Every <Class> has a class "name" and can contain a number of <DataMember> 
and additional <Method>s, besides three standard methods for input and 
output of a class into the C++ streams. <Class>es can also be nested 
inside a class (for example the Arg class is in scope ExampleClass::Arg). In
addition, a <Class> can contain two other attributes:
- "comment" to explain the purpose of the class
- "parent" to define another superclass, besides the predefined 
"XML::Serializable"


<Method> and <Function> can contain arbitrary C++ code (not just function)
and must be enclosed in a <![CDATA[ ... ]]> section (so "]]>" is the
only forbidden sequence of characters).

<DataMember> has the following required attributes:
- "type" can be one of the predefined types (they use a predefined XML 
marshall and unmarshall), or types defined in the file. The user can
also specify a custom type, and needs to provide the marshall/unmarshall
methods. The following ones are the predefined types (with their C++ type):
  + text (std::string) XML marshalled using ASCII (so only use valid xml 
characters: between 32 and 126 inclusive; we take care of the special XML 
characters <>'"&)
  + binary (std::string) marshalled using byte64 encoding so any byte
is valid 
  + uint32 (unsigned)
  + uchar (unsigned char)
  + bool (bool)
- "name" is the data member name (note that we put a "_" in front)

and the optional attributes:
- "comment" just to appear in the .h generated file
- "ocurrences" is "one" (default) or "many" for a vector of data members
- "inConstructor" 1 (default) or 0 if the data member should appear in the
constructor of the class (not that this only matters for single ("one") data
members of predefined types
- "isOptional" 0 (default) or 1 if the data member is required (or not) in
the C++ stream
- "isInherited" it comes from a parent class; it does not appear in the
data members of the current class, but it is marshalled/unmarshalled



To run:
../scripts/xidlMain.py Example.xml


Created:     Jose Castanos (1/3/05)
Last edited: Jose Castanos (1/3/05)
 

 

