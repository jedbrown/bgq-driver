/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2011                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

/*!
// generic parameter manager...
//
*/

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include "BgParm.h"
#include <string.h>
#include <iomanip>
#include <unistd.h>

#ifdef METHOD
#undef METHOD
#endif
#define METHOD "BgParm::"<<__FUNCTION__<<"() "

using namespace std;

BgParam::BgParam()
{
  // all parameter lists get a default parameter.
  //
  setDefaultBool("help", false);        // default values

}
BgParam::~BgParam()
{

}

/*!
// parse the program arguments.
//
// @param argc -- argc from main
// @param argv -- argv from main
// @returns none
//
// throws BgParam_error
*/
void BgParam::parseArgs( int argc, char *argv[] )
{
  m_exeName = argv[0];
  char **parg = argv + 1;
  for (unsigned n = 1; n < (unsigned)argc; n++, parg++)
    {
      if (*parg[0] == '-')            // parse options.
        {   
	  char *p = *parg;
	  p++;                // skip the 1st -
	  if (*p == '-')      // skip the optional -- character..
	    p++;
	  parseParm(p);
	  continue;
        }
      m_argv.push_back(*parg);
    }
}

/*!
// parse the program arguments.
//
// @param options -- options string
// @returns none
//
// throws BgParam_error
*/
void BgParam::parseOptions(const std::string sExeName, const std::string options )
{
  m_exeName = sExeName.c_str();
  StringTokenizer optionVector;
  optionVector.tokenize(options, " ");
  unsigned size=optionVector.size();
 
  for (unsigned n = 0; n < size; n++)
    {
      std::string p(optionVector[n]);
      if (p[0] == '-')         // parse options.
        {   
	  p.erase(p.begin());  // skip the 1st -
	  if (p[0] == '-')      // skip the optional -- character..
	    p.erase(p.begin());
	}
      //cout << __FUNCTION__ << "() Parsing: " << p << endl;
      parseParm(p);
      m_argv.push_back(p);
    }
}

/*!
// return the non option argument count
// @param none.
// @returns argument count
*/
unsigned BgParam::argc()
{
  return(m_argv.size());
}

/*!
// return the positional argument n
// @param n
// @return positional argument.
*/
std::string BgParam::argv(unsigned n)
{
  if (n < m_argv.size())
    return(m_argv[n]);
  else
    return("");
}


/*!
// parse parameter value.
//
// parameters are in the form of key=value
//
// @param param -- param to parse
// @returns none,  
//    throws BgParam_error
*/
void BgParam::parseParm(const std::string &param)
{
  vector<string> tokens;
  tokens = tokenize(param, "=");   
  TypeMap::iterator it;

  if (param.size() == 0)
    throw bg_param_error(string(__FUNCTION__) + "() " + "null option");

  if ((tokens.size() == 0) || (tokens.size() > 2))
    throw bg_param_error(string(__FUNCTION__) + "() " + string("invalid option ") + param);

  it = m_typeMap.find(tokens[0]);
  if (it == m_typeMap.end())
    throw bg_param_error(string(__FUNCTION__) + "() " + string("invalid option ") + param);

  if ((it->second != T_BOOL) && (tokens.size() != 2))
    throw bg_param_error(string(__FUNCTION__) + "() " + string("invalid option (missing \"=\") ") + param);

  string key = tokens[0];
  string value;
  if (tokens.size() > 1)
    value = tokens[1];
  else 
    if (it->second == T_BOOL)           // single parameter bool 
      value = "true";                 // defaults to true
    

  // ok now we have the correct number and form of options.
  // next, check the type
  switch (it->second)
    {
    case T_STRING:
      m_stringMap[key] = value;
      break;
    case T_LONG:
      {
	char *endptr;
	long v;
	if (value.find("0x") == 0)
	  v = strtoul(value.substr(2).c_str(), &endptr, 16);
	else
	  v = strtoul(value.c_str(), &endptr, 10);
	if (*endptr != 0)
	  throw bg_param_error(string(__FUNCTION__) + "() " + string("invalid integer option -") + param);
	m_longMap[key] = v;
	break;
      }
    case T_BOOL:
      if ((value == "true") || (value == "1"))
	m_boolMap[key] = true;
      else
	if ((value == "false") || (value == "0"))
	  m_boolMap[key] = false;
	else
	  throw bg_param_error(string(__FUNCTION__) + "() " + string("invalid bool option -") + param);
      break;
    case T_DOUBLE:
      {
	char *endptr;
	double v = strtod(value.c_str(), &endptr);
	if (*endptr != 0)
	  throw bg_param_error(string(__FUNCTION__) + "() " + string("invalid number option -") + param);
	m_doubleMap[key] = v;
	break;
      }
    default:
      bg_param_error(string(__FUNCTION__) + "() " + "internal error: invalid param type");
    }

}

