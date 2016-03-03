#!/usr/bin/env bash
aws iot create-keys-and-certificate --set-as-active --certificate-pem-outfile awsCerts/certificate.pem.crt --public-key-outfile awsCerts/public.pem.key --private-key-outfile awsCerts/private.pem.key > keys_and_certificates.json

# Note the output ARN and use in the following command

aws iot attach-principal-policy --principal "certificate_ARN" --policy-name "PubSubToAnyTopic"
