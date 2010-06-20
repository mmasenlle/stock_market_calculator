
-- copy values to value_ids
INSERT INTO feeder_value_ids (source_id, name)
SELECT CAST (code AS text), name FROM feeder_values;

-- copy prices to data
INSERT INTO feeder_value_data (value, date, time, price, volume, capital)
SELECT CAST (value AS text), date, time, price, volume, capital FROM feeder_prices;
