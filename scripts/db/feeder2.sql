
CREATE TABLE feeder_value_codes (
    source_id       text PRIMARY KEY,
    name            text,
    code            varchar(16) DEFAULT NULL
);

CREATE TABLE feeder_value_prices (
    value           varchar(16) REFERENCES feeder_value_codes,
    date            date DEFAULT CURRENT_DATE,
    time            time,
    price           double precision,
    volume          double precision,
    capital         double precision,
    PRIMARY KEY (value, date, time)
);

CREATE TABLE feeder_orphan_prices (
    source_id       text REFERENCES feeder_value_codes,
    date            date DEFAULT CURRENT_DATE,
    time            time,
    price           double precision,
    volume          double precision,
    capital         double precision,
    PRIMARY KEY (source_id, date, time)
);
