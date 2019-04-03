


#include <mysql.h>


typedef struct schema_col_def {
	char* name;
	char* def_value;
	char* comment; // BUG: not fetched
	enum enum_field_types type;
	
	uint64_t char_max_len;
	int numeric_precision;
	int numeric_scale;
	
	char not_null;
	char auto_inc; // BUG: not fetched
	char def_null; // BUG: not fetched
	char def_fn; // don't escape the value; it's something like NOW() // BUG: not fetched
	char unique_key; // BUG: not fetched
	char primary_key; // BUG: not fetched
	
} schema_col_def_t;



typedef struct schema_index_def {
	char* type;
	char** names;
	char desc;
} schema_index_def_t;


typedef struct schema_table_def {
	char* name;
	schema_col_def_t* columns;
	schema_index_def_t* indices;
	
} schema_table_def_t;


typedef struct schema_stmts {
	MYSQL* con;
	
	MYSQL_STMT* db_exists;
	MYSQL_STMT* table_exists;
	MYSQL_STMT* get_col_def;
	MYSQL_STMT* get_key_def;
	
} schema_stmts_t;


schema_stmts_t* prepare_schema(MYSQL* con);

int schema_create_db(schema_stmts_t* ss, char* name);
int schema_use_db(schema_stmts_t* ss, char* name);
int schema_db_exists(schema_stmts_t* ss, char* name, int* res);
int schema_ensure_db(schema_stmts_t* ss, char* name);
int schema_table_exists(schema_stmts_t* ss, char* db, char* table, int* res);
int schema_create_table(schema_stmts_t* ss, char* db, char* table, int* res);


