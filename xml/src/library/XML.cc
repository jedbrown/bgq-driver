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

#include <sstream>
#include <fstream>
#include <cstring>
#include <pthread.h>
#include <vector>
#include <algorithm>
#include "xml/include/library/XML.h"

using namespace std;
using namespace XML;

//#define XML_BUFFER_SIZE 64*1024
//#define XML_BUFFER_SIZE 16384
#define XML_RETRY 20

/* ********************************************************************** */
/*              Parser constructor, destructor                            */
/* ********************************************************************** */

Parser::Parser(
        bool multiline
        )
  : _multiline(multiline),
    _root(0),
    _xmlstack(),
    _parser(XML_ParserCreate(0)),
    _rootInitialized(false)
{
    XML_SetUserData(_parser, (void *) this);
    XML_SetElementHandler(_parser, _startXML, _endXML);
}


Parser::~Parser()
{
    XML_ParserFree(_parser);
}


int
Parser::getLine()
{
    return XML_GetCurrentLineNumber(_parser);
}


int
Parser::getCol()
{
    return XML_GetCurrentColumnNumber(_parser);
}


void
Parser::parse(
        std::istream &s,
        Serializable * root
        )
{
    _root = root;
    _xmlstack.push(_root); // insert the root in the stack; this prevents early termination of the do/while loop below
    unsigned retry = 0;

    do {
        int bytes_read = 0;      // how many bytes to parse
        int bytes_available = 0; // how many bytes are available in the input stream
        int bytes_toread = 0;    // how many of those bytes we should try to read

        // first check how many bytes are in the input buffer
        bytes_available = s.rdbuf()->in_avail();
        if (bytes_available == 0) { // if we have none, we force another read by trying to read 1 byte.
            // This solves the problem of blocking waiting for bytes that will never arrive (i.e. in sockets)
            bytes_read = s.rdbuf()->sgetn(_buffer, 1);
            bytes_available = s.rdbuf()->in_avail();
            //returnCode = s.rdbuf()->sbumpc();
            //cout << "sbumpc returnCode = " << returnCode << endl;
            //if ( !s || s.eof() || s.fail() ) { // don't loop forever on incomplete xml object
            if ( s.eof() || s.fail() || retry > XML_RETRY ) { // don't loop forever on incomplete xml object
                char buf[256];
                snprintf(buf, sizeof(buf)-1,
                        "XML unexpected EOF at: %s at line %ld, col %ld",
                        XML_ErrorString(XML_GetErrorCode(_parser)),
                        XML_GetCurrentLineNumber(_parser),
                        XML_GetCurrentColumnNumber(_parser));
                throw Exception(buf);
            }

            retry++;
            // Should there be a variable delay here to keep this from looping too tightly?
            //usleep(5000);
            //	    continue;
        }

        // If we reset the retry to 0 here, then we never time out
        // and can hang.  Don't know why this was ever here?  WMS
        retry = 0;
        // now be sure that we read enough, but not too many bytes
        bytes_toread = bytes_available > XML_BUFFER_SIZE-bytes_read ?
                XML_BUFFER_SIZE-bytes_read : bytes_available;

        bytes_read += s.rdbuf()->sgetn(&(_buffer[bytes_read]), bytes_toread);

        // check if there was a problem with the stream and throw an exception if so
        if (!s || s.fail()) {
            char buf[256];
            snprintf(buf, sizeof(buf)-1,
                    "XML read error at: %s at line %ld, col %ld",
                    XML_ErrorString(XML_GetErrorCode(_parser)),
                    XML_GetCurrentLineNumber(_parser),
                    XML_GetCurrentColumnNumber(_parser));
            throw Exception(buf);
        }

        //cout << "_buffer (" << bytes_read << ") = '" << _buffer << "'end" << endl;
        // try to parse, and throw an exception if we can't
        if (!XML_Parse(_parser, _buffer, bytes_read, 0)) {
            char buf[256];
            snprintf(buf, sizeof(buf)-1,
                    "XML syntax error: %s at line %ld, col %ld",
                    XML_ErrorString(XML_GetErrorCode(_parser)),
                    XML_GetCurrentLineNumber(_parser),
                    XML_GetCurrentColumnNumber(_parser));
            throw Exception(buf);
        }

    } while (_xmlstack.size() > 0 ); // until we have gone through all the hierarchy, or reached the end

    //  cerr << "XML- stack size=" << _xmlstack.size() << endl;
}


