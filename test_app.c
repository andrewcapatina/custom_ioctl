/**
 *	Andrew Capatina
 *
 *	4/27/2022
 *
 *	Test application for custom ioctl() implementation.
 *
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "operations.h"

int main()
{
	int fd;
	u_int8_t value, number;

	fd = open("/dev/test_dev", O_RDWR);

	if(fd < 0)
	{
		printf("Cannot open device file. %s \n", strerror(errno));
		return -1;
	}

	number = 0xF0;
	ioctl(fd, WR_VALUE, (u_int8_t*) &number);

	ioctl(fd, RD_VALUE, (u_int8_t*) &value);
	printf("Value from ioctl() is: %x\n", value);

	close(fd);

	return 0;
}
