// With regards to https://en.wikipedia.org/wiki/IPv4_header_checksum

#include <stdio.h>
#include <stdlib.h>

int main() {
	
	unsigned int bytes[10] = 
	{
		0x4500, 0x0073, 0x0000, 0x4000, 0x4011, \
		0xb861, 0xc0a8, 0x001, 0xc0a8, 0x00c7
	};

	void *ptr = &bytes;

	int i;
	long int sum = 0;
	int curVal;

	for (i = 0; i < 10; ++i) {
		curVal = *(int *)ptr;
		printf("Looking at byte %#010x\n", curVal);
		if (i != 5) {
			sum += curVal;
		}
		ptr += sizeof(int);
	}

	long int temp = sum;
	int len = 0;
	while (temp & 0xFFFFF) {
		temp >>= 4;
		len += 1;
	}

	long int carry = (sum & 0xF0000) >> 4 * (len - 1);
	sum &= 0xFFFF;
	
	sum += carry;

	printf("Resulting checksum is %#010x\n", sum);

	return 0;
}
