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
# create bgp schemas: tables, synonyms, etc
# parameteres: database name, user and password.
# default: BGDB0 for database name; userid running this script as dabase user
#          if userid is provided, password should be also provided
#
function create_file_header()
{ print "// Automatically generated: don't edit it\n">F1FILE
  print "#include <iostream>" >F1FILE
  print "#include <stdio.h>" > F1FILE
  print "#include <string>" > F1FILE
  print "#include <sql.h>" > F1FILE
  print "#include <sqlext.h>" > F1FILE
  print "#include <sqltypes.h>" > F1FILE
  printf "#include \"tableapi/gensrc/%s.h\"\n", TABLENAME>F1FILE
  print "#include \"tableapi/utilcli.h\"">F1FILE
  print "#include \"tableapi/DBConnection.h\"" >F1FILE
  print "#include \"tableapi/dbdataconv.h\"\n\n">F1FILE
  print "using std::string;\n" > F1FILE
  print "using std::ostream;\n" > F1FILE
  print "using std::endl;\n" > F1FILE
  print "using namespace BGQDB;\n" > F1FILE
  print "\n" > F1FILE
  printf "unsigned %s::getColumnCount() const { return N_%s_ATTR; }\n\n", TABLENAME, SHORTTB > F1FILE
  print "\n" > F1FILE
}

function create_f1_header()
{  BCCLINE="\n             sqlrc=SQLBindCol( hstmt, ++pos, %s, %s, %d,&_ind[i]); break;\n"
   BCILINE="\n             sqlrc=SQLBindCol( hstmt, ++pos, %s, &%s, %d,&_ind[i]); break;\n"

   printf  SEPLINE, "bind_col()" > F1FILE
   printf "SQLRETURN %s::bind_col(SQLHANDLE hdbc, SQLHANDLE hstmt) {\n", TABLENAME >F1FILE
   print  "SQLRETURN         sqlrc=SQL_SUCCESS;\n" >F1FILE
   print  "if ( _columns == 0x0) return SQL_ERROR;" > F1FILE
   print  "int pos=0;" > F1FILE
   print  FORL >F1FILE
   printf "%28s",BITL >F1FILE
   printf "%18s",SWL  >F1FILE
}

function create_f1_body()
{
    printf  "          %s",CASEL >F1FILE;
    if (FTYPE=="CHARACTER" || FTYPE=="VARCHAR" || FTYPE=="CHAR")
    {
	if (CODEPAGE==0)
	    printf BCCLINE,"SQL_C_BINARY", CLASSFNAME, FSIZE > F1FILE;
	else
	    printf BCCLINE,"SQL_C_CHAR", CLASSFNAME, FSIZE+1 > F1FILE;
     }
    else if (FTYPE=="TIMESTAMP" ||FTYPE=="TIMESTMP"  )
        printf BCCLINE,"SQL_C_CHAR", CLASSFNAME, 27 >F1FILE;
    else if (FTYPE=="BIGINT" )
        printf BCILINE,"SQL_C_SBIGINT", CLASSFNAME, FSIZE >F1FILE;
    else if ( FTYPE=="INTEGER")
        printf BCILINE,"SQL_C_LONG", CLASSFNAME, FSIZE >F1FILE;
    else if ( FTYPE=="SMALLINT")
        printf BCILINE,"SQL_C_SHORT", CLASSFNAME, FSIZE >F1FILE;
    else if ( FTYPE=="DOUBLE")
        printf BCILINE,"SQL_C_DOUBLE", CLASSFNAME, FSIZE >F1FILE;
    else if ( FTYPE=="DECIMAL")
        printf BCILINE,"SQL_C_FLOAT", CLASSFNAME, FSIZE >F1FILE;
    else  print "sql type unknown"  >F1FILE
}

function create_f1_footer()
{    print "         default:break;\n       }"  >F1FILE
     printf "       %s", HANDLEL >F1FILE
     print "    }\n}\nreturn sqlrc;\n}\n" >F1FILE
}

function create_f2_header()
{   F2CLINE="  if (_ind[i] == -1) { %s[0]='\\0'; } break;\n";
    F2CLINE2="  if (_ind[i] == -1) { %s[0]='\\0'; } else { _trim_spaces(%s,%s); } break;\n";

    BITL1="     if ( (1ULL<<i) & _columns ) {"
    printf  SEPLINE, "fetch_col()" > F2FILE
    printf "SQLRETURN %s::fetch_col(SQLHANDLE hdbc, SQLHANDLE hstmt) {\n", TABLENAME >F2FILE
    printf "SQLRETURN         sqlrc;\n\n" >F2FILE
    printf "sqlrc = SQLFetch(hstmt);\n" > F2FILE
    print  HANDLEL >F2FILE
    printf "if (sqlrc!=SQL_NO_DATA_FOUND) {\n" >F2FILE
    printf "%45s\n",FORL   >F2FILE
    printf "%35s\n",BITL1  >F2FILE
    printf "%20s",SWL    >F2FILE
}

