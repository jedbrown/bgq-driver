
====================================================================

Usage examples can be found in the "hello world" client and
server as well as the test bucket in the src/cxxsockets directories.  
"make all" to produce binaries.

Note:  You'll need a bg.properties file to initialize logging.  
       CxxSockets uses log4cxx as does the SSL code on which it
       depends.  You can modify bgq/utility/etc/bg.properties with
       the following two lines and pass it as a parameter to your
       program:
       log4j.logger.ibm.utility.security       = DEBUG  # Secure socket library
       log4j.logger.ibm.utility.cxxsockets     = DEBUG  # CxxSockets concurrent sockets library 
====================================================================

Dealing with IPv4 and IPv6 addresses:
There are subtleties with how you specify address families and
hostnames or IP addresses and what you actually get.

Family     | Hostname/IP | IPs available | Client/Server | Behavior
AF_UNSPEC  | NULL        | v4 and v6     | S             | One v6 socket listens for both v4 and v6
AF_UNSPEC  | valid name  | v4 and v6     | S             | Two sockets. One for each IP version.
AF_UNSPEC  | NULL        | v4 and v6     | C             | Connect failure
AF_UNSPEC  | valid       | v4 and v6     | C             | Try both serially, v6 first
AF_INET    | NULL        | v4 and v6     | S             | v4 only
AF_INET    | valid       | v4 and v6     | S             | v4 only
AF_INET    | NULL        | v4 and v6     | C             | v4 only
AF_INET    | valid       | v4 and v6     | C             | v4 only
AF_INET6   | NULL        | v4 and v6     | S             | Listens for both v4 and v6 connections on one socket
AF_INET6   | valid       | v4 and v6     | S             | v6 only
AF_INET6   | NULL        | v4 and v6     | C             | Try both serially, v6 first
AF_INET6   | valid       | v4 and v6     | C             | v6 only

If you use sets instead of individual socket objects, the rules are less 
of a concern because some of the subtleties are abstracted away.
For example, if you open a ListeningSocket with AF_UNSPEC and a valid
host/ip, then you'll get a v6 listener ONLY.  Why?  Because a ListeningSocket
only represents one socket so it gets the first valid address which is always
IPv6.

=========================================================================

Managed vs. Unmanaged data transfer:
CxxSockets allows data to be sent and received either managed or unmanaged.
Managed data transfer allows Message objects to be sent atomically without
concern for byte counting, concurrency or completion.  Send a message on one
end, receive it in its entirety on the other.  Unmanaged requires that you
take a more active hand in data management.

* You may use managed I/O when you have CxxSockets on both ends of the connection.
* You must use unmanaged I/O when you may or may not have CxxSockets on one end 
  of the connection.

=========================================================================

Scaling considerations:
Polling allows you to multiplex several sockets in a single thread.  PolledReceive() 
operations on polling and epolling socket sets are managed operations that occur
under the polling set's lock and each socket's receive lock in turn.  They are useful
for small numbers of polled, managed connections.  Larger numbers don't scale well
because the model requires two loops.  The first happens within PolledReceive() as
each socket does a blocking receive() in turn.  The second happens when your code
consumes each message in turn.  For large numbers of sockets, you should use bare
polling operations and manage your own locks on your poll/consume loop.  

=========================================================================

Concurrency considerations:
* Individual sockets are controlled with a socket level lock, a send lock and a receive lock.
  Socket locks are automatic and they will lock what they need when they need it.
  Because socket connections are full duplex, this means that you can get a send lock and
  send data on a socket locked on a receive.  You can also half-close a socket and still get
  the lock for the other half and use it.  The socket-level lock is used for operations 
  other than send/receive such as binding, connecting and closing.
* Socket sets have their own lock and will lock sockets under them as needed
* Polling/Epolling sets will lock themselves and all sockets in the set unless performing
  an explicitly unprotected operation.  Socket objects will =never= lock socket sets so that
  they will not deadlock them.  Blocked sockets will, however, hang sets that attempt
  to use them until they become unblocked.
* Note that there is an inherent race condition between a poll() operation and a receive()
  (or send() ) operation.  If you need to guarantee that a receive() following a poll()
  gets data, then you need to either use a PolledReceive() operation or provide your own
  thread level lock.
* Also note that there is no lock in MsgMap objects.  They are intended to be declared
  as automatic variables and consumed immediately within the thread of execution.
* A complete set of smart pointer typedefs is provided for CxxSocket user objects.  These
  guarantee the existence of the pointee until they are no longer used.  They do the
  right thing on destruction (eg close(), shutdown() etc.)

==========================================================================
