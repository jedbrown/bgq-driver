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
/* (C) Copyright IBM Corp.  2007, 2011                              */
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

#ifndef TIME_HEADER
#define TIME_HEADER

#include <sys/time.h>
#include <string>
#include <sstream>
#include <assert.h>
#include <string.h>
#include <stdio.h>

class Time {
public:
    static const size_t MILLION = 1000000;

public:
  Time(bool init = true) {
    if (init) {
      gettimeofday(&_tv, 0);
    } else {
      _tv.tv_sec = 0;
      _tv.tv_usec = 0;
    }
  }

  Time(const Time& other) {
    _tv = other._tv;
  }

  Time(const struct timeval& tv) {
    _tv = tv;
  }

  Time(double t) {
    _tv.tv_sec = (int) t;
    _tv.tv_usec = (int) (t - ((double)_tv.tv_sec)) * MILLION;
  }

  double convert() const {
    return _tv.tv_sec * 1.0 + _tv.tv_usec * 1.0e-6;
  }

  void setCurrentTime() {
    gettimeofday(&_tv, 0);
  }

  void clear() {
    _tv.tv_sec = 0;
    _tv.tv_usec = 0;
  }

  void adjust() {
    if (_tv.tv_usec < 0) {
      // make the microseconds value positive
      _tv.tv_usec += MILLION;
      --_tv.tv_sec;
    } else if (_tv.tv_usec >= static_cast<signed>( MILLION )) {
      // make the microseconds smaller than a second
      _tv.tv_usec -= MILLION ;
      ++_tv.tv_sec;
    }
  }

  Time& operator+=(const Time& adder) {
    _tv.tv_sec += adder._tv.tv_sec;
    _tv.tv_usec += adder._tv.tv_usec;
    adjust();
    return *this;
  }

  friend Time operator+(const Time& adder1, const Time& adder2) {
      Time result(adder1);
      return result += adder2;
  }

  Time& operator-=(const Time& sub) {
    _tv.tv_sec -= sub._tv.tv_sec;
    _tv.tv_usec -= sub._tv.tv_usec;
    adjust();
    return *this;
  }

  friend Time operator-(const Time& sub1, const Time& sub2) {
      Time result(sub1);
      return result -= sub2;
  }

  friend Time operator/(const Time& time, int divisor) {
    Time result;
    if (divisor != 0) {
      timeval tmp = time._tv;
      result._tv.tv_sec = time._tv.tv_sec / divisor;
      result._tv.tv_usec = ((tmp.tv_sec % divisor) * MILLION + time._tv.tv_usec) / divisor;
    }
    return time;
  }

  Time &operator +=(unsigned secs) {
    _tv.tv_sec += secs;
    return *this;
  }

  unsigned getSeconds() const {
    return _tv.tv_sec;
  }

  timeval getTimeval() const {
    return _tv;
  }


  bool operator<(const Time& other) const {
    return (_tv.tv_sec < other._tv.tv_sec) ||
      (_tv.tv_sec == other._tv.tv_sec && _tv.tv_usec < other._tv.tv_usec);
  }

  bool operator>(const Time& other) const {
    return (_tv.tv_sec > other._tv.tv_sec) ||
      (_tv.tv_sec == other._tv.tv_sec && _tv.tv_usec > other._tv.tv_usec);
  }

  bool operator==(const Time& other) const {
    return (_tv.tv_sec == other._tv.tv_sec &&
        _tv.tv_usec == other._tv.tv_usec);
  }


   std::string asLongString(bool sortable = false, bool usecs = false, bool millisecs = false, bool local = true) const
   {
      struct tm tm;
      std::string result;
      if (_tv.tv_sec > 0)
      {
          if( local )
              localtime_r(&_tv.tv_sec, &tm);  // thread-safe version of localtime().
         char tbuf[256];
         if (!sortable)
         {
            strftime(tbuf, sizeof(tbuf), "%m/%d/%y %X", &tm);
         }
         else
         {  // yy-mm-dd-hh:mm:ss, use it when we want a C99 sortable
            strftime(tbuf, sizeof(tbuf), "%F %X", &tm);
         }
         result = tbuf;
      }
      else
      {
         result = "";
      }

      if (usecs)
      {  // we want the microseconds as well
         char ubuf[256];
         sprintf(ubuf, ":%06ld", (long )_tv.tv_usec);
         result += ubuf;
      }
      else
      {  // we want the milliseconds as well
         if (millisecs)
         {
            char ubuf[256];
            sprintf(ubuf, ".%03ld", (long )(_tv.tv_usec/1000));
            result += ubuf;
         }
      }

      return result;
   }


  std::string curTimeString(bool sortable = false, bool usecs = false, bool millisecs = true)
  {
      setCurrentTime();
      return asLongString(sortable, usecs, millisecs);
  }


  std::string asDB2timestamp(bool sortable = false, bool usecs = false) const {
    std::string result;
    if (_tv.tv_sec > 0) {
      char tbuf[256];
     // yy-mm-dd-hh.mm.ss, use it when we want a C99 sortable
      struct tm tm;
     strftime(tbuf, sizeof(tbuf), "%F-%H.%M.%S", localtime_r(&_tv.tv_sec,&tm));
      result = tbuf;
    } else {
      result = "";
    }

    // we want the microseconds as well
      char ubuf[256];
      sprintf(ubuf, ".%06ld", (long)_tv.tv_usec);
      result += ubuf;

    return result;
  }


  // these two methods are used to insert and remove date stamps in databases (sing the datetime SQL type
  std::string forDB(bool local = true) const {
    std::string result;

    char tbuf[256];
    struct tm tm;
    if( local )
        strftime(tbuf, sizeof(tbuf), "%F %T", localtime_r(&_tv.tv_sec,&tm));
    else
        strftime(tbuf, sizeof(tbuf), "%F %T", gmtime_r(&_tv.tv_sec,&tm));

    result = tbuf;

    return result;
  }

  void fromDB(const std::string& time) {
    struct tm tm;
    char *rc = strptime(time.c_str(), "%F %T", &tm);
    if (rc != 0) assert(rc != 0 && rc[0] == 0);

    _tv.tv_sec = mktime(&tm);
    _tv.tv_usec = 0;
  }


std::string asISO8601(bool local = true ) const
{
    std::ostringstream result;

    char tbuf[20];
    struct tm tm;
    if( local )
        strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", localtime_r(&_tv.tv_sec,&tm));
    else
        strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", gmtime_r(&_tv.tv_sec,&tm));

    result << tbuf;
    result << ".";
    result << _tv.tv_usec;

    return result.str();
}

std::string asISO8601interval( ) const
{
    std::ostringstream result;

    result << _tv.tv_sec;
    result << ".";
    result << _tv.tv_usec;

    return result.str();
}

protected:
  struct timeval _tv;

};

#endif
