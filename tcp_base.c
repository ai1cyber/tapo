#include "tcp_base.h"
#include "def.h"

#include <string.h>

#define G 200 // current Linux implementation
#define K 4

void init_rtt(struct rtt_t *rtt)
{
	memset(rtt, 0, sizeof(struct rtt_t));
	rtt->rto = TIME_TO_TICK(1); // according to RFC 6298
}

/* Since we do not need exactly the same srtt/rto estimation as the 
 * TCP stack implementation, we use the specification from RFC 2988/6298. 
 * It's OK. :)
 */
void update_rtt(struct rtt_t *rtt, int32_t m)
{
	if (m == 0) m = 1;
	if (rtt->srtt == 0) {
		/* the first rtt value */
		rtt->srtt = m << 3;
		rtt->rttvar = m << 2;
	}
	else {
		/* alpha = 1/8, beta = 1/4 */
		m -= (rtt->srtt >> 3);
		rtt->srtt += m;

		if (m < 0) m = -m;
		m -= (rtt->rttvar >> 2);
		rtt->rttvar += m;
	}

	rtt->rto = (rtt->srtt >> 3) + MAX(G, K*rtt->rttvar);
}

int rtt_thres(struct rtt_t *rtt)
{
#define DELTA 2
	if (rtt->srtt == 0)
		return rtt->rto;
	else
		return MIN((rtt->srtt >> 3)*DELTA, rtt->rto);
}
