#include "lithium.h"
#include "radix_tree.h"
#include <string>
#include <stdio.h>
using namespace std;
using namespace li;

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
	
	//tree.erase (1ULL << 36);
	//tree.erase (1ULL << 26);
	//tree.erase (1ULL << 6);
	
	tree.erase (tree.find (1ULL << 36));
	tree.erase (tree.find (1ULL << 26));
	tree.erase (tree.find (1ULL << 6));

	radix_tree<string>::iterator it;

	for (int i = 0; i < 64; ++i)
	{
		sprintf (buf, "%llu", 1ULL << i);
		string str = string (buf);
		if (tree.find (1ULL << i) != tree.end())
		{
			puts (tree[1ULL << i].c_str());
		}
	}

	for (radix_tree<string>::iterator it = tree.begin();
			it != tree.end();
			++it)
	{
		printf ("%s\n", (*it).c_str());
	}

	return 0;
}
