# --------------------------------------------------------------  #
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

BEGIN {
  }
#
# create bgq schemas: tables, synonyms, etc
# parameteres: database name, user and password.
# default: BGDB0 for database name; userid running this script as dabase user
#          if userid is provided, password should be also provided
#
function create_struct_el()
{
    while ((getline<DBFILE) >0)
    {
	print $0;
	FTYPE=$2;
	FSIZE=$3;
	FNAME=$4;
	CODEPAGE=$7;
	AUX=$4;

    printf "   static const std::string %s_COL;\n", FNAME > TMPFILE;

    COLTYPE="Other";
    if (FTYPE=="CHARACTER"  || FTYPE=="CHAR")  COLTYPE = "Char";
    else if ( FTYPE == "VARCHAR" )  COLTYPE = "Varchar";
    else if ( FTYPE == "BIGINT" )  COLTYPE = "Bigint";
    else if ( FTYPE == "INTEGER" )  COLTYPE = "Integer";
    else if ( FTYPE == "TIMESTAMP" || FTYPE=="TIMESTMP" )  COLTYPE = "Timestamp";
    else if ( FTYPE == "SMALLINT" )  COLTYPE = "Smallint";
    else if ( FTYPE == "DOUBLE" )  COLTYPE = "Double";
    else if ( FTYPE == "DECIMAL" )  COLTYPE = "Decimal";

    printf "   static const ColumnType::Value %s_COL_TYPE = ColumnType::%s;\n", FNAME, COLTYPE > TMPFILE;


    if (FTYPE=="CHARACTER" || FTYPE=="VARCHAR"  || FTYPE=="CHAR")
	{
	    printf "   static const unsigned %s_SIZE = %s;\n", FNAME, FSIZE > TMPFILE;

	    FTYPE="char";
	    if (CODEPAGE==0)
		FTYPE="unsigned char";
	    else
		FSIZE=FSIZE+1;
	    FNAME=sprintf("%s[%s]",FNAME,FSIZE);
	}
	else if (FTYPE=="BIGINT")
	    FTYPE="long long int";
	else if(FTYPE=="INTEGER")
	    FTYPE="signed int";
	else if ( FTYPE=="TIMESTAMP" || FTYPE=="TIMESTMP")
	{
	    FTYPE="char";
	    FNAME=sprintf("%s[%s]",FNAME,"27");
	}
	else if ( FTYPE=="SMALLINT" )
	    FTYPE="short int";
	else if ( FTYPE=="DOUBLE" )
	    FTYPE="double";
	else if ( FTYPE=="DECIMAL" )
	    FTYPE="float";
	else
	    FTYPE="unknown";

	if (FTYPE!="unknown")
	{
	    printf "      %s,\n", AUX > OUTFILE;
	    printf "   %-30s _%s;\n", tolower(FTYPE), tolower(FNAME) > TMPFILE;
	}
    }
}

function create_struct_header()
{ print "// Automatically generated: don't edit it" >OUTFILE
  printf"#ifndef _%s_H\n",TABLENAME >OUTFILE
  printf"#define _%s_H\n\n",TABLENAME >OUTFILE
  print "#include <db/include/api/tableapi/dbbasic.h>" > OUTFILE
  print "#include <db/include/api/tableapi/DBObj.h>\n" > OUTFILE
  print "namespace BGQDB {\n" > OUTFILE
  printf "class %s;\n", TABLENAME>OUTFILE
  printf"\n// %s class  definition\n", TABLENAME>OUTFILE
  printf"class %s : public DBObj {\npublic:\n", TABLENAME>OUTFILE
  printf  "  typedef boost::shared_ptr<%s> Ptr;\n", TABLENAME>OUTFILE
  print "  enum {" >OUTFILE
}

function create_public()
{
  FNAME=sprintf("N_%s_ATTR", SHORTTB)
  printf "      %s\n   };\n", FNAME> OUTFILE

  printf "\n   explicit %s( const BGQDB::ColumnsBitmap& cols = BGQDB::ColumnsBitmap() );\n", TABLENAME > TMPFILE

  if (FLAG=="T")   # it's table - update/delete/insert are not allowed for views
  { print  "   const std::string getDeleteStatement(bool bykey);" > TMPFILE
    print  "   const std::string getInsertStatement();" > TMPFILE
    print  "   const std::string getUpdateStatement(bool bykey);" > TMPFILE
  }
  print  "   const std::string getSelectStatement(bool bykey);" > TMPFILE
  print  "   SQLRETURN bind_col(SQLHANDLE hdbc, SQLHANDLE hstmt);"    >TMPFILE
  print  "   SQLRETURN bind_param(SQLHANDLE hdbc, SQLHANDLE hstmt);"  >TMPFILE
  print  "   SQLRETURN fetch_col( SQLHANDLE hstmt, SQLHANDLE  hdbc);" >TMPFILE
  printf "   const std::type_info* typeinfo() {return &typeid(%s); };\n",TABLENAME >TMPFILE
  printf "   const char* getTableName() const {return \"BGQ%s\"; };\n",SHORTTB >TMPFILE
  print  "   void dump(std::ostream& out=std::cout);\n" >TMPFILE
  print  "   unsigned getColumnCount() const;\n" >TMPFILE
  print  "   ColumnNames calcColumnNames() const;\n" >TMPFILE
}


function create_struct_footer()
{
  print "\n" >TMPFILE
  print "};" >TMPFILE
  print "\n" >TMPFILE
  print "} // BGQDB\n" >TMPFILE
  print "\n" >TMPFILE
  print "#endif" >TMPFILE
}

function create_private()
{
  print "private:" > TMPFILE
  print "   std::string buildFieldString(int *count, bool includeKey=false);" > TMPFILE
}

{
  SHORTTB=toupper(substr(TABLENAME,4,length(TABLENAME)))
  create_struct_header()
  create_struct_el()
  create_public()
  create_private()
  create_struct_footer()
}
