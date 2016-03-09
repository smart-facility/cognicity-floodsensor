var awsIot = require('aws-iot-device-sdk');

var usonic = require('r-pi-usonic');

var device = awsIot.device({
   keyPath: './awsCerts/private.pem.key',
  certPath: './awsCerts/certificate.pem.crt',
    caPath: './rootCA.pem',
  clientId: process.env.CLIENT_ID,
    region: 'us-west-2'
});

//
// Device is an instance returned by mqtt.Client(), see mqtt.js for full
// documentation.
//


const offset = 100; // cm, needs calibration

device
  .on('connect', function() {
    console.log('connect');
    usonic.init(function (error) {
        if (error) {
            console.log('Error')
        } else {
          var sensor = usonic.createSensor(24, 23, 450); // needs calibration
          var sensor_reading = sensor().toFixed(2);
          device.publish('topic/floodsensor', JSON.stringify({ height: offset - sensor_reading}));
          console.log('published'+JSON.stringify({height: offset - sensor_reading}));
        }
    });
  });
