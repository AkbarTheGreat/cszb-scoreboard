#!/bin/bash
cd "${0%/*}"
export PWD=`pwd`
export SSL_CERT_FILE="${PWD}/cert.pem"
export REQUESTS_CA_BUNDLE="${PWD}/cert.pem"
echo "SSL_CERT_FILE=${SSL_CERT_FILE}"
echo "REQUESTS_CA_BUNDLE=${REQUESTS_CA_BUNDLE}"
./cszb-scoreboard
