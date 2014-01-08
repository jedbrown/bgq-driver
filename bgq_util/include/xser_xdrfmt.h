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

#ifndef _XSER_XDRFMT_H_
#define _XSER_XDRFMT_H_

#include <vector>
#include <deque>
#include <list>
#include <map>

// for now assert on unexpected end of records.
//
// later we will try to handle this, i.e. reading zeros, or throwing
// an exception...
// 

#include "xser_stream.h"

using namespace std;

/*!
// class that controls formatting of basic elements into xdr format
//
// this internally knows how to format the basic c++ types into the xdr format
// as well as variable length arrays of basic types and compound types.
//
// for input class constuct with an input stream, output class construct
// with an output stream.
//
*/

//
// base formatt class...
//
class XdrBasicFmt {
public:
    XdrBasicFmt() {};
    virtual ~XdrBasicFmt() {};
    virtual bool isOstream() {return(false);};
    virtual bool isIstream() {return(false);};

protected:
    // convert to network byte order...
    // only if needed.
    #if (__BYTE_ORDER != __BIG_ENDIAN)
    //
    // convert to network byte order in place..
    //
    template <typename T>
    static T nb_order(T x) {
        // dead branches will be elimited here.
        //
        if (sizeof(T) == 1)
        {
            return(x);
        }
        else if (sizeof(T) == 2)
        {
        #if 0
            uint16_t xout = *((uint16_t*)&x);
            xout = (xout >> 8) | (xout << 8);
            x = *(T*)&xout;
            return(xout);
        #else
        union xu_t {
            T x;
            uint16_t xout;
        };
        xu_t xu;
        xu.x = x;
        xu.xout = (xu.xout >> 8) | (xu.xout << 8);
        #endif
        }
        else if (sizeof(T) == 4)
        {
            T xout;
            uint8_t *psrc = ((uint8_t*)&x);
            uint8_t *pdest = ((uint8_t*)&xout) + 4;
            for (unsigned n = 0; n < 4; n++)
                *--pdest = *psrc++;
            return(xout);
        }
        else if (sizeof(T) == 8)
        {   
            T xout;
            uint8_t *psrc = ((uint8_t*)&x);
            uint8_t *pdest = ((uint8_t*)&xout) + 8;
            for (unsigned n = 0; n < 8; n++)
                *--pdest = *psrc++;
            return(xout);
        }
    };

    #else
        #define nb_order(x) x
    #endif
private:

};

/*!
// Xdr output format class.
//
*/
class XdrOformat : public XdrBasicFmt
{
public:
    XdrOformat(XserOstream &slaOstream) :
        _slaOstream(slaOstream) {};
    XserOstream &_slaOstream;
    virtual bool isOstream() {return(true);};
    
    void endr() {
        _slaOstream.write_endr(); };

    void fmt(bool &i)        { fmt_x<bool, uint32_t>(i);} ;
    void fmt(char &i)        { fmt_x<char, uint32_t>(i);} ;
    void fmt(uint8_t &i)     { fmt_x<uint8_t, uint8_t>(i);} ;
    void fmt(int16_t &i)     { fmt_x<int16_t, uint16_t>(i);} ;
    void fmt(uint16_t &i)    { fmt_x<uint16_t, uint32_t>(i);} ;
    void fmt(int32_t &i)     { fmt_x<int32_t, uint32_t>(i);} ;
    void fmt(uint32_t &i)    { fmt_x<uint32_t, uint32_t>(i);} ;
    void fmt(int64_t &i)     { fmt_x<int64_t, uint64_t>(i);} ;
    void fmt(uint64_t &i)    { fmt_x<uint64_t, uint64_t>(i);} ;
    void fmt(float &i)       { fmt_x<float, float>(i);} ;
    void fmt(double &i)      { fmt_x<double, double>(i);} ;

    void fmt(std::string &i)
    {
        uint32_t s = i.size();
        fmt(s);        // output size, followed by vector elements
        fmt_x(i.c_str(), i.size());
        
    }


    // single byte vector, make it more efficient...
    void fmt(std::vector<uint8_t> &x)          // variable length vector
    {
        uint32_t s = x.size();
        fmt(s);        // output size, followed by vector elements
        fmt_x((char *)&x[0], s);
    };


    template <class T, int i>  // fixed length array
    void fmt(T (&x)[i]) {   
        for (unsigned n = 0; n < i; n++)
            fmt(x[n]);
    };

