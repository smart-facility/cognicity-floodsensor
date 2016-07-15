var config = require('./config.js');
var awsIot = require('aws-iot-device-sdk');
var usonic = require('r-pi-usonic');
var dht_sensor_lib = require('node-dht-sensor');
var logger = require('winston');
var fs = require('fs');
var path = require('path');

var device = awsIot.device({
   keyPath: './awsCerts/private.pem.key',
  certPath: './awsCerts/certificate.pem.crt',
    caPath: './rootCA.pem',
  clientId: 'sensor' + config.clientId.toString(),
    region: config.aws.region
});

// Set up logging to log files
var logPath = ( config.logger.logDirectory ? config.logger.logDirectory : __dirname );
// Check that log file directory can be written to
try {
	fs.accessSync(logPath, fs.W_OK);
} catch (e) {
	console.log( "Log directory '" + logPath + "' cannot be written to"  );
	throw e;
}
logPath += path.sep;
logPath += config.instance + ".log";

logger
	.add(logger.transports.File, {
		filename: logPath, // Write to projectname.log
		json: false, // Write in plain text, not JSON
		maxsize: config.logger.maxFileSize, // Max size of each file
		maxFiles: config.logger.maxFiles, // Max number of files
		level: config.logger.level // Level of log messages
	})
	// Console transport is no use to us when running as a daemon
	.remove(logger.transports.Console);


device
  .on('connect', function() {
    logger.info('connect');
    usonic.init(function (error) { // initialise the sensor
        if (error) {
            logger.error('Error initialising ultrasound sensor.');
        } else {
          var ultrasound_sensor = usonic.createSensor(24, 23, 450);
          if (config.hasDHT) {
            dht_sensor_lib.initialize(22,4);
          }
          setInterval( function() {
            var average = 0;
            var count = 0;
            var averagingInterval = setInterval( function() {
              if (count < 5) {
                count++;
                average += ultrasound_sensor();
              } else {
                var ultrasound_sensor_reading = (average/5).toFixed(2);
                // publish reading
                if (config.hasDHT) {
                  var dht_readout = dht_sensor_lib.read();
                  device.publish('topic/floodsensor', JSON.stringify({id: config.clientId, time: (new Date()).valueOf(), distance: ultrasound_sensor_reading, temperature: dht_readout.temperature.toFixed(2), humidity: dht_readout.humidity.toFixed(2)}));
                  logger.info('published '+JSON.stringify({id: config.clientId, time: (new Date()).valueOf(), distance: ultrasound_sensor_reading, temperature: dht_readout.temperature.toFixed(2), humidity: dht_readout.humidity.toFixed(2)}));
                } else {
                  device.publish('topic/floodsensor', JSON.stringify({id: config.clientId, time: (new Date()).valueOf(), distance: ultrasound_sensor_reading}));
                  logger.info('published '+JSON.stringify({id: config.clientId, time: (new Date()).valueOf(), distance: ultrasound_sensor_reading}));
                }
                clearInterval(averagingInterval);
              }
            }, 1000);
          }, config.interval*1000);
        }
      });
  });
