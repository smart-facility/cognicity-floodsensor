var config = require('./config.js');
var usonic = require('r-pi-usonic');
var dht_sensor_lib = require('node-dht-sensor');
var logger = require('winston');
var fs = require('fs');
var pg = require('pg');
var path = require('path');

// Set up logging to log files
var logPath = ( config.logger.logDirectory ? config.logger.logDirectory : __dirname );
// Check that log file directory can be written to
try {
	fs.accessSync(logPath, fs.W_OK);
} catch (e) {
	console.log( "Log directory '" + logPath + "' cannot be written to"  );
	throw e;
}
logPath += path.sep + config.logger.logFileName;

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



var publish = function(event) {


	var conn = config.pg.conString; // Could set via config stored in S3 or in Lambda

	if (Number(event.distance) < 450 && Number(event.temperature) > 0) {

		var client = new pg.Client(conn);
		client.connect();

		var query = client.query(
			{
				text: "INSERT INTO sensor_data (sensor_id, measurement_time, distance, temperature, humidity)" +
				"VALUES (" +
				"$1, " +
				"to_timestamp($2), " +
				"$3," +
				"$4," +
				"$5" +
				");",
				values: [
					event.id,
					event.time/1000,
					event.distance,
					event.temperature,
					event.humidity
				]
			}
		);

		query.on("row", function (row, result) {
			result.addRow(row);
		});

		query.on("end", function (result) {
			var jsonString = JSON.stringify(result.rows);
			var jsonObj = JSON.parse(jsonString);
			client.end();
			context.succeed(jsonObj);
			context.done(null,'finished successfully');
		});
	} else {
		var jsonString = JSON.stringify(event);
		var jsonObj = JSON.parse(jsonString);
		logger.info('finished successfully (filtered)');
	}
}


logger.info('connect');
usonic.init(function (error) { // initialise the sensor
	if (error) {
		logger.error('Error initialising ultrasound sensor.');
	} else {
		var ultrasound_sensor = usonic.createSensor(24, 23, config.usonic_timeout);
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
						var event = {distance: ultrasound_sensor_reading, id: config.clientId, time: new Date().valueOf(), temperature: dht_readout.temperature.toFixed(2), humidity: dht_readout.humidity.toFixed(2)};
						publish(event);
						logger.info('published '+JSON.stringify({id: config.clientId, time: (new Date()).valueOf(), distance: ultrasound_sensor_reading, temperature: dht_readout.temperature.toFixed(2), humidity: dht_readout.humidity.toFixed(2)}));
					} else {
						var event = {distance: ultrasound_sensor_reading, id: config.clientId, time: new Date().valueOf()};
						publish(event);
						logger.info('published '+JSON.stringify({id: config.clientId, time: (new Date()).valueOf(), distance: ultrasound_sensor_reading}));
					}
					clearInterval(averagingInterval);
				}
			}, 1000);
		}, config.interval*1000);
	}
});