    template<typename T>
    void fmt(std::vector<T> &x)          // variable length vector
    {
        uint32_t s = x.size();
        fmt(s);        // output size, followed by vector elements
        typename std::vector<T>::iterator it;
        for (it = x.begin(); it != x.end(); it++)
            fmt(*it);
    };

    template<typename T>
    void fmt(std::vector<T*> &x)          // variable length list
    {
        uint32_t s = x.size();
        fmt(s);        // output size, followed by vector elements
        typename std::vector<T *>::iterator it; 
        for (it = x.begin(); it != x.end(); it++)
            fmt(*it);
    };


    template<typename T>
    void fmt(std::deque<T> &x)          // variable length deque
    {
        uint32_t s = x.size();
        fmt(s);        // output size, followed by vector elements
        typename std::deque<T>::iterator it;
        for (it = x.begin(); it != x.end(); it++)
            fmt(*it);
    };

    template<typename T>
    void fmt(std::deque<T *> &x)          // variable length deque
    {
        uint32_t s = x.size();
        fmt(s);        // output size, followed by vector elements
        typename std::deque<T *>::iterator it;
        for (it = x.begin(); it != x.end(); it++)
            fmt(*it);
    };

    template<typename T>
    void fmt(std::list<T> &x)          // variable length list
    {
        uint32_t s = x.size();
        fmt(s);        // output size, followed by vector elements
        typename std::list<T>::iterator it;
        for (it = x.begin(); it != x.end(); it++)
            fmt(*it);
    };

    template<typename T>
    void fmt(std::list<T*> &x)          // variable length list
    {
        uint32_t s = x.size();
        fmt(s);        // output size, followed by vector elements
        typename std::list<T *>::iterator it;
        for (it = x.begin(); it != x.end(); it++)
            fmt(*it);
    };

    template <typename f, typename s>
    void fmt(std::map<f,s> &x)          // variable length map
    {
        uint32_t sz = x.size();
        fmt(sz);        // output size, followed by vector elements
        typename std::map<f,s>::iterator it;
        for (it = x.begin(); it != x.end(); it++)
        {
            fmt(const_cast<f&>(it->first));
            fmt(const_cast<s&>(it->second));
        }
    };


    // class which will format an arbitrary structure...
    template <class T>
    void fmt(T &x) {            // arbitrary structure, not overloaded above
        //std::cout << "fmt(T &x): "  << typeid(T).name() << std::endl;
        x.fmt(*this);    // self formatting
    };
    template <class T>
    void fmt(T *&x) {            // arbitrary ptr to structure, not overloaded above
        bool isNull = (x == NULL);      // store if this a null ptr.
        fmt(isNull);
        if (!isNull)
            x->fmt(*this);    // self formatting
    };



protected:

    void fmt_x(const char * sz, size_t size) {      // this is for the fixed length opaque structure...
        if (size)
            _slaOstream.write(sz, size);
    };
    template <typename T1, typename T2>
    void fmt_x(T1 &b) {
        T2 o = static_cast<T2>(b);
        o = nb_order(o);        // convert basic type to nb byte order...
        _slaOstream.write((char *) &o, sizeof(o));
    };
private:

};

/*!
// Xdr input format class.
//
*/
class XdrIformat : public XdrBasicFmt
{
public:
    XdrIformat(XserIstream &slaIstream) :
        _slaIstream(slaIstream) {};
    virtual bool isIstream() {return(true);};

    bool eof() {
        return(_slaIstream.eof()); };
    bool is_endr() {
        return(_slaIstream.is_endr()); };

    void endr() {
        assert(is_endr());      // change to throw exception...
    };
    void next_rec() {
        _slaIstream.next_rec(); };

    /*!
    // Check to see if we saw a record underflow.
    //
    // @param none.
    // @returns true if we had a record underflow, false if not
    */
    bool rec_underflow() {
        return(_slaIstream.rec_underflow()); };

    void fmt(bool &i)        { fmt_x<bool, uint32_t>(i);} ;
    void fmt(char &i)        { fmt_x<char, uint32_t>(i);} ;
    void fmt(uint8_t &i)     { fmt_x<uint8_t, uint8_t>(i);} ;
    void fmt(int16_t &i)     { fmt_x<int16_t, uint16_t>(i);} ;
    void fmt(uint16_t &i)    { fmt_x<uint16_t, uint32_t>(i);} ;
    void fmt(int32_t &i)     { fmt_x<int32_t, uint32_t>(i);} ;
    void fmt(uint32_t &i)    { fmt_x<uint32_t, uint32_t>(i);} ;
    void fmt(int64_t &i)     { fmt_x<int64_t, uint64_t>(i);} ;
    void fmt(uint64_t &i)    { fmt_x<uint64_t, uint64_t>(i);} ;
    void fmt(float &i)       { fmt_x<float, float>(i);} ;
    void fmt(double &i)      { fmt_x<double, double>(i);} ;

