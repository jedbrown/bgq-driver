
# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# ================================================================ 
#                                                                  
# (C) Copyright IBM Corp.  2010, 2011                              
# Eclipse Public License (EPL)                                     
#                                                                  
# ================================================================ 
#                                                                  
# end_generated_IBM_copyright_prolog                               

# Use --noperms to not set permissions


args=`getopt -n $0 -l noperms -o "" -- "$@"`

if [ $? -ne 0 ]
then
  echo "Usage: $0 [--noperms]"
  exit 1
fi

set -- $args

do_perms=yes

for o
do case "$o" in
  --noperms) do_perms=no
esac
done


if [[ $do_perms == "yes" ]]
then
umask 077
fi

mkdir security

mkdir security/ca
 # Certificate Authority files. Will have the self-signed certificate.

mkdir security/admin
 # Administrative certificate files.

mkdir security/command
 # Command certificate files.


# Generate CA private key
openssl genrsa -out security/ca/key.pem 1024

# Create request for CA certificate
openssl req -new -key security/ca/key.pem -sha1 -nodes -subj '/C=US/ST=Minnesota/L=Rochester/O=IBM/OU=STG/CN=Root CA' -out security/ca/req.pem

# Generate CA certificate
openssl x509 -req -in security/ca/req.pem -sha1 -days 9999 -extensions v3_ca -signkey security/ca/key.pem -out security/ca/cert.pem

rm security/ca/req.pem

# Generate CA JKS file
keytool -import -noprompt -alias ca -file security/ca/cert.pem -keystore security/ca/cert.jks -storepass changeit


# Generate administrative private key
openssl genrsa -out security/admin/key.pem 1024

# Create request for administrative certificate
openssl req -new -key security/admin/key.pem -sha1 -nodes -subj '/C=US/ST=Minnesota/L=Rochester/O=IBM/OU=STG/CN=Blue Gene administrative' -out security/admin/req.pem

# Generate administrative certificate
openssl x509 -req -in security/admin/req.pem -sha1 -days 9999 -extensions usr_cert -CA security/ca/cert.pem -CAkey security/ca/key.pem -CAcreateserial -out security/admin/cert.pem
rm security/admin/req.pem

# Verify the administrative certificate
openssl verify -CAfile security/ca/cert.pem security/admin/cert.pem

# Create administrative JKS file
openssl pkcs12 -export -inkey security/admin/key.pem -in security/admin/cert.pem -out security/admin/key.p12 -password pass:changeit
keytool -importkeystore -srckeystore security/admin/key.p12 -srcstoretype PKCS12 -destkeystore security/admin/key.jks -deststorepass changeit -srcstorepass changeit
rm security/admin/key.p12


# Generate command private key
openssl genrsa -out security/command/key.pem 1024

# Create request for command certificate
openssl req -new -key security/command/key.pem -sha1 -nodes -subj '/C=US/ST=Minnesota/L=Rochester/O=IBM/OU=STG/CN=Blue Gene command' -out security/command/req.pem

# Generate command certificate
openssl x509 -req -in security/command/req.pem -sha1 -days 9999 -extensions usr_cert -CA security/ca/cert.pem -CAkey security/ca/key.pem -CAcreateserial -out security/command/cert.pem

rm security/command/req.pem

# Verifying the command certificate
openssl verify -CAfile security/ca/cert.pem security/command/cert.pem


# Clean up
rm -r security/ca/key.pem security/ca/cert.srl


# Here's the suggested permissions.

if [[ $do_perms == "yes" ]]
then

chmod 755 security

chmod 755 security/ca
chmod 644 security/ca/cert.pem
chmod 644 security/ca/cert.jks


chmod 755 security/admin

chgrp bgqadmin security/admin/key.pem
chmod 640 security/admin/key.pem

chgrp bgqadmin security/admin/key.jks
chmod 640 security/admin/key.jks

chmod 644 security/admin/cert.pem


chmod 755 security/command

chown bgqcommand:root security/command/key.pem
chmod 600 security/command/key.pem

chmod 644 security/command/cert.pem

fi

