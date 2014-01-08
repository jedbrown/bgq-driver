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

#ifndef __BG_PARM_H__
#define __BG_PARM_H__

#include <stdint.h>
#include <vector>
#include <map>
#include <string.h>
#include "string_tokenizer.h"

/*!
// 
// parameter error, derived from std::exception similar to runtime error
//
*/
class bg_param_error : public std::exception 
{

public:
    /** Takes a character string describing the error.  */
    explicit  bg_param_error(const std::string&  __arg) : 
        exception(), _M_msg(__arg) { };
    virtual  ~bg_param_error() throw() {};
    /** Returns a C-style character string describing the general cause of
     *  the current error (the same string passed to the ctor).  */
    virtual const char* what() const throw() {
        return _M_msg.c_str();};
private:
    std::string _M_msg;
};

class BgParam
{
public:
    BgParam();
    virtual ~BgParam();

    /*!
    // parse the program arguments.
    //
    // @param argc -- argc from main
    // @param argv -- argv from main
    // @returns none
    //
    // throws BgParam_error
    */
    void parseArgs( int argc, char *argv[] );

     /*!
    // parse the program arguments.
    //
    // @param sExeName -- name of executable
    // @param options  -- option string to parse
    // @returns none
    //
    // throws BgParam_error
    */
    void parseOptions(std::string sExeName="", std::string options="" );

    /*!
    // return the non option argument count
    // @param none.
    // @returns argument count
    */
    unsigned argc();    

    /*!
    // return the positional argument n
    // @param n
    // @return positional argument.
    */
    std::string argv(unsigned n);

    /*!
    // Return the exename saved when parseArgs was called.
    // @param none.
    // @returns -- exe name
    */
    std::string getExeName() {
        return(m_exeName); };
    /*!
    // parse parameter value.
    //
    // parameters are in the form of key=value
    // keys with out values or = signs are assumed to be
    // key=true
    //
    // @param param -- param to parse
    // @returns none,  
    //    throws BgParam_error
    */
    void parseParm(const std::string &param);
    
    /*!
    // Set the value of the key specified.
    //
    // @param key -- key of the value to set
    // @param value -- value to set it to
    //
    // @returns none,
    // throws BgParam_error
    */
    void setDefaultStr(const std::string &key, const std::string &value);
    /*!
    // Set the value of the key specified.
    //
    // @param key -- key of the value to set
    // @param value -- value to set it to
    //
    // @returns none,
    // throws BgParam_error
    */
    void setDefaultLong(const std::string &key, long value);
    /*!
    // Set the value of the key specified.
    //
    // @param key -- key of the value to set
    // @param value -- value to set it to
    //
    // @returns none,
    // throws BgParam_error
    */
    void setDefaultBool(const std::string &key, bool value);
    /*!
    // Set the value of the key specified.
    //
    // @param key -- key of the value to set
    // @param value -- value to set it to
    //
    // @returns none,
    // throws BgParam_error
    */
    void setDefaultDouble(const std::string &key, double value); 

    /*!
    // Set the value of the key specified.
    //
    // @param key -- key of the value to set
    // @param value -- value to set it to
    //
    // @returns none,
    // throws BgParam_error
    */
    void setValueStr(const std::string &key, const std::string &value);
    /*!
    // Set the value of the key specified.
    //
    // @param key -- key of the value to set
    // @param value -- value to set it to
    //
    // @returns none,
    // throws BgParam_error
    */
    void setValueLong(const std::string &key, long value);
    /*!
    // Set the value of the key specified.
    //
    // @param key -- key of the value to set
    // @param value -- value to set it to
    //
    // @returns none,
    // throws BgParam_error
    */
    void setValueBool(const std::string &key, bool value);
    /*!
    // Set the value of the key specified.
    //
    // @param key -- key of the value to set
    // @param value -- value to set it to
    //
    // @returns none,
    // throws BgParam_error
    */
    void setValueDouble(const std::string &key, double value); 

    /*!
    // Retrieve a previously stored value.
    // @param key -- key of the value to set
    // @param value -- retrieved value
    //
    */
    std::string getValueStr(const std::string &key);

    /*!
    // Retrieve a previously stored value as an vector. Example: -key=a,b,c
    // @param key -- key of the value to set
    // @param separator -- the character/string separator for the same key. 
    //
    */
    std::vector<std::string> getValueVectorStr(const std::string &key, const std::string &separator=",");

    /*!
    // Convert string parm to vector of uint32_t values
    // @param key -- key of the value to set
    // @param separator -- the character/string sparator for the same key.
    */
    std::vector<uint32_t> getValueVectorUint32(const std::string &key, const std::string &separator=",");

    /*!
    // Retrieve a previously stored value.
    // @param key -- key of the value to set
    // @param value -- retrieved value
    //
    */
    long getValueLong(const std::string &key);
    /*!
    // Retrieve a previously stored value.
    // @param key -- key of the value to set
    // @param value -- retrieved value
    //
    */
    bool getValueBool(const std::string &key);
    /*!
    // Retrieve a previously stored value.
    // @param key -- key of the value to set
    // @param value -- retrieved value
    //
    */
    double getValueDouble(const std::string &key);

    /*!
    // check to see if the parameter list has a key
    // @param  key -- key to search for.
    // @returns true if it has the key
    */
    bool hasKey(const std::string &key);

    std::string showValueBool(const std::string what="", int size=30);
    std::string showValueStr(const std::string what="", int size=30);
    std::string showValueLong(const std::string what="", int len=16, int size=30);
    std::string showValueDouble(const std::string what="", int len=16, int size=30);
    std::string numKeys();
    unsigned numStrings();
    unsigned numLongs();
    unsigned numBools();
    unsigned numDoubles();
    std::string showAll();
  
protected:
    std::vector<std::string> tokenize(const std::string &rStr,
                                      const std::string &szDelimiters);

private:
    typedef enum {
        T_STRING,
        T_LONG,
        T_BOOL,
        T_DOUBLE,
        T_UNKNOWN,
    } KeyType;
    KeyType keyType(const std::string &key);
    typedef std::map<std::string, KeyType> TypeMap;
    typedef std::map<std::string, std::string> StringMap;
    typedef std::map<std::string, long> LongMap;
    typedef std::map<std::string, bool> BoolMap;
    typedef std::map<std::string, double> DoubleMap;


    TypeMap   m_typeMap;         // key to type

    StringMap m_stringMap;       // map of string parameters...
    LongMap   m_longMap;         // key to long
    BoolMap   m_boolMap;         // key to bool
    DoubleMap m_doubleMap;       // key to double

    std::string m_exeName;

    std::vector<std::string> m_argv;        // positional arguments
};


#endif

