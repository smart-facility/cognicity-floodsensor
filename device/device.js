var config = require('./config.js');
var awsIot = require('aws-iot-device-sdk');
var usonic = require('r-pi-usonic');
var dht_sensor_lib = require('node-dht-sensor');

var device = awsIot.device({
   keyPath: './awsCerts/private.pem.key',
  certPath: './awsCerts/certificate.pem.crt',
    caPath: './rootCA.pem',
  clientId: config.clientId.toString(),
    region: config.aws.region
});

device
  .on('connect', function() {
    console.log('connect');
    usonic.init(function (error) { // initialise the sensor
        if (error) {
            console.log('Error')
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
                  device.publish('topic/floodsensor', JSON.stringify({id: config.clientI, time: (new DATE()).valueOf(), distance: ultrasound_sensor_reading, temperature: dht_readout.temperature.toFixed(2), humidity: dht_readout.humidity.toFixed(2)}));
                } else {
                  device.publish('topic/floodsensor', JSON.stringify({id: config.clientId, time: (new Date()).valueOf(), distance: ultrasound_sensor_reading}));
                  console.log('published'+JSON.stringify({id: config.clientId, time: (new Date()).valueOf(), distance: ultrasound_sensor_reading}));
                }
                clearInterval(averagingInterval);
              }
            }, 1000);
          }, config.interval*1000);
        }
      });
  });
