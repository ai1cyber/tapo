#ifndef __TCP_TOOL_DEFINE_H__
#define __TCP_TOOL_DEFINE_H__

#include <stdint.h>

#define PROG_NAME "tapo"
#define PROG_VERSION "0.1.2"

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

#define DIV_CEIL(seg, size) (((seg)+size-1)/size)


#define LINKTYPE_ETHERNET       1      /* also for 100Mb and up */
#define LINKTYPE_LINUX_SLL      113    /* Linux cooked socket capture */

#endif