/* ********************************************************************** */
/*           ExPat callback implementations.                              */
/* ********************************************************************** */
void
Parser::_startXML(
        void *ud,
        const XML_Char *name,
        const XML_Char **atts
			)
{
#if 0
    cerr << "XML-startXML: " << name << "( ";
    for (unsigned i = 0; atts[i]; i += 2) {
        cerr << atts[i] << "='" << atts[i + 1] << "' ";
    }
    cerr << ")" << endl;
#endif

    Parser *parser = (Parser *)ud;
    assert (parser != NULL);

    if (!parser->_rootInitialized) {
        parser->_root->setAttributes(atts);
        parser->_rootInitialized = true;
    } else {
        Serializable *curr = parser->_xmlstack.top();
        assert (curr != NULL);
        Serializable * newobject = curr->addSubEntity((const char *)name, (const char **)atts);
        assert (newobject != NULL);
        parser->_xmlstack.push(newobject);
    }
}


void
Parser::_endXML(
        void *ud,
        const XML_Char *name
        )
{
    //  cerr << "XML-endXML " << endl;

    Parser *parser = (Parser *)ud;
    assert (parser != NULL && parser->_xmlstack.size() > 0);
    parser->_xmlstack.pop();
}


// Expat calls the setAttributes method of every Serializable object
// a null terminated vector of char* of size 2n+1 (n is number of attrs),
// where the attribute value follows the attribute name.
// This vector is passed in ATTR_VEC and the current evaluation point
// is I (so the attribute name is ATTR_VEC[I] and its value is in ATTR_VEC[I+1]

// verify that ATTR_VEC[I] entry corresponds to the desired ATTR_NAME and that they are not null
bool
XML::verify_attr(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        bool isOptional
        )
{
    assert(attrName != 0);

    if (attr0 == 0 || strcmp(attrName, attr0)) {
        if( isOptional ) {
            return false; }
        XMLLIB_THROW("XML %s: expecting attribute %s, received attribute %s", className, attrName, attr0);  /*passing null object*/
    }

    if (attr1 == 0) {
        XMLLIB_THROW("XML %s: missing value for %s", className, attrName);
    }
    return true;
}


// verify that after we have processed all the attributes
// then ATTR_VEC[I] == 0, so there are not unknown attributes in the stream
void
XML::verify_last_attr(
        const char *className,
        const char *attr0
        )
{
    if (attr0 != 0) {
        XMLLIB_THROW("XML %s: extra attribute %s at the end of attribute list", className, attr0);
    }
}


void
XML::write_text(
        std::ostream &os,
        const char *className,
        const char *attrName,
        const std::string& attrValue
        )
{
    os << " " << attrName << "='";

    for (unsigned i = 0; i < attrValue.size(); i++) {
        char c = attrValue[i];
        if (c == '<') { os << "&lt;"; }
        else if (c == '&') { os << "&amp;"; }
        else if (c == '>') { os << "&gt;"; }
        else if (c == '"') { os << "&quot;"; }
        else if (c == '\'') { os << "&apos;"; }  // '
        else if (c == '\0') { os << "NULL"; }  // null - probably a bogus string
        else if (c >= ' ' && c <= '~') { os << c; }
        else {
            XMLLIB_THROW("XML %s: invalid character in text stream in attribute %s (%d)\n", className, attrName, (int) c);
        }
    }

    os << "'";
}


