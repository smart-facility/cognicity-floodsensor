var awsIot = require('aws-iot-device-sdk');

var device = awsIot.device({
   keyPath: '~/awsCerts/private.pem.key',
  certPath: '~/awsCerts/certificate.pem.crt',
    caPath: '~/rootCA.crt',
  clientId: process.env.CLIENT_ID,
    region: 'us-west-2',
      host: process.env.ENDPOINT,
  protocol: mqtt,
      port: 8883
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
