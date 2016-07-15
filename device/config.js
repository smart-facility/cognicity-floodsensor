config = {};
config.interval = 1800; // seconds between sensor values being uploaded
config.clientId = 1; // client ID, integer, must match sensor_metadata table
config.hasDHT = true; // if there's a DHT temperature and humidity sensor attached


config.aws = {};
config.aws.region = 'us-west-2'; // AWS region your IoT service is set up in

// Logging configuration
config.logger = {};
config.logger.level = "info"; // What level to log at; info, verbose or debug are most useful. Levels are (npm defaults): silly, debug, verbose, info, warn, error.
config.logger.maxFileSize = 1024 * 1024 * 100; // Max file size in bytes of each log file; default 100MB
config.logger.maxFiles = 10; // Max number of log files kept
config.logger.logDirectory = '/home/pi/cognicity-floodsensor/logs'; // Set this to a full path to a directory - if not set logs will be written to the application directory.
config.logger.logFileName = 'cognicity-floodsensor.log'

module.exports = config;
