#ifndef __CMS_utils_h__
#define __CMS_utils_h__

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <mysql.h>



#define pcalloc(x) calloc(1, sizeof(*(x)))



static inline int is_yes(char* s) {
	if(*s     != 'y' && *s     != 'Y') return 0;
	if(*(s+1) != 'e' && *(s+1) != 'E') return 0;
	if(*(s+2) != 's' && *(s+2) != 'S') return 0;
	return 1;
}

static inline int is_no(char* s) {
	if(*s     != 'n' && *s     != 'N') return 0;
	if(*(s+1) != 'o' && *(s+1) != 'O') return 0;
	return 1;
}


enum enum_field_types get_mysql_type_from_string(char* s, int len);



int query_str1_num_rows(MYSQL_STMT* stmt, char* str, int* rows);
int query_str2_num_rows(MYSQL_STMT* stmt, char* str1, char* str2, int* rows);



#define CAT3_(a,b,c) a ## b ## c
#define CAT3(a,b,c) CAT3_(a,b,c)


// for params
#define bind_charp(bind, str, lenp) \
	(bind).buffer_type = MYSQL_TYPE_STRING; \
	(bind).buffer = (str); \
	(bind).buffer_length = *(len); \
	(bind).is_null = 0; \
	(bind).length = (len);

#define bind_charp_strlen(bind, str) \
	unsigned long CAT3(_bind_strlen_, __func__, __LINE__) = strlen(str); \
	(bind).buffer_type = MYSQL_TYPE_STRING; \
	(bind).buffer = (str); \
	(bind).buffer_length = CAT3(_bind_strlen_, __func__, __LINE__); \
	(bind).is_null = 0; \
	(bind).length = &(CAT3(_bind_strlen_, __func__, __LINE__));

	
	
// for results
#define rbind_u64(bind, u) \
	(bind).buffer_type = MYSQL_TYPE_STRING; \
	(bind).buffer = &(u); \


#define rbind_charp(bind, str, buflen, len, isnull) \
	(bind).buffer_type = MYSQL_TYPE_STRING; \
	(bind).buffer = (str); \
	(bind).buffer_length = (buflen); \
	(bind).is_null = &(isnull); \
	(bind).length = &(len);







#endif // __CMS_utils_h__
