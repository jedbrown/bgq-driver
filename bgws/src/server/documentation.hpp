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


/*! \mainpage

The \subpage webServices provides a
<a href="http://en.wikipedia.org/wiki/Representational_State_Transfer#RESTful_web_services">RESTful web service</a>
for the Blue Gene system.

(C) Copyright IBM Corp.  2010, 2011

Eclipse Public License (EPL)

  
\internal

BGWS is primary accessed by the Blue Gene Navigator administrative web application.
These web services can also be accessed using
- the command line by using the \subpage commandClients "command clients"
- the curl command line utility, \subpage curlexamples
- any other HTTP client, for example, <a href="http://docs.python.org/library/httplib.html">Python</a>
- there are some \subpage sampleClients "sample clients" that use the BGWS like the Navigator (not using the command or administrative certificates).


The \subpage server implements the services.

\section Setup

- \subpage apacheSetup
- \ref sampleClientSetup "Setting up the sample clients"

\section Development

- \subpage personalSetup

 */

/*! \page webServices Blue Gene Web Services (BGWS)

\section resources Web Service Resources

\subsection Runtime

- \subpage blocksResource
- \subpage blockResource
- \subpage jobsResource
- \subpage jobResource

\subsection Alerts

- \subpage alertsResource
- \subpage alertResource

\subsection RAS

- \subpage rasResource
- \subpage rasDetailsResource

\subsection Hardware

- \subpage machineResource
- \subpage hardwareResource
- \subpage hardwareReplacementsResource

\subsection Diagnostics

- \subpage diagnosticsBlocksResource
- \subpage diagnosticsBlockResource
- \subpage diagnosticsLocationsResource
- \subpage diagnosticsRunsResource
- \subpage diagnosticsRunResource
- \subpage diagnosticsTestcasesResource

\subsection Environmentals

- \subpage bulkPowerEnvironmentalsResource
- \subpage coolantEnvironmentalsResource
- \subpage fanEnvironmentalsResource
- \subpage ioCardEnvironmentalsResource
- \subpage linkChipEnvironmentalsResource
- \subpage nodeEnvironmentalsResource
- \subpage nodeCardEnvironmentalsResource
- \subpage opticalEnvironmentalsResource
- \subpage serviceCardEnvironmentalsResource
- \subpage serviceCardTempEnvironmentalsResource

\subsection serviceActionResources Service Actions

- \subpage serviceActionsResource
- \subpage serviceActionResource

\subsection Summary

- \subpage jobsSummaryResource
- \subpage summaryMachineResource
- \subpage rasSummaryResource
- \subpage systemSummaryResource
- \subpage utilizationSummaryResource
- \subpage perfResource

\subsection bgwsServerResources BGWS Server

- \subpage bgwsServerResource
- \subpage loggingResource
- \subpage sessionsResource
- \subpage sessionResource


\subsection userAuth User authentication

There are two ways that a user authenticates to the BGWS:
- \ref Sessions "creating a session"
- presenting a Blue Gene certificate and sending the user information in a header

One way to authenticate to the BGWS is to
connect using a Blue Gene certificate and sending the user information in a header.
Customer applications can't use this method because we don't document
the format of the serialized bgq::utility::UserId information.
If the client presents the command certificate,
the request must include the X-Bgws-User-Id with the user's
serialized bgq::utility::UserId information.
If the client presents the administrative certificate,
the request may include the X-Bgws-User-Id header.
If the X-Bgws-User-Id header is not present then the
request will be processed as that user, otherwise it will be some anonymous administrator.


\subsection Sessions

The normal way that applications authenticate to the BGWS is by creating a session.
This is the only way to authenticate when proxying through a web server,
which is how the Navigator is used.

A user creates a session by \ref sessionsResourcePost "POSTing their credentials to the /bg/bgws/sessions resource".

See \ref serverPwauth to see what the server does with the credentials.

If the credentials are accepteed,
a session resource is created and
the server responds with a <em>201 Created</em> response with
the session URI in the <em>Location</em> header.
The session URI contains the session ID as the last component.

This client must send this session ID on subsequent requests to identify the user.
The session ID is sent by setting the
<b>X-Bgws-Session-Id</b> request header to the session ID.

A session ID is a random string of letters and numbers.

A session is represented on the BGWS server by a URI, like \ref sessionResource.

A session will be destroyed if it's not used for some period of time.
The timeout period is \ref serverConfiguration "configurable".

Sessions can be destroyed (ended) by sending a \ref sessionResource "DELETE request on the session's URI".

Sessions are not persistent and are destroyed when the BGWS server shuts down.

A request containing an invalid session ID will fail with an HTTP status of <b>400 Bad Request</b>. The ID in the \ref bgwsErrors "error document" is <b>invalidSession</b>.


\section inputFormat Input format

BGWS expects input whenever the method is PUT or POST.
The input must be in <a href="http://www.json.org">JSON format</a>.
There are JSON libraries for most languages out there.

The client should set the <b>Content-Type</b> request header to <b>application/json</b>.
If the client doesn't set the Content-Type, then application/json is assumed.
If the client sets the Content-Type to something other than application/json,
the response will be <b>415 Unsupported Media Type</b>.

\see \ref requestDataErrors


\section outputFormat Output format

Output is also in <a href="http://www.json.org">JSON format</a>.

\section partialResponse Partial response

Some resources support partial responses.
This is used to limit the amount of data the server has to provide to the clients when a request could generate a large response.
If the resource supports partial responses, the following applies:

Clients can request to only receive part of the total result set by
setting the <b>Range</b> header in the request.
The Range value must be formatted like

<pre>
Range: items=<i>x</i>-<i>y</i>
</pre>

Where the x is the first item number to receive and y is the last.
Item numbers start at 0 (rather than 1).

If the server doesn't send back the full results,
it will set the status in the response to <b>206 Partial Content</b>
and the Content-range header to the range of items and the total number of items.
The format of the Content-range header is: items <i>x</i>-<i>y</i>/<i>t</i>


If the resource does not support partial responses,
the Range header is ignored and the server will always send all the results.


\section bgwsErrors Errors

When an error occurs,
the HTTP status in the response will be set to an appropriate value.
For example, if a block doesn't exist the status code will
be <b>404 Not Found</b>.

See http://www.ietf.org/rfc/rfc2616.txt for the HTTP status codes.

Along with the status code, if the Content-Type is application/json,
then the response contains an \subpage errorDocument
that further describes the error.

- \subpage requestDataErrors
- \subpage error_Authentication

 */

