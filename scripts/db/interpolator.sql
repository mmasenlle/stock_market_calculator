
CREATE TABLE interpolator (
	value	varchar(16),
	date	date,
	PP		double precision,
	error	double precision,
	a0		double precision,
	a1		double precision,
	a2		double precision,
	a3		double precision,
	a4		double precision,
	a5		double precision,
	a6		double precision,
	a7		double precision,
	a8		double precision,
	a9		double precision,
	a10		double precision,
	a11		double precision,
	a12		double precision,
	a13		double precision,
	a14		double precision,
	a15		double precision,
	a16		double precision,
	a17		double precision,
	a18		double precision,
	a19		double precision,
	PRIMARY KEY (value, date)
);

GRANT ALL ON interpolator TO manu;
