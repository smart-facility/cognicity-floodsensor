#!/usr/bin/env bash
while read name
do
  # Create a thing
  aws iot create-thing --thing-name "$name" >> things.json
  # Register a certificate - replace certificate ARN with an actual cerficate ARN
  aws iot attach-thing-principal --thing-name "$name" --principal "certificate_ARN"
done < things.txt
