import os
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

def writeApiCHeader(fileObject, objects):
    """
    Write C++ header files for the given API objects.
    
    \param fileObject
             an XmlFile object containing the File element information.
    \param objects
             a list of the API objects.
    """
    
    # Determine header file name.
    headerFile = fileObject._name + ".h"
    
    print "Generating " + headerFile
    
    # Open/create the header file.
    fd = open(headerFile, "w")
    
    # Write the header file's header (leading comment, includes, namespace statement, etc)
    fd.write(fileObject.getHeader(True, "c++"))
    fd.write("\n")
    
    # Initialize the amount of indentation.
    indent = ""
    
    # Indent a bit more if we have a namespace we're underneath.
    if fileObject._namespace != None:
        indent = "  "

    # Write all enum declarations to the file.
    for object in objects:
        if object._typeName == "ValueSet":
            fd.write(object.getDecl(indent, None, "c++"))
            fd.write("\n\n")

    # Write all class declarations to the file.
    for object in objects:
        if object._typeName == "Class":
            fd.write("\n\n")
            fd.write(object.getDecl(indent, None, "c++"))
    
    # Write the file's footer.
    fd.write("\n")
    fd.write(fileObject.getFooter(True, "c++"))
    
    # Close the header file.
    fd.close()
    
def writeApiCImpl(fileObject, objects):
    """
    Write C++ implementation files for the given API objects.
    
    \param fileObject
             an XmlFile object containing the File element information.
    \param objects
             a list of the API objects.
    """

    # Determine the C++ file name.
    implFile = fileObject._name + ".cc"
    
    print "Generating " + implFile
    
    # Open/create the file.
    fd = open(implFile, "w")
    
    # Write the header file's header (leading comment, includes, namespace statement, etc)
    fd.write(fileObject.getHeader(False, "c++"))

    # Initialize the amount of indentation.
    indent = ""

    # Indent a bit more if we have a namespace we're underneath.
    if fileObject._namespace != None:
        indent = "  "

    # Write all class implementations to the file.
    for object in objects:
        if object._typeName == "Class":
            fd.write("\n\n")
            fd.write(object.getImpl(indent, None))
            
    
    # Write the file's footer.
    fd.write(fileObject.getFooter(False, "c++"))
    
    if fileObject._type == "boostXML" or fileObject._type == "boostBinary" or fileObject._type == "boostText":
        for object in objects:
            if object._typeName == "Class":
                fullyQualifiedName = object._name
                if fileObject._namespace != None:
                    fullyQualifiedName = fileObject._namespace + "::" + fullyQualifiedName  
                # reduce serialization overhead by not storing version
                fd.write("BOOST_CLASS_IMPLEMENTATION(" + fullyQualifiedName + ", boost::serialization::object_serializable);\n")
                # reduce runtime cost of object tracking
                fd.write("BOOST_CLASS_TRACKING(" + fullyQualifiedName + ", boost::serialization::track_never);\n")




    # Close the file.
    fd.close()
    
    # Check to see if we need to generate an API header file.
    if fileObject._apiHeader != None:
        fileObject.generateApiHeader()
        
    # Check to see if we need to generate an API implementation file.
    if fileObject._apiImplementation != None:
        fileObject.generateApiImplementation()
    
    # Check to see if we need to generate a API console source files.
    if fileObject._apiConsole != None:
        fileObject.generateConsoleHeader()
        fileObject.generateConsoleImplementation()

def writeApiJava(fileObject, objects):
    """
    Write Java files for the given API objects.
    
    \param fileObject
             an XmlFile object containing the File element information.
    \param objects
             a list of the API objects.
    """

    # Determine the package name.
    packageName = fileObject.getPackageName()
    
    print "Generating package " + packageName
    
    # Create the package's source directory.
    if not os.path.exists(packageName):
        os.mkdir(packageName)
        
    # Change to the package's source directory.
    os.chdir(packageName)
    
    # Generate a basic manifest file for use in creating JAR files.
    fileObject.generateManifestFile()

    # Create source files for all the objects.
    for object in objects:
        
        # Determine the file name for the object.
        implFile = object._name + ".java"
        
        # Open/create the source file for the object.
        fd = open(implFile, "w")
        
        # Write the file's header (leading comment, imports, etc)
        fd.write(fileObject.getHeader(False, "java"))
        fd.write("\n")

        # Write the source to the file.
        if object._typeName == "Class":
            fd.write(object.getDecl("", None, "java"))
            
        elif object._typeName == "ValueSet":
            fd.write(object.getDecl("", None, "java"))
    
        # Write the file's footer.
        fd.write(fileObject.getFooter(False, "java"))
        
        # Close the file.
        fd.close()
    
    os.chdir("..")
    
