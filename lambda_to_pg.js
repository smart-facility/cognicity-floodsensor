var pg = require('pg')
var config = require

exports.handler = function(event, context) {
  console.log('Received event:');
  console.log(JSON.stringify(event, null, '  '));

  // do stuff to store in pg
  var conn = "pg://user:password@host:5432/bd_name"; // Could set via config stored in S3 or in Lambda

  var client = new pg.Client(conn);
  client.connect();

  var query = client.query("SELECT * FROM BLA WHERE ID = 1"); // do something with event, which is object as per JSON emitted in device.js#24
  query.on("row", function (row, result) {
    result.addRow(row);
  });
  query.on("end", function (result) {
    var jsonString = JSON.stringify(result.rows);
    var jsonObj = JSON.parse(jsonString);
    console.log(jsonString);
    client.end();
    context.succeed(jsonObj);
  });

  context.done(null,'finished successfully');

};
