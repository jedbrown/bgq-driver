#!/usr/bin/python
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
# (C) Copyright IBM Corp.  2004, 2011                              
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


import re

from optparse import OptionParser
from string import Template


copyright_text = """

/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2011                              */
/* Eclipse Public License (EPL)                                     */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

"""

class Field:
    def __init__( self, file_name ):
        self.filename = file_name

        lines = open( self.filename, "r" ).readlines() # read the file

        # first line has -- name
        name_line = lines[0]

        match = re.compile( r'\s*--\s*(\S+)\s*' ).match( name_line )

        self.name = match.group(1)

        # rest of file is the SQL.
        del lines[0]
        
        self.sql_lines = []
        self.param_names = []

        for line in lines:
            line = line.rstrip()
            
            if line == '':
                self.sql_lines.append( "" )
                continue

            comment_start_loc = line.find( '#' )

            if comment_start_loc == -1: # not found
                self.sql_lines.append( line.rstrip() )
                continue
            
            sql = line[:comment_start_loc].rstrip()
            param_names_str = line[comment_start_loc+1:].strip()
            
            param_names = param_names_str.split()

            self.sql_lines.append( sql )
            self.param_names.extend( param_names )


    def calcSqlCstringText( self ):
        res = ""

        for line in self.sql_lines:
            if line.strip() == "":
                res = res + "\n"
            else:
                m = re.compile( r'(\s*)(.*)' ).match( line.rstrip() )
                res = res + m.group(1) + '" ' + m.group(2) + '"\n'

        return res


    def calcParamNamesBoostAssignText( self ):
        res = ""
        
        for param_name in self.param_names:
            res = res + '        ( "' + param_name + '" )\n'

        return res


#######################################################################


def generateHeader(
            header_guard_name,
            file_name,
            namespace_start_text, namespace_end_text,
            fields
        ):

    fields_text = ""

    for field in fields:
        fields_text = fields_text + "extern const std::string " + field.name + "; // FROM " + field.filename + "\n"
        if len( field.param_names ) != 0:
            fields_text = fields_text + "extern const cxxdb::ParameterNames " + field.name + "_PARAMETER_NAMES;\n"
        fields_text = fields_text + "\n"

    t = Template(
                 
"""

""" + copyright_text +
"""

// GENERATED FILE

#ifndef $header_guard_name
#define $header_guard_name


#include <db/include/api/cxxdb/fwd.h>

#include <string>


$namespace_start


$fields


$namespace_end

#endif

"""

         )

    file_contents = t.substitute(
             header_guard_name= header_guard_name,
             namespace_start= namespace_start_text,
             fields= fields_text,
             namespace_end= namespace_end_text
         )

    print >> open( file_name, "w" ), file_contents


def generateImplementation( 
            header_file_name,
            output_file_name,
            namespace_start_text, namespace_end_text,
            fields
        ):
    
    FIELD_TEMPLATE = Template( """
// FROM $filename
const std::string $name(

$lines

    );
"""

        )


    fields_text = ""

    for field in fields:
        field_text = FIELD_TEMPLATE.substitute(
                name= field.name,
                filename= field.filename,
                lines= field.calcSqlCstringText()
            )

        fields_text = fields_text + field_text

        if len( field.param_names ) != 0:
            fields_text = fields_text + "const cxxdb::ParameterNames " + field.name + "_PARAMETER_NAMES = boost::assign::list_of\n" + field.calcParamNamesBoostAssignText() + "\n        ;\n"


    t = Template(

"""

""" + copyright_text +
"""

// GENERATED FILE


#include "$header_file_name"

#include <boost/assign.hpp>


$namespace_start

$fields

$namespace_end

"""
        )

    file_contents = t.substitute(
            header_file_name= header_file_name,
            namespace_start= namespace_start_text,
            fields= fields_text,
            namespace_end= namespace_end_text
        )

    print >> open( output_file_name, "w" ), file_contents,


#----------------------------------------------------------------------


parser = OptionParser()

parser.add_option( "--generate", dest="generate", help="generate header file", metavar=".hpp|.cc" )
parser.add_option( "--header", dest="header", help="header file", metavar="FILE" )
parser.add_option( "--impl", dest="impl", help="implementation file", metavar="FILE" )
parser.add_option( "--header-guard", dest="headerGuard", help="header guard name", metavar="NAME" )
parser.add_option( "--namespace", dest="namespace", help="namespace", metavar="NAMESPACE" )
parser.add_option( "--include", dest="include", help="include to use in impl to get header if not same as header", metavar="FILE" )


(options, inputs) = parser.parse_args()


namespace_str = options.namespace
namespace_parts = namespace_str.split( "::" )

namespace_start_text = ""

for namespace_part in namespace_parts:
    namespace_start_text = namespace_start_text + "namespace " + namespace_part + " {\n"

namespace_end_text = ("} " * len( namespace_parts )) + "// namespace " + namespace_str


fields = []

for input_file_name in inputs:
    f = Field( input_file_name )

    fields.append( f )


if options.generate == ".hpp" or options.generate == ".h":
    print "Header: ", options.header

    generateHeader(
            options.headerGuard,
            options.header,
            namespace_start_text, namespace_end_text,
            fields
        )


if options.generate == ".cc":
    print "Impl: ", options.impl

    include = options.header
    if options.include:
        include = options.include

    generateImplementation(
            include,
            options.impl,
            namespace_start_text, namespace_end_text,
            fields
        )