void
XML::write_cstring(
        std::ostream &os,
        const char *className,
        const char *attrName,
        const char* attrValue
        )
{
    os << " " << attrName << "='";

    for (unsigned i = 0; attrValue[i] != 0; i++) {
        char c = attrValue[i];
        if (c == '<') { os << "&lt;"; }
        else if (c == '&') { os << "&amp;"; }
        else if (c == '>') { os << "&gt;"; }
        else if (c == '"') { os << "&quot;"; }
        else if (c == '\'') { os << "&apos;"; }  // '
        else if (c >= ' ' && c <= '~') { os << c; }
        else {
            XMLLIB_THROW("XML %s: invalid character in text stream in attribute %s (%d)\n", className, attrName, (int) c);
        }
    }

    os << "'";
}

#ifdef XML_FIXES
void
XML::write_binary(
        std::ostream &os,
        const char *className,
        const char *attrName,
        const std::string& attrValue
        )
{
    os << " " << attrName << "='";
    encode64_new(os, attrValue);
    os << "'";
}
#else
void
XML::write_binary(
        std::ostream &os,
        const char *className,
        const char *attrName,
        const std::string& attrValue
        )
{
    os << " " << attrName << "='";
    encode64(os, attrValue, false);
    os << "'";
}


void
XML::write_binary(
        std::ostream &os,
        const char *className,
        const char *attrName,
        const char* buf,
        unsigned buflen
        )
{
    os << " " << attrName << "='";
    encode64(os, buf, buflen, false);
    os << "'";
}
#endif


void
XML::write_uint32(
        std::ostream &os,
        const char *className,
        const char *attrName,
        unsigned attrValue
        )
{
    os << " " << attrName << "='" << attrValue << "'";
}


void
XML::write_uint64(
        std::ostream &os,
        const char *className,
        const char *attrName,
        unsigned long long attrValue
        )
{
    os << " " << attrName << "='" << attrValue << "'";
}


void
XML::write_uint32h(
        std::ostream &os,
        const char *className,
        const char *attrName,
        unsigned attrValue
        )
{
    os.fill('0');
    os << " " << attrName << "='0x" << hex << setw(8) << attrValue << dec << "'";
    os.fill(' ');
}


void
XML::write_uint16h(
        std::ostream &os,
        const char *className,
        const char *attrName,
        unsigned short attrValue
        )
{
    os.fill('0');
    os << " " << attrName << "='0x" << hex << setw(4) << attrValue << dec << "'";
    os.fill(' ');
}


void
XML::write_uint8h(
        std::ostream &os,
        const char *className,
        const char *attrName,
        unsigned char attrValue
        )
{
    os.fill('0');
    os << " " << attrName << "='0x" << hex << setw(2) << (unsigned) attrValue << dec << "'";
    os.fill(' ');
}


void
XML::write_int32(
        std::ostream &os,
        const char *className,
        const char *attrName,
        int attrValue
        )
{
    os << " " << attrName << "='" << attrValue << "'";
}


void
XML::write_uint64h(
        std::ostream &os,
        const char *className,
        const char *attrName,
        unsigned long long attrValue
        )
{
    os.fill('0');
    os << " " << attrName << "='0x" << hex << setw(16) << attrValue << dec << "'";
    os.fill(' ');
}


void
XML::write_uchar(
        std::ostream &os,
        const char *className,
        const char *attrName,
        unsigned char attrValue
        )
{
    os << " " << attrName << "='" << (unsigned) attrValue << "'";
}


void
XML::write_float(
        std::ostream &os,
        const char *className,
        const char *attrName,
        long double attrValue
        )
{
    os << " " << attrName << "='" << fixed << attrValue << "'";
}


void
XML::write_ucharh(
        std::ostream &os,
        const char *className,
        const char *attrName,
        unsigned char attrValue
        )
{
    os.fill('0');
    os << " " << attrName << "='0x" << std::hex << std::setw(2) << (unsigned) attrValue << "'" << std::dec;
    os.fill(' ');
}