function create_f2_body()
{
    if (FTYPE=="CHARACTER" || FTYPE=="VARCHAR" || FTYPE=="TIMESTAMP" || FTYPE=="TIMESTMP"|| FTYPE=="CHAR")
    {  if (FSIZE>1 && CODEPAGE!=0)
       {
           printf "            %s",CASEL  >F2FILE
           if (FTYPE=="CHARACTER" || FTYPE=="CHAR")
           {
               printf F2CLINE2, CLASSFNAME, CLASSFNAME, FSIZE >F2FILE
           } else {
               printf F2CLINE, CLASSFNAME >F2FILE
           }
       }
    }
}

function create_f2_footer()
{    printf "            %s",DEFL  >F2FILE
     print "\n}\nreturn sqlrc;\n}\n" >F2FILE
}

function create_f3_header()
{  F3CLINE="\n              out << \"%-25s \" << \"= \" << %s << endl; break;\n";
   F3BLINE="\n              out << \"%-25s \" << \"= \" << bitDataToChar64(%s,%d) << endl; break;\n";
   BITL2="     if ( ((1ULL<<i) & _columns) && (_ind[i] != -1) ) { \n"
   printf  SEPLINE, "dump(ostream& out)" > F3FILE
   printf "void %s::dump(ostream& out) {\n",  TABLENAME >F3FILE
   print  "if ( _columns == 0x0) return;" > F3FILE
   printf "out << \"\\n\\n==> %s record \" << endl;\n", TABLENAME >F3FILE
   print  FORL         >F3FILE
   printf "%28s",BITL2 >F3FILE
   printf "%18s",SWL   >F3FILE
}

function create_f3_body()
{  printf "          %s",CASEL                    >F3FILE
   if ((FTYPE=="CHARACTER" || FTYPE=="VARCHAR" || FTYPE=="CHAR") && (CODEPAGE==0) )
   {
       printf F3BLINE, CLASSFNAME, CLASSFNAME, FSIZE >F3FILE
   } else
       printf F3CLINE, CLASSFNAME, CLASSFNAME >F3FILE
}

function create_f3_footer()
{  printf "         %s",DEFL >F3FILE
   print "\n}\n"               >F3FILE
}

function create_f4_header()
{
   printf  SEPLINE, "constructor" > F4FILE
   printf "%s::%s( const BGQDB::ColumnsBitmap& cols ) : DBObj( cols ) {\n",TABLENAME,TABLENAME >F4FILE
}
function create_f4_body()
{ FORMAT4="\"%s\""
  if ( DEFVALUE != "-" ) {
     if (FTYPE=="CHARACTER" || FTYPE=="VARCHAR"|| FTYPE=="CHAR")
        printf "sprintf(%s,%s,\"%s\");\n", CLASSFNAME,FORMAT4,substr(DEFVALUE,2,(length(DEFVALUE)-2)) >F4FILE
     else if (FTYPE=="BIGINT" || FTYPE=="INTEGER")
        printf "%s=%s;\n", CLASSFNAME,DEFVALUE >F4FILE
    # else
    #    print "unknown" >F4FILE
   }
}

function create_f4_footer()
{
   print "}\n" >F4FILE
}