/*! \page curlexamples curl examples
 *

Note that when using curl (except when starting a session), use <b>--header "X-Bgws-Session-Id: replaceme"</b> to set the session ID.


\section startSession Start a session

Replace myuserid:mypassword with your username and password.

<pre>
$ <b>echo -n "myuserid:mypassword" | base64</b>
bXl1c2VyaWQ6bXlwYXNzd29yZA==

$ <b>curl -i -d@- -H "Content-Type: application/json" --insecure https://localhost/bg/bgws/sessions
{
  "auth" : "bXl1c2VyaWQ6bXlwYXNzd29yZA=="
}
^D
</b>
HTTP/1.1 201 Created
Date: Thu, 01 Jul 2010 21:35:56 GMT
Server: Apache/2.2.3 (Red Hat)
Location: /bg/bgws/sessions/mc1HUWsfUiiXmKePMAaE
Connection: close
Transfer-Encoding: chunked

</pre>

The session ID is in the Location header.


\section endSession End a session

<pre>
$ <b>curl -i -X DELETE -H "X-Bgws-Session-Id: mc1HUWsfUiiXmKePMAaE" --insecure "https://localhost/bg/bgws/sessions/mc1HUWsfUiiXmKePMAaE"</b>
HTTP/1.1 204 No Content
Date: Thu, 01 Jul 2010 21:49:32 GMT
Server: Apache/2.2.3 (Red Hat)
Connection: close
Content-Type: application/x-sh
</pre>

\section createBlock Create block

Create a block on R00-M0 called R000. Note that curl -d@- issues a POST request, reading the post data from stdin.
<pre>
$ <b>curl -d@- --header "Content-Type: application/json" --header "X-Bgws-Session-Id: replaceme" -i --insecure https://localhost/bg/blocks
{
  "id" : "R000",
  "midplanes" : [ "R00-M0" ]
}
^D
</b>
HTTP/1.1 201 Created
Location: /bg/blocks/R000
</pre>


\section blockSummary Block summary

Get a summary of blocks on the system. curl without any options issues a GET request.
<pre>
$ <b>curl -H "X-Bgws-Session-Id: replaceme" --insecure https://localhost/bg/blocks</b>

{
  "R000" : {
      "URI" : "\/bg\/blocks\/R000",
      "computeNodeCount" : 512,
      "description" : "Generated via web services",
      "ioNodeCount" : 0,
      "owner" : "",
      "status" : "Free",
      "statusSequenceId" : 0,
      "torus" : "ABCDE"
    },
  ...
}
</pre>


\section deleteBlock Delete block

Delete a block. curl -X DELETE issues a DELETE request.
<pre>
$ <b>curl -i -X DELETE -H "X-Bgws-Session-Id: replaceme" --insecure https://localhost/bg/blocks/R000</b>
HTTP/1.1 204 No Content
</pre>

 */

