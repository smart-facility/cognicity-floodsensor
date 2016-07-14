#!/usr/bin/env bash

# Set up the role using the AWS policy as in actions_role.json
aws iam create-role --role-name iot-actions-role --assume-role-policy-document file://assume_role_policy.json > assume_role_policy_output.json
aws iam create-policy --role-name iot-actions-role --policy-document file://actions-role.json > actions_role_policy_output.json

# You must edit the following command to replace policy-ARN with the actual policy ARN from actions_role_policy_output.json putting the ARN inside the quotes.
aws iam attach-role-policy --role-name iot-actions-role --policy-arn "policy-ARN"

# Define a pubsub topic and assign policy
aws iot create-policy --policy-name PubSubToAnyTopic --policy-document file://iot_policy.json
