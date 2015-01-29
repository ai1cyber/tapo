#ifndef __MALLOC_H__
#define __MALLOC_H__

#include <stdlib.h>

inline void *my_malloc(size_t s);
inline void my_free(void *ptr);

#define MALLOC_N(type, n) ({ \
	void *ptr = my_malloc(sizeof(type)*n); \
	(type *)(ptr); \
})

#define MALLOC(type) MALLOC_N(type, 1)
#define FREE(ptr) my_free(ptr)

#endif
