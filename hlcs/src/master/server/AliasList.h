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


#ifndef MASTER_ALIAS_LIST_H_
#define MASTER_ALIAS_LIST_H_


#include "types.h"

#include <boost/thread.hpp>

#include <string>
#include <vector>


//! \brief Locking wrapper for alias vector
class AliasList
{
    std::vector<AliasPtr> _alias_list;
    mutable boost::mutex _mutex;
public:
    //! \brief Find the alias associated with the passed string
    bool find_alias(const std::string& al, AliasPtr& alias);

    // Get a copy of the list for local usage.  Don't try to
    // update it, it's a copy!
    std::vector<AliasPtr> get_list_copy() { return _alias_list; }

    typedef std::vector<AliasPtr>::iterator iterator;
    typedef std::vector<AliasPtr>::const_iterator const_iterator;
    size_t size() {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _alias_list.size(); }
    void push_back(AliasPtr sp) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _alias_list.push_back(sp); }
    void pop_back() {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _alias_list.pop_back(); }
    iterator insert (iterator position, AliasPtr sp) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _alias_list.insert(position, sp); }
    iterator begin() {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _alias_list.begin(); }
    iterator end() {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _alias_list.end(); }
    iterator erase ( iterator first, iterator last ) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _alias_list.erase(first, last); }
    const_iterator begin() const {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _alias_list.begin(); }
    const_iterator end() const {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _alias_list.end(); }
    void clear() {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _alias_list.clear(); }
};


#endif
