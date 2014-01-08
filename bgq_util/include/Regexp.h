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

#ifndef REGEXP_HEADER
#define REGEXP_HEADER

#include <sys/types.h>
#include <regex.h>
#include <iostream>
#include <string>


// match a name against a pattern
//  string pattern = "xyz[^ ]*vwz";
// * means 0 or more occurences
// + means 1 or more occurrences
// ? 0 or one occurrences
// [] defines a list
// [^a] a list with all characters but a

	/*! Special characters in regular expressions */
	static const char RegexpSpecialChars[] = ".*+?[]{}|^$()";

class Regexp
{
public:
	
	
	/*! Returns true if string "looks" like a regular expression, does not check validity. */
	static bool containsSpecialChars( std::string str )
	{
		return str.find_first_of( RegexpSpecialChars ) != str.npos;
	};
	
    Regexp(const std::string& pattern) 
    : _isValid(false)
    , _pattern(pattern) 
    {
        bool rc = regcomp(&_preg, pattern.c_str(), REG_EXTENDED|REG_NOSUB);
        if (rc == 0)
        {
            _isValid = true;
        }
        else
        {
            char errbuf[256];
            regerror(rc, &_preg, errbuf, sizeof(errbuf));
            _error = errbuf;
        }
    };

    ~Regexp() {
        regfree(&_preg);
    };

    bool matches(const std::string& name) {
        bool result = false;
        if (_isValid)
        {
            bool rc = regexec(&_preg, name.c_str(), 0, 0, 0);
            if (rc == 0)
            {
                result = true;
            }
            else if (rc != REG_NOMATCH)
            {
                char errbuf[256];
                regerror(rc, &_preg, errbuf, sizeof(errbuf));
                _error = errbuf;
            }
        }
        return result;
    };

    std::string pattern() {
        return(_pattern);};
    bool isValid() {
        return(_isValid);};
protected:
    regex_t _preg;
    bool _isValid;  // successful compilation of regular expression
    std::string _error; // if not succesful compilation, what error?
    std::string _pattern;     // saved pattern...

};



#endif
