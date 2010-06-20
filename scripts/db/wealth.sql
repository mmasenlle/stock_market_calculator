
CREATE TABLE wealth_sday (
    market          text,
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
    PRIMARY KEY (market, date)
);

GRANT ALL ON wealth_sday TO manu;

CREATE TABLE wealth_trends (
    market  text,
	date	date,
	P		double precision,
	R1		double precision,
	S1		double precision,
	R2		double precision,
	S2		double precision,
	R3		double precision,
	S3		double precision,
	R4		double precision,
	S4		double precision,
	MF		double precision,
    PRIMARY KEY (market, date)
);

GRANT ALL ON wealth_trends TO manu;

CREATE TABLE wealth_trends_acum (
    market  text,
	date	date,
	SMA		double precision,
	MAD		double precision,
	CCI		double precision,
	ROC		double precision,
	AD		double precision,
	MFI		double precision,
	OBV		double precision,
    PRIMARY KEY (market, date)
);

GRANT ALL ON wealth_trends_acum TO manu;