/*!
// Set the value of the key specified.
//
// @param key -- key of the value to set
// @param value -- value to set it to
//
// @returns none,
// throws BgParam_error
*/
void BgParam::setDefaultStr(const std::string &key, const std::string &value)
{
  ostringstream ostr;
  ostr << "duplicate key: " << key;
  if (hasKey(key))
    throw bg_param_error(string(__FUNCTION__) + "() " + ostr.str());

  m_typeMap[key] = T_STRING;
  m_stringMap[key] = value;
}
/*!
// Set the value of the key specified.
//
// @param key -- key of the value to set
// @param value -- value to set it to
//
// @returns none,
// throws BgParam_error
*/
void BgParam::setDefaultLong(const std::string &key, long value)
{
  ostringstream ostr;
  ostr << "duplicate key: " << key;
  if (hasKey(key))
    throw bg_param_error(string(__FUNCTION__) + "() " + ostr.str());
  m_typeMap[key] = T_LONG;
  m_longMap[key] = value;
}
/*!
// Set the value of the key specified.
//
// @param key -- key of the value to set
// @param value -- value to set it to
//
// @returns none,
// throws BgParam_error
*/
void BgParam::setDefaultBool(const std::string &key, bool value)
{
  if (hasKey(key))
    throw bg_param_error(string(__FUNCTION__) + "() " +
			 "duplicate parameter key: " + key);
  m_typeMap[key] = T_BOOL;
  m_boolMap[key] = value;
}
/*!
// Set the value of the key specified.
//
// @param key -- key of the value to set
// @param value -- value to set it to
//
// @returns none,
// throws BgParam_error
*/
void BgParam::setDefaultDouble(const std::string &key, double value)
{
  ostringstream ostr;
  ostr << "duplicate key: " << key;
  if (hasKey(key))
    throw bg_param_error(string(__FUNCTION__) + "() " + ostr.str());
  m_typeMap[key] = T_DOUBLE;
  m_doubleMap[key] = value;
}
/*!
// Set the value of the key specified.
//
// @param key -- key of the value to set
// @param value -- value to set it to
//
// @returns none,
// throws BgParam_error
*/
void BgParam::setValueStr(const std::string &key, const std::string &value)
{
  StringMap::iterator it = m_stringMap.find(key);
  if (it == m_stringMap.end())
    throw bg_param_error(string(__FUNCTION__) + "() " + string("key \"") + key + "\" not found");
  it->second = value;
  
}
/*!
// Set the value of the key specified.
//
// @param key -- key of the value to set
// @param value -- value to set it to
//
// @returns none,
// throws BgParam_error
*/
void BgParam::setValueLong(const std::string &key, long value)
{
  LongMap::iterator it = m_longMap.find(key);
  if (it == m_longMap.end())
    throw bg_param_error(string(__FUNCTION__) + "() " + string("key \"") + key + "\" not found");
  it->second = value;
}
/*!
// Set the value of the key specified.
//
// @param key -- key of the value to set
// @param value -- value to set it to
//
// @returns none,
// throws BgParam_error
*/
void BgParam::setValueBool(const std::string &key, bool value)
{
  BoolMap::iterator it = m_boolMap.find(key);
  if (it == m_boolMap.end())
    throw bg_param_error(string(__FUNCTION__) + "() " + string("key \"") + key + "\" not found");
  it->second = value;
  
}
/*!
// Set the value of the key specified.
//
// @param key -- key of the value to set
// @param value -- value to set it to
//
// @returns none,
// throws BgParam_error
*/
void BgParam::setValueDouble(const std::string &key, double value)
{
  DoubleMap::iterator it = m_doubleMap.find(key);
  if (it == m_doubleMap.end())
    throw bg_param_error(string(__FUNCTION__) + "() " + string("key \"") + key + "\" not found");
  it->second = value;
}

