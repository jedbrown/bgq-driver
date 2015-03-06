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

/*
 * A set of classes to help with parsing XML files into C++ objects
 * Implement a subclass of XML::Serializable if you want to build it
 * from XML files.
 * Create an XML::Parser and invoke it with a stream and with the
 * shell of your serializable object to read the object from the stream.
 */

#include "expat.h"
#include <stdio.h>
#include <assert.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <stack>
#include <string>
#include <sstream>
#include <vector>

#ifndef __XML_parser__
#define __XML_parser__

#include "xml/include/library/base64.h"

#define XML_BUFFER_SIZE 16384

namespace XML {

/* ********************************************************************* */
/*         Expat parsing error                                           */
/* ********************************************************************** */

class Exception {
public:
    Exception (const char *text): _comment(text) { }
    friend std::ostream & operator<<(std::ostream &os, const Exception &e) {
        os << e._comment;
        return os;
    }
private:
    std::string _comment;
};


// throw an XML exception with a comman separated list (first element
// should be format string
#define XMLLIB_THROW(...)                                \
{                                                 \
    char buf[256];                                \
    snprintf(buf, sizeof(buf)-1, __VA_ARGS__);    \
    throw XML::Exception(buf);                    \
}

/* ********************************************************************** */
/*        Common interface of all XML-serializable objects                */
/* ********************************************************************** */

class Serializable {
public:
    Serializable () {}
    virtual ~Serializable() {}

    virtual Serializable *addSubEntity (const char *name, const char **attrs) {
        throw Exception("XML: object does not allow subentities\n");

        return this;
    }

    virtual unsigned setAttributes (const char **attrs) {
        if (attrs[0] != 0) {
            throw Exception("XML: object does not allow attributes\n");
        }
        return 0;
    }

    // read a serialized xml object from a stream
    // return 1 on successful parsing; 0 otherwise
    bool read(std::istream &is, bool multiline = false);

    // write a serialized xml object into the stream
    virtual void write(std::ostream& o) const;

    /*!
     * Writes the XML to a boost archive.
     *
     * @param os
     *          the output stream to dump the XML to.
     */
    virtual void serializeXML(std::ostream& os) const;

    /*!
     * Reads the XML from a boost archive.
     *
     * @param os
     *          the input stream to read the XML from.
     */
    virtual void readXML(std::istream &is);

protected:

};

inline std::ostream &operator<<(std::ostream& o, const Serializable& s)
{
    s.write(o);
    return o;
}


/* ********************************************************************** */
/*       Encapsulating the Expat parser into a C++ object                 */
/* ********************************************************************** */

class Parser {
public:
    Parser(bool multiline);
    virtual ~Parser();

    int getLine();
    int getCol();
    void parse(std::istream &is, Serializable *root);
private:
    static void _startXML(void *ud, const XML_Char *name, const XML_Char **atts);
    static void _endXML  (void *ud, const XML_Char *name);

private:
    bool                        _multiline; // allow multiline input

    Serializable               *_root;
    std::stack<Serializable *>  _xmlstack;
    XML_Parser                  _parser;

    char                        _buffer[XML_BUFFER_SIZE];
    bool                        _rootInitialized;

};

 /* ********************************************************************** */
 /*        Common interface of all XML-serializable root objects           */
 /* ********************************************************************** */

inline bool Serializable::read(std::istream &is, bool multiline) {
    Parser theParser(multiline);
    bool result;

    try {
        theParser.parse(is, this);
        result = true; // successful parsing
    } catch (const Exception &e) {
        std::cerr << e << std::endl;
        result = false;
    }

    return result;
}


// auxiliary function to read and write "simple" data types
  bool verify_attr(const char *className, const char *attrName, const char *attr0, const char *attr1, bool isOptional);
  void verify_last_attr(const char *className, const char *attr0);

  void write_text   (std::ostream &os, const char *className, const char *attrName, const std::string& attrValue);
  void write_cstring(std::ostream &os, const char *className, const char *attrName, const char* attrValue);
  void write_binary (std::ostream &os, const char *className, const char *attrName, const std::string& attrValue);
  void write_binary (std::ostream &os, const char *className, const char *attrName, const char* buf, unsigned buflen);

  void write_int32(std::ostream &os, const char *className, const char *attrName, int attrValue);
  void write_uint32(std::ostream &os, const char *className, const char *attrName, unsigned attrValue);
  void write_uint64(std::ostream &os, const char *className, const char *attrName, unsigned long long attrValue);
  void write_uint32h(std::ostream &os, const char *className, const char *attrName, unsigned attrValue);
  void write_uint16h(std::ostream &os, const char *className, const char *attrName, unsigned short attrValue);
  void write_uint8h(std::ostream &os, const char *className, const char *attrName, unsigned char attrValue);
  void write_uint64h(std::ostream &os, const char *className, const char *attrName, unsigned long long attrValue);
  void write_uchar  (std::ostream &os, const char *className, const char *attrName, unsigned char attrValue);
  void write_float  (std::ostream &os, const char *className, const char *attrName, long double attrValue);
  void write_ucharh (std::ostream &os, const char *className, const char *attrName, unsigned char attrValue);
  void write_bool   (std::ostream &os, const char *className, const char *attrName, bool attrValue);

  std::string        read_text    (const char *className, const char *attrName, const char *attr0, const char *attr1, unsigned* index, bool isOptional);
  char *             get_attrValue(const char *className, const char *attrName, const char *attr0, const char *attr1, unsigned* index, bool isOptional);
  void               read_cstring (const char *className, const char *attrName, const char *attr0, const char *attr1, unsigned* index, bool isOptional,
				   char *buf, unsigned buflen);
  std::string        read_binary  (const char *className, const char *attrName, const char *attr0, const char *attr1, unsigned* index, bool isOptional);
  void               read_binary  (const char *className, const char *attrName, const char *attr0, const char *attr1, unsigned* index, bool isOptional,
				   char *buf, unsigned buflen);
  unsigned           read_uint32  (const char *className, const char *attrName, const char *attr0, const char *attr1, unsigned* index, bool isOptional);
  unsigned long long read_uint64  (const char *className, const char *attrName, const char *attr0, const char *attr1, unsigned* index, bool isOptional);
  unsigned           read_uint32h (const char *className, const char *attrName, const char *attr0, const char *attr1, unsigned* index, bool isOptional);
  unsigned short     read_uint16h (const char *className, const char *attrName, const char *attr0, const char *attr1, unsigned* index, bool isOptional);
  unsigned char      read_uint8h  (const char *className, const char *attrName, const char *attr0, const char *attr1, unsigned* index, bool isOptional);
  int                read_int32   (const char *className, const char *attrName, const char *attr0, const char *attr1, unsigned* index, bool isOptional);
  unsigned long long read_uint64h (const char *className, const char *attrName, const char *attr0, const char *attr1, unsigned* index, bool isOptional);
  unsigned char      read_uchar   (const char *className, const char *attrName, const char *attr0, const char *attr1, unsigned* index, bool isOptional);
  long double	     read_float   (const char *className, const char *attrName, const char *attr0, const char *attr1, unsigned* index, bool isOptional);
  unsigned char      read_ucharh  (const char *className, const char *attrName, const char *attr0, const char *attr1, unsigned* index, bool isOptional);
  bool               read_bool    (const char *className, const char *attrName, const char *attr0, const char *attr1, unsigned* index, bool isOptional);

}; /* namespace */


#endif
