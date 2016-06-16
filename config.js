config = {};
config.interval = 3600; // seconds between sensor values being uploaded
config.clientId = 1; // client ID, integer, must match sensor_metadata table
config.aws = {};
config.aws.region = 'us-west-2'; // AWS region your IoT service is set up in
module.exports = config;