/*!
// Retrieve a previously stored value.
// @param key -- key of the value to set
// @param value -- retrieved value
//
*/
std::string BgParam::getValueStr(const std::string &key)
{
  StringMap::iterator it = m_stringMap.find(key);
  if (it == m_stringMap.end())
    throw bg_param_error(string(__FUNCTION__) + "() " + string("key \"") + key + "\" not found");
  return(it->second);
}
/*!
// Retrieve a previously stored value as an vector. Example: -key=a,b,c
// @param key -- key of the value to set
// @param separator -- the character/string separator for the same key. 
//
*/
std::vector<string> BgParam::getValueVectorStr(const std::string &key, const std::string &separator) {
  StringMap::iterator it = m_stringMap.find(key);
  vector<std::string> fields;
    
  if (it == m_stringMap.end())
    throw bg_param_error(string(__FUNCTION__) + "() " + string("key \"") + key + "\" not found");
    
  fields = tokenize(it->second, separator);   
  return fields;
}

/*!
// Convert string parm to vector of uint32_t values
// @param key -- key of the value to set
// @param separator -- the character/string sparator for the same key.
*/
std::vector<uint32_t> BgParam::getValueVectorUint32(const std::string &key, const std::string &separator) {
  //StringMap::iterator it = m_stringMap.find(key);
  vector<std::string> fields;

  //if (it == m_stringMap.end())
  //  throw bg_param_error(string(__FUNCTION__) + "() " + string("key \"") + key + "\" not found");
    
  fields = tokenize(key, separator);
  //cout << "Size of string vector: " << dec << fields.size() << endl;
    
  vector<uint32_t> fields_conv(fields.size());
  char *endptr;

  for (uint i=0; i < fields.size(); i++)
    {
      fields_conv[i] = strtoul(fields[i].c_str(), &endptr, 10);
      //  cout << "Uint32 vector values: " << fields_conv[i] << endl;
    }

  return fields_conv;

}

/*!
// Retrieve a previously stored value.
// @param key -- key of the value to set
// @param value -- retrieved value
//
*/
long BgParam::getValueLong(const std::string &key)
{
  LongMap::iterator it = m_longMap.find(key);
  if (it == m_longMap.end())
    throw bg_param_error(string(__FUNCTION__) + "() " + string("key \"") + key + "\" not found");
  return(it->second);
}
/*!
// Retrieve a previously stored value.
// @param key -- key of the value to set
// @param value -- retrieved value
//
*/
bool BgParam::getValueBool(const std::string &key)
{
  BoolMap::iterator it = m_boolMap.find(key);
  if (it == m_boolMap.end())
    throw bg_param_error(string(__FUNCTION__) + "() " + string("key \"") + key + "\" not found");
  return(it->second);
}
/*!
// Retrieve a previously stored value.
// @param key -- key of the value to set
// @param value -- retrieved value
//
*/
double BgParam::getValueDouble(const std::string &key)
{
  DoubleMap::iterator it = m_doubleMap.find(key);
  if (it == m_doubleMap.end())
    throw bg_param_error(string(__FUNCTION__) + "() " + string("key \"") + key + "\" not found");
  return(it->second);
}

/*!
// check to see if the parameter list has a key
// @param  key -- key to search for.
// @returns true if it has the key
*/
bool BgParam::hasKey(const std::string &key)
{
  return(m_typeMap.find(key) != m_typeMap.end());


}

/*!
// Retrieve the key type associated with a given key.
//
// @param key -- key to look up in the type map
// @returns keytype
*/
BgParam::KeyType BgParam::keyType(const std::string &key)
{
  TypeMap::iterator it = m_typeMap.find(key);
  if (it != m_typeMap.end())
    return(it->second);
  return(T_UNKNOWN);
}

