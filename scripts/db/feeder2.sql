
CREATE TABLE feeder_value_ids (
    source_id       text PRIMARY KEY,
    name            text,
    ccltor_id       varchar(16) DEFAULT NULL
);

GRANT ALL ON feeder_value_ids TO manu;

CREATE TABLE feeder_value_data (
    value           text REFERENCES feeder_value_ids,
    date            date DEFAULT CURRENT_DATE,
    time            time,
    price           double precision,
    volume          double precision,
    capital         double precision,
    PRIMARY KEY (value, date, time)
);

GRANT ALL ON feeder_value_data TO manu;