/*! \page apacheSetup Setting up the BGWS server

Administrators should follow these instructions to get the BGWS server configured and running.


\section prereq Download the prerequisite packages

It's recommended to configure the Apache server to use SSL,
which is enabled by installing the mod_ssl package.
This package is shipped with RedHat, so use yum to install it.

<pre>
# yum install mod_ssl pam-devel
</pre>

You will also need the pwauth package, download it from the Internet:

- http://code.google.com/p/pwauth/


\section createUser Create a bgws user

<b>Internally</b>:
Create a %bgws system account with uid 495, like this:

<pre>
# useradd -u 495 -c "Blue Gene Web Services" -r %bgws
</pre>


<b>Customer</b>:
Create a %bgws system account.
The BGWS server will run under this account.
The pwauth program will be configured such that only %bgws can run it
for security reasons.

<pre>
# useradd -c "Blue Gene Web Services" -r %bgws
</pre>


\section setupPwauth Set up pwauth

<b>Internally</b>:

pwauth was built on bgqts6sn. Copy it from there, it's /usr/local/libexec/pwauth.

<b>Customer</b>:
Un-tar the pwauth you downloaded.

cd into the source directory.
Edit config.h: comment out SHADOW_SUN, uncomment PAM, set SERVER_UIDS to the %bgws uid.

Also, in Makefile (same directory as config.h), need to comment out this line:

<pre>
#%LIB= -lcrypt
</pre>

and uncomment this one:

<pre>
%LIB=-lpam -ldl
</pre>

Build pwauth and install it to /usr/local/libexec with the correct permissions (setuid) using the following commands:

<pre>
$ make
# cp pwauth /usr/local/libexec/pwauth
# chown root:root /usr/local/libexec/pwauth
# chmod 4755 /usr/local/libexec/pwauth
</pre>

pwauth in PAM mode uses the "pwauth" PAM stack, so create a pwauth pam config in /etc/pam.d,

<pre>
# cd /etc/pam.d
# ln -s system-auth pwauth
</pre>


\section configureBgwsServer Configure the BGWS server

Check \ref serverConfiguration "the BGWS server configuration options" to see if there's anything you want to change in there
(like the machine_name).


\section bgmasterConfig Configure bgmaster

\note This should all be set up in the template bg.properties so you shouldn't have to do anything.

BG Master should start up the bgws_server automatically.

Here's how that's configured, (in /bgsys/local/etc/bg.properties by default):

in [master.binmap], bgws_server=/bgsys/drivers/ppcfloor/bgws/sbin/bgws_server

in [master.startup], add bgws_server to start_order.


After changing the bg.properties file, tell bgmaster to re-read its config %file:
<pre>
$ bgmaster_server_refresh_config
</pre>

Start the bgws_server:
<pre>
$ master_start bgws_server
</pre>

Check the BGWS server log to make sure it started up OK, it's /bgsys/logs/BGQ/*bgws_server.log


\section apacheConfigBgws Configure Apache to forward BGWS requests to the BGWS server

The BGWS server is an HTTP server, but it only serves BGWS.
To run the Blue Gene Navigator administrative web application,
Apache should be configured to serve up the BGWS server by proxy
and the Navigator, so when the Apache server gets a request
for in the /bg resource space, it forwards the request
to the BGWS server and then sends the response back to the client.

Apache must use SSL/TLS when proxying the BGWS server
so Apache must accept the administrative certificate
presented by the BGWS server.

To configure Apache,
create a symlink from /etc/httpd/conf.d/bgws.conf to bgws.conf in the driver.

<pre>
# ln -s /bgsys/drivers/ppcfloor/bgws/etc/bgws.conf /etc/httpd/conf.d/
</pre>

Optionally,
copy /bgsys/drivers/ppcfloor/bgws/etc/bgws.conf to /etc/httpd/conf.d/bgws.conf and make any changes necessary.

\section startApache Start Apache

Here's how you start apache:

<pre>
# /sbin/service httpd start
</pre>

You should configure Apache to start automatically.

<pre>
# /sbin/chkconfig httpd on
</pre>

 */


