#include "hash_table.h"
#include "def.h"
#include "log.h"
#include "malloc.h"

#include <string.h>
#include <assert.h>
#include <errno.h>

// in buf, skip s bits, then read n bits
// ensure len(buf)*8 >= (n+s) and n <= 32
static inline int read_nbits(unsigned char *buf, int n, int s)
{
	buf += s/8;
	s %= 8;
	int m = 0;
	int val = 0;
	if (s > 0) {
		m = 8-s;
		val = (*buf) & ((1 << m)-1);
		buf += 1;
	}

	if (m > n) {
		val >>= (m-n);
		return val;
	}
	n -= m;

	while (n > 0) {
		m = MIN(8, n);
		val = (val << m) | (*buf) >> (8-m);
		buf += 1;
		n -= m;
	}

	return val;
}

static inline int hash(struct tcp_key *key)
{
	assert(BITS < 24);

	int val = 0;
	int shift = 3;
	int size = sizeof(struct tcp_key)*8;
	unsigned char *buf = (unsigned char *)key;
	int i = 0;
	while (i < size) {
		int n = MIN(BITS, size-i);
		int t = read_nbits(buf, n, i);
		val ^= ((t << shift)&((1<<BITS)-1)) | (t >> shift);
		i += n;
	}

	return val;
}

struct hash_table_entry **new_hash_table()
{
	struct hash_table_entry **ht = MALLOC_N(struct hash_table_entry *, HASH_TABLE_SIZE);
	if (ht == NULL) {
		LOG(ERROR, "malloc hash table failed: %s\n", strerror(errno));
		exit(1);
	}

	return ht;
}

struct tcp_state *find_ts_entry(struct hash_table_entry **hash_table, struct tcp_key *key)
{
	struct hash_table_entry *entry = hash_table[hash(key)];
	while (entry) {
		if (memcmp(&entry->ts->key, key, sizeof(struct tcp_key)) == 0)
			return entry->ts;
		entry = entry->next;
	}

	return NULL;
}

int insert_ts_entry(struct hash_table_entry **hash_table, struct tcp_state *ts)
{
	int hv = hash(&ts->key);

	struct hash_table_entry *entry = MALLOC(struct hash_table_entry);
	entry->ts = ts;
	entry->next = hash_table[hv];
	hash_table[hv] = entry;

	return 0;
}

int delete_ts_entry(struct hash_table_entry **hash_table, struct tcp_state *ts)
{
	int hv = hash(&ts->key);

	struct hash_table_entry *entry = hash_table[hv],
							*temp = NULL;
	if (memcmp(&entry->ts->key, &ts->key, sizeof(struct tcp_key)) == 0) {
		temp = entry;
		hash_table[hv] = temp->next;
		finish_tcp_state(temp->ts);
		FREE(temp);
		return 1;
	}
	else {
		while (entry->next) {
			temp = entry->next;
			if (memcmp(&temp->ts->key, &ts->key, sizeof(struct tcp_key)) == 0) {
				entry->next = temp->next;
				finish_tcp_state(temp->ts);
				FREE(temp);
				return 1;
			}

			entry = entry->next;
		}

		return 0;
	}
}

void cleanup_hash_table(struct hash_table_entry **hash_table)
{
	int i = 0;
	for (; i < HASH_TABLE_SIZE; i++) {
		while (hash_table[i] != NULL) {
			struct hash_table_entry *temp = hash_table[i];
			hash_table[i] = hash_table[i]->next;
			finish_tcp_state(temp->ts);
			FREE(temp);
		}
	} 

	FREE(hash_table);
}
