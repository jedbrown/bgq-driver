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
#include <boost/throw_exception.hpp>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include <boost/thread/once.hpp>
#include <boost/bind.hpp>
#include <utility/include/UserId.h>
#include <utility/include/portConfiguration/SslConfiguration.h>
#include "cxxsockets/SocketTypes.h"
#include "cxxsockets/CxxSocket.h"
#include "CxxSocketInlines.h"

using namespace CxxSockets;

LOG_DECLARE_FILE( "utility.cxxsockets" );

namespace {

boost::once_flag init_once_flag = BOOST_ONCE_INIT;

void
init_ssl()
{
    LOG_DEBUG_MSG( "initializing SSL library" );
    SSL_library_init();
}

}

SecureTCPSocket::SecureTCPSocket() :
    TCPSocket()
{
    boost::call_once( &init_ssl, init_once_flag );
    _ctx = 0;
    _ssl = 0;
}

SecureTCPSocket::SecureTCPSocket(
        int family, 
        int fd
        ) :
    TCPSocket(family, fd)
{
    boost::call_once( &init_ssl, init_once_flag );
    _ctx = 0;
    _ssl = 0;
}

SecureTCPSocket::~SecureTCPSocket() {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    LOG_TRACE_MSG(__FUNCTION__ << " " << _ssl << " " << _fileDescriptor);
    if(_ctx)
        SSL_CTX_free(_ctx);
    if(_ssl)            
        SSL_free(_ssl);
    _ctx = 0;
    _ssl = 0;
}


void getErrStr(std::ostringstream& error) {
    int errorint = 0;    
    do {
        errorint = ERR_get_error();
        char* errorstring = (char*)(malloc(128));
        bzero(errorstring, 128);
        ERR_error_string(errorint, errorstring);
        error << errorstring << ", ";
        free(errorstring);
    } while (errorint != 0);
    return;
}

std::string SecureTCPSocket::printSSLError(SSL* ssl, int rc) {
    int sslerror = SSL_get_error(ssl, rc);
    std::ostringstream error;
    switch(sslerror) {
    case SSL_ERROR_NONE:
        error << "SSL_ERROR_NONE";
        break;
    case SSL_ERROR_ZERO_RETURN:
        error << "SSL_ERROR_ZERO_RETURN";
        break;
    case SSL_ERROR_WANT_READ:
        error << "SSL_ERROR_WANT_READ";
        break;
    case SSL_ERROR_WANT_WRITE:
        error << "SSL_ERROR_WANT_WRITE";
        break;
    case SSL_ERROR_WANT_CONNECT:
        error << "SSL_ERROR_WANT_CONNECT";
        break;
    case SSL_ERROR_WANT_ACCEPT:
        error << "SSL_ERROR_WANT_ACCEPT";
        break;
    case SSL_ERROR_WANT_X509_LOOKUP:
        error << "SSL_ERROR_WANT_X509_LOOKUP";
        break;
    case SSL_ERROR_SYSCALL:
        error << "SSL_ERROR_SYSCALL";
        break;
    case SSL_ERROR_SSL:
        error << "SSL_ERROR_SSL: ";
        break;
    }
    SSL_load_error_strings();
    getErrStr(error);
    LOG_DEBUG_MSG(error.str());
    return error.str();
}

std::string extract_peer_cn(SSL* ssl)
{
    X509 *cert(SSL_get_peer_certificate(ssl));
    int error = 0;
    if ( ! cert ) {
        std::string estr = SecureTCPSocket::printSSLError(ssl, error);
        throw SockHardError(error, "failed to get peer certificate" + estr);
    }

    X509_NAME *subject_name(X509_get_subject_name( cert ));

    if ( ! subject_name ) {
        std::string estr = SecureTCPSocket::printSSLError(ssl, error);
        throw SockHardError(error, "failed to get peer subject name" + estr);
    }

    char peer_cn[256];
    int rc = X509_NAME_get_text_by_NID( subject_name, NID_commonName, peer_cn, sizeof ( peer_cn ) );

    if ( rc == -1 ) {
        std::string estr = SecureTCPSocket::printSSLError(ssl, error);
        throw SockHardError(error, "failed to get peer CN from certificate" + estr);
    }

    return std::string(peer_cn);
}

