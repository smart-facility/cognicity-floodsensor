//test_query.js - manually test insert query to postgres database

var pg = require('pg');

// do stuff to store in pg
var conn = "pg://postgres@localhost:5432/cognicity_schema_2_0_2"; // Could set via config stored in S3 or in Lambda

var client = new pg.Client(conn);
client.connect();

//var query = client.query("SELECT * FROM BLA WHERE ID = 1"); // do something with event, which is object as per JSON emitted in device.js#24

var data = {'id':1, 'time':0, 'distance':100.1};

var query = client.query(
  {
    text: "INSERT INTO sensor_data (sensor_id, measurement_time, distance, temperature)" +
    "VALUES (" +
    "$1, " +
    "to_timestamp($2), " +
    "$3," +
    "$4" +
    ");",
    values: [
      data.id,
      data.time,
      data.distance,
      data.temperature
    ]
  }
);

query.on("row", function (row, result) {
  result.addRow(row);
});
query.on("end", function (result) {
  console.log('end');
  client.end();
});