function create_f5_header()
{  BPCCLINE="\n             sqlrc=SQLBindParameter( hstmt, ++pos, SQL_PARAM_INPUT, %s, %s, %d, %d, %s, strlen(%s) ,NULL);break;\n"
   BPCILINE="\n             sqlrc=SQLBindParameter( hstmt, ++pos, SQL_PARAM_INPUT, %s, %s, %d, %d, &%s, %d, NULL);break;\n"
   BPCBLINE="\n             _ind[i]=%d;\n             sqlrc=SQLBindParameter( hstmt, ++pos, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, %d, %d, %s, %d, &_ind[i]);break;\n"
   printf  SEPLINE, "bind_param" > F5FILE
   printf "SQLRETURN %s::bind_param(SQLHANDLE hdbc, SQLHANDLE hstmt) {\n", TABLENAME >F5FILE
   print  "SQLRETURN         sqlrc = SQL_SUCCESS;\n" >F5FILE
   print  "if ( _columns == 0x0) return SQL_ERROR;" >F5FILE
   if (HASBD == "T")
     print  "int pos=0;"  >F5FILE
   else
     print  "int pos=0; "  >F5FILE
   print  FORL >F5FILE
   printf "%28s",BITL >F5FILE
   printf "%18s",SWL  >F5FILE
}
function create_f5_body()
{
    printf  "          %s",CASEL >F5FILE;
    if (FTYPE=="CHARACTER" || FTYPE=="VARCHAR"|| FTYPE=="CHAR")
	if (CODEPAGE==0)
	    printf BPCBLINE, FSIZE, FSIZE, 0, CLASSFNAME, FSIZE >F5FILE;
	else
	    printf BPCCLINE,"SQL_C_CHAR", "SQL_CHAR", FSIZE, 0,CLASSFNAME, CLASSFNAME >F5FILE;

     else if (FTYPE=="TIMESTAMP" || FTYPE=="TIMESTMP")
	 printf BPCCLINE,"SQL_C_CHAR", "SQL_TIMESTAMP", 0,0,CLASSFNAME,CLASSFNAME  >F5FILE;
     else if (FTYPE=="BIGINT" )
	 printf BPCILINE,"SQL_C_SBIGINT","SQL_BIGINT", 0,0,CLASSFNAME, 0                  >F5FILE;
     else if ( FTYPE=="INTEGER")
	 printf BPCILINE,"SQL_C_LONG", "SQL_INTEGER",0,0,CLASSFNAME, 0                 >F5FILE;
     else if ( FTYPE=="SMALLINT")
	 printf BPCILINE,"SQL_C_SHORT", "SQL_SMALLINT",0,0,CLASSFNAME, 0                 >F5FILE;
    else if ( FTYPE=="DOUBLE")
	printf BPCILINE,"SQL_C_DOUBLE", "SQL_DOUBLE",0,0,CLASSFNAME, 0                 >F5FILE;
    else if ( FTYPE=="DECIMAL")
	printf BPCILINE,"SQL_C_FLOAT", "SQL_DECIMAL",0,0,CLASSFNAME, 0                 >F5FILE;
    else  print "sql type unknown"                                                       >F5FILE;
}
function create_f5_footer()
{    print "         default:break;\n       }"                      >F5FILE
     printf "       %s", HANDLEL                                    >F5FILE
     print "    }\n}\n"                          >F5FILE
     print "return sqlrc;\n}\n"                                     >F5FILE
}


