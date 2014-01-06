#include "hash.h"

#pragma pack (push, 1)
struct S
{
	u32 first, second;
};
#pragma pack (pop)

int main ()
{
	S a, b;
	a.first = 0x12345678;
	a.second = 0x87654321;
	b.first = a.second;
	b.second = a.first;
	printf ("a : %llx\n", _hash (a));
	printf ("b : %llx\n", _hash (b));
	return 0;
}
