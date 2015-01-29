#include "malloc.h"

#include <assert.h>
#include <string.h>

// TODO: If deployed in high-end servers, there should be 
// a memory-pool module.

void *my_malloc(size_t size)
{
	void *ptr = malloc(size);
	assert(ptr != NULL);
	memset(ptr, 0, size);
	return ptr;
}

void my_free(void *ptr)
{
	free(ptr); 
}
