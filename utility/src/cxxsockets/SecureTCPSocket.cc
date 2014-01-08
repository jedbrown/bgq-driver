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

#include "cxxsockets/SecureTCPSocket.h"

#include "cxxsockets/FileLocker.h"

#include "portConfiguration/SslConfiguration.h"

#include <boost/lexical_cast.hpp>
#include <boost/thread/once.hpp>
#include <boost/bind.hpp>

LOG_DECLARE_FILE( "utility.cxxsockets" );

namespace {

boost::once_flag init_once_flag = BOOST_ONCE_INIT;

void
init_ssl()
{
    LOG_TRACE_MSG( "Initializing SSL library." );
    SSL_library_init();
}

void
getErrStr(
        std::ostringstream& message
        )
{
    size_t count = 0;
    while ( 1 ) {
        const unsigned long error = ERR_get_error();
        if ( !error ) break;

        if ( ++count > 1 ) {
            message << ", ";
        }

        char buf[256] = {0};
        ERR_error_string_n(error, buf, sizeof(buf));
        
        message << buf;
    }
}

std::string
printSSLError(
        SSL* ssl,
        const int rc
        )
{
    const int sslerror = SSL_get_error(ssl, rc);
    std::ostringstream error;
    switch (sslerror) {
        case SSL_ERROR_NONE:
            error << "SSL_ERROR_NONE: ";
            break;
        case SSL_ERROR_ZERO_RETURN:
            error << "SSL_ERROR_ZERO_RETURN: ";
            break;
        case SSL_ERROR_WANT_READ:
            error << "SSL_ERROR_WANT_READ: ";
            break;
        case SSL_ERROR_WANT_WRITE:
            error << "SSL_ERROR_WANT_WRITE: ";
            break;
        case SSL_ERROR_WANT_CONNECT:
            error << "SSL_ERROR_WANT_CONNECT: ";
            break;
        case SSL_ERROR_WANT_ACCEPT:
            error << "SSL_ERROR_WANT_ACCEPT: ";
            break;
        case SSL_ERROR_WANT_X509_LOOKUP:
            error << "SSL_ERROR_WANT_X509_LOOKUP: ";
            break;
        case SSL_ERROR_SYSCALL:
            error << "SSL_ERROR_SYSCALL: ";
            break;
        case SSL_ERROR_SSL:
            error << "SSL_ERROR_SSL: ";
            break;
    }
    SSL_load_error_strings();
    getErrStr(error);
    return error.str();
}

} // anonymous namespace

