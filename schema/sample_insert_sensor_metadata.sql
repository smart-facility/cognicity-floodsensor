/* Sample Insert for Sensor Metadata
 * Location is Wollongong (longitude, latitude), WGS 1984
 */

INSERT INTO sensor_metadata(id, height_above_riverbed, location)
VALUES (1, 3.2, ST_GeomFromText('POINT(150.883056 -34.433056)',4326));