/*!
// String tokenizer
//
// This takes the string and breaks it into a vector of tokens
//
// @param rStr [in] string to break up into tokens.
// @param szDelimiters [in] delimiters to use to break the string up.
//
// @returns vector of string tokens.
*/
vector<string> BgParam::tokenize(const string &rStr,
				 const string &szDelimiters)
{
  vector<string> words;
    
  string::size_type lastPos(rStr.find_first_not_of(szDelimiters, 0));
  string::size_type pos(rStr.find_first_of(szDelimiters, lastPos));
  while (string::npos != pos || string::npos != lastPos)
    {
      words.push_back(rStr.substr(lastPos, pos - lastPos));
      lastPos = rStr.find_first_not_of(szDelimiters, pos);
      pos = rStr.find_first_of(szDelimiters, lastPos);
    }
  return words;
}

string BgParam::showValueBool(string key, int size)
{
  ostringstream ossTemp("");
  ostringstream ostr1("");
  ostr1 << key << " = ";
  ossTemp << setw(size) << setfill(' ') << ostr1.str() << ""
	  << ((getValueBool(key)==true) ? "true" : "false");
  return(ossTemp.str());
}

string BgParam::showValueStr(string key, int size)
{
  ostringstream ossTemp("");
  ostringstream ostr1("");
  ostr1 << key << " = ";
  ossTemp << setw(size) << setfill(' ') << ostr1.str()
	  << getValueStr(key);
  return(ossTemp.str());
}

string BgParam::showValueLong(string key, int len, int size)
{
  ostringstream ossTemp("");
  ostringstream ostr1("");
  ostr1 << key << " = ";
  ossTemp << setw(size) << setfill(' ') << ostr1.str() << ""
	  << dec << getValueLong(key) << " = 0x"
	  << hex << setw(len) << setfill('0') << getValueLong(key);
  return(ossTemp.str());
}

string BgParam::showValueDouble(string key, int len, int size)
{
  ostringstream ossTemp("");
  ostringstream ostr1("");
  ostr1 << key << " = ";
  ossTemp << setw(size) << setfill(' ') << ostr1.str() << ""
	  << dec << getValueDouble(key) << " = 0x"
	  << hex << setw(len) << setfill('0') << getValueDouble(key);
  return(ossTemp.str());
}

string BgParam::numKeys()
{
  ostringstream ostr("");
  ostr << METHOD << "Found "
       << dec << numStrings() << " string(s), "
       << dec<< numLongs() << " long(s), "
       << dec<< numBools() << " bool(s), "
       << dec<< numDoubles() << " double(s)."
       << endl;
  return(ostr.str());
}

unsigned BgParam::numStrings()
{
  unsigned n=0;
  for(StringMap::iterator it = m_stringMap.begin();
      it != m_stringMap.end(); ++it)
    {
      n++;
    }
  return(n);
}
unsigned BgParam::numLongs()
{
  unsigned n=0;
  for(LongMap::iterator it = m_longMap.begin();
      it != m_longMap.end(); ++it)
    {
      n++;
    }
  return(n);
}
unsigned BgParam::numBools()
{
  unsigned n=0;
  for(BoolMap::iterator it = m_boolMap.begin();
      it != m_boolMap.end(); ++it)
    {
      n++;
    }
  return(n);
}

unsigned BgParam::numDoubles()
{
  unsigned n=0;
  for(DoubleMap::iterator it = m_doubleMap.begin();
      it != m_doubleMap.end(); ++it)
    {
      n++;
    }
  return(n);
}

string BgParam::showAll()
{
  ostringstream ostr("");
  ostr << METHOD << "Showing all available parameter values:" << endl;
  // print strings, long, bool, double
  for(StringMap::iterator it = m_stringMap.begin();
      it != m_stringMap.end(); ++it)
    {
      ostr << showValueStr(it->first) << endl;
    }
  for(LongMap::iterator it = m_longMap.begin();
      it != m_longMap.end(); ++it)
    {
      ostr << showValueLong(it->first) << endl;
    }
  for(BoolMap::iterator it = m_boolMap.begin();
      it != m_boolMap.end(); ++it)
    {
      ostr << showValueBool(it->first) << endl;
    }
  for(DoubleMap::iterator it = m_doubleMap.begin();
      it != m_doubleMap.end(); ++it)
    {
      ostr << showValueDouble(it->first) << endl;
    }
  ostr << numKeys();
  return(ostr.str());
}