int SecureTCPSocket::ServerHandshake(const bgq::utility::ServerPortConfiguration& port_config) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    const std::string administrative_cn( port_config.getAdministrativeCn() );
    const std::string command_cn( port_config.getCommandCn() );
    const bool administrative_only( port_config.getConnectionType() == 
                                    bgq::utility::ServerPortConfiguration::ConnectionType::AdministrativeOnly );
    // Get the client's CN so can compare it vs the expected CNs.
    std::string client_cn;

    client_cn = extract_peer_cn(_ssl);

    LOG_DEBUG_MSG( "Client's CN='" << client_cn << "'" );

    if(client_cn == administrative_cn) {
        _utype = CxxSockets::Administrator;
    } else if (client_cn == command_cn) {
        _utype = CxxSockets::Normal;
    } else {
        throw std::runtime_error(std::string() + 
                                 "client sent certificate with invalid CN. The client's CN is '" 
                                 + client_cn + "'");
    }

    if ( administrative_only && (_utype != Administrator ) ) {
        throw std::runtime_error(std::string() + 
                                 "only accepting administrative certificate and client using command certificate");
    }

    // Read the user info off the socket.
    typedef std::vector<char> _UserIdBuf;
    typedef boost::shared_ptr<_UserIdBuf> UserIdBufPtr;
    UserIdBufPtr user_id_buf_ptr( new _UserIdBuf );
    user_id_buf_ptr->resize( 4 ); // read 4 bytes off the socket, which is the length in ascii text.

    // Get the user id info here
    Message msg;
    SecureTCPReceiveFunctor recvf(_ssl);
    InternalReceiveUnManaged(msg, 4, 0, recvf);
    uint32_t user_id_size;
    if(msg.str() == "") {
        throw CxxSockets::SockHardError(0,"No User ID received on this connection.");
    }

    try {
        user_id_size = boost::lexical_cast<uint32_t>(msg.str());
    } catch (boost::bad_lexical_cast& e) {
        throw CxxSockets::SockHardError(0, e.what());
    }

    if ( user_id_size == 0 ) {
        // No user info provided. Only if using administrative certificate is this ok.
        if ( _utype != Administrator ) {
            throw std::runtime_error(std::string() +  
                                     "handshake with client failed because user info was not sent and the administrative certificate wasn't used");
        }
        
        // only OK because using the administrative certificate.
        LOG_INFO_MSG( "Client didn't send user info but used administrative certificate." );
        
        return 0;
    }

    user_id_buf_ptr->resize(user_id_size);
    Message uidmsg;
    unsigned int bytes = InternalReceiveUnManaged(uidmsg, user_id_size, 0, recvf);
    if(bytes != user_id_size) {
        std::ostringstream msg;
        msg << "Unable to receive user id. " << bytes << " bytes received, expected " << user_id_size << " bytes.";
        LOG_ERROR_MSG(msg.str());
        throw CxxSockets::SockHardError(0, msg.str());
    }

    LOG_DEBUG_MSG("Received serialized user ID: " << uidmsg.str());

    const std::string buf( uidmsg.str() );
    _UserIdBuf ubuff(buf.begin(), buf.end());
    bgq::utility::UserId::ConstPtr user_id_ptr(new bgq::utility::UserId(ubuff));
    _uid_ptr = user_id_ptr;
    
    return 0;
}

int SecureTCPSocket::ClientHandshake(const bgq::utility::ClientPortConfiguration& port_config) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    // Get the server's CN so can compare it vs the expected administrative certificate CN.
    std::string server_cn;

    try {
        server_cn = extract_peer_cn(_ssl);
    } catch ( std::exception& e ) {
        std::string error_str( std::string() + "handshake failed, couldn't get commonName from subject name from server's certificate" );
        LOG_ERROR_MSG( error_str );
        throw CxxSockets::SockHardError(0, error_str); 
    }

    LOG_INFO_MSG( "Server's CN='" << server_cn << "'" );

    if ( server_cn != port_config.getAdministrativeCn() ) {
        std::string error_str( std::string() + "server sent invalid certificate with cn='" + server_cn + "' expected '" + port_config.getAdministrativeCn() + "'" );
        throw CxxSockets::SockHardError(0, error_str); 
    }

    // Send the user info.
    //  Gather the user info and serialize it to a std::string.
    //  Sends the length as 4 ascii bytes, then sends the user id
    bgq::utility::UserId user_id;

    std::string user_id_str( user_id.serialize() );
    uint32_t user_id_str_len( user_id_str.size() );

    std::ostringstream oss;
    oss.width( 4 );
    oss.fill( '0' );
    oss << user_id_str_len;

    Message msg;
    msg << oss.str();
    msg << user_id_str;

    LOG_TRACE_MSG ("Sending client uid =" << msg.str());
    SecureTCPSendFunctor sendf(_ssl);
    InternalSendUnmanaged(msg, 0, sendf);

    return 0;
}

