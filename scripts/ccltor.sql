CREATE TABLE feeder_feeds (
    feeder          text,
    time            timestamp DEFAULT CURRENT_TIMESTAMP,
    count           integer,
    PRIMARY KEY (feeder, time)
);

CREATE TABLE feeder_values (
    code            varchar(16) PRIMARY KEY,
    name            text
);

CREATE TABLE feeder_prices (
    value           varchar(16) REFERENCES feeder_values,
    date            date DEFAULT CURRENT_DATE,
    time            time,
    price           double precision,
    volume          double precision,
    capital         double precision,
    PRIMARY KEY (value, date, time)
);

CREATE TABLE statistics_of_day (
    value           varchar(16) REFERENCES feeder_values,
    date            date,
    cnt_price       integer,
    cnt_volume      integer,
    cnt_capital     integer,
    min_price       double precision,
    min_volume      double precision,
    min_capital     double precision,
    mean_price      double precision,
    mean_volume     double precision,
    mean_capital    double precision,
    max_price       double precision,
    max_volume      double precision,
    max_capital     double precision,
    std_price       double precision,
    std_volume      double precision,
    std_capital     double precision,
    PRIMARY KEY (value, date)
);

CREATE TABLE statistics_of_month (
    value           varchar(16) REFERENCES feeder_values,
    date            date, -- day 1 of the month
    cnt_price       integer,
    cnt_volume      integer,
    cnt_capital     integer,
    min_price       double precision,
    min_volume      double precision,
    min_capital     double precision,
    mean_price      double precision,
    mean_volume     double precision,
    mean_capital    double precision,
    max_price       double precision,
    max_volume      double precision,
    max_capital     double precision,
    std_price       double precision,
    std_volume      double precision,
    std_capital     double precision,
    PRIMARY KEY (value, date)
);

CREATE TABLE statistics_of_year (
    value           varchar(16) REFERENCES feeder_values,
    date            date, -- day 1 of january of the year
    cnt_price       integer,
    cnt_volume      integer,
    cnt_capital     integer,
    min_price       double precision,
    min_volume      double precision,
    min_capital     double precision,
    mean_price      double precision,
    mean_volume     double precision,
    mean_capital    double precision,
    max_price       double precision,
    max_volume      double precision,
    max_capital     double precision,
    std_price       double precision,
    std_volume      double precision,
    std_capital     double precision,
    PRIMARY KEY (value, date)
);

