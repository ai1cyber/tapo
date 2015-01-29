#ifndef __ALGORITHM_H__
#define __ALGORITHM_H__

#include "tcp_base.h"

// including
int left_bound(uint32_t *array, int n, uint32_t val)
{
	int low = 0, high = n, mid;
	while (low < high) {
		mid = (low+high)/2;
		if (after(val, array[mid]))
			low = mid+1;
		else if (before(val, array[mid]))
			high = mid-1;
		else
			return mid;
	}
	
	if (low == n || !before(array[low], val))
		return low;
	else
		return low+1;
}

// excluding
int right_bound(uint32_t *array, int n, uint32_t val)
{
	int pos = left_bound(array, n, val);
	while (pos > 0 && (!after(val, array[pos])))
		pos -= 1;

	return pos;
}

int array_range(uint32_t *array, int n, uint32_t left, uint32_t right)
{
	return left_bound(array, n, right) - left_bound(array, n, left);
}

#endif
