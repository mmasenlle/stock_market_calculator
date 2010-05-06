
CREATE TABLE trends (
	value	varchar(16) REFERENCES feeder_values,
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
	PRIMARY KEY (value, date)
);

CREATE TABLE trends_acum (
	value	varchar(16) REFERENCES feeder_values,
	date	date,
	SMA		double precision,
	MAD		double precision,
	CCI		double precision,
	ROC		double precision,
	MFI		double precision,
	PRIMARY KEY (value, date)
);
