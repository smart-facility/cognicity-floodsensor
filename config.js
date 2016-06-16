config = {};
config.interval = 3600; // seconds between sensor values being uploaded
config.clientId = 1; // client ID, integer, must match sensor_metadata table
config.hasDHT = true; // if there's a DHT temperature and humidity sensor attached
config.aws = {};
config.aws.region = 'us-west-2'; // AWS region your IoT service is set up in
module.exports = config;
