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

//////////////////////////////////////////////////////////////////////
//
// Basic string tokenizer class
//

#include "bgq_util/include/string_tokenizer.h"

///////////////////////////////////////////////////////////////////////////
int StringTokenizer::tokenize(const std::string &rStr, 
                              const char *pDelimiters,
                              const char *pComment,
                              unsigned int nLimit)
//
// inputs:
//    rStr -- source string to tokenize
//    rDelimiters -- delimiter string
//    rComment -- comment characters
//    nLimit -- upper limit of number of tokens to parse out.
//              0 == no limit.
// outputs:
//    returns -- number of tokens parsed.
//    this -- contains a vector of the parsed strings.
//
//
{
    clear();        // clear out the last string.
    if (pDelimiters == NULL)            // no delimiters, then nothing to do...
        return(0);
	std::string::size_type lastPos(rStr.find_first_not_of(pDelimiters, 0));
	std::string::size_type pos(rStr.find_first_of(pDelimiters, lastPos));
	while (std::string::npos != pos || std::string::npos != lastPos)
	{
        // do we have comment delimiters
        if (pComment)       
        {
		    if (rStr.find_first_of(pComment, lastPos) == 0)     // if so, is this a comment.
                break;                                          // then we are done...
        }                
        
        if (nLimit)
        {
            if (nLimit <= size()+1)
            {
                push_back(rStr.substr(lastPos));     // take the remainder as one token...
                break;
            }
        
        }
		push_back(rStr.substr(lastPos, pos - lastPos));
		lastPos = rStr.find_first_not_of(pDelimiters, pos);
		pos = rStr.find_first_of(pDelimiters, lastPos);
        
	}
    return(size());
}


