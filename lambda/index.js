var pg = require('pg');
var config = require('./config.js');

exports.handler = function(event, context) {
  console.log('Received event:');
  console.log(JSON.stringify(event, null, '  '));

  // do stuff to store in pg
  var conn = 'postgres://' + process.env.DB_USERNAME + ':' + process.env.DB_PASSWORD + '@' + process.env.DB_HOSTNAME '/cognicity?ssl=true';

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
      context.done(null,'finished successfully (filtered)');
  }
};
