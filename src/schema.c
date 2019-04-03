#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "schema.h"

#include <mysql.h>





static const char* Q_DB_EXISTS = "\
SELECT SCHEMA_NAME \
FROM INFORMATION_SCHEMA.SCHEMATA \
WHERE SCHEMA_NAME = ?";

 

static const char* Q_TABLE_EXISTS = "\
SELECT TABLE_NAME \
FROM INFORMATION_SCHEMA.TABLES \
WHERE \
	TABLE_SCHEMA = ? \
	AND TABLE_NAME = ?";

static const char* Q_GET_COL_DEF = "\
SELECT \
	COLUMN_DEFAULT, \
	IS_NULLABLE,  \
	DATA_TYPE,  \
	CHARACTER_MAXIMUM_LENGTH,  \
	NUMERIC_PRECISION,  \
	NUMERIC_SCALE  \
FROM INFORMATION_SCHEMA.COLUMNS \
WHERE \
	TABLE_SCHEMA = ? \
	AND TABLE_NAME = ? \
	AND COLUMN_NAME = ?";



static const char* Q_GET_KEY_DEF = "\
SELECT \
	CONSTRAINT_NAME, \
	ORDINAL_POSITION, \
	POSITION_IN_UNIQUE_CONSTRAINT  \
FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE \
WHERE \
	TABLE_SCHEMA = ? \
	AND TABLE_NAME = ? \
	AND COLUMN_NAME = ?";






schema_stmts_t* prepare_schema(MYSQL* con) {
	schema_stmts_t* ss;
	my_bool true = 1;
	
	ss = pcalloc(ss);
	ss->con = con;
	
#define prepare(prop, q) \
	ss->prop = mysql_stmt_init(con); \
	if(mysql_stmt_prepare(ss->prop, q, strlen(q))) { \
		fprintf(stderr, "failed to prepare query: '%s'\n", q); \
	} \
	mysql_stmt_attr_set(ss->prop, STMT_ATTR_UPDATE_MAX_LENGTH, &true);
	
	prepare(db_exists, Q_DB_EXISTS);
	prepare(table_exists, Q_TABLE_EXISTS);
	prepare(get_col_def, Q_GET_COL_DEF);
	prepare(get_key_def, Q_GET_KEY_DEF);

#undef prepare
	
	return ss;
}







int schema_create_db(schema_stmts_t* ss, char* name) {
	char* q;
	char buffer[200];
	
	unsigned long len = strlen(name);
	if(len > 32) {
		fprintf(stderr, "Database name is too long (%ld): '%s'\n", len, name);
		return 1;
	}
	if(len < 1) {
		fprintf(stderr, "Database name is empty\n");
		return 2;
	}
	
	char* q1 = "CREATE DATABASE `";
	strcpy(buffer, q1);
	
	mysql_real_escape_string(ss->con, buffer + strlen(q1), name, strlen(name));
	
	strcat(buffer, "`;");
	
	
	if(mysql_query(ss->con, buffer)) {
		fprintf(stderr, "query failed: %s\n", mysql_error(ss->con));
		return 3;
	}
	
	return 0;
}



int schema_db_exists(schema_stmts_t* ss, char* name, int* res) {
	char* q;
	MYSQL_BIND bind[1];
	
	unsigned long len = strlen(name);
	if(len > 32) {
		fprintf(stderr, "Database name is too long (%ld): '%s'\n", len, name);
		return 1;
	}
	if(len < 1) {
		fprintf(stderr, "Database name is empty\n");
		return 2;
	}
	
	
	memset(bind, 0, sizeof(bind));
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = name;
	bind[0].buffer_length = len;
	bind[0].is_null = 0;
	bind[0].length = &len;
	
	if(mysql_stmt_bind_param(ss->db_exists, bind)) {
		fprintf(stderr, "bind param failed: %s\n", mysql_stmt_error(ss->db_exists));
		return 3;
	}
	
	if(mysql_stmt_execute(ss->db_exists)) {
		fprintf(stderr, "execute failed (schema_db_exists): %s\n", mysql_stmt_error(ss->db_exists));
		return 4;
	}
	
	if(mysql_stmt_store_result(ss->db_exists)) {
		fprintf(stderr, "failed to store result (schema_db_exists): %s\n", mysql_stmt_error(ss->db_exists));
		return 5;
	}
	
	int n = mysql_stmt_affected_rows(ss->db_exists);
	if(res) *res = mysql_stmt_affected_rows(ss->db_exists) == 0;
	
	if(mysql_stmt_free_result(ss->db_exists)) {
		fprintf(stderr, "failed to free result (schema_db_exists): %s\n", mysql_stmt_error(ss->db_exists));
		return 6;
	}

	return 0;
}