/*! \page personalSetup Setting up a personal instance of BGWS and Navigator

\section personalBgwsConfig Configure personal BGWS server

Edit your personal bg.properties,
change the port that the BGWS server runs on.

Set the path to your personal pwauth executable in your properties file.

Start up your personal instance of the BGWS server.

<pre>
$ ./bgws_server --properties ~/bgq/bg.properties
</pre>

\section personalApacheSetup Set up your personal Apache instance

<pre>
mkdir -p ~/bgq/personal-apache
cd ~/bgq/personal-apache
cp -r /etc/httpd/conf .

mkdir conf.d
cp ~/bgq/sb/bgq/bgws/bgws.conf conf.d/
cp ~/bgq/sb/bgq/navigator/navigator.conf conf.d/
cp /etc/httpd/conf.d/ssl.conf conf.d/

ln -s /etc/httpd/modules

mkdir logs
mkdir run

mkdir security
sudo cp /etc/pki/tls/certs/localhost.crt security/
sudo cp /etc/pki/tls/private/localhost.key security/
sudo chown bknudson:bknudson security/*

</pre>


\section s2 Modify configuration files

<pre>
vi conf/httpd.conf
</pre>

- Change ServerRoot to ~/bgq/personal-apache
- Comment out Listen 80 (you'll use the SSL port only)
- Comment out User and Group (httpd runs as you)

<pre>
vi conf.d/ssl.conf
</pre>

- Change Listen to 55901 or whatever, change "VirtualHost _default_:443" to "VirtualHost _default_:55901"
- Change SSLCertificateFile to security/localhost.crt
- Change SSLCertificateKeyFile to security/localhost.key

<pre>
vi conf.d/bgws.conf
</pre>

 - Change the port to your personal BGWS instance port.

<pre>
vi conf.d/navigator.conf
</pre>

 - Change Alias to your work directory.


\section startup Start Apache with your ServerRoot

To start your Apache instance (note that it forks into the background):

<pre>
/usr/sbin/httpd -d ~/bgq/personal-apache
</pre>

To shut it down:

<pre>
/usr/sbin/httpd -d ~/bgq/personal-apache -k stop
</pre>

 */


/*! \page timestampFormat Timestamp Format
 *

JSON doesn't have a data type for timestamps,
so BGWS uses the following convention.

Timestamps are JSON strings and follow the
<a href="http://en.wikipedia.org/wiki/ISO_8601#Time_intervals">ISO8601 format</a>,
formatted like "<YYYYMMDD>T<HH:MM:SS.ssssss>".

For example: <pre>2010-10-01T14:31:09.080616</pre>

 */
