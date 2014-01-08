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


#ifndef TIMESTAMPFORMAT_HPP_
#define TIMESTAMPFORMAT_HPP_


#include <boost/regex.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <string>
#include <vector>


namespace log_merge {

class TimestampFormat
{
public:
    TimestampFormat();

    virtual bool split( const std::string& line, std::string* time_str_out, std::string* remaining_out ) const =0;

    virtual boost::posix_time::ptime parse( const std::string& time_str ) const =0;

    virtual ~TimestampFormat();
};


class TimestampFormats
{
public:
    TimestampFormats();

    void split(
            const std::string& line,
            TimestampFormat const** timestamp_format_p_out,
            std::string* time_str_out,
            std::string* remaining_out
        ) const;

    bool add( const TimestampFormat& format );


    static const TimestampFormats& getInstance()  { return instance; }

    static TimestampFormats instance;


private:
    typedef std::vector<const TimestampFormat*> _Formats;

    _Formats _formats;
};


class TimestampFormat1 : public TimestampFormat
{
public:
    TimestampFormat1();
    bool split( const std::string& line, std::string* time_str_out, std::string* remaining_out ) const;
    boost::posix_time::ptime parse( const std::string& time_str ) const;

    static const TimestampFormat1 INSTANCE;


private:

    static const boost::regex _LINE_RE;
    static const std::locale _PARSE_LOCALE;
};


class TimestampFormat3 : public TimestampFormat
{
public:
    TimestampFormat3();
    bool split( const std::string& line, std::string* time_str_out, std::string* remaining_out ) const;
    boost::posix_time::ptime parse( const std::string& time_str ) const;

    static const TimestampFormat3 INSTANCE;


private:

    static const boost::regex _LINE_RE;
    static const std::locale _PARSE_LOCALE;
};


class TimestampFormatQ : public TimestampFormat
{
public:
    TimestampFormatQ();
    bool split( const std::string& line, std::string* time_str_out, std::string* remaining_out ) const;
    boost::posix_time::ptime parse( const std::string& time_str ) const;

    static const TimestampFormatQ INSTANCE;

private:

    static const boost::regex _LINE_RE;
};

} // namespace log_merge


#endif
