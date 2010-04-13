
#include "logger.h"
#include "CcltorDB.h"

CcltorDB::CcltorDB() : conn(NULL) {}

CcltorDB::~CcltorDB()
{
	release();
}

int CcltorDB::connect(const char *conninfo)
{
	DLOG("CcltorDB::connect(%s)", conninfo);
	if (0 /*!PQisthreadsafe()*/)
	{
		WLOG("CcltorDB::connect(%s) -> libpq is not thread safe !!");
	}
	conn = PQconnectdb(conninfo);
	ConnStatusType status = PQstatus(conn);
	if (status != CONNECTION_OK)
	{
		ELOG("CcltorDB::connect(%s) -> status not OK %d-'%s'", conninfo, status, PQerrorMessage(conn));
	}
	return status;
}

void CcltorDB::release()
{
	if (conn)
	{
		PQfinish(conn);
		conn = NULL;
	}
}

PGresult *CcltorDB::exec_sql(const char *sql)
{
	if (conn)
	{
		PGresult *result = PQexec(conn, sql);
		if (!result)
		{
			ELOG("CcltorDB::exec_sql(%s) -> result == NULL", sql);
		}
		else
		{
			ExecStatusType status = PQresultStatus(result);
			DLOG("CcltorDB::exec_sql(%s) -> status: %d-'%s'", sql, status, PQresStatus(status));
			char *errtext = PQresultErrorMessage(result);
			if (errtext && *errtext)
			{
				ELOG("CcltorDB::exec_sql(%s) -> status: %d, error: '%s'", sql, status, errtext);
				PQclear(result);
				result = NULL;
			}
		}
		return result;
	}
	return NULL;
}
