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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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

//! \brief A container class for nonblocking thread communications.
//! Multiple threads may send but only a single receiver is supported.  
//! It's called a "dropoff queue" because the senders "drop off" messages 
//! and the receiver "picks up" messages from the other end.  

//! It's a templated class so a "message" may be any kind of object.

//! The receiver only writes to an atomic flag maintained for each
//! message.  The flag indicates that the entry has been received.
//! Every time a dropoff is perfromed (a send), every flagged entry 
//! is removed. The existence of the entry and its flag are protected 
//! internally by a smart pointer.

//! Send side actions (dropoff and clearing) are controlled with locks.

//! You are guaranteed not to deadlock between a thread that only sends
//! and a thread that only receives.

//! The send side locks and the atomic read flag mean that the integrity
//! of the queue is protected in even highly concurrent environments.

//! Be aware that dropping off a message doesn't guarantee that it will 
//! ever be picked up by the receiver.  The receiver still has to do
//! the work of calling pickUp().  

#include <ostream>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/detail/atomic_count.hpp>

#ifndef MASTER_DROPOFFQUEUE_H
#define MASTER_DROPOFFQUEUE_H

template<class Type> class DropoffQueue
{
    // Helper node class.
    template<class NodeType> class Node
    {
        friend class DropoffQueue;
        template<class NT>
        // Print the node data for debug.
        friend std::ostream& operator<<(std::ostream& os, const boost::shared_ptr<Node<NT> >& n) {
            if(n)
                os << n->_element << "::" << n->_prev.get() << "<-" << n.get() 
                   << "->" << n->_next.get() << "::" << n->_dirtybit;
            return os;
        }

        boost::detail::atomic_count _dirtybit;
        NodeType _element;
        boost::shared_ptr<Node> _next, _prev;
    public:
        Node(NodeType element) : _dirtybit(0), _element(element) { _next.reset(); _prev.reset(); }
        Node(const Node& n) : _dirtybit(0) { _element = n._element; };
        NodeType getElement() { return _element; }
        ~Node() {}
        std::string print();
    };


    boost::shared_ptr<Node<Type> > _head;
    boost::shared_ptr<Node<Type> > _tail;
    boost::mutex _dropofflock;
    unsigned _removed;
    unsigned _dropped_off;
    unsigned clearRead_nl();
public:
    //! \brief constructor
    DropoffQueue();

    //! \brief destructor
    ~DropoffQueue() { clear(); }

    //! \brief Write a new node to the dropoff queue.
    //! This will also do a lazy cleanup of picked up Nodes. 
    //! \param newnode The message object to drop off
    void dropOff(Type newnode);

    //! \brief Get an item off the queue and mark it for later cleanup.
    //! \param pu An object (or an empty instance thereof) from the queue.
    //! \returns true if a message was found, false if the queue is empty.
    bool pickUp(Type& pu);

    //! \brief Clean up all nodes that have already been read.
    //! \returns Number of nodes cleared.
    unsigned clearRead();

    //! \brief Current number of items (both read and unread) on the queue.
    unsigned size();

    //! \brief debug method to show the current state of the queue.
    void printChain();

    //! \brief Get the total number of messages that have been removed from the queue.
    //! \returns Count of removed messages.
    unsigned get_removed() { return _removed; }

    //! \brief Get the total number of unread messages on the queue.
    //! \returns Count of unread messages.
    unsigned get_unread();

    //! \brief Remove all remaining messages.  Locking.
    //! \returns Number of messages cleared.
    //! Note that this means that unread messages will not be delivered.
    unsigned clear();
};

template<class Type>
DropoffQueue<Type>::DropoffQueue() : _removed(0), _dropped_off(0) {
    clear();
}