void
XML::write_bool(
        std::ostream &os,
        const char *class_name,
        const char *attr_name,
        bool attr_value
        )
{
    if (attr_value) {
        os << " " << attr_name << "='true'";
    } else {
        os << " " << attr_name << "='false'";
    }

    //  os << " " << attr_name << "='" << attr_value << "'";
}


// validate that the position matched the desired ATTR_NAME and then assign ATTR_VEC[I+1] to result.
// Increment I so we can now process the next attribute
std::string XML::read_text(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        unsigned* i,
        bool isOptional
        )
{
    if ( verify_attr(className, attrName, attr0, attr1, isOptional) ) {
        (*i)+=2;
        return std::string(attr1);
    } else {
        return std::string("");
    }
}


char *
XML::get_attrValue(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        unsigned* i,
        bool isOptional
        )
{
    if ( verify_attr(className, attrName, attr0, attr1, isOptional) ) {
        (*i)+=2;
        return const_cast<char*>(attr1);
    } else {
        return NULL;
    }
}


#ifdef XML_FIXES
std::string
XML::read_binary(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        unsigned* i,
        bool isOptional
        )
{
    verify_attr(className, attrName, attr0, attr1);

    return decode64_new(attr1, strlen(attr1));
}
#else
std::string
XML::read_binary(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        unsigned* i,
        bool isOptional
        )
{
    if ( verify_attr(className, attrName, attr0, attr1, isOptional) ) {
        (*i)+=2;
        std::ostringstream os;
        decode64(os, attr1, strlen(attr1));
        return os.str();
    } else {
        return std::string("");
    }
}


void
XML::read_binary(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        unsigned* i,
        bool isOptional,
        char *buf,
        unsigned buflen
        )
{
    if ( verify_attr(className, attrName, attr0, attr1, isOptional) ) {
        (*i)+=2;

        std::ostringstream os;
        decode64(os, attr1, strlen(attr1));
        memcpy(buf, os.str().c_str(), buflen);
    }
}
#endif


void
XML::read_cstring(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        unsigned* i,
        bool isOptional,
        char *buf,
        unsigned buflen
        )
{
    if ( verify_attr(className, attrName, attr0, attr1, isOptional) ) {
        (*i)+=2;

        if (strlen(attr1) > buflen-1) {
            XMLLIB_THROW("XML %s: not enough space to copy attribute %s ", className, attrName);
        }
        strncpy(buf, attr1, buflen);
    }
}


// same as READ_ATTR_TEXT but convert to unsigned
unsigned
XML::read_uint32(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        unsigned* i,
        bool isOptional
        )
{
    if ( verify_attr(className, attrName, attr0, attr1, isOptional) ) {
        (*i)+=2;
        unsigned temp;
        //  sscanf(attr1, "%u", &temp);

        istringstream is(attr1);
        is >> temp;

        return temp;
    } else {
        return 0;
    }
}

unsigned long long
XML::read_uint64(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        unsigned* i,
        bool isOptional
        )
{
    if ( verify_attr(className, attrName, attr0, attr1, isOptional) ) {
        (*i)+=2;
        unsigned long long temp;
        //  sscanf(attr1, "%lu", &temp);

        istringstream is (attr1);
        is >> temp;

        return temp;
    }  else {
        return 0;
    }
}


unsigned
XML::read_uint32h(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        unsigned* i,
        bool isOptional
        )
{
    if ( verify_attr(className, attrName, attr0, attr1, isOptional) ) {
        (*i)+=2;

        unsigned temp;
        istringstream is(attr1);
        is >> hex >> temp >> dec;
        return temp;
    }  else {
        return 0;
    }
}


unsigned short
XML::read_uint16h(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        unsigned* i,
        bool isOptional
        )
{
    if ( verify_attr(className, attrName, attr0, attr1, isOptional) ) {
        (*i)+=2;

        unsigned temp;
        istringstream is(attr1);
        is >> hex >> temp >> dec;

        if (temp < 0x10000) {
            return (unsigned short) temp;
        } else {
            XMLLIB_THROW("XML %s: attribute %s too big for short (%x) ", className, attrName, temp);
            return 0;
        }
    } else {
        return 0;
    }
}

