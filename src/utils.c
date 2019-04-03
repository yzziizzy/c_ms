#include <stdio.h>

#include "utils.h"







int query_str1_num_rows(MYSQL_STMT* stmt, char* str, int* rows) {
	char* q;
	MYSQL_BIND bind[1];
	
	unsigned long len = strlen(str);
	
	memset(bind, 0, sizeof(bind));
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = str;
	bind[0].buffer_length = len;
	bind[0].is_null = 0;
	bind[0].length = &len;
	
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
	
	int n = mysql_stmt_affected_rows(stmt);
	if(rows) *rows = mysql_stmt_affected_rows(stmt);
	
	if(mysql_stmt_free_result(stmt)) {
		fprintf(stderr, "failed to free result: %s\n", mysql_stmt_error(stmt));
		return 6;
	}

	return 0;
}










int query_str2_num_rows(MYSQL_STMT* stmt, char* str1, char* str2, int* rows) {
	char* q;
	MYSQL_BIND bind[2];
	
	unsigned long len1 = strlen(str1);
	unsigned long len2 = strlen(str2);
	
	memset(bind, 0, sizeof(bind));
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = str1;
	bind[0].buffer_length = len1;
	bind[0].is_null = 0;
	bind[0].length = &len1;
	
	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = str2;
	bind[1].buffer_length = len2;
	bind[1].is_null = 0;
	bind[1].length = &len2;
	
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
	
	int n = mysql_stmt_affected_rows(stmt);
	if(rows) *rows = mysql_stmt_affected_rows(stmt);
	
	if(mysql_stmt_free_result(stmt)) {
		fprintf(stderr, "failed to free result: %s\n", mysql_stmt_error(stmt));
		return 6;
	}

	return 0;
}





enum enum_field_types get_mysql_type_from_string(char* s, int len) {
	if(strncasecmp(s, "bigint", len)) return MYSQL_TYPE_LONGLONG;
	if(strncasecmp(s, "bit", len)) return MYSQL_TYPE_BIT;
	if(strncasecmp(s, "binary", len)) return MYSQL_TYPE_BLOB;
	if(strncasecmp(s, "blob", len)) return MYSQL_TYPE_BLOB;
	if(strncasecmp(s, "char", len)) return MYSQL_TYPE_STRING;
	if(strncasecmp(s, "date", len)) return MYSQL_TYPE_DATE;
	if(strncasecmp(s, "datetime", len)) return MYSQL_TYPE_DATETIME;
	if(strncasecmp(s, "decimal", len)) return MYSQL_TYPE_DECIMAL;
	if(strncasecmp(s, "double", len)) return MYSQL_TYPE_DOUBLE;
	if(strncasecmp(s, "enum", len)) return MYSQL_TYPE_ENUM;
	if(strncasecmp(s, "float", len)) return MYSQL_TYPE_FLOAT;
	if(strncasecmp(s, "integer", len)) return MYSQL_TYPE_LONG;
	if(strncasecmp(s, "longblob", len)) return MYSQL_TYPE_BLOB;
	if(strncasecmp(s, "longtext", len)) return MYSQL_TYPE_BLOB;
	if(strncasecmp(s, "null", len)) return MYSQL_TYPE_NULL;
	if(strncasecmp(s, "numeric", len)) return MYSQL_TYPE_DECIMAL;
	if(strncasecmp(s, "mediumtext", len)) return MYSQL_TYPE_BLOB;
	if(strncasecmp(s, "mediumint", len)) return MYSQL_TYPE_INT24;
	if(strncasecmp(s, "real", len)) return MYSQL_TYPE_FLOAT;
	if(strncasecmp(s, "set", len)) return MYSQL_TYPE_SET;
	if(strncasecmp(s, "smallint", len)) return MYSQL_TYPE_SHORT;
	if(strncasecmp(s, "text", len)) return MYSQL_TYPE_BLOB;
	if(strncasecmp(s, "time", len)) return MYSQL_TYPE_TIME;
	if(strncasecmp(s, "timestamp", len)) return MYSQL_TYPE_TIMESTAMP;
	if(strncasecmp(s, "tinyint", len)) return MYSQL_TYPE_TINY;
	if(strncasecmp(s, "tinytext", len)) return MYSQL_TYPE_BLOB;
	if(strncasecmp(s, "varbinary", len)) return MYSQL_TYPE_VAR_STRING;
	if(strncasecmp(s, "varchar", len)) return MYSQL_TYPE_VAR_STRING;
	if(strncasecmp(s, "year", len)) return MYSQL_TYPE_YEAR;
	
	fprintf(stderr, "Unknown mysql type: '%*s'\n", len, s);
	
	return 0;
} 







