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
#include <string.h>		/* memcpy() */
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include <machine/xpio.h>

#include "config.h"
#include "xpfe.h"

#define XPFE_BLKSIZE 512
#define	XPFE_DISKMAX_BLK (16 * 256 * 256) /* XXX: 512MB at this moment */

/* extern */
extern struct xpfe_if_t *xpfe_if;

/* static */
int xpdisk_fd;
off_t xpdisk_size;

void *
xpdisk_open(const char *fname)
{
	int fd;
	off_t size;
	void *buf;
	struct stat sb;

	fd = open(fname, O_RDWR | 0666);
	if (fd < 0)
		err(EXIT_FAILURE, "can not open disk image %s", fname);
	if (fstat(fd, &sb) != 0)
		err(EXIT_FAILURE, "can not stat disk image %s", fname);

	size = sb.st_size;
	if (size == 0 || size > XPFE_DISKMAX_BLK * XPFE_BLKSIZE)
		err(EXIT_FAILURE, "invalid size of %s (%d)", fname, size);

	xpdisk_fd = fd;
	xpdisk_size = sb.st_size;
}

void
xpdisk_close(void)
{
	close(xpdisk_fd);
}

void
xpdisk_transfer(struct xpfe_if_t *xpfe_if)
{
	volatile uint32_t *xpdcom = &(xpfe_if->d_command);
	volatile uint8_t  *xpdbuf = (*xpfe_if).d_buf;
	uint8_t buf[XPFE_BLKSIZE];
	int dir, index;
	off_t ret;

	dir = (*xpdcom & 0xff000000) >> 24;
	index = *xpdcom & 0x00ffffff;

	ret = lseek(xpdisk_fd, index * XPFE_BLKSIZE, SEEK_SET);
	if (ret == -1) {
		warnx("%s: seek error, index = 0x08x", __func__, index);
		return;
	}

	if (dir == 0) {
		read(xpdisk_fd, buf, XPFE_BLKSIZE);
		memcpy((void *)xpdbuf, buf, XPFE_BLKSIZE);
	} else {
		memcpy(buf, (void *)xpdbuf, XPFE_BLKSIZE);
		write(xpdisk_fd, buf, XPFE_BLKSIZE);
	}
}

void
xpdisk_io(void)
{
	volatile uint32_t *xpdflag = &(xpfe_if->d_flag);

	/* disk I/O */
	if (*xpdflag & 0xff000000) {
		xpdisk_transfer(xpfe_if);
		*xpdflag &= 0x00ffffff;
		*xpdflag |= 0x00ff0000;
	}
}