function create_f6_header()
{  if (FLAG =="T")
   { printf  SEPLINE, "getInsertStatement()"                                              >F6FILE
     printf "const string %s::getInsertStatement() {\n",TABLENAME                         >F6FILE
     print "if ( _columns == 0x0) return NULL;"                                             >F6FILE
     print "int count=0;"                                                                 >F6FILE
     printf "string sql(\"insert into BGQ%s (\");\n",SHORTTB                                >F6FILE
     print "sql+=buildFieldString(&count,true);"                                          >F6FILE
     print "sql+=\") values (\";"                                                         >F6FILE
     print "for (int i=0; i< count-1; i++) sql+=\"?,\";"                                  >F6FILE
     print "sql+=\"?)\";"                                                                 >F6FILE
     print "return sql;\n}"                                                       >F6FILE


     printf  SEPLINE, "getDeleteStatement()"                                              >F6FILE
     printf "const string %s::getDeleteStatement(bool bykey) {\n",TABLENAME               >F6FILE
     if ( KEYNAME == "")
       print "if (bykey) return NULL;" >F6FILE
     else
     { print "if (bykey) {"  >F6FILE
       if (KEYTYPE=="CHAR" || KEYTYPE=="VARCHAR" )
        {
        if (KEYCODEPAGE == 0)
           {
             printf "   string sql(\"delete from BGQ%s where %s=x'\");\n",SHORTTB,KEYNAME >F6FILE
             printf "   sql+= bitDataToChar(_%s,%d);\n", KEYNAME, KEYLEN               >F6FILE
           } else {
             printf "   if (_%s[0] == '\\0' ) return NULL;\n", KEYNAME                    >F6FILE
             printf "   string sql(\"delete from BGQ%s where %s='\");\n",SHORTTB,KEYNAME  >F6FILE
             printf "   sql+= _%s;\n", KEYNAME                                            >F6FILE
           }
             print  "   sql+=\"'\";"                                                      >F6FILE
        } else {
           printf "   if (_%s<0 ) return NULL;\n", KEYNAME                                >F6FILE
           printf "   string sql(\"delete from BGQ%s where %s=\");\n",SHORTTB,KEYNAME     >F6FILE
           print  "   char buf[32];"                                                      >F6FILE
           AUX="%d"
           printf  "   sprintf(buf,\"%s\",_%s);\n",AUX, KEYNAME                           >F6FILE
           print   "   sql+=buf;"                                                         >F6FILE
        }
        print "   return sql;\n}"                                                 >F6FILE
     }
     printf "return (\"delete from BGQ%s \");\n}\n",SHORTTB  >F6FILE

     printf  SEPLINE, "getUpdateStatement()"                                              >F6FILE
     printf "const string %s::getUpdateStatement(bool bykey) {\n",TABLENAME               >F6FILE
     if ( KEYNAME == "")
       print "if (bykey) return NULL;" >F6FILE

      printf "string sql(\"update BGQ%s set (\");\n",SHORTTB                                 >F6FILE
      print "int count;"                                                                   >F6FILE
      print "sql+=buildFieldString(&count,false);"                                         >F6FILE
      print "sql+=\")=(\";"                                                                >F6FILE
      print "for (int i=0; i< count-1; i++) sql+=\"?,\";"                                  >F6FILE
      print "sql+=\"?)\";" >F6FILE
      if ( KEYNAME != "")
      {
	  print "if (bykey) {"  >F6FILE
         if (KEYTYPE=="CHAR" || KEYTYPE=="VARCHAR" )
           if (KEYCODEPAGE == 0)
               printf "   sql+=\" where %s=x'\"+bitDataToChar(_%s,%d)+string(\"'\");\n}\n",KEYNAME,KEYNAME,KEYLEN >F6FILE
           else
               printf "   sql+=\" where %s='\"+string(_%s)+string(\"'\");\n}\n",KEYNAME,KEYNAME >F6FILE
         else  {
           print  "   char buf[32];"                                                             >F6FILE
           AUX="%d"
           printf  "   sprintf(buf,\"%s\",_%s);\n",AUX, KEYNAME                                          >F6FILE
           printf  "   sql+=\" where %s=\"+string(buf);\n}\n",KEYNAME >F6FILE
        }
        printf "if ( (1ULL<<%s) & _columns ) _columns-=(1ULL<<%s);\n", toupper(KEYNAME), toupper(KEYNAME)   >F6FILE
     }
     print "return sql;\n}"                                                       >F6FILE
   }
   printf  SEPLINE, "getSelectStatement()"       >F6FILE
   printf "const string %s::getSelectStatement(bool bykey) {\n",TABLENAME                         >F6FILE
   print  "if (_columns == 0x0) return NULL;"  >F6FILE
   if ( KEYNAME == "")
     print "if (bykey) return NULL;" >F6FILE

   print  "string sql;"  >F6FILE
   if (FLAG =="T")
     printf "if (_columns >= (1ULL<<N_%s_ATTR)-1) sql= \"select * from TBGQ%s \";\n",SHORTTB, SHORTTB >F6FILE
   else
     printf "if (_columns >= (1ULL<<N_%s_ATTR)-1) sql= \"select * from BGQ%s \";\n",SHORTTB, SHORTTB >F6FILE

   print  "else {\n   int count=0;"                                                        >F6FILE
   print  "   sql=\"select \";"                                                     >F6FILE
   print  "   sql+=buildFieldString(&count,true);"                                          >F6FILE
   printf "   sql+=\" from BGQ%s\";\n}\n", SHORTTB                                            >F6FILE
   if ( KEYNAME != "")
   {
       print "if (bykey) {"  >F6FILE
      if (KEYTYPE=="CHAR" || KEYTYPE=="VARCHAR" )
         if (KEYCODEPAGE==0)
            printf "   sql+=\" where %s=x'\"+bitDataToChar(_%s,%d)+string(\"'\");\n}\n",KEYNAME,KEYNAME,KEYLEN >F6FILE
         else
            printf "   sql+=\" where %s='\"+string(_%s)+string(\"'\");\n}\n",KEYNAME,KEYNAME >F6FILE
       else  {
         print  "   char buf[32];"                                                             >F6FILE
         AUX="%d"
         printf  "   sprintf(buf,\"%s\",_%s);\n",AUX, KEYNAME                                          >F6FILE
         printf  "   sql+=\" where %s=\"+string(buf);\n}\n",KEYNAME >F6FILE
      }
   }

   print "return sql;\n}"                                                       >F6FILE

   printf  SEPLINE, "buildFieldString()"                                                >F6FILE
   printf "string %s::buildFieldString(int *count, bool includeKey) {\n",TABLENAME  >F6FILE
   print  "int n=0;"                                                                     >F6FILE
   print  "string sql;"                                                                 >F6FILE
   print   FORL                                                                         >F6FILE
   printf "%28s",BITL                                                                    >F6FILE
   printf "%18s",SWL                                                                     >F6FILE
}