template<class Type>
unsigned int DropoffQueue<Type>::clearRead_nl() {
    typedef boost::shared_ptr<Node<Type> > NodePtr;
    // Now loop through the list and remove anything that has
    // been read by the pickup side.
    NodePtr prev = _head;
    NodePtr curr = _head;
    unsigned cleared = 0;
    unsigned unread = 0;
    while(curr != 0) {
        NodePtr removed_node;
        if(curr->_dirtybit != 0) {
            // Remove it because it has been picked up.
            prev->_next = curr->_next;
            if(prev->_next) {
                prev->_next->_prev = prev;
            }
            if(curr == _tail) 
                _tail = prev; // If we're picking off the last one, move the tail pointer.
            if(curr == _head)
                curr->_prev.reset();
            ++_removed;
            removed_node = curr;
        } else {
            prev = curr;
        }
        curr = curr->_next;
        if(removed_node) {
            ++cleared;
            removed_node->_next.reset();
            removed_node->_prev.reset();
            if(_head == _tail && _head == removed_node) {
                // This is the only node left.
                // Make sure it goes away.
                _head.reset();
                _tail.reset();
            }
        } else ++unread;
    }
    return cleared;
}

template<class Type>
unsigned int DropoffQueue<Type>::clearRead() {
    boost::mutex::scoped_lock scope_lock(_dropofflock);
    return clearRead_nl();
}

// Dropoff side places a new element in the queue
// and then removes anything that has already 
// been read.
template<class Type>
void DropoffQueue<Type>::dropOff(const Type element) {
    ++_dropped_off;
    typedef boost::shared_ptr<Node<Type> > NodePtr;
    NodePtr newnode(new Node<Type>(element));
    boost::mutex::scoped_lock scope_lock(_dropofflock);
    // Put it in the head of the list.
    if(_head == 0) {
        // First add.
        assert(_tail == 0);
        _tail = newnode;
      } else {
        newnode->_next = _head;
        _head->_prev = newnode;
    }
    _head = newnode;
    clearRead_nl();
}

// Pickup never removes anything.  We get a smart
// pointer to the tail, mark it "dirty" (or read)
// and return it.  The smart pointer guarantees
// its existence so the dropoff side can't take it
// away.
template<class Type>
bool DropoffQueue<Type>::pickUp(Type& pu) {
    typedef boost::shared_ptr<Node<Type> > NodePtr;
    NodePtr retptr, tmp;
    for(tmp = _tail; tmp != 0; tmp = tmp->_prev) {
        if(tmp->_dirtybit == 0) {
            ++tmp->_dirtybit;
            retptr = tmp;
            break;
        }
    }
    if(retptr == 0) {
        return false;
    }
    pu = retptr->getElement();
    return true;
}

template<class Type>
void DropoffQueue<Type>::printChain() {
    typedef boost::shared_ptr<Node<Type> > NodePtr;
    for(NodePtr curr = _head; curr != _tail; curr = curr->_next) {
        std::cout << curr.get() << "->";
    }
    std::cout << _tail.get() << "->0" << std::endl;
}

template<class Type>
unsigned DropoffQueue<Type>::clear() {
    typedef boost::shared_ptr<Node<Type> > NodePtr;
    boost::mutex::scoped_lock scope_lock(_dropofflock);
    unsigned cleared = 0;
    unsigned db = 0;
    NodePtr curr = _head;
    while(curr != 0) {
        if(curr->_dirtybit) ++db;
        NodePtr next = curr->_next;
        curr->_next.reset();
        if(next)
            next->_prev.reset();
        curr = next;
        ++cleared;
    }
    _tail.reset();
    _head.reset();
    _dropped_off = 0;
    return cleared;
}

template<class Type>
unsigned DropoffQueue<Type>::size() {
    typedef boost::shared_ptr<Node<Type> > NodePtr;
    boost::mutex::scoped_lock scope_lock(_dropofflock);
    unsigned size = 0;
    NodePtr curr = _head;
    while(curr != 0) {
        ++size;
        curr = curr->_next;
    }
    return size;
}

template<class Type>
unsigned DropoffQueue<Type>::get_unread() {
    typedef boost::shared_ptr<Node<Type> > NodePtr;
    boost::mutex::scoped_lock scope_lock(_dropofflock);
    unsigned unread_count = 0;
    NodePtr curr = _head;
    while(curr != 0) {
        if(curr->_dirtybit == 0)
            ++unread_count;
        curr = curr->_next;
    }
    return unread_count;
}

#endif
