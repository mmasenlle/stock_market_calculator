
#include <stdio.h>
#include "logger.h"
#include "DBfeeder.h"

int main(int argc, char *argv[])
{
	INIT_DEFAULT_LOGGER;
	
	const char *conninfo = "";
	if (argc > 1) conninfo = argv[1];
	
	ILOG("testDB-> connecting with '%s' ...", conninfo);
	CcltorDB db;
	int r = db.connect(conninfo);
	ILOG("testDB-> db.init(%s) = %d - press enter to continue", conninfo, r);
	getchar();
	
	ILOG("testDB-> sending feed 'test:555@testing.org', 115");
	r = DBfeeder::insert_feed(&db, "test:555@testing.org", 115);
	ILOG("testDB-> sending feed r = %d - press enter to continue", r);
	getchar();
	
	ILOG("testDB-> sending price PR PRUEBA 1.1 2.2 3.3 083500");
	r = DBfeeder::insert_price(&db, "PR", "PRUEBA", 1.1, 2.2, 3.3, 83500);
	ILOG("testDB-> sending price r = %d - press enter to continue", r);
	getchar();
	
	ILOG("testDB-> get value codes");
	std::vector<std::string> codes;
	r = DBfeeder::get_value_codes(&db, &codes);
	for (int i = 0; i < codes.size(); i++) DLOG("testDB-> code[%d] = %s", i, codes[i].c_str());
	ILOG("testDB-> get value codes r = %d - press enter to continue", r);
	getchar();
	
	ILOG("testDB-> get value of PR");
	std::string name;
	r = DBfeeder::get_value_name(&db, "PR", &name);
	DLOG("testDB-> name = %s", name.c_str());
	ILOG("testDB-> get value r = %d - press enter to continue", r);
	getchar();
	
	DLOG("testDB-> closing connection ...");
	db.release();
	DLOG("testDB-> we are done");
}