function create_f6_body()
{    printf  "          %s",CASEL >F6FILE
     if (FNAME==KEYNAME)
       printf "\n             if (includeKey) sql+=(n++) ? \",%s\" : \"%s\"; break;\n",FNAME,FNAME  >F6FILE
     else
       printf "\n             sql+=(n++) ? \",%s\" : \"%s\"; break;\n",FNAME,FNAME  >F6FILE
}

function create_f6_footer()
{    print "         default:break;\n      }\n   }\n}\n"  >F6FILE
     print "(*count)=n;"                        >F6FILE
     print "return sql;\n}"             >F6FILE
}

function create_f7()
{
   printf "const std::string %s::%s_COL(\"%s\");\n", TABLENAME, toupper(FNAME), FNAME >F7FILE
}

function create_f8_header()
{
   printf  SEPLINE, "calcColumnNames()" >F8FILE
   printf "%s::ColumnNames %s::calcColumnNames() const {\n", TABLENAME, TABLENAME >F8FILE
   print  "  ColumnNames ret;\n" >F8FILE
   printf "  for ( unsigned i=0 ; i < N_%s_ATTR ; ++i ) {\n", SHORTTB >F8FILE
   print  "    if ( (1ULL << i) & _columns ) {\n" > F8FILE
   print  "      switch ( i ) {\n" > F8FILE
}

function create_f8_body()
{
   printf "        case %s: ret.push_back( \"%s\" ); break;\n", toupper(FNAME), FNAME >F8FILE
}

function create_f8_footer()
{
   print  "        default: break;\n" >F8FILE
   print  "      }\n" >F8FILE
   print  "    }\n" >F8FILE
   print  "  }\n" >F8FILE
   print  "  return ret;\n" >F8FILE
   print  "}\n" >F8FILE
}


#### main
### for now I'm assuming all tables has keys and only one key
### FILENAME=="tablekeys" {TABLEKEY[$1]=$3;next}
function set_key()
{
    KEYNAME="";
    if (FLAG!="T") 		# this is not a table, therefore, it doesn't have a key
	return;
    while ((getline<"tablekeys") >0 && KEYNAME=="")
    { FTABLE=$1
      FTYPE=$2
      FNAME=$3
      KEYLEN=$4
      KEYCODEPAGE=$5
      if (SHORTTB==substr(FTABLE,5,length(FTABLE)))
      {  KEYNAME=tolower(FNAME)
         KEYTYPE=FTYPE
      }
    }
}

{
  print $0
  SEPLINE="/* ==============   %s   ===============  */\n\n"
  SHORTTB=toupper(substr(TABLENAME,4,length(TABLENAME)))
  DEFL="default: break;\n         }\n      }\n   }"
  HANDLEL="STMT_HANDLE_CHECK( hstmt, hdbc, sqlrc);\n"
  FORL=sprintf("for (unsigned i=0; i< N_%s_ATTR; i++)  {", SHORTTB )
  BITL="if ( (1ULL<<i) & _columns ) {\n"
  SWL="switch(i) {\n"
  REALTNAME=sprintf("TBGQ%s",SHORTTB)
  set_key()

  create_file_header()
  create_f1_header()
  create_f2_header()
  create_f3_header()
  create_f4_header()
  create_f5_header()
  create_f6_header()
  create_f8_header()
  while ((getline<DBFILE) >0)
     {  FTYPE=$2
        FSIZE=$3
        FNAME=tolower($4)
        CASEL=sprintf("case %s:", $4)
        CLASSFNAME=sprintf("_%s",tolower($4))
        ISTKEY=$5
        DEFVALUE=$6
        CODEPAGE=$7
        create_f1_body();
        create_f2_body();
        create_f3_body();
        create_f4_body();
        create_f5_body();
        create_f6_body();
        create_f7();
        create_f8_body();
     }
  create_f1_footer()
  create_f2_footer()
  create_f3_footer()
  create_f4_footer()
  create_f5_footer()
  create_f6_footer()
  create_f8_footer()
}