int schema_ensure_db(schema_stmts_t* ss, char* name) {
	int res;
	
	
	
}


int schema_table_exists(schema_stmts_t* ss, char* db, char* table, int* res) {
	int r, ret;
	if(ret = query_str2_num_rows(ss->table_exists, db, table, &r)) {
		return ret;
	}
	
	if(res) *res = r == 0;
	
	return 0;
}



int schema_get_col_def(schema_stmts_t* ss, char* db, char* table, char* col, schema_col_def_t** col_def) {
	MYSQL_STMT* stmt = ss->get_col_def;
	MYSQL_BIND bind[3];
	schema_col_def_t* sc;
	
	
	memset(bind, 0, sizeof(bind));
	bind_charp_strlen(bind[0], db);
	bind_charp_strlen(bind[1], table);
	bind_charp_strlen(bind[2], col);
	
	if(mysql_stmt_bind_param(stmt, bind)) {
		fprintf(stderr, "bind param failed: %s\n", mysql_stmt_error(stmt));
		return 3;
	}
	
	if(mysql_stmt_execute(stmt)) {
		fprintf(stderr, "execute failed: %s\n", mysql_stmt_error(stmt));
		return 4;
	}
	
	if(mysql_stmt_store_result(stmt)) {
		fprintf(stderr, "failed to store result: %s\n", mysql_stmt_error(stmt));
		return 5;
	}
	
	// check if there was a result
	int n = mysql_stmt_affected_rows(stmt);
	if(n == 0) {
		if(col_def) col_def = 0;
		if(mysql_stmt_free_result(stmt)) {
			fprintf(stderr, "failed to free result: %s\n", mysql_stmt_error(stmt));
			return 6;
		}
	}
	
	
	// time to fetch the results
	unsigned int num_fields;
	MYSQL_FIELD* fields;
	MYSQL_RES* result;
	
	// TODO: error handling here. should be ok for now, these aren't flaky functions
	result = mysql_stmt_result_metadata(stmt);
	num_fields = mysql_num_fields(result);
	fields = mysql_fetch_fields(result);
	
	
	my_bool isnull[6];
	unsigned long length[6];
	MYSQL_BIND rbind[6];
	memset(rbind, 0, sizeof(rbind));
	
	
	char* column_default = malloc(fields[0].max_length + 1);
	column_default[fields[0].max_length] = 0;
	rbind_charp(bind[0], column_default, fields[0].max_length, length[0], isnull[0]);

	char* is_nullable = malloc(fields[1].max_length + 1); // "yes/no"
	is_nullable[fields[1].max_length] = 0;
	rbind_charp(bind[1], is_nullable, fields[1].max_length, length[1], isnull[1]);
	
	char* data_type = malloc(fields[2].max_length + 1);
	data_type[fields[2].max_length] = 0;
	rbind_charp(bind[2], column_default, fields[2].max_length, length[2], isnull[2]);
	
	uint64_t max_len;
	rbind_u64(rbind[3], max_len);

	uint64_t num_prec;
	rbind_u64(rbind[4], num_prec);

	uint64_t num_scale;
	rbind_u64(rbind[5], num_scale);
	
	if (mysql_stmt_bind_result(stmt, rbind)) {
		fprintf(stderr, "failed to bind result: %s\n", mysql_stmt_error(stmt));
		return 7;
	}
	
	if(mysql_stmt_fetch(stmt)) {
		fprintf(stderr, "failed to fetch result: %s\n", mysql_stmt_error(stmt));
		return 8;
	}
	
	
	// fill in the struct
	
	sc = pcalloc(sc);
	sc->name = strdup(col);
	sc->def_value = strndup(column_default, length[0]);
	sc->char_max_len = max_len;
	sc->numeric_precision = num_prec;
	sc->numeric_scale = num_scale;
	sc->not_null = is_yes(is_nullable);
	sc->type = get_mysql_type_from_string(data_type, length[2]);
	
	
	
	if(mysql_stmt_free_result(stmt)) {
		fprintf(stderr, "failed to free result: %s\n", mysql_stmt_error(stmt));
		free(sc);
		return 9;
	}
	
	free(column_default);
	free(is_nullable);
	free(data_type);
	
	if(col_def) *col_def = sc;
	else free(sc);
	
	return 0;
}



/*
int schema_create_table(schema_stmts_t* ss, schema_table_def_t* std) {
	
	
	
	
	
	
	return 0;
}
*/


