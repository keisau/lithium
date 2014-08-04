#include <hash.h>
#include <unistd.h>
#include <errno.h>
#include <lithium/string.h>
#include <stdio.h>
#include <fcntl.h>
#define BUF_SIZE	16384

using namespace li;
int main (int argc, char ** argv)
{
	if (argc > 1)
	{
		int fd = open (argv[1], O_RDONLY);
		if (fd >= 0)
		{
			index_t index = 0;
			u32 buf [BUF_SIZE] = {};
			int readlen;
			do
			{
				readlen = read (fd, buf, BUF_SIZE * sizeof (buf[0]));
				if (readlen < 0)
				{
					perror ("read");
					return -1;
				}
				readlen = readlen / sizeof (buf[0]);
				index ^= _hash (buf, readlen);
			} while (readlen > 0);
			printf ("checksum : %llx\n", index);
		}
	}
	return 0;
}
