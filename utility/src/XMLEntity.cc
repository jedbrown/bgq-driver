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
/* (C) Copyright IBM Corp.  2003, 2011                              */
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
/*********************************************************************/

#include "XMLEntity.h"

#include "expat.h"

#include <sys/stat.h>

#include <assert.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>

class XML_ParserStruct;

XMLEntity*
XMLEntity::readXML (const char* filename) throw(XMLException)
{
    XML_Parser parser = XML_ParserCreate(NULL);
    XMLEntity* o = new XMLEntity(parser);
    XML_SetUserData (parser, (void*)o);
    XML_SetElementHandler (parser,
            XMLEntity::_startXML,
            XMLEntity::_endXML);
    XML_SetCharacterDataHandler (parser,
            XMLEntity::_startCDATA);

    /* ------- Open the file -------- */
    FILE* fp = fopen(filename, "r");
    if (!fp)
    {
        char errbuffer[256];
        sprintf(errbuffer, "Cannot open file: %s", filename);
        throw XMLException(errbuffer, o);
    }

    /* ------- Parse the XML file --------- */
    bool done = false;
    do {
        char buf[256];
        size_t len = fread(buf, 1, sizeof(buf)-1, fp);
        done = len < sizeof(buf)-1;
        if (!XML_Parse((XML_Parser)parser, buf, len, done)) {
            char errbuffer[256];
            sprintf(errbuffer, "Syntax error: %s at line %d",
                    XML_ErrorString(XML_GetErrorCode(parser)),
                    (int) XML_GetCurrentLineNumber(parser));
            fclose(fp);
            throw XMLException(errbuffer, o);
        }
    } while(!done);

    fclose(fp);
    return o;
}

XMLEntity*
XMLEntity::readXML(std::istream& is) throw(XMLException)
{
    XML_Parser parser = XML_ParserCreate(NULL);
    XMLEntity* o = new XMLEntity(parser);
    XML_SetUserData (parser, (void*)o);
    XML_SetElementHandler (parser,
            XMLEntity::_startXML,
            XMLEntity::_endXML);

    /* ------- Parse the XML file --------- */
    bool done = false;

    do {
        std::string buf;
        std::getline(is, buf);

        // Check if the read was successful
        //if (is.good()) {
        // Successful read, so tack on the \n character
        buf.append("\n");
        //} else {
        // Last line was read, set done to true now
        //done = true;
        //}

        // Get the size of what we read
        size_t len = buf.size();

        // Parse the XML
        if (!XML_Parse((XML_Parser)parser, buf.c_str(), len, done)) {
            char errbuffer[256];
            sprintf(errbuffer, "Syntax error: %s at line %d column %d",
                    XML_ErrorString(XML_GetErrorCode(parser)),
                    (unsigned) XML_GetCurrentLineNumber(parser),
                    (unsigned) XML_GetCurrentColumnNumber(parser));
            throw XMLException(errbuffer, o);
        }
    } while (is.good() && !done);

    return o;
}

