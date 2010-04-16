
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

#define UNIQUE_VIOLATION "23505"

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
			DLOG("CcltorDB::exec_sql(%s) -> status: %d-%s", sql, status, PQresStatus(status));
			if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK)
			{
				char *errtext = PQresultErrorMessage(result);
				char *sqlstate = PQresultErrorField(result, PG_DIAG_SQLSTATE);
				if (errtext || sqlstate)
				{
					if (sqlstate && (strcmp(sqlstate, UNIQUE_VIOLATION) == 0))
					{
						DLOG("CcltorDB::exec_sql(%s) -> [%d-%s] %s", sql, status, sqlstate ?:"", errtext ?:"");
					}
					else
					{
						ELOG("CcltorDB::exec_sql(%s) -> [%d-%s] %s", sql, status, sqlstate ?:"", errtext ?:"");
					}
					PQclear(result);
					result = NULL;
				}
			}
		}
		return result;
	}
	return NULL;
}
