
CREATE TABLE interpolator_equation (
	value	varchar(16),
	date	date,
	type	integer,
	id		SERIAL UNIQUE,
	error	double precision,
	PRIMARY KEY (value, date, type)
);
GRANT ALL ON interpolator_equation TO manu;

CREATE TABLE interpolator_coefficients (
	id		integer REFERENCES interpolator_equation(id),
	num		integer,
	val		double precision,
	PRIMARY KEY (id, num)
);
GRANT ALL ON interpolator_coefficients TO manu;

CREATE TABLE interpolator_results (
	value		varchar(16),
	date		date,
	equation	integer REFERENCES interpolator_equation(id),
	result		double precision,
	PRIMARY KEY (value, date, equation)
);
GRANT ALL ON interpolator_results TO manu;
