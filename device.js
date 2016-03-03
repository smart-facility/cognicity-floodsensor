var awsIot = require('aws-iot-device-sdk');

var device = awsIot.device({
   keyPath: '~/awsCerts/private.pem.key',
  certPath: '~/awsCerts/certificate.pem.crt',
    caPath: '~/awsCerts/root-CA.crt',
  clientId: 'myAwsClientId',
    region: 'us-east-1'
});

//
// Device is an instance returned by mqtt.Client(), see mqtt.js for full
// documentation.
//
device
  .on('connect', function() {
    console.log('connect');
    device.subscribe('topic/floodsensor');
    device.publish('topic/floodsensor', JSON.stringify({ test_data: 1}));
    });

device
  .on('message', function(topic, payload) {
    console.log('message', topic, payload.toString());
  });
  