#ifndef CCLTORDB_H_
#define CCLTORDB_H_

#include <libpq-fe.h>

class CcltorDB
{
	PGconn *conn;
public:
	CcltorDB();
	~CcltorDB();
	int connect(const char *conninfo);
	void release();
	PGresult *exec_sql(const char *sql);
};

#endif /*CCLTORDB_H_*/
