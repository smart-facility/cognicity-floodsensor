let config = require('./config.js');
let awsIoT = require('aws-iot-device-sdk');
let logger = require('winston');
let fs = require('fs');
let path = require('path');
let SerialPort = require('serialport');
let exec = require('child_process').exec;

let device = awsIoT.device({
   keyPath: './awsCerts/private.pem.key',
  certPath: './awsCerts/certificate.pem.crt',
    caPath: './rootCA.pem',
  clientId: 'sensor' + config.clientId.toString(),
    region: config.aws.region
});

// Set up logging to log files
let logPath = ( config.logger.logDirectory ? config.logger.logDirectory : __dirname );
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

let port = new SerialPort(
  '/dev/ttyS0',
  {
    baudRate: 115200,
    dataBits: 8,
    parity: 'none',
    stopBits: 1,
    parser: SerialPort.parsers.readline('\n')
  }
);

port.on('open', function() {
  logger.info('serial port open');
});

let dataJSON = {};

let processData = function(data) {
  arrayOfData = data.split(',');

  device.publish('topic/floodsensor', JSON.stringify({id: config.clientId, time: new Date().valueOf - Number(arrayOfData[0]), distance: Number(arrayOfData[1])/100, temperature: Number(arrayOfData[2]), humidity: Number(arrayOfData[3])}));

}

device
  .on('connect', function() {
    port.on('data', function(data) {
      switch(data) {
        case 'PING':
          port.write('OK\n');
          break;
        case 'CONNECTED':
          port.write('DUMP\n');
          break;
        case 'FINISHED':
          //exec('/sbin/shutdown -h now', function (msg) {
          //  logger.info(msg) }
          //);
          break;
        default:
          processData(data);
      }
    });
  });
