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


#ifndef __genericxml_h__
#define __genericxml_h__

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

class XMLEntity;

class XMLException: public std::runtime_error
{
public:
    XMLException(const XMLEntity* obj): runtime_error(""), _obj(obj) { }
    XMLException(const std::string& m, const XMLEntity* obj): runtime_error(m), _obj(obj) {  }
    ~XMLException() throw() {}
    friend std::ostream &operator<<(std::ostream &os, const XMLException& node);
private:
    const XMLEntity* _obj;
};

class XMLEntity
{
public:
    static XMLEntity* readXML (const char* filename) throw(XMLException);
    static XMLEntity* readXML(std::istream& is) throw(XMLException);
    static void dumpXML(const std::stringstream& os, const char* description, const bool force);

    const std::string& name() const { return _name; }
    const std::string& cdata() const { return _cdata; }
    const char* attrByName(const char *name) const throw(XMLException);
    const char* attrOptByName(const char *name) const;
    const XMLEntity* subEntByName(const char *name) const throw(XMLException);
    std::vector<const XMLEntity*> subentities() const;
    unsigned lineno() const { return _lineno; }

    std::vector<XMLEntity>::const_iterator subEntitiesBegin() { return _entities.begin(); }
    std::vector<XMLEntity>::const_iterator subEntitiesEnd() { return _entities.end(); }

    void dump(unsigned indent=0) const ;

    ~XMLEntity();

private:
	std::string               _name;        // name of entity
	std::string               _cdata;       // cdata of the entity
    unsigned                  _lineno;	    // line number it occurred
    std::vector<std::string>  _attnames;	// names of XML attributes
    std::vector<std::string>  _attvalues;	// values of XML attributes
    std::vector<XMLEntity>    _entities;	// subentities
    XMLEntity*                _parent;  	// parent entity
    void*                     _parser;	    // associated XML parser

private:
    XMLEntity(void* parser);
    XMLEntity(const char* name,
	      const char** attributes,
	      unsigned lineno,
	      XMLEntity* parent);
    void* parser() const { return _parser; }
    XMLEntity* parent() const { return _parent; }
    static void _startXML(void* ud, const char* name, const char** atts);
    static void _endXML(void* ud, const char* name);
    static void _startCDATA(void* ud, const char* s, int len);
};

inline std::ostream &operator<<(std::ostream &os, const XMLException& ex)
{
    os << "XML Exception! " << ex.what() << std::endl;
    if (ex._obj) {
        os << "Current XML tag: " << ex._obj->name() << " in line " << ex._obj->lineno() << std::endl;
    }
    return os;
}

#endif
