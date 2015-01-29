#ifndef __TCP_TOOL_LOG_H__
#define __TCP_TOOL_LOG_H__

#include <stdio.h>

enum { INFO, DEBUG, WARN, ERROR };

#define FMT(fmt,level,args) "%s: " fmt, #level, ##args 

#define CAT(x,y) x y
#define TO_STR(x) #x

#define this_level INFO
#define LOG(level, fmt, ...) \
do { \
	if (level >= this_level) { \
		if (level == INFO) \
			fprintf(stdout, CAT("%s: ", fmt), TO_STR(level), ##__VA_ARGS__); \
		else \
			fprintf(stderr, CAT("%s: ", fmt), TO_STR(level), ##__VA_ARGS__); \
	} \
} \
while (0)

#endif
