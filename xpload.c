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
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include <machine/xpio.h>

#define XP_MAX_SIZE	65536
#define XP_DEV		"/dev/xp"

static void *
loadfile(const char *fname, int maxsize, int *sizep)
{
	int fd, size, count;
	void *buf;
	struct stat sb;

	fd = open(fname, O_RDONLY);
	if (fd < 0) {
		err(EXIT_FAILURE, "can not open %s", fname);
	}
	if (fstat(fd, &sb) != 0) {
		err(EXIT_FAILURE, "can not stat %s", fname);
	}
	size = sb.st_size;
	if (size == 0 || size > maxsize) {
		err(EXIT_FAILURE, "invalid size of %s (%d)",
		    fname, size);
	}
	buf = malloc(size);
	if (buf == NULL) {
		err(EXIT_FAILURE, "can not malloc buffer");
	}
	memset(buf, 0, size);

	count = read(fd, buf, size);
	if (count != size) {
		err(EXIT_FAILURE, "failed to read %s", fname);
	}
	close(fd);

	if (sizep != NULL)
		*sizep = size;
	return buf;
}

void
xp_load_reset(int xpfd, const char *fname)
{
	uint8_t *buf;
	int retval, size;
	struct xp_download xpdl;

	buf = loadfile(fname, XP_MAX_SIZE, &size);

	xpdl.size = size;
	xpdl.data = buf;

	retval = ioctl(xpfd, XPIOCDOWNLD, &xpdl);
	if (retval != 0) {
		err(EXIT_FAILURE, "ioctl failed");
	}

	free(buf);
}

void *
xp_mmap(int xpfd)
{
	uint8_t *xpshm;

	xpshm = mmap(NULL, XP_MAX_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED,
	    xpfd, 0);
	if (xpshm == MAP_FAILED) {
		err(EXIT_FAILURE, "mmap failed");
	}

	return xpshm;
}
