#!/bin/bash

CC=gcc
CFLAGS="-ggdb -O0 -std=gnu11"

GLOBAL_INCLUDE="src/utils.h"

OBJS="src/main.c src/schema.c src/utils.c src/ds.c src/hash.c src/MurmurHash3.c"



OUT=cms

$CC \
	$CFLAGS \
	`mysql_config --cflags --libs` \
	-include $GLOBAL_INCLUDE \
	$OBJS \
	-o $OUT \
















