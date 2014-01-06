#include "lithium.h"
#include "radix_tree.h"
#include <string>
using namespace std;

#define BUF_SIZE 2048
int main ()
{
	radix_tree<string> tree;
	char buf [BUF_SIZE];
	for (int i = 0; i < 64; ++i)
	{
		sprintf (buf, "%llu", 1ULL << i);
		tree.insert (1ULL << i, string (buf));
	}

	for (radix_tree<string>::iterator it = tree.begin();
			it != tree.end();
			++it)
	{
		printf ("%s\n", (*it).c_str());
	}

	return 0;
}
