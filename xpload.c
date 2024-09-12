/*
 * Copyright (c) 2024 Kenji Aoyama
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>		/* memset() */
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include <machine/xpio.h>

#include "config.h"

#define XP_MAX_SIZE	65536

static void *
loadfile(const char *fname, int maxsize, int *sizep)
{
	int fd, size, count;
	void *buf;
	struct stat sb;

	fd = open(fname, O_RDONLY);
	if (fd < 0)
		err(EXIT_FAILURE, "can not open XP program %s", fname);
	if (fstat(fd, &sb) != 0)
		err(EXIT_FAILURE, "can not stat XP program %s", fname);

	size = sb.st_size;
	if (size == 0 || size > maxsize)
		err(EXIT_FAILURE, "invalid size of %s (%d)", fname, size);

	buf = malloc(size);
	if (buf == NULL)
		err(EXIT_FAILURE, "can not malloc buffer");

	memset(buf, 0, size);

	count = read(fd, buf, size);
	if (count != size)
		err(EXIT_FAILURE, "failed to read %s", fname);

	close(fd);

	if (sizep != NULL)
		*sizep = size;
	return buf;
}

void
xp_load_reset(int xpfd, const char *fname)
{
	uint8_t *buf;
	int ret, size;
	struct xp_download xpdl;

	buf = loadfile(fname, XP_MAX_SIZE, &size);

	xpdl.size = size;
	xpdl.data = buf;

	ret = ioctl(xpfd, XPIOCDOWNLD, &xpdl);
	if (ret != 0)
		err(EXIT_FAILURE, "ioctl failed");

	free(buf);
}

void *
xp_mmap(int xpfd)
{
	uint8_t *shm;

	shm = mmap(NULL, XP_MAX_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED,
	    xpfd, 0);
	if (shm == MAP_FAILED)
		err(EXIT_FAILURE, "mmap failed");

	return shm;
}