void SecureTCPSocket::SetupCredentials(const bgq::utility::SslConfiguration& sslconfig) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    LOG_DEBUG_MSG("Using cert file=" << sslconfig.getMyCertFilename());
    int rc = SSL_CTX_use_certificate_chain_file(_ctx, sslconfig.getMyCertFilename().c_str());
    if(rc != 1) {
        std::ostringstream errstr;
        getErrStr(errstr);
        LOG_ERROR_MSG("Setting certificate chain file failed. " << errstr.str());
        throw CxxSockets::SockHardError(0, errstr.str());
    }

    LOG_DEBUG_MSG("Using private key file=" << sslconfig.getMyPrivateKeyFilename());
    rc = SSL_CTX_use_PrivateKey_file(_ctx, sslconfig.getMyPrivateKeyFilename().c_str(), SSL_FILETYPE_PEM);
    if (!rc) {
        std::ostringstream errstr;
        getErrStr(errstr);
        LOG_ERROR_MSG("Setting private key file failed. " << errstr.str());
        throw CxxSockets::SockHardError(0, errstr.str());
    }

    char* certpath = 0;
    char* certfile = 0;

    if(sslconfig.getCaCertificatesPath() != 0) {
        // add verify path getCaCertificatesPath
        LOG_DEBUG_MSG("Using CA path=" << sslconfig.getCaCertificatesPath());
        certpath = (char*)(sslconfig.getCaCertificatesPath()->c_str());
    }

    if(sslconfig.getCaCertificateFilename() != 0) {
        LOG_DEBUG_MSG("Using CA filename " << *(sslconfig.getCaCertificateFilename()));
        certfile = (char*)(sslconfig.getCaCertificateFilename()->c_str());
    }

    rc = SSL_CTX_load_verify_locations(_ctx, certfile, certpath);
    if(rc == 0) {
        std::ostringstream errstr;
        getErrStr(errstr);
        LOG_ERROR_MSG("Loading verify locations failed. " << errstr.str());
        throw CxxSockets::SockHardError(0, errstr.str());
    }

    // Default paths!
    if(sslconfig.getCaUseDefaultPaths() == true) {
        rc = SSL_CTX_set_default_verify_paths(_ctx);
        if (rc == 0 ) {
            std::ostringstream errstr;
            getErrStr(errstr);
            LOG_ERROR_MSG("Setting default very paths failed. " << errstr.str());
            throw CxxSockets::SockHardError(0, errstr.str());
        }
    }
}

void SecureTCPSocket::SetupContext(const bgq::utility::SslConfiguration& sslconf) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    LOG_TRACE_MSG(__FUNCTION__);
    if(_ssl || _ctx) return;

    // Set up context
    _ctx = SSL_CTX_new(TLSv1_method());
    if(!_ctx) {
        std::ostringstream errstr;
        getErrStr(errstr);
        LOG_ERROR_MSG("Creating SSL context failed." << errstr.str());
    }

    SSL_CTX_set_verify(_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, 0);
    SetupCredentials(sslconf);
    _ssl = SSL_new(_ctx);

    // Set up the bio
    std::ostringstream hostport;
    _cnnbio = BIO_new_socket(_fileDescriptor, BIO_NOCLOSE);
    SSL_set_bio(_ssl, _cnnbio, _cnnbio);
}

void SecureTCPSocket::MakeSecure(const bgq::utility::ClientPortConfiguration& port_config) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    LOG_DEBUG_MSG("handshaking with remote host");
    try {
        bgq::utility::SslConfiguration sslconf = port_config.createSslConfiguration();
        SetupContext(sslconf);
        // If keepalive isn't on, turn it on for the ssl connect
        int opt = 0;
        socklen_t optlen = sizeof(opt);
        if(getsockopt(_fileDescriptor, SOL_SOCKET, SO_KEEPALIVE, &opt, &optlen)) {
            throw CxxSockets::SockHardError(errno, "Could not determine if keepalive was on while securing socket.");
        }
        if(!opt) {
            setProbe(true,2,2,5);
        }
        // ssl connect.
        int rc = SSL_connect(_ssl);
        if(!opt) // If we turned on keepalive, turn it back off.  Leave things as they were.
            setProbe(false);
        if(rc < 0) {
            std::string errormsg = printSSLError(_ssl,rc);
            LOG_ERROR_MSG(errormsg);
            throw CxxSockets::SockHardError(SSL_ERROR, errormsg);
        }
        LOG_DEBUG_MSG("SSL connected");
        // Complete handshake
        ClientHandshake(port_config);
    } catch(std::runtime_error& e) {
        std::ostringstream msg;
        msg << "SSL handshake failed " << e.what();
        LOG_ERROR_MSG(msg.str());
        throw CxxSockets::SockHardError(SSL_ERROR, msg.str());
    }
    LOG_DEBUG_MSG("authorization complete");
}

