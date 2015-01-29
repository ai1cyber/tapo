#include "tcp_stall_state.h"
#include "tcp_state.h"
#include "algorithm.h"

void init_tcp_stall(struct tcp_state *ts, struct tcp_stall_state *tss, double duration)
{
	tss->init_rwnd = ts->init_rwnd;
	tss->max_snd_seg_size = ts->max_snd_seg_size;

	tss->rwnd = ts->rwnd;
	tss->ca_state = ts->ca_state;

	tss->cur_time = ts->last_time - ts->start_time;
	tss->duration = duration;
	tss->srtt = TICK_TO_TIME(ts->rtt.srtt >> 3);
	tss->rto = TICK_TO_TIME(ts->rtt.rto);

	tss->snd_una = ts->snd_una - ts->seq_base;
	tss->snd_nxt = ts->snd_nxt - ts->seq_base;

	tss->packets_out = ts->packets_out;
	tss->sacked_out = ts->sacked_out;
	tss->holes = ts->holes;
	tss->outstanding = ts->outstanding;
	
	tss->head = ts->head;
	tss->tail = ts->tail; 

	tss->cur_pkt_dir = DIR_UNDETERMINED;

	// XXX to be determined after the flow is finished
	tss->lost = 0;
	tss->spurious = 0;
	tss->cur_pkt_lost = 0;
	tss->cur_pkt_spurious = 0;
}

void fill_tcp_stall_list(struct tcp_state *ts, struct list_head *stall_list)
{
	struct list_head *pos;
	struct range_t *range;
	struct tcp_stall_state *tss;

	int lost_num = 0, spurious_num = 0;
	uint32_t *lost_array = NULL, *spurious_array = NULL;

	list_for_each(pos, &ts->lost_list) 
		lost_num += 1;
	list_for_each(pos, &ts->spurious_retrans_list)
		spurious_num += 1;

	if (lost_num != 0) {
		lost_array = MALLOC_N(uint32_t, lost_num);
		int itr = 0;
		list_for_each(pos, &ts->lost_list) {
			range = list_entry(pos, struct range_t, list);
			lost_array[itr++] = range->begin - ts->seq_base;
		}
	}

	if (spurious_num != 0) {
		spurious_array = MALLOC_N(uint32_t, spurious_num);
		int itr = 0;
		list_for_each(pos, &ts->spurious_retrans_list) {
			range = list_entry(pos, struct range_t, list);
			spurious_array[itr++] = range->begin - ts->seq_base;
		}
	}

	list_for_each(pos, stall_list) {
		tss = list_entry(pos, struct tcp_stall_state, list); 
		tss->lost = array_range(lost_array, lost_num, tss->snd_una, tss->snd_nxt);
		tss->spurious = array_range(spurious_array, spurious_num, tss->snd_una, tss->snd_nxt);
		tss->cur_pkt_lost = array_range(lost_array, lost_num, tss->snd_una, tss->snd_una+1);
		tss->cur_pkt_spurious = array_range(spurious_array, spurious_num, tss->snd_una, tss->snd_una+1);
	}

	if (lost_num != 0)
		FREE(lost_array);
	if (spurious_num != 0)
		FREE(spurious_array);
}

void dump_tss_info(FILE *fp, struct tcp_stall_state *tss)
{
	fprintf(fp, "init_rwnd = %d, ", tss->init_rwnd);
	fprintf(fp, "max_snd_seg_size = %d, ", tss->max_snd_seg_size);
	fprintf(fp, "rwnd = %d, ", tss->rwnd);
	fprintf(fp, "ca_state = %s, ", tcp_ca_state[tss->ca_state]);
	fprintf(fp, "cur_time = %.3lf, ", tss->cur_time);
	fprintf(fp, "duration = %.3lf, ", tss->duration);
	fprintf(fp, "srtt = %.3lf, ", tss->srtt);
	fprintf(fp, "rto = %.3lf, ", tss->rto);
	fprintf(fp, "snd_una = %u, ", tss->snd_una);
	fprintf(fp, "snd_nxt = %u, ", tss->snd_nxt);

	fprintf(fp, "packets_out = %u, ", tss->packets_out);
	fprintf(fp, "sacked_out = %u, ", tss->sacked_out);
	fprintf(fp, "holes = %u, ", tss->holes);
	fprintf(fp, "outstanding = %u, ", tss->outstanding);
	fprintf(fp, "cur_pkt_lost = %u, ", tss->cur_pkt_lost);
	fprintf(fp, "cur_pkt_spurious = %u, ", tss->cur_pkt_spurious);

	fprintf(fp, "head = %d\n", tss->head);
	fprintf(fp, "tail = %d\n", tss->tail);
}