unsigned char
XML::read_uint8h(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        unsigned* i,
        bool isOptional
        )
{
    if ( verify_attr(className, attrName, attr0, attr1, isOptional) ) {
        (*i)+=2;

        unsigned temp;
        istringstream is(attr1);
        is >> hex >> temp >> dec;

        if (temp < 0x100) {
            return (unsigned char) temp;
        } else {
            XMLLIB_THROW("XML %s: attribute %s too big for char (%x) ", className, attrName, temp);
            return 0;
        }
    } else {
        return 0;
    }
}


// same as READ_ATTR_TEXT but convert to signed
int
XML::read_int32(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        unsigned* i,
        bool isOptional
        )
{
    if ( verify_attr(className, attrName, attr0, attr1, isOptional) ) {
        (*i)+=2;
        //  unsigned temp;
        //  sscanf(attr1, "%d", &temp);

        int temp;
        istringstream is(attr1);
        is >> temp;

        return temp;
    } else {
        return 0;
    }
}


unsigned long long
XML::read_uint64h(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        unsigned* i,
        bool isOptional
        )
{
    if ( verify_attr(className, attrName, attr0, attr1, isOptional) ) {
        (*i)+=2;
        unsigned long long temp;
        //  sscanf(attr1, "%llx", &temp);

        istringstream is(attr1);
        is >> hex >> temp >> dec;
        return temp;
    } else {
        return 0;
    }
}

long double
XML::read_float(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        unsigned* i,
        bool isOptional
        )
{
    if ( verify_attr(className, attrName, attr0, attr1, isOptional) ) {
        (*i)+=2;
        long double temp;
        //  sscanf(attr1, "%llx", &temp);

        istringstream is(attr1);
        is >> temp;
        return temp;
    } else {
        return 0;
    }
}


// same as READ_ATTR_TEXT but convert to unsigned char
unsigned char
XML::read_uchar(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        unsigned* i,
        bool isOptional
        )
{
    if ( verify_attr(className, attrName, attr0, attr1, isOptional) ) {
        (*i)+=2;
        unsigned temp;
        //  sscanf(attr1, "%u", &temp);

        istringstream is(attr1);
        is >> temp;

        if (temp > 255) {
            XMLLIB_THROW("XML %s: invalid unsigned char in attribute %s (%u)", className, attrName, temp);
        }
        return (unsigned char) temp;
    } else {
        return 0;
    }
}

unsigned char
XML::read_ucharh(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        unsigned* i,
        bool isOptional
        )
{
    return XML::read_uchar(className, attrName, attr0, attr1, i, isOptional);
}


// same as READ_ATTR_TEXT but convert to unsigned char
bool
XML::read_bool(
        const char *className,
        const char *attrName,
        const char *attr0,
        const char *attr1,
        unsigned* i,
        bool isOptional
        )
{
    if ( verify_attr(className, attrName, attr0, attr1, isOptional) ) {
        (*i)+=2;

        if (!strcmp("0", attr1) || !strcmp("false", attr1)) {
            return false;
        } else   if (!strcmp("1", attr1) || !strcmp("true", attr1)) {
            return true;
        } else {
            XMLLIB_THROW("XML %s: invalid bool in attribute %s (%s) ", className, attrName, attr1);
        }
    }

    return (bool) false;
}

void
XML::Serializable::write(
        std::ostream& o
        ) const
{
    XMLLIB_THROW("write method not implemented");
}

void
XML::Serializable::serializeXML(
        std::ostream& os
        ) const
{
    XMLLIB_THROW("serializeXML method not implemented");
}

void
XML::Serializable::readXML(
        std::istream &is
        )
{
    XMLLIB_THROW("readXML method not implemented");
}
