#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#include "tcp_base.h"
#include "tcp_state.h"

#define BITS 20
#define HASH_TABLE_SIZE (1 << BITS)

struct hash_table_entry {
	struct tcp_state *ts;
	struct hash_table_entry *next;
};

struct hash_table_entry **new_hash_table();
struct tcp_state *find_ts_entry(struct hash_table_entry **hash_table, struct tcp_key *key);
int insert_ts_entry(struct hash_table_entry **hash_table, struct tcp_state *ts);
int delete_ts_entry(struct hash_table_entry **hash_table, struct tcp_state *ts);
void cleanup_hash_table(struct hash_table_entry **hash_table);

#endif
