// Just the sensing code, and gives raw distance from the sensor, not a height.


var usonic = require('r-pi-usonic');

    usonic.init(function (error) {
        if (error) {
            console.log('Error')
        } else {
          setInterval( function() {
            var sensor = usonic.createSensor(24, 23, 450);
            var sensor_reading = sensor().toFixed(2);
            console.log('published'+JSON.stringify({coordinates: [150.87843, -34.405404], height: sensor_reading}));
          },3000);
        }
    });