    void fmt(std::string &i)
    {
        assert(!is_endr());
        //std::cout << "fmt(vector<T> x): " << typeid(T).name() << std::endl;
        uint32_t s;
        fmt(s);        // output size, followed by vector elements
        i.resize(s);
        fmt_x(const_cast<char*>(i.data()), s);

    }
    // single byte vector, make it more efficient...
    void fmt(std::vector<uint8_t> &x)          // variable length vector
    {
        uint32_t s;
        fmt(s);        // output size, followed by vector elements
        x.resize(s);
        fmt_x((char *)&x[0], s);
    };

    template <typename f, typename s>
    void fmt(std::map<f,s> &x)          // variable length map
    {
        uint32_t sz;
        fmt(sz);        // output size, followed by vector elements
        for (unsigned n = 0; n < sz; n++)
        {
            f key;
            s data;
            fmt(key);
            fmt(data);
            x[key] = data;
        }
    };


    template <class T, int i>  // fixed length array
    void fmt(T (&x)[i]) {       
        assert(!is_endr());
        for (unsigned n = 0; n < i; n++)
            fmt(x[n]);
    };
    template<typename T>
    void fmt(std::vector<T> &x)          // variable length vector
    {
        assert(!is_endr());
        uint32_t s;
        fmt(s);        // output size, followed by vector elements
        //std::cout << "fmt(vector<T> x): " << typeid(T).name() << " size = " << s << std::endl;
        x.resize(s);
        typename std::vector<T>::iterator it;
        for (it = x.begin(); it != x.end(); it++)
            fmt(*it);
    };
    template<typename T>
    void fmt(std::vector<T *> &x)          // variable length deque
    {
        assert(!is_endr());
        uint32_t s;
        fmt(s);        // output size, followed by deque elements
        x.resize(s);
        typename std::vector<T *>::iterator it;
        for (it = x.begin(); it != x.end(); it++)
            fmt(*it);
    };

    template<typename T>
    void fmt(std::deque<T> &x)          // variable length deque
    {
        assert(!is_endr());
        uint32_t s;
        fmt(s);        // output size, followed by deque elements
        x.resize(s);
        typename std::deque<T>::iterator it;
        for (it = x.begin(); it != x.end(); it++)
            fmt(*it);
    };

    template<typename T>
    void fmt(std::deque<T *> &x)          // variable length deque
    {
        assert(!is_endr());
        uint32_t s;
        fmt(s);        // output size, followed by deque elements
        x.resize(s);
        typename std::deque<T *>::iterator it;
        for (it = x.begin(); it != x.end(); it++)
            fmt(*it);
    };

    template<typename T>
    void fmt(std::list<T> &x)          // variable length deque
    {
        assert(!is_endr());
        uint32_t s;
        fmt(s);        // output size, followed by deque elements
        x.resize(s);
        typename std::list<T>::iterator it;
        for (it = x.begin(); it != x.end(); it++)
            fmt(*it);
    };
    template<typename T>
    void fmt(std::list<T *> &x)          // variable length deque
    {
        assert(!is_endr());
        uint32_t s;
        fmt(s);        // output size, followed by deque elements
        x.resize(s);
        typename std::list<T *>::iterator it;
        for (it = x.begin(); it != x.end(); it++)
            fmt(*it);
    };
    template <class T>
    void fmt(T &x) {            // arbitrary structure, not overloaded above
        //std::cout << "I fmt class"  << typeid(T).name() << std::endl;
        x.fmt(*this);    // self formatting
    };
    template <class T>
    void fmt(T * &x) {            // arbitrary structure, not overloaded above
        //std::cout << "I fmt class"  << typeid(T).name() << std::endl;
        bool isNull;
        fmt(isNull);
        if (isNull)
            x = NULL;
        else
        {
            x = new T();
            x->fmt(*this);    // self formatting
        };
    };

protected:
    XserIstream &_slaIstream;    

    template <typename T1, typename T2>
    void fmt_x(T1 &b) {
        assert(!is_endr());
        T2 i;
        _slaIstream.read((char *) &i, sizeof(i));
        i = nb_order(i);
        b = static_cast<T1>(i);
    };
    void fmt_x(char * sz, size_t size) {      // this is for the fixed length opaque structure...
        if (size)
            _slaIstream.read(sz, size);
    };

private:

};

#endif


