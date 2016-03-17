var config = require('./config.js');
var awsIot = require('aws-iot-device-sdk');
var usonic = require('r-pi-usonic');

var device = awsIot.device({
   keyPath: './awsCerts/private.pem.key',
  certPath: './awsCerts/certificate.pem.crt',
    caPath: './rootCA.pem',
  clientId: config.clientId,
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
            var average = 0;
            var count = 0;
			var averagingInterval = setInterval( function() {
				count++
				average += sensor();
				if (count==5) {
					clearInterval(averagingInterval);
				}
			}, 1000);
			             
            var sensor_reading = (average/5).toFixed(2);
            
            // publish heigh = max_distance - distance from sensor_reading.
            device.publish('topic/floodsensor', JSON.stringify({id: config.clientId, time: (new Date().valueOf(), height: config.max_distance - sensor_reading}));
            console.log('published'+JSON.stringify({id: config.clientId, time: (new Date().valueOf(), height: config.max_distance - sensor_reading}));
          },config.interval*1000);
        }
    });
  });