void
XMLEntity::dumpXML(const std::stringstream& os, const char* description, const bool force)
{
    if (force) {
        // Fall through since we should always dump XML if we're forced to
    } else {
        // Environmental to dump XML, note value does not matter, as long as it's not NULL
        if (getenv("BG_DUMP_XML") == NULL) {
            return;
        }
    }

    // Get our process id
    pid_t pid = getpid();

    // Create filename in the form of "bg_pid.XXXXXX" where XXXXXX will be replaced by unique
    // characters in the mkstemp syscall
    std::ostringstream filenameString;
    filenameString << "/tmp/bg_";
    if (description != NULL) {
        filenameString << description << "_";
    }
    filenameString << pid << ".XXXXXX";
    char* tempFilename = new char[filenameString.str().size()];
    snprintf(tempFilename, filenameString.str().size() + 1, "%s", filenameString.str().c_str());
    int tempFileFd = mkstemp(tempFilename);
    if (tempFileFd == -1) {
        printf("Could not create temp XML file");
        delete[] tempFilename;
        return;
    }

    // Change mode of file descriptor so everyone can read
    int rc = fchmod(tempFileFd, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (rc == -1) {
        printf("Could not chmod on temp XML file");
        delete[] tempFilename;
        return;
    }

    // Write XML
    rc = write(tempFileFd, os.str().c_str(), os.str().size());
    if (rc == -1) {
        printf("Error writing to XML file");
        delete[] tempFilename;
        return;
    }
    unsigned int bytesWritten = static_cast<unsigned int>(rc);

    // Close XML file
    close(tempFileFd);

    // Rename the file
    std::ostringstream newFilename;
    newFilename << tempFilename << ".xml";
    rename(tempFilename, newFilename.str().c_str());

    // Check we wrote everything
    if (bytesWritten != os.str().size()) {
        printf("Mismatch writing to XML file");
    }
    delete[] tempFilename;
    return;
}


void
XMLEntity::dump(unsigned indent) const
{
    for (unsigned i=0; i < indent; i++) printf("  ");
    printf("%s\n", _name.c_str());
    for (unsigned j=0; j<_entities.size(); j++) _entities[j].dump(indent+2);
}

const XMLEntity*
XMLEntity::subEntByName(const char* name) const throw(XMLException)
{
    for (unsigned i=0; i < _entities.size(); i++)
        if (_entities[i]._name == name) return &_entities[i];
    char errbuffer[256];
    sprintf(errbuffer, "Cannot find subentity %s", name);
    throw XMLException(errbuffer, this);
}

std::vector<const XMLEntity*>
XMLEntity::subentities() const
{
    std::vector<const XMLEntity*> result;
    if (_entities.size() > 32) {
        //cout << "Reserving storage for " <<  _entities.size() <<  " elements." << endl;
        result.reserve(_entities.size());
    }
    for (unsigned i=0; i < _entities.size(); i++) {
        //cout << "Capacity is " << (int) result.capacity() << endl;
        result.push_back((const XMLEntity *)& _entities[i]);
    }
    return result;
}

const char*
XMLEntity::attrByName(const char* name) const throw(XMLException)
{
    for (unsigned i=0; i < _attnames.size(); i++)
        if (_attnames[i] == name) return _attvalues[i].c_str();
    char errbuffer[256];
    sprintf(errbuffer, "Cannot find attribute %s", name);
    throw XMLException(errbuffer, this);
}

const char*
XMLEntity::attrOptByName(const char* name) const
{
    for (unsigned i=0; i < _attnames.size(); i++)
        if (_attnames[i] == name) return _attvalues[i].c_str();
    return NULL;
}

XMLEntity::XMLEntity(void* parser):
    _name("*TopLevel*"),
    _cdata(),
    _lineno(XML_GetCurrentLineNumber((XML_Parser)parser)),
    _attnames(),
    _attvalues(),
    _entities(),
    _parent(NULL),
    _parser((XML_Parser)parser)
{
}

XMLEntity::XMLEntity(const char* name,
		     const char** attributes,
		     unsigned lineno,
		     XMLEntity* parent):
    _name(name),
    _cdata(),
    _lineno(lineno),
    _attnames(),
    _attvalues(),
    _entities(),
    _parent(parent),
    _parser(NULL)
{
    assert (parent != NULL && attributes != NULL);
    _parser = parent->parser();
    if (attributes) for (unsigned i = 0; attributes[i] && attributes[i+1]; i+=2)
    {
        _attnames.push_back(std::string(attributes[i]));
        _attvalues.push_back(std::string(attributes[i+1]));
    }
}


XMLEntity::~XMLEntity()
{
    if (_parent == NULL && _parser) {
        // Toplevel...free the parser.
        XML_ParserFree((XML_ParserStruct*)_parser);
    }
}

void
XMLEntity::_startXML (void* ud, const char* name, const char** atts)
{
    XMLEntity* root = (XMLEntity*)ud;
    assert (root != (XMLEntity*)NULL);
    root->_entities.push_back(XMLEntity(name,
            atts,
            XML_GetCurrentLineNumber((XML_Parser)root->_parser),
            root));
    XML_SetUserData((XML_Parser)root->parser(),
            (void*) & (root->_entities.back()));
}

void
XMLEntity::_endXML (void* ud, const char*)
{
    XMLEntity* root = (XMLEntity*)ud;
    assert (root != (XMLEntity*)NULL);
    XML_SetUserData((XML_Parser)root->parser(), root->parent());
}

void
XMLEntity::_startCDATA(void* ud, const char* s, int len)
{
    XMLEntity* root = (XMLEntity*)ud;
    root->_cdata.append(std::string(s, len));
}
