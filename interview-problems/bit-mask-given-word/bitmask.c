/*
 * Given a word (int) `original' and two integers i and j, return the ith through the jth bits inclusive of `original' only (in place).
 *
 * 1. Form a mask with ones in the ith through the jth bit positions and
 *		zeroes elsewhere.
 *		a. Set int `result' to 0.
 *		b. Validate i and j (neither can be larger than the highest bit of a
 *			word on this platform, neither can be <0, and they cannot be equal).
 *		c. Loop: For each bit position from the lesser of i and j to the
 * 			greater,
 *				`result' |= (1 << position).
 *
 * 2. Return `result'.
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#define MIN(a,b) ((a < b) ? (a) : (b))
#define MAX(a,b) ((a > b) ? (a) : (b))

int
main(int argc, char *argv[])
{
	int original = 0xaaaa;
	int result = 0;
	int i = 5;
	int j = 9;
	int k;
	int pos;

	assert(i >= 0);
	assert(i <= (sizeof(int)*8 - 1));
	assert(j >= 0);
	assert(j <= (sizeof(int)*8 - 1));
	assert(i != j);

	for (k = MIN(i,j); k <= MAX(i,j); k++) {
		result |= (original & (1 << k));
	}

	printf("original= 0x%x, i= %d, j= %d, result is 0x%x\n",
				original, i, j, result);
	exit(0);
}
