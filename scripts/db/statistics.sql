
CREATE TABLE statistics_of_day (
    value           varchar(16),
    date            date,
    cnt_price       integer,
    cnt_volume      integer,
    cnt_capital     integer,
    open_price      double precision,
    open_volume     double precision,
    open_capital    double precision,
    close_price     double precision,
    close_volume    double precision,
    close_capital   double precision,
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

GRANT ALL ON statistics_of_day TO manu;

CREATE TABLE statistics_of_month (
    value           varchar(16),
    date            date, -- day 1 of the month
    cnt_price       integer,
    cnt_volume      integer,
    cnt_capital     integer,
    open_price      double precision,
    open_volume     double precision,
    open_capital    double precision,
    close_price     double precision,
    close_volume    double precision,
    close_capital   double precision,
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

GRANT ALL ON statistics_of_month TO manu;

CREATE TABLE statistics_of_year (
    value           varchar(16),
    date            date, -- day 1 of january of the year
    cnt_price       integer,
    cnt_volume      integer,
    cnt_capital     integer,
    open_price      double precision,
    open_volume     double precision,
    open_capital    double precision,
    close_price     double precision,
    close_volume    double precision,
    close_capital   double precision,
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

GRANT ALL ON statistics_of_year TO manu;
