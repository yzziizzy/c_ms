#include <stddef.h>
#include <stdio.h>

#include <mysql.h>


#include "schema.h"






int main(int argc, char* argv[]) {
	
	
	
	MYSQL *con = mysql_init(NULL);
	
	if(con == NULL) {
		fprintf(stderr, "%s\n", mysql_error(con));
		exit(1);
	}
	
	mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
	
	unsigned int port = 0;
	unsigned long client_flags = 0;
	if(mysql_real_connect(con, "localhost", "root", "pass", NULL, port, NULL, client_flags) == NULL) {
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		exit(1);
	}  
	
	
	schema_stmts_t* ss;
	ss = prepare_schema(con);
	
	int res;
	schema_db_exists(ss, "foo_test", &res);
	if(res == 0) {
		printf("exists\n");
	} 
	else {
		printf("not exists\n");
		schema_create_db(ss, "foo_test");
	}
	
	
	  
	mysql_close(con);
	
}