void SecureTCPSocket::MakeSecure(const bgq::utility::ServerPortConfiguration& port_config) {
    try {
        bgq::utility::SslConfiguration sslconf = port_config.createSslConfiguration();
        SetupContext(sslconf);
        int rc = SSL_accept(_ssl);
        if(rc <= 0) {
            std::string errormsg = SecureTCPSocket::printSSLError(_ssl, rc);
            LOG_ERROR_MSG(errormsg);
            throw CxxSockets::SockHardError(SSL_ERROR, errormsg);
        }
        LOG_DEBUG_MSG("ssl accept successful");
        ServerHandshake(port_config);
        LOG_INFO_MSG("Accepted secure socket file descriptor " << _fileDescriptor);
    } catch (std::runtime_error& e) {
        std::ostringstream msg;
        msg << "MakeSecure: SSL handshake failed accepting new connection " << e.what();
        LOG_ERROR_MSG(msg.str());
        throw CxxSockets::SockHardError(SSL_ERROR, msg.str());
    }
}

void SecureTCPSocket::Connect(SockAddr& remote_sa, const bgq::utility::ClientPortConfiguration& port_config) {
    LOGGING_DECLARE_FD_MDC; 
    FileLocker locker;
    LockFile(locker);

    LOG_DEBUG_MSG("Connecting secure TCP socket.");

    // Call the base unlocked, unsecured method and then
    // do our own security work.
    TCPSocket::mConnect(remote_sa);
    LOGGING_DECLARE_FT_MDC;
    MakeSecure(port_config);
}

int SecureTCPSocket::Send(Message& msg, int flags) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    // We do the locking and build the functor for the
    // send method we need to pass to the send logic
    PthreadMutexHolder mutex;
    int lockrc = LockSend(mutex);
    if(lockrc != 0) {
        std::ostringstream msg;
        if(lockrc != -1)
            msg << "Send error.  Socket send side lock error: " << strerror(lockrc);
        else
            msg << "Send error.  Socket send side closed: " << 0;
        LOG_INFO_MSG(msg.str());
        throw CxxSockets::SockSoftError(lockrc, msg.str());
    }

    SecureTCPSendFunctor sendf(_ssl);
    return InternalSend(msg, flags, sendf);
}

int SecureTCPSocket::SendUnManaged(Message& msg, int flags) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    PthreadMutexHolder mutex;
    int lockrc = LockSend(mutex);
    if(lockrc != 0) {
        std::ostringstream msg;
        if(lockrc != -1)
            msg << "SendUnManaged error.  Socket send side lock error: " << strerror(lockrc);
        else
            msg << "SendUnManaged error.  Socket send side closed: " << 0;
        throw CxxSockets::SockSoftError(lockrc, msg.str());
    }

    SecureTCPSendFunctor sendf(_ssl);
    return InternalSendUnmanaged(msg, flags, sendf);
}

int SecureTCPSocket::Receive(Message& msg, int flags) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    PthreadMutexHolder mutex;
    int lockrc = LockReceive(mutex);

    if(lockrc != 0) {
        std::ostringstream msg;
        if(lockrc != -1)
            msg << "Receive error.  Socket receive side lock error: " << strerror(lockrc);
        else
            msg << "Receive error.  Socket receive side closed: " << 0;
        LOG_DEBUG_MSG(msg.str());
        throw CxxSockets::SockSoftError(lockrc, msg.str());
    }

    SecureTCPReceiveFunctor recvf(_ssl);
    return InternalReceive(msg, flags, recvf);
}

int SecureTCPSocket::ReceiveUnManaged(Message& msg, unsigned int bytes, int flags){
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    PthreadMutexHolder mutex;
    int lockrc = LockReceive(mutex);
    if(lockrc != 0) {
        std::ostringstream msg;
        if(lockrc != -1)
            msg << "ReceiveUnManaged error.  Socket receive side lock error: " << strerror(lockrc);
        else
            msg << "ReceiveUnManaged error.  Socket receive side closed: " << 0;
        LOG_INFO_MSG(msg.str());
        throw CxxSockets::SockSoftError(lockrc, msg.str());
    }

    SecureTCPReceiveFunctor recvf(_ssl);
    return InternalReceiveUnManaged(msg, bytes, flags, recvf);
}