namespace CxxSockets {

SecureTCPSocket::SecureTCPSocket(
        const TCPSocketPtr& socket,
        const bgq::utility::ServerPortConfiguration& port_config
        ) :
    TCPSocket( 0 /* family */, socket->getFileDescriptor() )
{
    boost::call_once( &init_ssl, init_once_flag );
    _ctx = 0;
    _ssl = 0;

    socket->releaseFd();

    try {
        this->MakeSecure( port_config );
    } catch ( const std::exception& e ) {
        LOG_DEBUG_MSG( e.what() );

        // clean up memory for resources we potentially allocated
        if (_ctx) SSL_CTX_free(_ctx);
        if (_ssl) SSL_free(_ssl);
        throw;
    }
}

SecureTCPSocket::SecureTCPSocket(
        const int family,
        const int fd
        ) :
    TCPSocket(family, fd)
{
    boost::call_once( &init_ssl, init_once_flag );
    _ctx = 0;
    _ssl = 0;
}

SecureTCPSocket::~SecureTCPSocket()
{
    // LOG_TRACE_MSG(__FUNCTION__ << " " << _ssl << " " << _fileDescriptor);
    if (_ctx) {
        SSL_CTX_free(_ctx);
    }

    if (_ssl) {
        SSL_free(_ssl);
    }

    _ctx = 0;
    _ssl = 0;
}

std::string
extract_peer_cn(
        SSL* ssl
        )
{
    const boost::shared_ptr<X509> cert(
            SSL_get_peer_certificate( ssl ),
            boost::bind( &X509_free, _1)
            );
    if ( ! cert ) {
        int error = 0;
        const std::string estr = printSSLError(ssl, error);
        throw HardError(error, "Failed to get peer certificate " + estr);
    }

    X509_NAME *subject_name(X509_get_subject_name( cert.get() ));

    if ( ! subject_name ) {
        int error = 0;
        const std::string estr = printSSLError(ssl, error);
        throw HardError(error, "Failed to get peer subject name " + estr);
    }

    char peer_cn[256];
    const int rc = X509_NAME_get_text_by_NID( subject_name, NID_commonName, peer_cn, sizeof(peer_cn) );

    if ( rc == -1 ) {
        int error = 0;
        const std::string estr = printSSLError(ssl, error);
        throw HardError(error, "Failed to get peer CN from certificate " + estr);
    }

    return std::string(peer_cn);
}

void
SecureTCPSocket::ServerHandshake(
        const bgq::utility::ServerPortConfiguration& port_config
        )
{
    const std::string administrative_cn( port_config.getAdministrativeCn() );
    const std::string command_cn( port_config.getCommandCn() );
    const bool administrative_only(
            port_config.getConnectionType() ==
            bgq::utility::ServerPortConfiguration::ConnectionType::AdministrativeOnly
            );

    // Get the client's CN so can compare it vs the expected CNs.
    const std::string client_cn( extract_peer_cn(_ssl) );
    LOG_TRACE_MSG( "Client's CN='" << client_cn << "'" );

    if (client_cn == administrative_cn) {
        _utype = Administrator;
    } else if (client_cn == command_cn) {
        _utype = Normal;
    } else {
        throw std::runtime_error("Client sent certificate with invalid CN. The client's CN is '" + client_cn + "'");
    }

    if ( administrative_only && (_utype != Administrator) ) {
        throw std::runtime_error("Only accepting administrative certificate and client using command certificate.");
    }

    // Read the user info off the socket.
    char buffer[5];
    memset( buffer, 0, sizeof(buffer) );
    SecureTCPReceiveFunctor recvf(_ssl);
    int rc = recvf( _fileDescriptor, buffer, sizeof(buffer) - 1 );

    if (rc < 0) {
        char buf[256];
        strerror_r( errno, buf, sizeof(buf) );
        throw HardError(errno, std::string() + "No User ID received on this connection:" + buf);
    } else if ( rc == 0 ) {
        throw HardError(0, "No User ID size received on this connection." );
    } else if ( rc != sizeof(buffer) - 1 ) {
        throw HardError(0, 
                "Received " + boost::lexical_cast<std::string>(rc) + " bytes of expected " +
                boost::lexical_cast<std::string>(sizeof(buffer) - 1) + " user ID size bytes"
                );
    }

    uint32_t user_id_size;
    try {
        user_id_size = boost::lexical_cast<uint32_t>(buffer);
    } catch (const boost::bad_lexical_cast& e) {
        throw HardError(0, std::string() + "Could not convert user size (" + buffer + ") to a number: " + e.what());
    }

    if ( user_id_size == 0 ) {
        // No user info provided. Only if using administrative certificate is this ok.
        if ( _utype != Administrator ) {
            throw std::runtime_error(std::string() +
                                     "Handshake with client failed because user info was not sent and the administrative certificate was not used.");
        }

        // Only OK because using the administrative certificate.
        LOG_TRACE_MSG( "Client didn't send user info but used administrative certificate." );

        return;
    }

    std::vector<char> uid( user_id_size );
    rc = recvf( _fileDescriptor, &uid[0], user_id_size );

    if (rc < 0) {
        char buf[256];
        strerror_r( errno, buf, sizeof(buf) );
        throw HardError(errno, std::string() + "No User ID received on this connection:" + buf);
    } else if ( rc == 0 ) {
        throw HardError(0, "No User ID received on this connection." );
    } else if ( static_cast<uint32_t>(rc) != user_id_size ) {
        throw HardError(0, 
                "Received " + boost::lexical_cast<std::string>(rc) + " bytes of expected " +
                boost::lexical_cast<std::string>(user_id_size) + " bytes"
                );
    }

    _uid_ptr.reset( new bgq::utility::UserId(uid) );
    LOG_TRACE_MSG("Received serialized user ID: " << _uid_ptr->getUser() );
}

void
SecureTCPSocket::ClientHandshake(
        const bgq::utility::ClientPortConfiguration& port_config
        )
{
    // Get the server's CN so can compare it vs the expected administrative certificate CN.
    const std::string server_cn( extract_peer_cn(_ssl) );
    LOG_TRACE_MSG( "Server's CN='" << server_cn << "'" );

    if ( server_cn != port_config.getAdministrativeCn() ) {
        const std::string error_str( std::string() + "Server sent invalid certificate with CN='" + server_cn + "' expected '" + port_config.getAdministrativeCn() + "'" );
        LOG_DEBUG_MSG( error_str );
        throw HardError(0, error_str);
    }

    // Send the user info.
    // Gather the user info and serialize it to a std::string.
    // Sends the length as 4 ascii bytes, then sends the user id.
    bgq::utility::UserId user_id;

    const std::string user_id_str( user_id.serialize() );
    const size_t user_id_str_len( user_id_str.size() );

    std::ostringstream oss;
    oss.width( 4 );
    oss.fill( '0' );
    oss << user_id_str_len;

    Message msg;
    msg << oss.str();
    msg << user_id_str;

    const std::string buffer( msg.str() );
    LOG_TRACE_MSG("Sending client uid: " << buffer);
    SecureTCPSendFunctor sendf(_ssl);
    sendf( _fileDescriptor, buffer.c_str(), buffer.size());
    // FIXME check return code?
}

void
SecureTCPSocket::SetupCredentials(
        const bgq::utility::SslConfiguration& sslconfig
        )
{
    LOG_TRACE_MSG("Using cert file=" << sslconfig.getMyCertFilename());
    int rc = SSL_CTX_use_certificate_chain_file(_ctx, sslconfig.getMyCertFilename().c_str());
    if (rc != 1) {
        std::ostringstream errstr;
        getErrStr(errstr);
        LOG_DEBUG_MSG("Setting certificate chain file failed: " << errstr.str());
        throw HardError(0, errstr.str());
    }

    LOG_TRACE_MSG("Using private key file=" << sslconfig.getMyPrivateKeyFilename());
    rc = SSL_CTX_use_PrivateKey_file(_ctx, sslconfig.getMyPrivateKeyFilename().c_str(), SSL_FILETYPE_PEM);
    if (!rc) {
        std::ostringstream errstr;
        getErrStr(errstr);
        LOG_DEBUG_MSG("Setting private key file failed: " << errstr.str());
        throw HardError(0, errstr.str());
    }

    char* certpath = 0;
    char* certfile = 0;

    if (sslconfig.getCaCertificatesPath() != 0) {
        // Add verify path getCaCertificatesPath
        LOG_TRACE_MSG("Using CA path=" << sslconfig.getCaCertificatesPath());
        certpath = (char*)(sslconfig.getCaCertificatesPath()->c_str());
    }

    if (sslconfig.getCaCertificateFilename() != 0) {
        LOG_TRACE_MSG("Using CA filename " << *(sslconfig.getCaCertificateFilename()));
        certfile = (char*)(sslconfig.getCaCertificateFilename()->c_str());
    }

    rc = SSL_CTX_load_verify_locations(_ctx, certfile, certpath);
    if (rc == 0) {
        std::ostringstream errstr;
        getErrStr(errstr);
        LOG_DEBUG_MSG("Loading verify locations failed: " << errstr.str());
        throw HardError(0, errstr.str());
    }

    // Default paths!
    if (sslconfig.getCaUseDefaultPaths() == true) {
        rc = SSL_CTX_set_default_verify_paths(_ctx);
        if (rc == 0 ) {
            std::ostringstream errstr;
            getErrStr(errstr);
            LOG_DEBUG_MSG("Setting default verify paths failed: " << errstr.str());
            throw HardError(0, errstr.str());
        }
    }
}

void
SecureTCPSocket::SetupContext(
        const bgq::utility::SslConfiguration& sslconf
        )
{
    if (_ssl || _ctx) {
        return;
    }

    // Set up context
    _ctx = SSL_CTX_new(TLSv1_method());
    if (!_ctx) {
        std::ostringstream errstr;
        getErrStr(errstr);
        LOG_ERROR_MSG("Creating SSL context failed: " << errstr.str());
        // Should this code really just continue?
    }

    SSL_CTX_set_verify(_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, 0);
    SetupCredentials(sslconf);
    _ssl = SSL_new(_ctx);

    // Set up the bio, the SSL object will own the bio so we don't need to
    // explicitly free it with BIO_free
    BIO* const cnnbio = BIO_new_socket(_fileDescriptor, BIO_NOCLOSE);
    SSL_set_bio(_ssl, cnnbio, cnnbio);
}

void
SecureTCPSocket::MakeSecure(
        const bgq::utility::ClientPortConfiguration& port_config
        )
{
    LOG_TRACE_MSG("Starting SSL handshake with remote host.");
    try {
        const bgq::utility::SslConfiguration sslconf = port_config.createSslConfiguration();
        this->SetupContext(sslconf);
        // If keepalive isn't on, turn it on for the ssl connect
        int opt = 0;
        socklen_t optlen = sizeof(opt);
        if (getsockopt(_fileDescriptor, SOL_SOCKET, SO_KEEPALIVE, &opt, &optlen)) {
            throw HardError(errno, "Could not determine if keepalive was on while securing socket.");
        }
        if (!opt) {
            setProbe(true,2,2,5);
        }
        const int rc = SSL_connect(_ssl);
        if (!opt) {  // If we turned on keepalive, turn it back off. Leave things as they were.
            setProbe(false);
        }
        if (rc < 0) {
            const std::string errormsg = printSSLError(_ssl, rc);
            LOG_DEBUG_MSG(errormsg);
            throw HardError(-1, errormsg);
        }
        LOG_TRACE_MSG("SSL connected.");
        // Complete handshake
        this->ClientHandshake(port_config);
    } catch (const std::runtime_error& e) {
        std::ostringstream msg;
        msg << "SSL handshake failed: " << e.what();
        LOG_DEBUG_MSG(msg.str());
        throw HardError(-1, msg.str());
    }
    LOG_TRACE_MSG("SSL handshake with remote host completed successfully.");
}

void
SecureTCPSocket::MakeSecure(
        const bgq::utility::ServerPortConfiguration& port_config
        )
{
    try {
        const bgq::utility::SslConfiguration sslconf = port_config.createSslConfiguration();
        this->SetupContext(sslconf);
        const int rc = SSL_accept(_ssl);
        if (rc <= 0) {
            const std::string errormsg = printSSLError(_ssl, rc);
            LOG_DEBUG_MSG(errormsg);
            throw HardError(-1, errormsg);
        }
        LOG_TRACE_MSG("SSL accept was successful.");
        ServerHandshake(port_config);
        LOG_TRACE_MSG("Accepted secure socket file descriptor " << _fileDescriptor);
    } catch (const std::runtime_error& e) {
        std::ostringstream msg;
        msg << "SSL handshake failed accepting new connection: " << e.what();
        LOG_DEBUG_MSG(msg.str());
        throw HardError(-1, msg.str());
    }
}

void
SecureTCPSocket::Connect(
        const SockAddr& remote_sa,
        const bgq::utility::ClientPortConfiguration& port_config
        )
{
    FileLocker locker;
    LockFile(locker);

    // Call the base unlocked, unsecured method and then do our own security work.
    TCPSocket::mConnect(remote_sa);
    MakeSecure(port_config);
}

int
SecureTCPSocket::Send(
        const Message& msg
        )
{
    // We do the locking and build the functor for the
    // send method we need to pass to the send logic.
    PthreadMutexHolder mutex;
    const int lockrc = LockSend(mutex);

    if (lockrc != 0) {
        std::ostringstream msg;
        if (lockrc != -1) {
            msg << "Send error, socket send side lock error: " << strerror(lockrc);
        } else {
            msg << "Send error, socket send side closed.";
        }
        LOG_DEBUG_MSG(msg.str());
        throw SoftError(lockrc, msg.str());
    }

    SecureTCPSendFunctor sendf(_ssl);
    return InternalSend(msg, sendf);
}

int
SecureTCPSocket::Receive(
        Message& msg
        )
{
    PthreadMutexHolder mutex;
    const int lockrc = LockReceive(mutex);

    if (lockrc != 0) {
        if (lockrc != -1) {
            std::ostringstream msg;
            msg << "Receive error, socket receive side lock error: " << strerror(lockrc);
            LOG_DEBUG_MSG( msg.str() );
            throw SoftError(lockrc, msg.str());
        }

        throw SoftError(0, "Receive error, socket receive side closed.");
    }

    SecureTCPReceiveFunctor recvf(_ssl);
    return InternalReceive(msg, recvf);
}

int
SecureTCPSendFunctor::operator()(
        int,
        const void* msg,
        size_t length
        )
{
    const int rc = SSL_write(_ssl, msg, static_cast<int>(length));
    if (rc <= 0) {
        const std::string msg = printSSLError(_ssl, rc);
        LOG_DEBUG_MSG( msg );
    }
    return rc;
}

int
SecureTCPReceiveFunctor::operator()(
        int,
        const void* msg,
        size_t length
        )
{
    const int rc = SSL_read(_ssl, const_cast<void*>(msg), static_cast<int>(length));
    if (rc < 0) {
        const std::string msg = printSSLError(_ssl, rc);
        LOG_DEBUG_MSG( msg );
    }
    return rc;
}

} // namespace CxxSockets
