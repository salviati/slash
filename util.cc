// Borrowed code from http://code.google.com/p/my-itoa/.
// Licensed under LGPL.

#include "util.h"

int u64toa(uint64_t val, char* buf, unsigned int radix) {
	char* p;
	uint64_t a;        //every digit
	int len;
	char* b;            //start of the digit char
	char temp;
	uint64_t u;

	p = buf;


	u = val;

	b = p;

	do
	{
		a = u % radix;
		u /= radix;

		*p++ = a + '0';

	} while (u > 0);

	len = (int)(p - buf);

	*p-- = 0;

	//swap
	do
	{
		temp = *p;
		*p = *b;
		*b = temp;
		--p;
		++b;

	} while (b < p);

	return len;
}