var config = require('./config.js');
var awsIot = require('aws-iot-device-sdk');
var usonic = require('r-pi-usonic');

var device = awsIot.device({
   keyPath: './awsCerts/private.pem.key',
  certPath: './awsCerts/certificate.pem.crt',
    caPath: './rootCA.pem',
  clientId: config.aws.clientId,
    region: config.aws.region
});

device
  .on('connect', function() {
    console.log('connect');
    usonic.init(function (error) { // initialise the sensor
        if (error) {
            console.log('Error')
        } else {
          setInterval( function() {
            var sensor = usonic.createSensor(24, 23, 450);
            var sensor_reading = sensor().toFixed(2);
            // publish heigh = max_distance - distance from sensor_reading.
            device.publish('topic/floodsensor', JSON.stringify({coordinates: [150.87843, -34.405404], height: config.max_distance - sensor_reading}));
            console.log('published'+JSON.stringify({coordinates: [150.87843, -34.405404], height: config.max_distance - sensor_reading}));
          },3000);
        }
    });
  });
